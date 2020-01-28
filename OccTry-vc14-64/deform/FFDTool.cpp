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
//曲面变形工具，保存基准曲面和变形曲面的信息。
// 先考虑整体变形，局部后续考虑
// 坐标系还需要考虑在内
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
	//要避免太接近
	if(p1.Distance(p2) < Precision::Confusion())
		return FALSE;

	//避免和坐标轴平行
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

	//转为nurbs曲面
	//转换为bsp曲面
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

	//升阶和插入控制点
	int ud = m_datumSurf->UDegree(),vd = m_datumSurf->VDegree();
	if(ud < nUDeg || vd < nVDeg)
	{
		if(ud < nUDeg) ud = nUDeg;
		if(vd < nVDeg) vd = nVDeg;
		m_datumSurf->IncreaseDegree(ud,vd);
		DTRACE("\n Increase Degree.");
	}

	//增加控制点
	TcBSplLib::IncSurfCtrlPnts(m_datumSurf,nUCtrlPnts,nVCtrlPnts);

	//记录平面信息
	m_bPlane = TRUE;
	m_aPlane = new Geom_Plane(pln);
	m_u1 = u1;m_v1 = v1;
	m_u2 = u2;m_v2 = v2;

	m_bValid = TRUE;

	return TRUE;
}

//初始化基准曲面
BOOL	FFDSurfTool::InitDatumSurf(const Handle(Geom_Surface)& pSurf)
{
	if(pSurf.IsNull())
		return FALSE;


	return FALSE;
}

//设置目标曲面
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

//下面两个方法适用于基于直线的FFD变形，原始直线为基准线，变形后的直线为目标曲线。
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
	//转为bspline
	Handle(Geom_BSplineCurve) aBspCur = GeomConvert::CurveToBSplineCurve(aTLin);
	if(aBspCur.IsNull())
		return FALSE;

	//进行必要的提升
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
	//简单的检查，看阶数和控制点数是否一致。
	if(aCur->Degree() != m_datumCurve->Degree() || aCur->NbPoles() != m_datumCurve->NbPoles())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_tarCurve = aCur;
	m_bChanged = TRUE;

	return TRUE;
}

//计算RMF
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

	//重新计算RMF
	//先计算目标曲线的。
	m_tarRmf.Clear();
	m_tarRmf.Init(m_tarCurve,m_tarCurve->FirstParameter(),m_tarCurve->LastParameter(),nSamps);
	if(!m_tarRmf.CalcSloanRMF() && !m_tarRmf.CalcRotateRMF())
	{
		DTRACE("\n calc rmf failed.");
		return FALSE;
	}

	//根据目标曲线计算基准曲线的rmf，注意：这里根据目前曲线的法矢计算基准曲线的初始法矢
	gp_Pnt pnt;
	gp_Vec tang;
	m_datumCurve->D1(m_datumCurve->FirstParameter(),pnt,tang);
	if(tang.Magnitude() < Precision::Confusion())
	{
		DTRACE("\n get datum curve first param tang failed.");
		return FALSE;//实际不会发生。
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

		//计算初始
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
// 基于曲线的变形工具。
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

//设置目标曲线
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

//计算RMF
BOOL	FFDCurveTool::CalcRMF(int nSamps,double datumT,double tarT,BOOL bStretch)
{
	if(m_datumCurve.IsNull() || m_tarCurve.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//参数对应关系
	double da = (datumT - m_df)/(m_dl - m_df);
	double db = (tarT - m_tf)/(m_tl - m_tf);
	if(da < 0.5 && db < 0.5) m_nMapType = eMap_PP; //正向-正向
	else if(da < 0.5 && db > 0.5) m_nMapType = eMap_PN;
	else if(da > 0.5 && db < 0.5) m_nMapType = eMap_NP;
	else 
		m_nMapType = eMap_NN;

	if(!bStretch)//两种不同的处理
	{//需要根据基准曲线的长度计算目标曲线的参数范围.
		//如果基准曲线比目标曲线短,则截断目标曲线,否则要延伸目标曲线,到制度长度,再进行流动.
		
	}

	//最简单的处理,先判断曲线是否直线,这可能是常用的情况
	if(m_datumCurve->IsKind(STANDARD_TYPE(Geom_Line)))
	{
		//重新计算RMF
		//先计算目标曲线的。
		m_tarRmf.Clear();
		m_tarRmf.Init(m_tarCurve,m_tf,m_tl,nSamps);
		if(!m_tarRmf.CalcSloanRMF() && !m_tarRmf.CalcRotateRMF())
		{
			DTRACE("\n calc rmf failed.");
			return FALSE;
		}

		//根据目标曲线计算基准曲线的rmf，注意：这里根据目前曲线的法矢计算基准曲线的初始法矢
		gp_Pnt pnt;
		gp_Vec tang;
		m_datumCurve->D1(m_df,pnt,tang);
		if(tang.Magnitude() < Precision::Confusion())
		{
			DTRACE("\n get datum curve first param tang failed.");
			return FALSE;//实际不会发生。
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

			//计算初始
			startN = B.Crossed(T);
			bGot = TRUE;
		}

		//TODO:这里,流动的基准点不同,则映射位置等都不同,可能需要反向处理等.

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
		//先计算目标曲线的。
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

	//记录映射比例
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
		//转为nurbs
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
		//转为nurbs
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
//倾斜工具
FFDShearTool::FFDShearTool()
{
	m_drad = 0.0;
}

FFDShearTool::~FFDShearTool()
{

}

//初始化,基准点和基准方向
BOOL	FFDShearTool::Init(const gp_Pnt& org,const gp_Vec& baseDir)
{
	//默认baseDir为y轴,z轴为缺省z轴,并且basedir在xy平面上.
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

//都是基于世界坐标系.
BOOL	FFDShearTool::Init(const gp_Pnt& org,const gp_Vec& baseDir,const gp_Ax2& ucs)
{
	return TRUE;
}

//倾斜方向
BOOL	FFDShearTool::Shear(const gp_Vec& newDir)
{
	ASSERT(m_bValid);
	//先转入到局部坐标系中,再计算和Y轴的方向
	gp_Dir dir(newDir);
	dir.Transform(m_tsf);

	//计算角度
	m_drad = dir.AngleWithRef(m_locAx.YDirection(),m_locAx.Direction());
	m_bValid = TRUE;

	return TRUE;
}

//倾斜角度(弧度)
BOOL	FFDShearTool::Shear(double drad)
{
	ASSERT(m_bValid);

	m_drad = drad;
	m_bValid = TRUE;

	return TRUE;
}

//对点进行变换
BOOL	FFDShearTool::TrsfPoint(const gp_Pnt& pnt,gp_Pnt& tsfpnt)
{
	//转入局部坐标系
	tsfpnt = pnt;

	if(fabs(m_drad) < 1e-10)
		return TRUE;

	tsfpnt.Transform(m_tsf);
	tsfpnt.SetX(tsfpnt.X() + tsfpnt.Y() * tan(m_drad));

	//转换原来坐标系
	tsfpnt.Transform(m_tsfr);

	return TRUE;
}

//返回变换
gp_GTrsf		FFDShearTool::ShearTrsf()
{
	//变换过程:转入UCS->转入局部坐标系->SHEAR变换->转回局部坐标系->转回WCS
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
	m_shearTrsf.Multiply(gp_GTrsf(wtou));//等价如下变换:utow*ltou*shtsf*utol*wtou*point or vector

	return m_shearTrsf;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// 扭转工具
FFDTwistTool::FFDTwistTool()
{
	m_bValid = FALSE;
	m_bExtend = FALSE;
}

FFDTwistTool::~FFDTwistTool()
{

}

//不考虑局部坐标系,默认为WCS空间
BOOL	FFDTwistTool::Init(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Vec& baseDir)
{
	if(pnt1.Distance(pnt2) < 1e-6 || baseDir.Magnitude() < 1e-6)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//通常情况下,应当保证baseDir和旋转轴的正交.
	gp_Vec ax(pnt1,pnt2);
	if(ax.IsParallel(baseDir,0.001))//避免平行
	{
		ASSERT(FALSE);
		return FALSE;
	}
	double dval = ax.Dot(baseDir);
	if(fabs(dval) > 1e-10)
	{
		//重新计算
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

//对点进行旋转
//使用点绕任意轴旋转算法
BOOL	FFDTwistTool::RotatePoint(const gp_Pnt& pnt,gp_Pnt& newpnt)
{
	//使用使用计算角度的
	double t;
	if(!AxisProject(pnt,t))
		return FALSE;

	double dRad = RotRad(t);//旋转角度
	gp_Trsf tsf;
	tsf.SetRotation(gp_Ax1(m_axPnt1,gp_Dir(m_axVec)),dRad);

	newpnt = pnt;
	newpnt.Transform(tsf);
	
	return TRUE;
}

//点投影到直线,获取参数
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

//旋转角度
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
// 弯曲bend工具.

FFDBendTool::FFDBendTool()
{
	m_bExtend = TRUE;
	m_bSymmetric = FALSE;
}

FFDBendTool::~FFDBendTool()
{
	
}

//必要的初始化
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

	//计算基准曲线
	//仿照犀牛的功能,基准曲线应当是一个直线.包含骨干.
	double ta1 = 0.0,ta2 = dLen,tf,tl;
	Handle(Geom_Line) aLin = new Geom_Line(pnt1,gp_Dir(gp_Vec(pnt1,pnt2)));
	//获取另两个点在直线上的参数.
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

	//适当增大范围,保证变形成功.
	tf -= dLen * 0.1;
	tl += dLen * 0.1;

	m_preLen = ta1 - tf;
	m_afterLen = tl - ta2;

	m_datumCurve = new Geom_TrimmedCurve(aLin,tf,tl);

	return TRUE;
}

//根据通过点和骨干计算圆弧曲线,进而计算
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

	//首先计算圆弧的圆心和半径.区分两种情况:
	//通过点在圆弧上和通过点不在圆弧上.需要判断一下.
	gp_Pnt cen;
	double radius;

	gp_Dir v1(gp_Vec(m_axPnt1,m_axPnt2)),v2(gp_Vec(m_axPnt1,pnt)),v3,vc;
	if(v1.IsParallel(v2,0.001))
	{
		return FALSE;
	}

	v3 = v1.Crossed(v2);
	vc = v3.Crossed(v1);//获取圆心所在的方向
	
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
	double dEndLen = 0.0;//圆弧剩余部分
	double dAxLen = m_axPnt1.Distance(m_axPnt2);

	if(bExtend)//骨干可以延伸,比较简单
	{
		//构造圆弧,看通过点是否在圆弧上
		gp_Ax2 ax(cen,v3,vc.Reversed());
		aCirc = new Geom_Circle(ax,radius);
		//创建圆弧
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
	{//保证圆弧长度和骨干一致,或小于骨干.
		//计算圆弧的角度,保证骨干的长度和圆弧长度一致.这是要保证的,可能通过点在圆弧上,圆弧会短一些,先不考虑.
		double dang = dAxLen / radius;
		gp_Ax2 ax(cen,v3,vc.Reversed());
		aCirc = new Geom_Circle(ax,radius);

		//通过点或者在圆弧上,或者不在圆弧上.如果在圆弧上,则圆弧的末端应当是通过点,否则不是.
		GeomAPI_ProjectPointOnCurve ppc(pnt,aCirc);
		if(ppc.NbPoints() <= 0)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		double dt = ppc.LowerDistanceParameter();
		if(dt <= dang)
		{//在圆弧上
			//创建圆弧对象,局部坐标系
			gp_Ax2 ax(cen,v3,vc.Reversed());
			aCirc = new Geom_Circle(ax,radius);
			aArc = new Geom_TrimmedCurve(aCirc,0.0,dt);//注意这里
			dang1 = 0.0;
			dang2 = dt;
			//由于可能圆弧长度小于骨干,余下长度要添加到骨干上
			double dtlen = dt * radius;
			dEndLen = dAxLen - dtlen;
		}
		else
		{//在圆弧外,计算圆心
			cen = CalcCircCenter(m_axPnt1,m_axPnt2,pnt,vc,v3);
			radius = cen.Distance(m_axPnt1);
			dang = m_axPnt1.Distance(m_axPnt2) / radius;
			//
			gp_Ax2 ax(cen,v3,vc.Reversed());
			aCirc = new Geom_Circle(ax,radius);
			aArc = new Geom_TrimmedCurve(aCirc,0.0,dang);//注意这里
			dang1 = 0.0;
			dang2 = dang;
		}
		//获取尾端
		pntArcBegin = m_axPnt1;
		aCirc->D0(dang2,pntArcEnd);
	}
	
	Handle(Geom_TrimmedCurve) aArcCpy = aArc;
	if(bSymmetric)
	{
		aArcCpy = Handle(Geom_TrimmedCurve)::DownCast(aArc->Copy());
		//镜像或其他方法?
		dang1 = dang1 - (dang2 - dang1);//扩充圆弧.
		GC_MakeArcOfCircle mac(aCirc->Circ(),dang1,dang2,Standard_True);
		if(!mac.IsDone())
			return FALSE;

		aArc = mac.Value();
		aArc->D0(dang1,pntArcBegin);
	}

	//构造其他两端直线
	Handle(Geom_TrimmedCurve) aTC1,aTC2;

	//构造前后两条直线
	//pre直线
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

	//after直线
	gp_Pnt tmpnt;
	gp_Vec dv;
	aArc->D1(dang2,tmpnt,dv);
	Handle(Geom_Line) aLin2 = new Geom_Line(pntArcEnd,gp_Dir(dv));
	aTC2 = new Geom_TrimmedCurve(aLin2,0.0,m_afterLen + dEndLen);

	//拼接并转换成bspline曲线
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

	//构造arc曲线
	GeomConvert_CompCurveToBSplineCurve ccb(aArcCpy);
	ccb.Add(aTC2,0.0001);
	m_arcCurve = ccb.BSplineCurve();

	m_bExtend = bExtend;
	m_bSymmetric = bSymmetric;
	
	m_bValid = TRUE;

	return TRUE;
}

//
// 根据骨干和通过点计算圆弧圆心.圆弧长度要和骨干的长度一致.
// 思路:设定半径r,根据r计算圆心,圆和长度一致的点pt,将圆心cen投影到paspnt和pt的直线上,如果距离为r,就找到了
// 否则重新找合适的r.r使用折半查找法,当计算距离太小时,减小r,当距离太大是,则增大r.
gp_Pnt	FFDBendTool::CalcCircCenter(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Pnt& paspnt,
									const gp_Dir& cdir,const gp_Dir& N)
{
	gp_Pnt cen;
	double dlen = pnt1.Distance(pnt2);
	double r1 = dlen/(2.0 * M_PI),r2 = paspnt.Distance(pnt1),r;//r最大不会大过通过点到骨干起点的长度,
	                                                                      //最小不会小过以骨干长度为圆周的圆.
	double dmax = r2,dmin = r1;

	Handle(Geom_Circle) aCirc;
	gp_Pnt ptang;
	BOOL bDone = FALSE;
	int nC = 0;
	while(!bDone)
	{
		if(fabs(r1 - r2) < 0.001)
		{
			if(fabs(r1 - dmax) < 0.001)//可能半径很大,超出了范围.加倍
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
		//计算圆心
		cen.SetX(pnt1.X() + r * cdir.X());
		cen.SetY(pnt1.Y() + r * cdir.Y());
		cen.SetZ(pnt1.Z() + r * cdir.Z());
		//计算园
		gp_Ax2 ax(cen,N,cdir.Reversed());
		aCirc = new Geom_Circle(ax,r);
		//计算和骨干等长的圆弧终点
		double dtang = dlen / r;
		gp_Vec dv;
		aCirc->D1(dtang,ptang,dv);
		//判断角度
		gp_Vec vp(ptang,paspnt);
		double dangle = vp.AngleWithRef(dv,N);
		if(fabs(dangle) < 0.001)
		{
			bDone = TRUE;
		}
		else if(dangle < 0)//半径太大
		{
			r2 = r;
		}
		else//半径太小
		{
			r1 = r;
		}
		nC ++;
	}

	return cen;
}

//////////////////////////////////////////////////////////////////////////
// 锥形化工具

FFDTaperTool::FFDTaperTool()
{
	m_bExtend = TRUE;
	m_bBoth	  = TRUE;
}

FFDTaperTool::~FFDTaperTool()
{

}

//初始化.传入骨干的两个点,还有起始的方向和长度.bBoth表示是两个方向还是一个方向锥形化.
BOOL	FFDTaperTool::Init(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Vec& beginVec,
						   BOOL bExtend,BOOL bBoth)
{
	if(pnt1.Distance(pnt2) < 1e-6 || beginVec.Magnitude() < 1e-6)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	gp_Vec vec(pnt1,pnt2);
	if(fabs(vec.Dot(beginVec)) > 1e-6)//要保证和骨干垂直
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

//更新另一个方向的长度,实现锥形化.
BOOL	FFDTaperTool::SetEndLen(double dlen)
{
	if(dlen < 0.0)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_dEndLen = dlen;

	//计算变形系数

	m_bValid = TRUE;

	return TRUE;
}

//
// 对一个点进行变换.先将点变换到局部坐标系,生成变换矩阵,变换点,再变换到原来的坐标系中.
//
BOOL	FFDTaperTool::TrsfPoint(const gp_Pnt& pnt,gp_Pnt& newpnt)
{
	ASSERT(IsValid());

	newpnt = pnt;
	newpnt.Transform(m_wtolTsf);

	//进行taper变换

	newpnt.Transform(m_ltowTsf);
	return TRUE;
}
