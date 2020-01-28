#include "StdAfx.h"

#include <Precision.hxx>
#include <ElSLib.hxx>
#include <GeomConvert.hxx>

#include "ScBrepLib.h"
#include "ScCmdBSpline.h"

#include "FFDObj.h"
#include "FFDAlgo.h"
#include ".\ffdtool.h"

FFDTool::FFDTool(void)
{
	m_bValid = FALSE;
}

FFDTool::~FFDTool(void)
{
}

//////////////////////////////////////////////////////////////////////////
//������ι��ߣ������׼����ͱ����������Ϣ��
// �ȿ���������Σ��ֲ���������
// ����ϵ����Ҫ��������
FFDSurfTool::FFDSurfTool()
{
	m_bPlane = FALSE;
}

FFDSurfTool::~FFDSurfTool()
{

}

//
BOOL	FFDSurfTool::InitRect(const gp_Pnt& p1,const gp_Pnt& p2,
		int nUDeg,int nVDeg,int nUCtrlPnts,int nVCtrlPnts)
{
	gp_Ax2 ax;
	return InitRect(p1,p2,ax,nUDeg,nVDeg,nUCtrlPnts,nVCtrlPnts);
}

BOOL	FFDSurfTool::InitRect(const gp_Pnt& p1,const gp_Pnt& p2,const gp_Ax2& ax,
					 int nUDeg,int nVDeg,int nUCtrlPnts,int nVCtrlPnts)
{
	//Ҫ����̫�ӽ�
	if(p1.Distance(p2) < Precision::Confusion())
		return FALSE;

	//�����������ƽ��
	gp_Vec vec(p1,p2);
	if(vec.IsParallel(gp_Vec(ax.XDirection()),0.001) ||
		vec.IsParallel(gp_Vec(ax.YDirection()),0.001))
		return FALSE;

	gp_Pln pln = gce_MakePln(p1,ax.Direction());

	double u,v,u1,v1,u2,v2;
	ElSLib::PlaneParameters(pln.Position(),p2,u,v);
	u1 = min(0,u);
	u2 = std::max(0.0,u);
	v1 = min(0,v);
	v2 = std::max(0.0,v);

	BRepBuilderAPI_MakeFace mf(pln,u1,u2,v1,v2);
	if(!mf.IsDone())
	{
		return FALSE;
	}
	TopoDS_Shape aF = mf.Shape();
	if(aF.IsNull())
		return FALSE;

	//תΪnurbs����
	//ת��Ϊbsp����
	BRepBuilderAPI_NurbsConvert nc;
	nc.Perform(aF);
	if(!nc.IsDone())
	{
		DTRACE("\n nurbs convert failed.");
		return FALSE;
	}

	TopoDS_Shape aNF = nc.Shape();
	if(aNF.ShapeType() != TopAbs_FACE)
		return FALSE;

	TopoDS_Face aFace = TopoDS::Face(aNF);
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);//maybe a copy
	if(aSurf.IsNull())
		return FALSE;

	m_datumSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
	if(m_datumSurf.IsNull())
		return FALSE;

	//���׺Ͳ�����Ƶ�
	int ud = m_datumSurf->UDegree(),vd = m_datumSurf->VDegree();
	if(ud < nUDeg || vd < nVDeg)
	{
		if(ud < nUDeg) ud = nUDeg;
		if(vd < nVDeg) vd = nVDeg;
		m_datumSurf->IncreaseDegree(ud,vd);
		DTRACE("\n Increase Degree.");
	}

	//���ӿ��Ƶ�
	TcBSplLib::IncSurfCtrlPnts(m_datumSurf,nUCtrlPnts,nVCtrlPnts);

	//��¼ƽ����Ϣ
	m_bPlane = TRUE;
	m_aPlane = new Geom_Plane(pln);
	m_u1 = u1;m_v1 = v1;
	m_u2 = u2;m_v2 = v2;

	m_bValid = TRUE;

	return TRUE;
}

//��ʼ����׼����
BOOL	FFDSurfTool::InitDatumSurf(const Handle(Geom_Surface)& pSurf)
{
	if(pSurf.IsNull())
		return FALSE;


	return FALSE;
}

//����Ŀ������
BOOL	FFDSurfTool::SetTarSurf(const Handle(Geom_Surface)& pSurf)
{
	m_tarSurf = pSurf;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
FFDLineTool::FFDLineTool()
{
	m_nSamps = 0;
	m_bChanged = FALSE;
}

FFDLineTool::~FFDLineTool()
{
	
}

//�����������������ڻ���ֱ�ߵ�FFD���Σ�ԭʼֱ��Ϊ��׼�ߣ����κ��ֱ��ΪĿ�����ߡ�
BOOL	FFDLineTool::InitLine(const gp_Pnt& p1,const gp_Pnt& p2,int nDeg,int ncpts)
{
	double dist = p1.Distance(p2);
	if(dist < Precision::Confusion())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	gp_Dir vd(gp_Vec(p1,p2));
	Handle(Geom_Line) aLin = new Geom_Line(p1,vd);
	Handle(Geom_TrimmedCurve) aTLin = new Geom_TrimmedCurve(aLin,0,dist);
	//תΪbspline
	Handle(Geom_BSplineCurve) aBspCur = GeomConvert::CurveToBSplineCurve(aTLin);
	if(aBspCur.IsNull())
		return FALSE;

	//���б�Ҫ������
	if(aBspCur->Degree() < nDeg)
	{
		aBspCur->IncreaseDegree(nDeg);
	}

	TcBSplLib::IncCurveCtrlPnts(aBspCur,ncpts,
		aBspCur->FirstParameter(),aBspCur->LastParameter());

	m_datumCurve = aBspCur;

	m_bValid = TRUE;
	m_bChanged = TRUE;

	return TRUE;
}

BOOL	FFDLineTool::SetLineDfmCurve(const Handle(Geom_BSplineCurve)& aCur)
{
	//�򵥵ļ�飬�������Ϳ��Ƶ����Ƿ�һ�¡�
	if(aCur->Degree() != m_datumCurve->Degree() || aCur->NbPoles() != m_datumCurve->NbPoles())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_tarCurve = aCur;
	m_bChanged = TRUE;

	return TRUE;
}

//����RMF
BOOL	FFDLineTool::CalcRMF(int nSamps)
{
	if(!m_bValid || m_tarCurve.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if(!m_bChanged && m_nSamps == nSamps)
	{
		return TRUE;//no change
	}

	//���¼���RMF
	//�ȼ���Ŀ�����ߵġ�
	m_tarRmf.Clear();
	m_tarRmf.Init(m_tarCurve,m_tarCurve->FirstParameter(),m_tarCurve->LastParameter(),nSamps);
	if(!m_tarRmf.CalcSloanRMF() && !m_tarRmf.CalcRotateRMF())
	{
		DTRACE("\n calc rmf failed.");
		return FALSE;
	}

	//����Ŀ�����߼����׼���ߵ�rmf��ע�⣺�������Ŀǰ���ߵķ�ʸ�����׼���ߵĳ�ʼ��ʸ
	gp_Pnt pnt;
	gp_Vec tang;
	m_datumCurve->D1(m_datumCurve->FirstParameter(),pnt,tang);
	if(tang.Magnitude() < Precision::Confusion())
	{
		DTRACE("\n get datum curve first param tang failed.");
		return FALSE;//ʵ�ʲ��ᷢ����
	}
	
	BOOL bGot = FALSE;
	gp_Dir T(tang),B,startN;
	UINT uix;
	for(uix = 0;uix < m_tarRmf.m_frmArray.size();uix ++)
	{
		FFDCurvePointFrame& pfrm = m_tarRmf.m_frmArray.at(uix);
		B = T.Crossed(pfrm._dN);
		if(B.XYZ().Modulus() < Precision::Confusion())
			continue;

		//�����ʼ
		startN = B.Crossed(T);
		bGot = TRUE;
	}

	m_datumRmf.Clear();
	m_datumRmf.Init(m_datumCurve,m_datumCurve->FirstParameter(),m_datumCurve->LastParameter(),nSamps);
	if(bGot)
	{
		m_datumRmf.SetStartNormal(startN);
	}

	if(!m_datumRmf.CalcSloanRMF() && !m_datumRmf.CalcRotateRMF())
	{
		DTRACE("\n calc rmf failed.");
		return FALSE;
	}

	m_bChanged = FALSE;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// �������ߵı��ι��ߡ�
FFDCurveTool::FFDCurveTool()
{

}

FFDCurveTool::~FFDCurveTool()
{

}

BOOL	FFDCurveTool::SetDatumCurve(const Handle(Geom_Curve)& aCur)
{
	return SetDatumCurve(aCur,aCur->FirstParameter(),aCur->LastParameter());
}

BOOL	FFDCurveTool::SetDatumCurve(const Handle(Geom_Curve)& aCur,double df,double dl)
{
	if(aCur.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_datumCurve = aCur;
	m_df = df;
	m_dl = dl;

	return TRUE;
}

//����Ŀ������
BOOL	FFDCurveTool::SetTarCurve(const Handle(Geom_Curve)& aCur)
{
	return SetTarCurve(aCur,aCur->FirstParameter(),aCur->LastParameter());
}

BOOL	FFDCurveTool::SetTarCurve(const Handle(Geom_Curve)& aCur,double df,double dl)
{
	if(aCur.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_tarCurve = aCur;
	m_tf = df;
	m_tl = dl;

	return TRUE;
}

//����RMF
BOOL	FFDCurveTool::CalcRMF(int nSamps,double datumT,double tarT,BOOL bStretch)
{
	if(m_datumCurve.IsNull() || m_tarCurve.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//������Ӧ��ϵ
	double da = (datumT - m_df)/(m_dl - m_df);
	double db = (tarT - m_tf)/(m_tl - m_tf);
	if(da < 0.5 && db < 0.5) m_nMapType = eMap_PP; //����-����
	else if(da < 0.5 && db > 0.5) m_nMapType = eMap_PN;
	else if(da > 0.5 && db < 0.5) m_nMapType = eMap_NP;
	else 
		m_nMapType = eMap_NN;

	if(!bStretch)//���ֲ�ͬ�Ĵ���
	{//��Ҫ���ݻ�׼���ߵĳ��ȼ���Ŀ�����ߵĲ�����Χ.
		//�����׼���߱�Ŀ�����߶�,��ض�Ŀ������,����Ҫ����Ŀ������,���ƶȳ���,�ٽ�������.
		
	}

	//��򵥵Ĵ���,���ж������Ƿ�ֱ��,������ǳ��õ����
	if(m_datumCurve->IsKind(STANDARD_TYPE(Geom_Line)))
	{
		//���¼���RMF
		//�ȼ���Ŀ�����ߵġ�
		m_tarRmf.Clear();
		m_tarRmf.Init(m_tarCurve,m_tf,m_tl,nSamps);
		if(!m_tarRmf.CalcSloanRMF() && !m_tarRmf.CalcRotateRMF())
		{
			DTRACE("\n calc rmf failed.");
			return FALSE;
		}

		//����Ŀ�����߼����׼���ߵ�rmf��ע�⣺�������Ŀǰ���ߵķ�ʸ�����׼���ߵĳ�ʼ��ʸ
		gp_Pnt pnt;
		gp_Vec tang;
		m_datumCurve->D1(m_df,pnt,tang);
		if(tang.Magnitude() < Precision::Confusion())
		{
			DTRACE("\n get datum curve first param tang failed.");
			return FALSE;//ʵ�ʲ��ᷢ����
		}

		BOOL bGot = FALSE;
		gp_Dir T(tang),B,startN;
		UINT uix;
		for(uix = 0;uix < m_tarRmf.m_frmArray.size();uix ++)
		{
			FFDCurvePointFrame& pfrm = m_tarRmf.m_frmArray.at(uix);
			B = T.Crossed(pfrm._dN);
			if(B.XYZ().Modulus() < Precision::Confusion())
				continue;

			//�����ʼ
			startN = B.Crossed(T);
			bGot = TRUE;
		}

		//TODO:����,�����Ļ�׼�㲻ͬ,��ӳ��λ�õȶ���ͬ,������Ҫ�������.

		m_datumRmf.Clear();
		m_datumRmf.Init(m_datumCurve,m_df,m_dl,nSamps);
		if(bGot)
		{
			m_datumRmf.SetStartNormal(startN);
		}

		if(!m_datumRmf.CalcSloanRMF() && !m_datumRmf.CalcRotateRMF())
		{
			DTRACE("\n calc rmf failed.");
			return FALSE;
		}
	}
	else
	{
		//�ȼ���Ŀ�����ߵġ�
		m_tarRmf.Clear();
		m_tarRmf.Init(m_tarCurve,m_tf,m_tl,nSamps);
		if(!m_tarRmf.CalcSloanRMF() && !m_tarRmf.CalcRotateRMF())
		{
			DTRACE("\n calc rmf failed.");
			return FALSE;
		}

		m_datumRmf.Clear();
		m_datumRmf.Init(m_datumCurve,m_df,m_dl,nSamps);

		if(!m_datumRmf.CalcSloanRMF() && !m_datumRmf.CalcRotateRMF())
		{
			DTRACE("\n calc rmf failed.");
			return FALSE;
		}
	}

	//��¼ӳ�����
	m_dMapRatio = (m_tl - m_tf) / (m_dl - m_df);
	
	m_bValid = TRUE;

	return TRUE;
}

double		FFDCurveTool::MapParam(double datumt)
{
	double t;
	switch(m_nMapType)
	{
	case eMap_PP:
		{
			t = m_tf + (datumt - m_df) * m_dMapRatio;
		}
		break;
	case eMap_PN://[0,1] -> [1,0]
		{
			t = m_tl - (datumt - m_df) * m_dMapRatio;
		}
		break;
	case eMap_NP:// [1,0] -> [0,1] t > 0
		{
			t = m_tl -  (datumt - m_df) * m_dMapRatio;
		}
		break;
	case eMap_NN://[1,0] -> [1,0] t > 0
		{
			t = m_tl - (datumt - m_df) * m_dMapRatio;
		}
		break;
	default:
		break;
	}

	return t;
}

//////////////////////////////////////////////////////////////////////////
//
FFDSurfFlowTool::FFDSurfFlowTool()
{
	m_bStretch = TRUE;
	m_bUseTrimSurf = FALSE;
}

FFDSurfFlowTool::~FFDSurfFlowTool()
{

}

BOOL	FFDSurfFlowTool::SetDatumSurf(const Handle(Geom_Surface)& aSurf)
{
	if(aSurf.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	double u1,u2,v1,v2;
	aSurf->Bounds(u1,u2,v1,v2);
	return SetDatumSurf(aSurf,u1,v1,u2,v2);
}

BOOL	FFDSurfFlowTool::SetDatumSurf(const Handle(Geom_Surface)& aSurf,double u1,double v1,double u2,double v2)
{
	if(aSurf.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	BOOL bRet = TRUE;

	m_datumSurf = aSurf;
	m_dU1 = u1;m_dV1 = v1;
	m_dU2 = u2;m_dV2 = v2;

	if(m_bUseTrimSurf)
	{
		//תΪnurbs
	}

	if(!m_tarSurf.IsNull())
	{
		bRet = CalcMapInfo();
		m_bValid = bRet;
	}

	return bRet;
}

BOOL	FFDSurfFlowTool::SetTarSurf(const Handle(Geom_Surface)& aSurf)
{
	if(aSurf.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	double u1,u2,v1,v2;
	aSurf->Bounds(u1,u2,v1,v2);

	return SetTarSurf(aSurf,u1,v1,u2,v2);
}

BOOL	FFDSurfFlowTool::SetTarSurf(const Handle(Geom_Surface)& aSurf,double u1,double v1,double u2,double v2)
{
	if(aSurf.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	BOOL bRet = TRUE;

	m_tarSurf = aSurf;
	m_tU1 = u1;m_tV1 = v1;
	m_tU2 = u2;m_tV2 = v2;

	if(m_bUseTrimSurf)
	{
		//תΪnurbs
	}

	if(!m_datumSurf.IsNull())
	{
		bRet = CalcMapInfo();
		m_bValid = bRet;
	}

	return bRet;
}

BOOL	FFDSurfFlowTool::MapPoint(double u,double v,gp_Pnt2d& uv)
{
	ASSERT(IsValid());

	double tu,tv;
	tu = m_tU1 + (u - m_dU1) * m_dUMapRatio;
	tv = m_tV1 + (v - m_dV1) * m_dVMapRatio;

	uv.SetCoord(tu,tv);

	return TRUE;
}

BOOL	FFDSurfFlowTool::CalcMapInfo()
{
	m_dUMapRatio = (m_tU2 - m_tU1)/(m_dU2 - m_dU1);
	m_dVMapRatio = (m_tV2 - m_tV1)/(m_dV2 - m_dV1);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//��б����
FFDShearTool::FFDShearTool()
{
	m_drad = 0.0;
}

FFDShearTool::~FFDShearTool()
{

}

//��ʼ��,��׼��ͻ�׼����
BOOL	FFDShearTool::Init(const gp_Pnt& org,const gp_Vec& baseDir)
{
	//Ĭ��baseDirΪy��,z��Ϊȱʡz��,����basedir��xyƽ����.
	gp_Dir xd,yd(baseDir),zd(0,0,1.0);
	ASSERT(fabs(yd.Dot(zd)) < 1e-10);
	xd = yd.Crossed(zd);
	if(xd.XYZ().Modulus() < 1e-6)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_locAx = gp_Ax2(org,zd,xd);
	//
	m_tsf.SetTransformation(gp_Ax3(m_locAx));
	m_tsfr = m_tsf.Inverted();

	m_bValid = TRUE;
	
	return TRUE;
}

//���ǻ�����������ϵ.
BOOL	FFDShearTool::Init(const gp_Pnt& org,const gp_Vec& baseDir,const gp_Ax2& ucs)
{
	return TRUE;
}

//��б����
BOOL	FFDShearTool::Shear(const gp_Vec& newDir)
{
	ASSERT(m_bValid);
	//��ת�뵽�ֲ�����ϵ��,�ټ����Y��ķ���
	gp_Dir dir(newDir);
	dir.Transform(m_tsf);

	//����Ƕ�
	m_drad = dir.AngleWithRef(m_locAx.YDirection(),m_locAx.Direction());
	m_bValid = TRUE;

	return TRUE;
}

//��б�Ƕ�(����)
BOOL	FFDShearTool::Shear(double drad)
{
	ASSERT(m_bValid);

	m_drad = drad;
	m_bValid = TRUE;

	return TRUE;
}

//�Ե���б任
BOOL	FFDShearTool::TrsfPoint(const gp_Pnt& pnt,gp_Pnt& tsfpnt)
{
	//ת��ֲ�����ϵ
	tsfpnt = pnt;

	if(fabs(m_drad) < 1e-10)
		return TRUE;

	tsfpnt.Transform(m_tsf);
	tsfpnt.SetX(tsfpnt.X() + tsfpnt.Y() * tan(m_drad));

	//ת��ԭ������ϵ
	tsfpnt.Transform(m_tsfr);

	return TRUE;
}

//���ر任
gp_GTrsf		FFDShearTool::ShearTrsf()
{
	//�任����:ת��UCS->ת��ֲ�����ϵ->SHEAR�任->ת�ؾֲ�����ϵ->ת��WCS
	gp_Trsf wtou,utow,utol,ltou;
	gp_GTrsf shtsf;
	wtou.SetTransformation(gp_Ax3(m_ucsAx));
	utow = wtou.Inverted();

	utol.SetTransformation(gp_Ax3(m_ucsAx),gp_Ax3(m_locAx));
	ltou = utol.Inverted();

	shtsf.SetValue(1,2,tan(m_drad));
	
	m_shearTrsf = gp_GTrsf(utow);
	m_shearTrsf.Multiply(gp_GTrsf(ltou));
	m_shearTrsf.Multiply(shtsf);
	m_shearTrsf.Multiply(gp_GTrsf(utol));
	m_shearTrsf.Multiply(gp_GTrsf(wtou));//�ȼ����±任:utow*ltou*shtsf*utol*wtou*point or vector

	return m_shearTrsf;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// Ťת����
FFDTwistTool::FFDTwistTool()
{
	m_bValid = FALSE;
	m_bExtend = FALSE;
}

FFDTwistTool::~FFDTwistTool()
{

}

//�����Ǿֲ�����ϵ,Ĭ��ΪWCS�ռ�
BOOL	FFDTwistTool::Init(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Vec& baseDir)
{
	if(pnt1.Distance(pnt2) < 1e-6 || baseDir.Magnitude() < 1e-6)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//ͨ�������,Ӧ����֤baseDir����ת�������.
	gp_Vec ax(pnt1,pnt2);
	if(ax.IsParallel(baseDir,0.001))//����ƽ��
	{
		ASSERT(FALSE);
		return FALSE;
	}
	double dval = ax.Dot(baseDir);
	if(fabs(dval) > 1e-10)
	{
		//���¼���
		gp_Vec T = ax.Crossed(baseDir);
		m_baseDir = T.Crossed(ax);
	}
	else
	{
		m_baseDir = baseDir;
	}
	m_baseDir.Normalize();

	m_axPnt1 = pnt1;
	m_axPnt2 = pnt2;
	m_axLen = pnt1.Distance(pnt2);
	m_axVec = ax;

	m_axLine = new Geom_Line(pnt1,gp_Dir(m_axVec));

	m_bValid = TRUE;

	return TRUE;
}

BOOL	FFDTwistTool::Rotate(double dRand,BOOL bExtend)
{
	m_bExtend = bExtend;
	m_dRotRad = dRand;

	//
	m_radSlope = dRand / m_axLen;

	return TRUE;
}

//�Ե������ת
//ʹ�õ�����������ת�㷨
BOOL	FFDTwistTool::RotatePoint(const gp_Pnt& pnt,gp_Pnt& newpnt)
{
	//ʹ��ʹ�ü���Ƕȵ�
	double t;
	if(!AxisProject(pnt,t))
		return FALSE;

	double dRad = RotRad(t);//��ת�Ƕ�
	gp_Trsf tsf;
	tsf.SetRotation(gp_Ax1(m_axPnt1,gp_Dir(m_axVec)),dRad);

	newpnt = pnt;
	newpnt.Transform(tsf);
	
	return TRUE;
}

//��ͶӰ��ֱ��,��ȡ����
BOOL	FFDTwistTool::AxisProject(const gp_Pnt& pnt,double& t)
{
	try{
		GeomAPI_ProjectPointOnCurve prj(pnt,m_axLine);
		if(prj.NbPoints() > 0)
		{
			t = prj.LowerDistanceParameter();
			return TRUE;
		}
	}catch(Standard_Failure){
		
	}

	DTRACE("\n axis project failed.");
	
	return FALSE;
}

//��ת�Ƕ�
double	FFDTwistTool::RotRad(double t)
{
	double drad = t * m_radSlope;

	if(!m_bExtend)
	{
		if(t < 0.0) drad = 0.0;
		else if(t > m_axLen) drad = m_dRotRad;
	}

	return drad;
}

//////////////////////////////////////////////////////////////////////////
// ����bend����.

FFDBendTool::FFDBendTool()
{
	m_bExtend = TRUE;
	m_bSymmetric = FALSE;
}

FFDBendTool::~FFDBendTool()
{
	
}

//��Ҫ�ĳ�ʼ��
BOOL	FFDBendTool::Init(const gp_Pnt& pnt1,const gp_Pnt& pnt2,
						  const gp_Pnt& pntFrm,const gp_Pnt& pntTo)
{
	double dLen = pnt1.Distance(pnt2);
	if(dLen < 1e-6)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_axPnt1 = pnt1;
	m_axPnt2 = pnt2;

	//�����׼����
	//����Ϭţ�Ĺ���,��׼����Ӧ����һ��ֱ��.�����Ǹ�.
	double ta1 = 0.0,ta2 = dLen,tf,tl;
	Handle(Geom_Line) aLin = new Geom_Line(pnt1,gp_Dir(gp_Vec(pnt1,pnt2)));
	//��ȡ����������ֱ���ϵĲ���.
	GeomAPI_ProjectPointOnCurve prj1(pntFrm,aLin);
	if(prj1.NbPoints() == 0)
		return FALSE;
	tf = prj1.LowerDistanceParameter();

	GeomAPI_ProjectPointOnCurve prj2(pntTo,aLin);
	if(prj2.NbPoints() == 0)
		return FALSE;
	tl = prj2.LowerDistanceParameter();

	if(tf > tl)
	{
		double dval = tf;tf = tl;tl = dval;
	}

	tf = (tf < ta1) ? tf : ta1;
	tl = (tl > ta2) ? tl : ta2;

	//�ʵ�����Χ,��֤���γɹ�.
	tf -= dLen * 0.1;
	tl += dLen * 0.1;

	m_preLen = ta1 - tf;
	m_afterLen = tl - ta2;

	m_datumCurve = new Geom_TrimmedCurve(aLin,tf,tl);

	return TRUE;
}

//����ͨ����͹Ǹɼ���Բ������,��������
BOOL	FFDBendTool::BuildCurveTool(const gp_Pnt& pnt,BOOL bExtend,BOOL bSymmetric)
{
	if(m_datumCurve.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	double dLen = pnt.Distance(m_axPnt1);
	if(dLen < 1e-6)
		return FALSE;

	//���ȼ���Բ����Բ�ĺͰ뾶.�����������:
	//ͨ������Բ���Ϻ�ͨ���㲻��Բ����.��Ҫ�ж�һ��.
	gp_Pnt cen;
	double radius;

	gp_Dir v1(gp_Vec(m_axPnt1,m_axPnt2)),v2(gp_Vec(m_axPnt1,pnt)),v3,vc;
	if(v1.IsParallel(v2,0.001))
	{
		return FALSE;
	}

	v3 = v1.Crossed(v2);
	vc = v3.Crossed(v1);//��ȡԲ�����ڵķ���
	
	double cosa = vc.Dot(v2);
	if(fabs(cosa) < 1e-6)
		return FALSE;
	radius = fabs(dLen * 0.5 / cosa);

	cen.SetX(m_axPnt1.X() + radius * vc.X());
	cen.SetY(m_axPnt1.Y() + radius * vc.Y());
	cen.SetZ(m_axPnt1.Z() + radius * vc.Z());

	Handle(Geom_Circle) aCirc;
	Handle(Geom_TrimmedCurve) aArc;
	double dang1 = 0.0,dang2;
	gp_Pnt pntArcBegin,pntArcEnd;
	double dEndLen = 0.0;//Բ��ʣ�ಿ��
	double dAxLen = m_axPnt1.Distance(m_axPnt2);

	if(bExtend)//�Ǹɿ�������,�Ƚϼ�
	{
		//����Բ��,��ͨ�����Ƿ���Բ����
		gp_Ax2 ax(cen,v3,vc.Reversed());
		aCirc = new Geom_Circle(ax,radius);
		//����Բ��
		try{
			GC_MakeArcOfCircle mac(aCirc->Circ(),m_axPnt1,pnt,Standard_True);
			if(!mac.IsDone())
				return FALSE;

			aArc = mac.Value();
			dang1 = aArc->FirstParameter();
			dang2 = aArc->LastParameter();
			pntArcBegin = m_axPnt1;
			pntArcEnd = pnt;
		}catch(Standard_Failure){
			return FALSE;
		}
	}
	else
	{//��֤Բ�����Ⱥ͹Ǹ�һ��,��С�ڹǸ�.
		//����Բ���ĽǶ�,��֤�Ǹɵĳ��Ⱥ�Բ������һ��.����Ҫ��֤��,����ͨ������Բ����,Բ�����һЩ,�Ȳ�����.
		double dang = dAxLen / radius;
		gp_Ax2 ax(cen,v3,vc.Reversed());
		aCirc = new Geom_Circle(ax,radius);

		//ͨ���������Բ����,���߲���Բ����.�����Բ����,��Բ����ĩ��Ӧ����ͨ����,������.
		GeomAPI_ProjectPointOnCurve ppc(pnt,aCirc);
		if(ppc.NbPoints() <= 0)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		double dt = ppc.LowerDistanceParameter();
		if(dt <= dang)
		{//��Բ����
			//����Բ������,�ֲ�����ϵ
			gp_Ax2 ax(cen,v3,vc.Reversed());
			aCirc = new Geom_Circle(ax,radius);
			aArc = new Geom_TrimmedCurve(aCirc,0.0,dt);//ע������
			dang1 = 0.0;
			dang2 = dt;
			//���ڿ���Բ������С�ڹǸ�,���³���Ҫ��ӵ��Ǹ���
			double dtlen = dt * radius;
			dEndLen = dAxLen - dtlen;
		}
		else
		{//��Բ����,����Բ��
			cen = CalcCircCenter(m_axPnt1,m_axPnt2,pnt,vc,v3);
			radius = cen.Distance(m_axPnt1);
			dang = m_axPnt1.Distance(m_axPnt2) / radius;
			//
			gp_Ax2 ax(cen,v3,vc.Reversed());
			aCirc = new Geom_Circle(ax,radius);
			aArc = new Geom_TrimmedCurve(aCirc,0.0,dang);//ע������
			dang1 = 0.0;
			dang2 = dang;
		}
		//��ȡβ��
		pntArcBegin = m_axPnt1;
		aCirc->D0(dang2,pntArcEnd);
	}
	
	Handle(Geom_TrimmedCurve) aArcCpy = aArc;
	if(bSymmetric)
	{
		aArcCpy = Handle(Geom_TrimmedCurve)::DownCast(aArc->Copy());
		//�������������?
		dang1 = dang1 - (dang2 - dang1);//����Բ��.
		GC_MakeArcOfCircle mac(aCirc->Circ(),dang1,dang2,Standard_True);
		if(!mac.IsDone())
			return FALSE;

		aArc = mac.Value();
		aArc->D0(dang1,pntArcBegin);
	}

	//������������ֱ��
	Handle(Geom_TrimmedCurve) aTC1,aTC2;

	//����ǰ������ֱ��
	//preֱ��
	if(bSymmetric)
	{
		gp_Pnt tmpt;
		gp_Vec dtang;
		aArc->D1(dang1,tmpt,dtang);
		Handle(Geom_Line) aLin1 = new Geom_Line(pntArcBegin,gp_Dir(dtang.Reversed()));
		aTC1 = new Geom_TrimmedCurve(aLin1,0.0,m_preLen + dEndLen);
	}
	else
	{
		gp_Dir ld1(gp_Vec(m_axPnt2,m_axPnt1));
		Handle(Geom_Line) aLin1 = new Geom_Line(pntArcBegin,ld1);
		aTC1 = new Geom_TrimmedCurve(aLin1,0.0,m_preLen);
	}

	//afterֱ��
	gp_Pnt tmpnt;
	gp_Vec dv;
	aArc->D1(dang2,tmpnt,dv);
	Handle(Geom_Line) aLin2 = new Geom_Line(pntArcEnd,gp_Dir(dv));
	aTC2 = new Geom_TrimmedCurve(aLin2,0.0,m_afterLen + dEndLen);

	//ƴ�Ӳ�ת����bspline����
	GeomConvert_CompCurveToBSplineCurve ccConv(aTC1);
	if(!ccConv.Add(aArc,0.0001))
	{
		ASSERT(FALSE);
		return FALSE;
	}
	if(!ccConv.Add(aTC2,0.0001))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_tarCurve = ccConv.BSplineCurve();

	//����arc����
	GeomConvert_CompCurveToBSplineCurve ccb(aArcCpy);
	ccb.Add(aTC2,0.0001);
	m_arcCurve = ccb.BSplineCurve();

	m_bExtend = bExtend;
	m_bSymmetric = bSymmetric;
	
	m_bValid = TRUE;

	return TRUE;
}

//
// ���ݹǸɺ�ͨ�������Բ��Բ��.Բ������Ҫ�͹Ǹɵĳ���һ��.
// ˼·:�趨�뾶r,����r����Բ��,Բ�ͳ���һ�µĵ�pt,��Բ��cenͶӰ��paspnt��pt��ֱ����,�������Ϊr,���ҵ���
// ���������Һ��ʵ�r.rʹ���۰���ҷ�,���������̫Сʱ,��Сr,������̫����,������r.
gp_Pnt	FFDBendTool::CalcCircCenter(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Pnt& paspnt,
									const gp_Dir& cdir,const gp_Dir& N)
{
	gp_Pnt cen;
	double dlen = pnt1.Distance(pnt2);
	double r1 = dlen/(2.0 * M_PI),r2 = paspnt.Distance(pnt1),r;//r��󲻻���ͨ���㵽�Ǹ����ĳ���,
	                                                                      //��С����С���ԹǸɳ���ΪԲ�ܵ�Բ.
	double dmax = r2,dmin = r1;

	Handle(Geom_Circle) aCirc;
	gp_Pnt ptang;
	BOOL bDone = FALSE;
	int nC = 0;
	while(!bDone)
	{
		if(fabs(r1 - r2) < 0.001)
		{
			if(fabs(r1 - dmax) < 0.001)//���ܰ뾶�ܴ�,�����˷�Χ.�ӱ�
			{
				r2 = dmax * 2;
				dmax = r2;
			}
			if(fabs(r2 - dmin) < 0.001)
			{
				r1 = dmin/2.0;
				dmin = r1;
			}
		}
		r = (r1 + r2)/2.0;
		//����Բ��
		cen.SetX(pnt1.X() + r * cdir.X());
		cen.SetY(pnt1.Y() + r * cdir.Y());
		cen.SetZ(pnt1.Z() + r * cdir.Z());
		//����԰
		gp_Ax2 ax(cen,N,cdir.Reversed());
		aCirc = new Geom_Circle(ax,r);
		//����͹Ǹɵȳ���Բ���յ�
		double dtang = dlen / r;
		gp_Vec dv;
		aCirc->D1(dtang,ptang,dv);
		//�жϽǶ�
		gp_Vec vp(ptang,paspnt);
		double dangle = vp.AngleWithRef(dv,N);
		if(fabs(dangle) < 0.001)
		{
			bDone = TRUE;
		}
		else if(dangle < 0)//�뾶̫��
		{
			r2 = r;
		}
		else//�뾶̫С
		{
			r1 = r;
		}
		nC ++;
	}

	return cen;
}

//////////////////////////////////////////////////////////////////////////
// ׶�λ�����

FFDTaperTool::FFDTaperTool()
{
	m_bExtend = TRUE;
	m_bBoth	  = TRUE;
}

FFDTaperTool::~FFDTaperTool()
{

}

//��ʼ��.����Ǹɵ�������,������ʼ�ķ���ͳ���.bBoth��ʾ������������һ������׶�λ�.
BOOL	FFDTaperTool::Init(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Vec& beginVec,
						   BOOL bExtend,BOOL bBoth)
{
	if(pnt1.Distance(pnt2) < 1e-6 || beginVec.Magnitude() < 1e-6)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	gp_Vec vec(pnt1,pnt2);
	if(fabs(vec.Dot(beginVec)) > 1e-6)//Ҫ��֤�͹Ǹɴ�ֱ
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_axPnt1 = pnt1;
	m_axPnt2 = pnt2;
	m_dBeginLen = beginVec.Magnitude();

	m_locAx = gp_Ax2(pnt1,gp_Dir(gp_Vec(pnt1,pnt2)),gp_Dir(beginVec));

	m_wtolTsf.SetTransformation(gp_Ax3(m_locAx));
	m_ltowTsf = m_wtolTsf.Inverted();

	m_bExtend = bExtend;
	m_bBoth   = bBoth;

	return TRUE;
}

//������һ������ĳ���,ʵ��׶�λ�.
BOOL	FFDTaperTool::SetEndLen(double dlen)
{
	if(dlen < 0.0)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_dEndLen = dlen;

	//�������ϵ��

	m_bValid = TRUE;

	return TRUE;
}

//
// ��һ������б任.�Ƚ���任���ֲ�����ϵ,���ɱ任����,�任��,�ٱ任��ԭ��������ϵ��.
//
BOOL	FFDTaperTool::TrsfPoint(const gp_Pnt& pnt,gp_Pnt& newpnt)
{
	ASSERT(IsValid());

	newpnt = pnt;
	newpnt.Transform(m_wtolTsf);

	//����taper�任

	newpnt.Transform(m_ltowTsf);
	return TRUE;
}
