#include "StdAfx.h"
#include "FFDObj.h"
#include "FFDTool.h"
#include ".\ffdalgo.h"

FFDAlgo::FFDAlgo(void)
{
	m_bValid = FALSE;
	m_bDone = FALSE;
}

FFDAlgo::~FFDAlgo(void)
{
}

//////////////////////////////////////////////////////////////////////////
//
FFDSurfToolMap::FFDSurfToolMap()
{
	m_nus = m_nvs = 0;
	m_bInit = FALSE;
	m_pSurfToolMap = NULL;
}

FFDSurfToolMap::~FFDSurfToolMap()
{
	if(m_pSurfToolMap)
	{
		delete[] m_pSurfToolMap;
		m_pSurfToolMap = NULL;
	}
}

BOOL	FFDSurfToolMap::Init(int nus,int nvs)
{
	m_nus = nus;
	m_nvs = nvs;
	int ntol = nus * nvs;
	m_pSurfToolMap = new FFDSurfToolMapItem[ntol];
	if(!m_pSurfToolMap)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_bInit = TRUE;

	return TRUE;
}

//��������ͶӰ������������
BOOL	FFDSurfToolMap::MapToDatumSurf(FFDSurfTool *pTool)
{
	//�����Ƶ�ͶӰ������������
	Handle(Geom_BSplineSurface) aSurf = pTool->m_datumSurf;//��׼����
	gp_Pnt pnt,prjp;
	double dst,u,v;
	int iu,iv,idx;

	for(iu = 1;iu <= m_aSurf->NbUPoles();iu ++)
	{
		for(iv = 1;iv <= m_aSurf->NbVPoles();iv ++)
		{
			pnt = m_aSurf->Pole(iu,iv);
			//ͶӰ����������
			GeomAPI_ProjectPointOnSurf pprj(pnt,aSurf);
			if(!pprj.IsDone() || pprj.NbPoints() == 0)
			{
				DTRACE("\n map to datum surf failed");
				return FALSE;
			}
			dst = pprj.LowerDistance();
			pprj.LowerDistanceParameters(u,v);

			idx = IndexOf(iu,iv);
			m_pSurfToolMap[idx].m_dist = dst;
			m_pSurfToolMap[idx].m_u = u;
			m_pSurfToolMap[idx].m_v = v;

			//��֤һ�£���һ��ֱ��ӳ�䵽ƽ��Ĳ����Ƕ���

		}
	}

	return TRUE;
}

void	FFDSurfToolMap::SetDist(int iu,int iv,double dist)
{
	ASSERT(m_bInit);
	int idx = IndexOf(iu,iv);
	m_pSurfToolMap[idx].m_dist = dist;
}

double	FFDSurfToolMap::GetDist(int iu,int iv)
{
	ASSERT(m_bInit);
	int idx = IndexOf(iu,iv);
	return m_pSurfToolMap[idx].m_dist;
}

void	FFDSurfToolMap::SetPrjUV(int iu,int iv,double u,double v)
{
	ASSERT(m_bInit);
	int idx = IndexOf(iu,iv);
	m_pSurfToolMap[idx].m_u = u;
	m_pSurfToolMap[idx].m_v = v;
}

gp_Pnt2d	FFDSurfToolMap::GetPrjUV(int iu,int iv)
{
	ASSERT(m_bInit);
	int idx = IndexOf(iu,iv);
	gp_Pnt2d uv(m_pSurfToolMap[idx].m_u,m_pSurfToolMap[idx].m_v);
	return uv;
}

int		FFDSurfToolMap::IndexOf(int iu,int iv)
{
	ASSERT(m_bInit);
	ASSERT((iu >= 1) && (iu <= m_nus) && (iv > 0) && (iv <= m_nvs));
	return (iu - 1) * m_nvs + iv - 1;
}


//��������ı���

FFDSurfAlgo::FFDSurfAlgo()
{

}

FFDSurfAlgo::~FFDSurfAlgo()
{

}

//�㷨��ʼ��,���б�Ҫ�ĳ�ʼ������Ҫ������͹��߽��й����ȡ�
BOOL	FFDSurfAlgo::Init(FFDSurfObj *pObj,FFDSurfTool *pTool)
{
	ASSERT(pObj && pTool);
	if(!pObj->IsValid() || !pTool->IsValid())
		return FALSE;

	//����������ͱ��ι��������
	TopoDS_Shape aShape = pObj->NurbsShape();
	Handle(Geom_BSplineSurface) aDatumSurf = pTool->m_datumSurf;//��׼����

	TopExp_Explorer ex;
	for(ex.Init(aShape,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(ex.Current());

		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
		if(aSurf.IsNull())
			continue;

		Handle(Geom_BSplineSurface) aBspSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
		if(aBspSurf.IsNull())
			continue;

		int nucpts = aBspSurf->NbUPoles(),nvcpts = aBspSurf->NbVPoles();
		FFDSurfData *pSurfData = new FFDSurfData;
		pSurfData->m_aFace = aFace;
		pSurfData->m_aMap.m_aSurf = aBspSurf;
		if(!pSurfData->m_aMap.Init(nucpts,nvcpts) ||
			!pSurfData->m_aMap.MapToDatumSurf(pTool))
		{
			delete pSurfData;
			continue;
		}

		m_surfDatas.push_back(pSurfData);
	}

	m_pObj = pObj;
	m_pTool = pTool;

	m_bValid = TRUE;

	return TRUE;
}

//
// ���ñ��ι��߽��б���
//
BOOL	FFDSurfAlgo::Deform()
{
	ASSERT(m_bValid);
	Handle(Geom_Surface) aTarSurf = m_pTool->m_tarSurf;
	Handle(Geom_BSplineSurface) aBspSurf;
	if(aTarSurf.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	//����face�����б���
	gp_Pnt2d uv;
	gp_Pnt pnt,newpt;
	gp_Vec d1u,d1v,N;
	double dst;
	UINT uix;
	for(uix = 0;uix < m_surfDatas.size();uix ++)
	{
		FFDSurfData *pData = m_surfDatas.at(uix);
		//���б���,���ɿ��������¿���������ԭ����Ϣ��
		aBspSurf = Handle(Geom_BSplineSurface)::DownCast(pData->m_aMap.m_aSurf->Copy());
		for(int iu = 1;iu <= aBspSurf->NbUPoles();iu ++)
		{
			for(int iv = 1;iv <= aBspSurf->NbVPoles();iv ++)
			{
				//��ȡ��Ҫ��Ϣ
				dst = pData->m_aMap.GetDist(iu,iv);
				uv = pData->m_aMap.GetPrjUV(iu,iv);

				//����������
				aTarSurf->D1(uv.X(),uv.Y(),pnt,d1u,d1v);
				N = d1u.Crossed(d1v);
				if(N.Magnitude() < 1e-10)
				{
					ASSERT(FALSE);
					return FALSE;
				}
				N.Normalize();
				newpt.SetX(pnt.X() + dst * N.X());
				newpt.SetY(pnt.Y() + dst * N.Y());
				newpt.SetZ(pnt.Z() + dst * N.Z());

				aBspSurf->SetPole(iu,iv,newpt);
			}
		}

		//�����������ԭ��face��Ϣ�������µ�face.
		TopoDS_Face aF = m_pObj->BuildNewFace(aBspSurf,pData->m_aFace);
		if(!aF.IsNull())
			BB.Add(aC,aF);
	}

	//���շ������
	m_aNewShape = m_pObj->SewNewFaces(aC);
	if(!m_aNewShape.IsNull())
	{
		m_bDone = TRUE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
FFDCurveToolMapItem::FFDCurveToolMapItem()
{
	m_t = 0.0;
}

//
//////////////////////////////////////////////////////////////////////////
//��¼ÿ�������������Ϣ
FFDCurveToolMap::FFDCurveToolMap()
{
	m_nus = m_nvs = 0;
	m_bInit = FALSE;
	m_pMapData = NULL;
}

FFDCurveToolMap::~FFDCurveToolMap()
{
	if(m_pMapData)
	{
		delete [] m_pMapData;
		m_pMapData = NULL;
	}
}

//��ʼ��
BOOL	FFDCurveToolMap::Init(const Handle(Geom_BSplineSurface)& aSurf)
{
	if(aSurf.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_nus = aSurf->NbUPoles();
	m_nvs = aSurf->NbVPoles();

	m_pMapData = new FFDCurveToolMapItem[m_nus * m_nvs];
	if(!m_pMapData)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_bInit = TRUE;

	return TRUE;
}

void	FFDCurveToolMap::SetParam(int iu,int iv,double t)
{
	ASSERT(m_bInit);
	int idx = IndexOf(iu,iv);
	m_pMapData[idx].m_t = t;
}

double	FFDCurveToolMap::GetParam(int iu,int iv)
{
	ASSERT(m_bInit);
	return m_pMapData[IndexOf(iu,iv)].m_t;
}

//ע��:iu��iv��1��ʼ
int		FFDCurveToolMap::IndexOf(int iu,int iv)
{
	ASSERT((iu >= 1) && (iu <= m_nus) && (iv >= 1) && (iv <= m_nvs));

	int idx = (iu - 1) * m_nvs + iv - 1;
	return idx;
}


//////////////////////////////////////////////////////////////////////////
//
//����ֱ�ߵı���

FFDLineAlgo::FFDLineAlgo()
{
	m_pObj = NULL;
	m_pTool = NULL;
}

FFDLineAlgo::~FFDLineAlgo()
{
	Clear();
}

//�㷨��ʼ��
BOOL	FFDLineAlgo::Init(FFDSurfObj *pObj,FFDLineTool *pTool)
{
	ASSERT(pObj && pTool);
	if(!pObj->IsValid() || !pTool->IsValid())
		return FALSE;

	//��ӦͶӰ����׼��,��ȡ������ͶӰ��
	//����������ݺͻ�׼���߹���,��ת��Ŀǰ�����趨��
	Handle(Geom_BSplineCurve) aDatumCur = pTool->m_datumCurve;
	TopoDS_Shape aShape = pObj->NurbsShape();
	TopExp_Explorer ex;
	for(ex.Init(aShape,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(ex.Current());

		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
		if(aSurf.IsNull())
			continue;

		Handle(Geom_BSplineSurface) aBspSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
		if(aBspSurf.IsNull())
			continue;

		FFDCurveSurfData *pData = new FFDCurveSurfData;
		pData->m_aFace = aFace;
		pData->m_aMap.Init(aBspSurf);

		//����ͶӰ��
		int iu,iv;
		gp_Pnt pnt,lcpnt,ptNear;
		for(iu = 1;iu <= aBspSurf->NbUPoles();iu ++)
		{
			for(iv = 1;iv <= aBspSurf->NbVPoles();iv ++)
			{
				gp_Pnt pnt = aBspSurf->Pole(iu,iv);

				//���㵽��׼���ߵ������Ĳ���
				GeomAPI_ProjectPointOnCurve pp(pnt,aDatumCur);
				if(pp.NbPoints() == 0)
				{
					Clear();
					DTRACE("\n project point on curve failed.");
					return FALSE;
				}

				double u = pp.LowerDistanceParameter();
				pData->m_aMap.SetParam(iu,iv,u);
			}
		}

		m_surfDatas.push_back(pData);
	}

	m_pObj = pObj;
	m_pTool = pTool;

	return TRUE;
}

void	FFDLineAlgo::Clear()
{
	UINT uix;
	for(uix = 0;uix < m_surfDatas.size();uix ++)
	{
		FFDCurveSurfData *pData = m_surfDatas.at(uix);
		if(pData)
			delete pData;
	}
	m_surfDatas.clear();
}

//���б���
BOOL	FFDLineAlgo::Deform()
{
	ASSERT(m_pObj && m_pTool);
	if(!m_pTool->CalcRMF(100))
	{
		DTRACE("\n calc tool rmf failed.");
		return FALSE;
	}

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	//����������ݺͻ�׼���߹���,��ת��Ŀǰ�����趨��
	Handle(Geom_Curve) aDatumCur = m_pTool->m_datumCurve;
	TopoDS_Shape aShape = m_pObj->NurbsShape();
	TopExp_Explorer ex;
	for(ex.Init(aShape,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(ex.Current());

		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
		if(aSurf.IsNull())
			continue;

		Handle(Geom_BSplineSurface) aBspSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
		if(aBspSurf.IsNull())
			continue;

		FFDCurveSurfData *pData = GetSurfData(aFace);
		if(!pData)
			continue;

		//����һ��,�Ա�ı�
		Handle(Geom_BSplineSurface) aDfmSurf = Handle(Geom_BSplineSurface)::DownCast(aBspSurf->Copy());
		int iu,iv;
		gp_Pnt pnt,lcpnt,ptNear;
		for(iu = 1;iu <= aBspSurf->NbUPoles();iu ++)
		{
			for(iv = 1;iv <= aBspSurf->NbVPoles();iv ++)
			{
				gp_Pnt pnt = aBspSurf->Pole(iu,iv);

				double t = pData->m_aMap.GetParam(iu,iv);
				
				//����ֲ�����ϵ�������ڲ��ľֲ�����
				if(!m_pTool->m_datumRmf.CalcLcsCoord(t,pnt,lcpnt))
				{
					Clear();
					DTRACE("\n datum rmf calc local coord failed.");
					return FALSE;
				}
				//�����������ָ����Ӧ��ϵ,����t��Ҫ���б任.

				//������Ŀ���������µ�����
				if(!m_pTool->m_tarRmf.CalcDfmCoord(t,lcpnt,pnt))
				{
					Clear();
					DTRACE("\n tarcurve rmf calc local coord failed.");
					return FALSE;
				}

				aDfmSurf->SetPole(iu,iv,pnt);
			}
		}
		//�����������ԭ��face��Ϣ�������µ�face.
		TopoDS_Face aF = m_pObj->BuildNewFace(aDfmSurf,aFace);
		if(!aF.IsNull())
			BB.Add(aC,aF);
	}

	//���շ������
	m_aNewShape = m_pObj->SewNewFaces(aC);
	if(!m_aNewShape.IsNull())
	{
		m_bDone = TRUE;
	}

	return TRUE;
}

FFDCurveSurfData*	FFDLineAlgo::GetSurfData(const TopoDS_Face& aFace)
{
	FFDCurveSurfData *pData = NULL;
	UINT uix = 0;
	for(uix = 0;uix < m_surfDatas.size();uix ++)
	{
		FFDCurveSurfData *pTmpData = m_surfDatas.at(uix);
		if(aFace.IsEqual(pTmpData->m_aFace))
		{
			pData = pTmpData;
			break;
		}
	}

	return pData;
}

//////////////////////////////////////////////////////////////////////////
//�������ߵı���
FFDCurveAlgo::FFDCurveAlgo()
{

}

FFDCurveAlgo::~FFDCurveAlgo()
{

}

//�㷨��ʼ��
BOOL	FFDCurveAlgo::Init(FFDSurfObj *pObj,FFDCurveTool *pTool)
{
	ASSERT(pObj && pTool);
	if(!pObj->IsValid() || !pTool->IsValid())
		return FALSE;

	m_pObj = pObj;
	m_pTool = pTool;

	return TRUE;
}

//
// ���б���.����RMF,�Ƚ�����ͶӰ��RMF��,��ʹ�ñ��κ��RMF�����µĶ���.
//
BOOL	FFDCurveAlgo::Deform()
{
	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	//����������ݺͻ�׼���߹���,��ת��Ŀǰ�����趨��
	Handle(Geom_Curve) aDatumCur = m_pTool->m_datumCurve;
	TopoDS_Shape aShape = m_pObj->NurbsShape();
	TopExp_Explorer ex;
	for(ex.Init(aShape,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(ex.Current());

		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
		if(aSurf.IsNull())
			continue;

		Handle(Geom_BSplineSurface) aBspSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
		if(aBspSurf.IsNull())
			continue;

		//����һ��,�Ա�ı�
		Handle(Geom_BSplineSurface) aDfmSurf = Handle(Geom_BSplineSurface)::DownCast(aBspSurf->Copy());
		int iu,iv;
		gp_Pnt pnt,lcpnt,ptNear;
		for(iu = 1;iu <= aBspSurf->NbUPoles();iu ++)
		{
			for(iv = 1;iv <= aBspSurf->NbVPoles();iv ++)
			{
				gp_Pnt pnt = aBspSurf->Pole(iu,iv);

				//���㵽��׼���ߵ������Ĳ���
				GeomAPI_ProjectPointOnCurve pp(pnt,aDatumCur);
				if(pp.NbPoints() == 0)
				{
					DTRACE("\n project point on curve failed.");
					return FALSE;
				}

				double t = pp.LowerDistanceParameter();

				//����ֲ�����ϵ�������ڲ��ľֲ�����
				if(!m_pTool->m_datumRmf.CalcLcsCoord(t,pnt,lcpnt))
				{
					DTRACE("\n datum rmf calc local coord failed.");
					return FALSE;
				}
				//�����������ָ����Ӧ��ϵ,����t��Ҫ���б任.
				double u = m_pTool->MapParam(t);

				//������Ŀ���������µ�����
				if(!m_pTool->m_tarRmf.CalcDfmCoord(u,lcpnt,pnt))
				{
					DTRACE("\n tarcurve rmf calc local coord failed.");
					return FALSE;
				}

				aDfmSurf->SetPole(iu,iv,pnt);
			}
		}
		//�����������ԭ��face��Ϣ�������µ�face.
		TopoDS_Face aF = m_pObj->BuildNewFace(aDfmSurf,aFace);
		if(!aF.IsNull())
			BB.Add(aC,aF);
	}

	//���շ������
	m_aNewShape = m_pObj->SewNewFaces(aC);
	if(!m_aNewShape.IsNull())
	{
		m_bDone = TRUE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// ����������
FFDSurfFlowAlgo::FFDSurfFlowAlgo()
{
	m_pObj = NULL;
	m_pTool = NULL;
}

FFDSurfFlowAlgo::~FFDSurfFlowAlgo()
{

}

//�㷨��ʼ��
BOOL	FFDSurfFlowAlgo::Init(FFDSurfObj *pObj,FFDSurfFlowTool *pTool)
{
	ASSERT(pObj && pTool);
	if(!pObj->IsValid() || !pTool->IsValid())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_pObj = pObj;
	m_pTool = pTool;

	return TRUE;
}

//���б���
BOOL	FFDSurfFlowAlgo::Deform()
{
	ASSERT(m_pObj && m_pTool);

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	//����������ͱ��ι��������
	TopoDS_Shape aShape = m_pObj->NurbsShape();
	Handle(Geom_Surface) aDatumSurf = m_pTool->m_datumSurf;//��׼����

	gp_Pnt pnt,prjp,newpt;
	double dst,u,v;
	gp_Vec d1u,d1v,N;
	TopExp_Explorer ex;
	for(ex.Init(aShape,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(ex.Current());

		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
		if(aSurf.IsNull())
			continue;

		Handle(Geom_BSplineSurface) aBspSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
		if(aBspSurf.IsNull())
			continue;

		int nucpts = aBspSurf->NbUPoles(),nvcpts = aBspSurf->NbVPoles();
		Handle(Geom_BSplineSurface) aNewSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf->Copy());
		for(int iu = 1;iu <= nucpts;iu ++)
		{
			for(int iv = 1;iv <= nvcpts;iv ++)
			{
				//ͶӰ����׼ƽ��
				pnt = aBspSurf->Pole(iu,iv);
				//ͶӰ����������
				GeomAPI_ProjectPointOnSurf pprj(pnt,aDatumSurf);
				if(!pprj.IsDone() || pprj.NbPoints() == 0)
				{
					DTRACE("\n map to datum surf failed");
					return FALSE;
				}
				dst = pprj.LowerDistance();
				pprj.LowerDistanceParameters(u,v);

				//ӳ��ΪĿ�������Ӧ�ĵ�
				gp_Pnt2d uv;
				m_pTool->MapPoint(u,v,uv);
				//����������
				m_pTool->m_tarSurf->D1(uv.X(),uv.Y(),pnt,d1u,d1v);
				N = d1u.Crossed(d1v);
				if(N.Magnitude() < 1e-10)
				{
					ASSERT(FALSE);
					return FALSE;
				}
				N.Normalize();
				newpt.SetX(pnt.X() + dst * N.X());
				newpt.SetY(pnt.Y() + dst * N.Y());
				newpt.SetZ(pnt.Z() + dst * N.Z());

				aNewSurf->SetPole(iu,iv,newpt);
			}
		}
		
		//�����������ԭ��face��Ϣ�������µ�face.
		TopoDS_Face aF = m_pObj->BuildNewFace(aNewSurf,aFace);
		//TopoDS_Face aF = BRepBuilderAPI_MakeFace(aNewSurf);
		if(!aF.IsNull())
			BB.Add(aC,aF);
	}

	//m_aNewShape = aC;
	//m_bDone = TRUE;

	//���շ������
	m_aNewShape = m_pObj->SewNewFaces(aC);
	if(!m_aNewShape.IsNull())
	{
		m_bDone = TRUE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// shear��б����
FFDShearAlgo::FFDShearAlgo()
{
	m_pObj = NULL;
	m_pTool = NULL;
}

FFDShearAlgo::~FFDShearAlgo()
{

}

//�㷨��ʼ��
BOOL	FFDShearAlgo::Init(FFDSurfObj *pObj,FFDShearTool *pTool)
{
	ASSERT(pObj && pTool);
	if(!pObj->IsValid() || !pTool->IsValid())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_pObj = pObj;
	m_pTool = pTool;

	return TRUE;
}

//���б���
BOOL	FFDShearAlgo::Deform()
{
	ASSERT(m_pObj && m_pTool);

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	//��ÿ���������δ���
	TopoDS_Shape aShape = m_pObj->NurbsShape();

	gp_Pnt pnt,prjp,newpt;
	TopExp_Explorer ex;
	for(ex.Init(aShape,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(ex.Current());

		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
		if(aSurf.IsNull())
			continue;

		Handle(Geom_BSplineSurface) aBspSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
		if(aBspSurf.IsNull())
			continue;

		int nucpts = aBspSurf->NbUPoles(),nvcpts = aBspSurf->NbVPoles();
		Handle(Geom_BSplineSurface) aNewSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf->Copy());
		for(int iu = 1;iu <= nucpts;iu ++)
		{
			for(int iv = 1;iv <= nvcpts;iv ++)
			{
				//ͶӰ����׼ƽ��
				pnt = aBspSurf->Pole(iu,iv);
				//���б任
				m_pTool->TrsfPoint(pnt,newpt);
				
				aNewSurf->SetPole(iu,iv,newpt);
			}
		}

		//�����������ԭ��face��Ϣ�������µ�face.
		TopoDS_Face aF = m_pObj->BuildNewFace(aNewSurf,aFace);
		if(!aF.IsNull())
			BB.Add(aC,aF);
	}

	//���շ������
	m_aNewShape = m_pObj->SewNewFaces(aC);
	if(!m_aNewShape.IsNull())
	{
		m_bDone = TRUE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Ťת�����㷨
FFDTwistAlgo::FFDTwistAlgo()
{
	m_pObj = NULL;
	m_pTool = NULL;
}

FFDTwistAlgo::~FFDTwistAlgo()
{

}

//�㷨��ʼ��
BOOL	FFDTwistAlgo::Init(FFDSurfObj *pObj,FFDTwistTool *pTool)
{
	ASSERT(pObj && pTool);
	if(!pObj->IsValid() || !pTool->IsValid())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_pObj = pObj;
	m_pTool = pTool;

	return TRUE;
}

//���б���
BOOL	FFDTwistAlgo::Deform()
{
	ASSERT(m_pObj && m_pTool);

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	//��ÿ���������δ���
	TopoDS_Shape aShape = m_pObj->NurbsShape();

	gp_Pnt pnt,prjp,newpt;
	TopExp_Explorer ex;
	for(ex.Init(aShape,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(ex.Current());

		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
		if(aSurf.IsNull())
			continue;

		Handle(Geom_BSplineSurface) aBspSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
		if(aBspSurf.IsNull())
			continue;

		int nucpts = aBspSurf->NbUPoles(),nvcpts = aBspSurf->NbVPoles();
		Handle(Geom_BSplineSurface) aNewSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf->Copy());
		for(int iu = 1;iu <= nucpts;iu ++)
		{
			for(int iv = 1;iv <= nvcpts;iv ++)
			{
				//ͶӰ����׼ƽ��
				pnt = aBspSurf->Pole(iu,iv);
				//���б任
				m_pTool->RotatePoint(pnt,newpt);

				aNewSurf->SetPole(iu,iv,newpt);
			}
		}

		//�����������ԭ��face��Ϣ�������µ�face.
		TopoDS_Face aF = m_pObj->BuildNewFace(aNewSurf,aFace);
		if(!aF.IsNull())
			BB.Add(aC,aF);
	}

	//���շ������
	m_aNewShape = m_pObj->SewNewFaces(aC);
	if(!m_aNewShape.IsNull())
	{
		m_bDone = TRUE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ���������㷨,�õ�������������������
FFDBendAlgo::FFDBendAlgo()
{
	m_pObj = NULL;
	m_pTool = NULL;
}

FFDBendAlgo::~FFDBendAlgo()
{

}

//��ʼ��
BOOL	FFDBendAlgo::Init(FFDSurfObj *pObj,FFDBendTool *pTool)
{
	ASSERT(pObj && pTool);
	if(!pObj->IsValid())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_pObj = pObj;
	m_pTool = pTool;

	m_bValid = TRUE;

	return TRUE;
}

//���б���
BOOL	FFDBendAlgo::Deform()
{
	ASSERT(IsValid());

	//ʹ���������������߽��б���
	FFDCurveTool curTool;
	if(!curTool.SetDatumCurve(m_pTool->GetDatumCurve()) || 
		!curTool.SetTarCurve(m_pTool->GetTarCurve()))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if(!curTool.CalcRMF(40,m_pTool->GetDatumCurve()->FirstParameter(),m_pTool->GetTarCurve()->FirstParameter()))
	{
		AfxMessageBox("init Tool Failed.");
		return FALSE;
	}

	FFDCurveAlgo curAlgo;
	if(!curAlgo.Init(m_pObj,&curTool))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if(curAlgo.Deform() && curAlgo.IsDone())
	{
		m_aNewShape = curAlgo.Shape();
		m_bDone = TRUE;

		return TRUE;
	}

	return FALSE;
}
