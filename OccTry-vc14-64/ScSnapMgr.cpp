#include "StdAfx.h"

#include <TopExp.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
//#include <SelectBasics_ListOfBox2d.hxx>//yxk
//#include <SelectBasics_ListIteratorOfListOfBox2d.hxx>//yxk
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Shape.hxx>

#include "ScView.h"
#include "ScUcsMgr.h"
#include ".\scsnapmgr.h"

//////////////////////////////////////////////////////////////////////////
//
void	ScSnapPoint::Dump()
{
	DTRACE("\nsnap point: ");
	DTRACE("pnt[%f,%f,%f],wnd pos[%d,%d]",m_pnt.X(),m_pnt.Y(),m_pnt.Z(),m_nX,m_nY);
}

//////////////////////////////////////////////////////////////////////////
//
ScSnapMgr::ScSnapMgr(const Handle(AIS_InteractiveContext)& aCtx)
{
	m_AISContext = aCtx;
	this->m_bEnable = FALSE;

	this->m_nSnapSize = 5;
	this->m_nMarkerSize = 5;

	m_pScView = NULL;

	m_pMarkerPen = new CPen(PS_SOLID, 2, RGB(255,255,0));
}

ScSnapMgr::~ScSnapMgr(void)
{
}

//
// ʹ�ܺͽ�ֹ��׽
//
void	ScSnapMgr::Enable(BOOL bEnable)
{
	Clear();
	m_bEnable = bEnable;
	//���ʹ�ܣ����µ�
	if(m_bEnable)
	{
		UpdateSnapPoints();
	}
}

//���µ㣬���»�ȡ����Ϣ��
//������ʾ���󣬻�ȡ��Ӧ�ĵ���Ϣ������¼��
BOOL	ScSnapMgr::UpdateSnapPoints()
{
	//����ɵ�
	Clear();

	//��ȡ�µ�
	GetCSSnapPoint();

	//��ȡ��������
	CRect wndrc;
	ASSERT(m_pScView);
	m_pScView->m_pViewWnd->GetClientRect(&wndrc);

	//���ݴ���������������ҵ��ɼ�����
	AIS_ListOfInteractive dispObjs;
	m_AISContext->DisplayedObjects(dispObjs/*,Standard_True*/);//yxk20200104

	//�������ж���
	AIS_ListIteratorOfListOfInteractive ite;
	for(ite.Initialize(dispObjs);ite.More();ite.Next())
	{
		Handle(AIS_Shape) aSS = Handle(AIS_Shape)::DownCast(ite.Value());
		if(aSS.IsNull())
			continue;

		Handle(AIS_InteractiveObject) aObj = ite.Value();
		//��ȡ
	//yxk20200104
		//Handle(SelectMgr_Selection) aSel = aObj->Selection(aObj->SelectionMode());
		Handle(SelectMgr_Selection) aSel = aObj->Selection(aObj->GlobalSelectionMode());
	//endyxk
		if(aSel.IsNull())
			continue;
	//yxk20200104
	/*
		BOOL bVisible = FALSE;
		aSel->Init();
		while(aSel->More())
		{
			Handle(SelectBasics_SensitiveEntity) aSen = aSel->Sensitive()->BaseSensitive();;
			if(!aSen.IsNull())
			{
				SelectBasics_ListOfBox2d lstbox;//
				aSen->Areas(lstbox);
				//
				SelectBasics_ListIteratorOfListOfBox2d lite(lstbox);
				while(lite.More())
				{
					Bnd_Box2d bb = lite.Value();
					double x1,x2,y1,y2;
					bb.Get(x1,y1,x2,y2);
					//DTRACE("\n %f,%f,%f,%f",x1,y1,x2,y2);
					//����ƽ��תΪ��Ļ
					int l,b,r,t;
					m_pScView->m_aView->Convert(x1,y1,l,b);
					m_pScView->m_aView->Convert(x2,y2,r,t);
					//DTRACE("\n %d,%d,%d,%d",l,b,r,t);

					CRect rc(l,t,r,b),rcint;
					rc.NormalizeRect();
					if(rcint.IntersectRect(&wndrc,&rc))
					{
						bVisible = TRUE;
						break;
					}
					lite.Next();
				}
			}

			if(bVisible)
				break;

			aSel->Next();
		}

		if(!bVisible)
			continue;
*/
//endyxk
		TopoDS_Shape aS = aSS->Shape();
		if(aS.IsNull())
			continue;

		//��ȡ��Ӧ�ĵ�
		GetShapeSnapPoints(aS,wndrc);
	}

#ifdef _VERBOSE_DEBUG
	UINT ix = 0;
	DTRACE("\n CS Set:");
	for(ix = 0;ix < m_csSet.size();ix ++)
	{
		ScSnapPoint& spt = m_csSet.at(ix);
		spt.Dump();
	}

	DTRACE("\n tem Set:");
	for(ix = 0;ix < m_temSet.size();ix ++)
	{
		ScSnapPoint& spt = m_temSet.at(ix);
		spt.Dump();
	}

	DTRACE("\n cen Set:");
	for(ix = 0;ix < m_cenSet.size();ix ++)
	{
		ScSnapPoint& spt = m_cenSet.at(ix);
		spt.Dump();
	}
#endif

	return TRUE;
}

//��ȡcs��صĵ�
void	ScSnapMgr::GetCSSnapPoint()
{
	//ԭ��
	ScSnapPoint spt;
	spt.m_pnt.SetCoord(0,0,0);
	WCSToPixel(spt.m_pnt,spt.m_nX,spt.m_nY);
	m_csSet.push_back(spt);

	//UCS��ԭ��
	ScUcs *pScUcs = m_pScView->m_pScUcsMgr->CurrentUCS();
	if(pScUcs)
	{
		m_pScView->UCSToWCS(spt.m_pnt);
		WCSToPixel(spt.m_pnt,spt.m_nX,spt.m_nY);
		
		m_csSet.push_back(spt);
	}
}

//��ȡ����ĵ�
BOOL	ScSnapMgr::GetShapeSnapPoints(const TopoDS_Shape& aShape,CRect& rc)
{
	//��ȡ�˵�
	ScSnapPoint spt;
	//Ϊ�˱����ظ���ȡ�㣬����map
	TopTools_IndexedMapOfShape vms;
	TopExp::MapShapes(aShape,TopAbs_VERTEX,vms);
	int ix,nb = vms.Extent();
	for(ix = 1;ix <= nb;ix ++)
	{
		const TopoDS_Shape& aS = vms.FindKey(ix);
		TopoDS_Vertex aV = TopoDS::Vertex(aS);
		if(aV.IsNull())
			continue;

		spt.m_pnt = BRep_Tool::Pnt(aV);
		WCSToPixel(spt.m_pnt,spt.m_nX,spt.m_nY);
		//��������Ļ�⣬�����
		if(rc.PtInRect(CPoint(spt.m_nX,spt.m_nY)))
			m_temSet.push_back(spt);
	}

	//��ȡԲ�ĺ��е�
	TopTools_IndexedMapOfShape ems;
	TopExp::MapShapes(aShape,TopAbs_EDGE,ems);
	nb = ems.Extent();
	for(ix = 1;ix <= nb;ix ++)
	{
		const TopoDS_Shape& aS = ems.FindKey(ix);
		TopoDS_Edge aE = TopoDS::Edge(aS);
		double df,dl;
		Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE,df,dl);
		if(aCur.IsNull())
			continue;

		//Բ��
		if(aCur->IsKind(STANDARD_TYPE(Geom_Circle)))
		{
			Handle(Geom_Circle) aCirc = Handle(Geom_Circle)::DownCast(aCur);
			spt.m_pnt = aCirc->Location();
			WCSToPixel(spt.m_pnt,spt.m_nX,spt.m_nY);
			//��������Ļ�⣬�����
			if(rc.PtInRect(CPoint(spt.m_nX,spt.m_nY)))
				m_cenSet.push_back(spt);
		}
	}

	return TRUE;
}

BOOL	ScSnapMgr::WCSToPixel(const gp_Pnt& pnt,int& nx,int& ny)
{
	double x,y,z;
	pnt.Coord(x,y,z);
	m_pScView->m_aView->Convert(x,y,z,nx,ny);
	return TRUE;
}

//
// �Զ���׽
//
void	ScSnapMgr::Snap(int nX,int nY,const Handle(V3d_View)& aView)
{
	if(!m_bEnable)
		return;

	int nR = 5;
	CRect snrc(nX - m_nSnapSize,nY - m_nSnapSize,nX + m_nSnapSize,nY + m_nSnapSize);
	TSnapPointSet tmpSet;

	UINT ix;
	for(ix = 0;ix < m_csSet.size();ix ++)
	{
		ScSnapPoint& spt = m_csSet.at(ix);
		if(snrc.PtInRect(CPoint(spt.m_nX,spt.m_nY)))
		{
			tmpSet.push_back(spt);
		}
	}

	//
	for(ix = 0;ix < m_temSet.size();ix ++)
	{
		ScSnapPoint& spt = m_temSet.at(ix);
		if(snrc.PtInRect(CPoint(spt.m_nX,spt.m_nY)))
		{
			tmpSet.push_back(spt);
		}
	}

	for(ix = 0;ix < m_cenSet.size();ix ++)
	{
		ScSnapPoint& spt = m_cenSet.at(ix);
		if(snrc.PtInRect(CPoint(spt.m_nX,spt.m_nY)))
		{
			tmpSet.push_back(spt);
		}
	}

	if(tmpSet.size() == 0)
	{
		//����ɵ���ʾ
		ClearSnapMarker();
		//
		m_bHasPoint = FALSE;

		return;
	}

	ScSnapPoint newPoint;
	if(tmpSet.size() == 1)
	{
		newPoint = tmpSet.at(0);
	}
	else
	{
		//Ѱ�Ҿ�����̵�
		newPoint = tmpSet.at(0);
		int distmin = (nX - tmpSet[0].m_nX) * (nX - tmpSet[0].m_nX) + 
			(nY - tmpSet[0].m_nY) * (nY - tmpSet[0].m_nY),dist;
		for(ix = 0;ix < tmpSet.size();ix ++)
		{
			if(ix == 0) continue;

			ScSnapPoint& spt = tmpSet.at(ix);
			dist = (nX - spt.m_nX) * (nX - spt.m_nX) + (nY - spt.m_nY) * (nY - spt.m_nY);
			if(dist < distmin)
			{
				distmin = dist;
				newPoint = spt;
			}
		}
	}

	if(m_bHasPoint)//ǰ���в����,�ж�ǰ�󲶻��Ƿ�һ��
	{
		if(newPoint.m_nX == m_snapPoint.m_nX &&
			newPoint.m_nY == m_snapPoint.m_nY)
		{
			//ǰ����غ�,�ٻ���
			DrawSnapMarker();
		}
		else
		{
			//����ɵ���ʾ
			ClearSnapMarker();

			m_snapPoint = newPoint;
			//����
			DrawSnapMarker();
		}
	}
	else
	{
		//����ɵ���ʾ
		ClearSnapMarker();

		m_bHasPoint = TRUE;
		m_snapPoint = newPoint;

		//����
		DrawSnapMarker();
	}
}

//��ȡ����pnt����Ĳ�׽��
BOOL	ScSnapMgr::GetSnapPoint(const gp_Pnt& pnt,gp_Pnt& snapPnt,BOOL bCompare)
{
	//��ת����Ļ����
	ScSnapPoint spt,sptmin;
	spt.m_pnt = pnt;
	WCSToPixel(pnt,spt.m_nX,spt.m_nY);

	if(!bCompare)
	{
		if(m_bHasPoint && IsNearest(spt.m_nX,spt.m_nY,m_snapPoint.m_nX,m_snapPoint.m_nY))
			snapPnt = m_snapPoint.m_pnt;
		else
			snapPnt = pnt;//������㹻����û�в���㣬�򷵻�ԭֵ

		return TRUE;
	}
	
	int distmin = 10000000,dist;
	if(m_bHasPoint)
		distmin = PixelDist(spt,m_snapPoint);

	UINT ix = 0;
	for(ix = 0;ix < m_csSet.size();ix ++)
	{
		const ScSnapPoint& tspt = m_csSet.at(ix);
		dist = PixelDist(tspt,spt);
		if(dist < distmin)
		{
			sptmin = tspt;
			distmin = dist;
		}
	}

	for(ix = 0;ix < m_temSet.size();ix ++)
	{
		const ScSnapPoint& tspt = m_temSet.at(ix);
		dist = PixelDist(tspt,spt);
		if(dist < distmin)
		{
			sptmin = tspt;
			distmin = dist;
		}
	}

	for(ix = 0;ix < m_cenSet.size();ix ++)
	{
		const ScSnapPoint& tspt = m_cenSet.at(ix);
		dist = PixelDist(tspt,spt);
		if(dist < distmin)
		{
			sptmin = tspt;
			distmin = dist;
		}
	}

	if(IsNearest(spt.m_nX,spt.m_nY,sptmin.m_nX,sptmin.m_nY))
		snapPnt = sptmin.m_pnt;
	else
		snapPnt = pnt;

	return TRUE;
}

int		ScSnapMgr::PixelDist(const ScSnapPoint& p1,const ScSnapPoint& p2)
{
	int nDist = 0;
	nDist = (p1.m_nX - p2.m_nX) * (p1.m_nX - p2.m_nX) +
		(p1.m_nY - p2.m_nY) * (p1.m_nY - p2.m_nY);
	return nDist;
}

BOOL	ScSnapMgr::IsNearest(int x1,int y1,int x2,int y2)
{
	if(abs(x1 - x2) <= m_nSnapSize + 1 && abs(y1 - y2) <= m_nSnapSize + 1)
		return TRUE;

	return FALSE;
}

void	ScSnapMgr::Clear()
{
	ClearSnapMarker();

	m_csSet.clear();
	m_temSet.clear();
	m_cenSet.clear();

	m_bHasPoint = FALSE;
}

//���ǰ���Զ���׽���
void	ScSnapMgr::ClearSnapMarker()
{
	if(m_bHasPoint)
	{
		if(!m_aPntMarker.IsNull())
		{
			m_AISContext->Remove(m_aPntMarker, Standard_True);
			m_aPntMarker.Nullify();
		}
		//CRect rc(m_snapPoint.m_nX - m_nMarkerSize - 2,m_snapPoint.m_nY - m_nMarkerSize - 2,m_snapPoint.m_nX + m_nMarkerSize + 2,
		//	m_snapPoint.m_nY + m_nMarkerSize + 2);
		//m_pScView->m_pViewWnd->InvalidateRect(&rc,FALSE);//ˢ������
	}
}

//�����Զ���׽���
void	ScSnapMgr::DrawSnapMarker()
{
	if(!m_bHasPoint)
		return;

	Handle(Geom_Point) aPnt = new Geom_CartesianPoint(m_snapPoint.m_pnt);
	if(m_aPntMarker.IsNull())
	{
		m_aPntMarker = new AIS_Point(aPnt);
		m_aPntMarker->SetMarker(Aspect_TOM_O_POINT);
		m_aPntMarker->SetColor(Quantity_Color(Quantity_NOC_YELLOW));

		m_AISContext->Display(m_aPntMarker, Standard_True);
	}
	else
	{
		m_aPntMarker->SetComponent(aPnt);

		m_AISContext->Redisplay(m_aPntMarker, Standard_True);
	}
	

	//int nDrawMode = R2_MERGEPENNOT;
	//CPen* pOldPen = NULL;
	//CClientDC clientDC(m_pScView->m_pViewWnd);
	//ASSERT(m_pMarkerPen);
	//pOldPen = clientDC.SelectObject(m_pMarkerPen);
	//clientDC.SetROP2(nDrawMode);//������

	//clientDC.MoveTo(m_snapPoint.m_nX - m_nMarkerSize,m_snapPoint.m_nY - m_nMarkerSize);
	//clientDC.LineTo(m_snapPoint.m_nX - m_nMarkerSize,m_snapPoint.m_nY + m_nMarkerSize);
	//clientDC.LineTo(m_snapPoint.m_nX + m_nMarkerSize,m_snapPoint.m_nY + m_nMarkerSize);
	//clientDC.LineTo(m_snapPoint.m_nX + m_nMarkerSize,m_snapPoint.m_nY - m_nMarkerSize);
	//clientDC.LineTo(m_snapPoint.m_nX - m_nMarkerSize,m_snapPoint.m_nY - m_nMarkerSize);

	//clientDC.SelectObject(pOldPen);
}