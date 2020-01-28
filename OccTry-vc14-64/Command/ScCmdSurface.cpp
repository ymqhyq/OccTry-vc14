#include "StdAfx.h"
#include <TopExp.hxx>
#include <ElSLib.hxx>
#include <GeomLib.hxx>
#include <BrepTools.hxx>
#include "ScDlgFillingOptions.h"
#include "..\ScView.h"
#include ".\sccmdsurface.h"

ScCmdSurface::ScCmdSurface(void)
{
}

ScCmdSurface::~ScCmdSurface(void)
{
}

ScCmdClosedPolyLine::ScCmdClosedPolyLine()
{
}

ScCmdClosedPolyLine::~ScCmdClosedPolyLine()
{
}

//����ƶ�����Ӧ����
void	ScCmdClosedPolyLine::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case LINE_POINT_FIRST:
		{
			SnapPoint(point.x,point.y,aView);
		}
		break;
	case LINE_POINT_NEXT:
		{
			RemoveCtxObj(m_aAIShape);

			//SnapPoint(point.x,point.y,aView);
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_pntPrev) < Precision::Confusion())
				return;

			m_seqOfPnt.Append(pnt);
			TopoDS_Shape aS = BuildNewShape();
			Display(m_aAIShape,aS);

			m_seqOfPnt.Remove(m_seqOfPnt.Length());
		}
		break;
	default:
		break;
	}
}

TopoDS_Shape	ScCmdClosedPolyLine::BuildNewShape()
{
	TopoDS_Shape aS;
	if(m_seqOfPnt.Length() < 2)
		return aS;

	if(m_seqOfPnt.Length() == 2)
	{
		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_seqOfPnt.Value(1),m_seqOfPnt.Value(2));
		aS = aE;
	}
	else
	{
		//����face������ʾ
		BRepBuilderAPI_MakePolygon mp;
		for(int ix = 1;ix <= m_seqOfPnt.Length();ix++)
		{
			mp.Add(m_seqOfPnt.Value(ix));
		}
		mp.Close();

		TopoDS_Wire aW = mp.Wire();
		TopoDS_Face aF = BRepBuilderAPI_MakeFace(aW,TRUE);
		aS = aF;
	}

	return aS;
}

//////////////////////////////////////////////////////////////////
//

//�����ͽ�������
int		ScCmdSurfSewing::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SURFS,"ѡ�����ڽ����棬�Ҽ�����");
	NeedMultiSelect(TRUE);

	return 0;
}

int		ScCmdSurfSewing::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdSurfSewing::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		TopoDS_Shape aS = BuildNewShape();
		Display(m_aNewObj,aS);

		Done();
	}
	else
	{
	}
}


TopoDS_Shape	ScCmdSurfSewing::BuildNewShape()
{
	TopoDS_Shape aNewShape;
	TopTools_ListOfShape asList;

	m_AISContext->InitCurrent();
	while(m_AISContext->MoreCurrent())
	{
		Handle(AIS_InteractiveObject) aObj = m_AISContext->Current();
		Handle(AIS_Shape) asObj = Handle(AIS_Shape)::DownCast(aObj);
		if(asObj.IsNull())
			return aNewShape;

		TopoDS_Shape aShape = asObj->Shape();
		if((aShape.ShapeType() != TopAbs_FACE) && 
			(aShape.ShapeType() != TopAbs_SHELL) &&
			(aShape.ShapeType() != TopAbs_COMPOUND))
			return aNewShape;

		asList.Append(aShape);
		this->m_seqOfObj.Append(aObj);//��¼���Ա�ɾ��

		m_AISContext->NextCurrent();
	}

	int nLen = asList.Extent();
	if(nLen < 2)
		return aNewShape;

	BRepBuilderAPI_Sewing sew;
    
	TopTools_ListIteratorOfListOfShape it(asList);
	for(;it.More();it.Next())
	{
		TopoDS_Shape atS = it.Value();
		sew.Add(atS);
	}

	try{
		sew.Perform();
		aNewShape = sew.SewedShape();
	}
	catch(Standard_Failure)
	{
		AfxMessageBox("���ʧ��.");
		return aNewShape;
	}

	if(!aNewShape.IsNull())
	{//ɾ���ɵ�����
		int nC = m_seqOfObj.Length();
		for(int ic = 1;ic <= nC;ic++)
		{
			m_AISContext->Remove(m_seqOfObj.Value(ic), Standard_True);
		}
	}

	return aNewShape;
}

/////////////////////////////////////////////////////////////////////////// 
// ����filling���ܡ�
//
ScCmdSurfFilling::ScCmdSurfFilling(int nType)
{
	ASSERT((nType >= eBoundary) && (nType <= eWithFace));
	this->m_nFillType = nType;
	this->m_pApiFill = NULL;
}

//�����ͽ�������
int	ScCmdSurfFilling::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SURF,"ѡ��һ�����߻�һ������,�Ҽ�����:");
	NeedSelect(TRUE);

	this->m_pApiFill = new BRepOffsetAPI_MakeFilling(3,10,2);
	this->m_nEdges = 0;

	return 0;
}

int	ScCmdSurfFilling::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdSurfFilling::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		switch(GetState())
		{
		case S_SURF:
			{
				if(m_nEdges == 0)//û�����ߣ������졣
				{
					Done();
					return;
				}

				if(this->m_nFillType == eBoundary)//�����߽�Լ����
				{
					TopoDS_Shape aS = BuildNewShape();
					Display(NewObj(0),aS);
					Done();
					return;
				}

				if(this->m_nFillType == eWithPoints)
				{
					SwitchState(S_POINTS,"ѡ��һ������Լ���㣬�Ҽ�����.");
					NeedSelect(TRUE);
					NeedMultiSelect(FALSE);
				}
				else if(this->m_nFillType == eWithCurves)
				{
					SwitchState(S_CURVES,"ѡ��һ�������Լ�����ߣ��Ҽ�����.");
				}
				else// this->m_nFillType == eWithPoints)
				{
				}
			}
			break;
		case S_EDGE:
			{
				TopTools_IndexedDataMapOfShapeListOfShape M;
				if(!m_aShape.IsNull())
				{
					TopExp::MapShapesAndAncestors(m_aShape, TopAbs_EDGE, TopAbs_FACE, M);//�ݴ��ҳ�edge��������face.
				}
				m_AISContext->InitSelected();
				while(m_AISContext->MoreSelected())
				{
					TopoDS_Edge aE = TopoDS::Edge(m_AISContext->SelectedShape());
					if(!m_aFace.IsNull())
					{
						m_pApiFill->Add(aE,m_aFace,GeomAbs_G1);
						m_nEdges ++;
					}
					else
					{
						TopoDS_Face aF = TopoDS::Face(M.FindFromKey(aE).First());
						if(!aF.IsNull())
						{
							m_pApiFill->Add(aE,aF,GeomAbs_G1);
							m_nEdges ++;
						}
					}

					m_AISContext->NextSelected();
				}
				//
				//m_AISContext->CloseLocalContex();//yxk20191207
				NeedMultiSelect(FALSE);
				NeedSelect(TRUE);

				SwitchState(S_SURF,"ѡ��һ�����߻�һ������:");
			}
			break;
		case S_POINTS:
			{
				TopoDS_Shape aS = BuildNewShape();
				Display(NewObj(0),aS);
				Done();
			}
			break;
		case S_CURVES:
			{
				TopoDS_Shape aS = BuildNewShape();
				Display(NewObj(0),aS);
				Done();
			}
			break;
		default:
			break;
		}
	}
	else
	{
		switch(GetState())
		{
		case S_SURF:
			{
				m_AISContext->InitCurrent();
				if(m_AISContext->MoreCurrent())
				{
					Handle(AIS_InteractiveObject) aObj = m_AISContext->Current();
					Handle(AIS_Shape) asObj = Handle(AIS_Shape)::DownCast(aObj);
					if(!asObj.IsNull())
					{
						TopoDS_Shape aShape = asObj->Shape();
						if((aShape.ShapeType() == TopAbs_FACE))//Ӧ�������Դ���Ǻ�solid�ȡ�
						{
							m_aFace = TopoDS::Face(aShape);

							SwitchState(S_EDGE,"ѡ��һ��������ߣ��Ҽ�����");
							NeedSelect(FALSE);
							NeedMultiSelect(TRUE);
							//m_AISContext->OpenLocalContext();//yxk20191207
							m_AISContext->Activate(aObj,2);
						}
						else if(aShape.ShapeType() == TopAbs_EDGE)
						{
							TopoDS_Edge aE = TopoDS::Edge(aShape);

							//�����ϵ����ߣ�Occ��֧�ֵ�C0��G0�����𣬸߼����򱣴档
							this->m_pApiFill->Add(aE,GeomAbs_C0);
							m_nEdges ++;
						}
						else if(aShape.ShapeType() == TopAbs_WIRE)
						{
							TopExp_Explorer Ex;
							for(Ex.Init(aShape,TopAbs_EDGE);Ex.More();Ex.Next())
							{
								//�����ϵ����ߣ�Occ��֧�ֵ�C0��G0�����𣬸߼����򱣴档
								m_pApiFill->Add(TopoDS::Edge(Ex.Current()),GeomAbs_C0);
								m_nEdges ++;
							}
						}
						else if(aShape.ShapeType() == TopAbs_SHELL ||
							aShape.ShapeType() == TopAbs_SOLID ||
							aShape.ShapeType() == TopAbs_COMPOUND ||
							aShape.ShapeType() == TopAbs_COMPSOLID)
						{
							m_aShape = aShape;

							SwitchState(S_EDGE,"ѡ��һ��������ߣ��Ҽ�����");
							NeedSelect(FALSE);
							NeedMultiSelect(TRUE);
							//m_AISContext->OpenLocalContext();//yxk20191207
							m_AISContext->Activate(aObj,2);
						}
					}
				}
			}
			break;
		case S_EDGE:
			{
				
			}
			break;
		case S_POINTS:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape();
					if(!aS.IsNull() && (aS.ShapeType() == TopAbs_VERTEX))
					{
						gp_Pnt pnt = BRep_Tool::Pnt(TopoDS::Vertex(aS));
						this->m_pApiFill->Add(pnt);
					}
				}
			}
			break;
		case S_CURVES:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape();
					if(!aS.IsNull() && 
						((aS.ShapeType() == TopAbs_EDGE) || 
						(aS.ShapeType() == TopAbs_WIRE)))
					{
						if(aS.ShapeType() == TopAbs_EDGE)
						{
							this->m_pApiFill->Add(TopoDS::Edge(aS),GeomAbs_C0,FALSE);
						}
						else
						{
							TopExp_Explorer Ex;
							for(Ex.Init(aS,TopAbs_EDGE);Ex.More();Ex.Next())
							{
								//�����ϵ����ߣ�Occ��֧�ֵ�C0��G0�����𣬸߼����򱣴档
								this->m_pApiFill->Add(TopoDS::Edge(Ex.Current()),GeomAbs_C0,FALSE);
							}
						}			
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

TopoDS_Shape	ScCmdSurfFilling::BuildNewShape()
{
	TopoDS_Shape aNewShape;

	if(m_nEdges == 0)
		return aNewShape;

	try{
		m_pApiFill->Build();

		aNewShape = m_pApiFill->Shape();
	}
	catch(Standard_Failure)
	{
		delete m_pApiFill;
		m_pApiFill = NULL;
		AfxMessageBox("����ƴ��ʧ��.");
		return aNewShape;
	}

	delete m_pApiFill;
	m_pApiFill = NULL;

	return aNewShape;
}

/////////////////////////////////////
//
TopoDS_Shape ScCmdSurfPlanClosedCurves::BuildNewShape()
{
	TopoDS_Shape aNewShape;
	TopTools_ListOfShape asList;
	m_AISContext->InitCurrent();
	while(m_AISContext->MoreCurrent())
	{
		Handle(AIS_InteractiveObject) aObj = m_AISContext->Current();
		Handle(AIS_Shape) asObj = Handle(AIS_Shape)::DownCast(aObj);
		if(asObj.IsNull())
			continue;

		TopoDS_Shape aShape = asObj->Shape();
		if((aShape.ShapeType() != TopAbs_EDGE) && 
			(aShape.ShapeType() != TopAbs_WIRE))
			continue;

		asList.Append(aShape);

		m_AISContext->NextCurrent();
	}

	int nLen = asList.Extent();
	if(nLen == 0)
		return aNewShape;

	TopoDS_Wire aWNew;
	if(nLen == 1)
	{
		TopTools_ListIteratorOfListOfShape it(asList);
		TopoDS_Shape atS = it.Value();
		if(!BRep_Tool::IsClosed(atS))
			return aNewShape;
		
		
		if(atS.ShapeType() == TopAbs_EDGE)
		{
			aWNew = BRepBuilderAPI_MakeWire(TopoDS::Edge(atS));
		}
		else
		{
			aWNew = TopoDS::Wire(atS);
		}
	}
	else
	{
		//�ȴ���wire
		Handle(ShapeExtend_WireData) aData = new ShapeExtend_WireData();

		TopTools_ListIteratorOfListOfShape it(asList);
		for(;it.More();it.Next())
		{
			TopoDS_Shape atS = it.Value();
			if(atS.ShapeType() == TopAbs_EDGE)
			{
				TopoDS_Edge aE = TopoDS::Edge(atS);
				aData->Add(aE);
			}
			else
			{
				TopoDS_Wire aW = TopoDS::Wire(atS);
				TopExp_Explorer ite;
				ite.Init(aW,TopAbs_EDGE);
				for(;ite.More();ite.Next())
				{
					TopoDS_Edge aE = TopoDS::Edge(ite.Current());
					aData->Add(aE);
				}
			}
		}

		//���򣬲����������
		ShapeFix_Wire wireFix;
		wireFix.Load(aData);

		wireFix.ClosedWireMode() = TRUE;
		if(!wireFix.Perform())
			return aNewShape;

		TopoDS_Wire aWire = wireFix.WireAPIMake();
		if(aWire.IsNull())
			return aNewShape;

		if(!BRep_Tool::IsClosed(aWire))
			return aNewShape;

		aWNew = aWire;
	}

	//��������
	try{
		TopoDS_Face aF = BRepBuilderAPI_MakeFace(aWNew);
		aNewShape = aF;
	}
	catch(Standard_Failure)
	{
		return aNewShape;
	}

	return aNewShape;
}

///////////////////////////////////////////////////////////
//

//�����ͽ�������
int	ScCmdSurfCut::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SURF1,"ѡ�񱻲ü�����:");
	NeedMultiSelect(TRUE);

	return 0;
}

int	ScCmdSurfCut::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdSurfCut::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		switch(GetState())
		{
		case S_CUT:
			{
				Done();
			}
			break;
		default:
			break;
		}
	}
	else
	{
		switch(GetState())
		{
		case S_SURF1:
			{
				InitSelect();
				if(MoreSelect())
				{
					Handle(AIS_InteractiveObject) aObj = SelectedObj(FALSE);
					TopoDS_Shape aShape = SelectedShape(FALSE);
					if(!aShape.IsNull() && (aShape.ShapeType() == TopAbs_FACE))
					{
						m_aFace1 = TopoDS::Face(aShape);
						DelObj(0) = aObj;
						//
						SwitchState(S_SURF2,"ѡ��ü���������:");
					}
				}
			}
			break;
		case S_SURF2:
			{
				InitSelect();
				while(MoreSelect())
				{
					TopoDS_Shape aShape = SelectedShape();
					if(aShape.IsNull())
						continue;

					if(aShape.ShapeType() == TopAbs_FACE)
					{
						m_aFace2 = TopoDS::Face(aShape);
						if(m_aFace2.IsSame(m_aFace1))
							continue;

						if(SplitSurf())
						{
							SwitchState(S_CUT,"ѡ��Ҫɾ��������,�Ҽ�����");
							ClearSelect();
							NeedMultiSelect(FALSE);
							NeedSelect(TRUE);
						}
						else
						{
							Done();
						}
					}
				}
			}
			break;
		case S_CUT:
			{
				TopoDS_Shape aShape;
				if(SEL_OK == GetCurrentShape(aShape))
				{
					int idx = FindNewObj(aShape);
					if(idx >= 0)
					{
						m_AISContext->Remove(NewObj(idx), Standard_True);
						RemoveNewObj(idx);
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

//
// �󽻺ͷָ���������
//
BOOL	ScCmdSurfCut::SplitSurf()
{
	BRepAlgoAPI_Section sec(m_aFace1,m_aFace2,Standard_False);
	sec.ComputePCurveOn1(TRUE);
	sec.Approximation(Standard_True);

	BOOL bDone = TRUE;
	try{
		sec.Build();
	}catch(Standard_Failure)
	{
		bDone = FALSE;
	}

	if(!bDone || !sec.IsDone())
	{
		AfxMessageBox("û�н���.");
		return FALSE;
	}

	TopoDS_Shape aInterCurve = sec.Shape();
	if(aInterCurve.IsNull())
	{
		AfxMessageBox("û�н���.");
		return FALSE;
	}

	//�ָ�����
	BRepFeat_SplitShape ssplit(m_aFace1);

	TopExp_Explorer ex;
	for(ex.Init(aInterCurve,TopAbs_EDGE);ex.More();ex.Next())
	{
		TopoDS_Edge aE = TopoDS::Edge(ex.Current());
		TopoDS_Shape aF;
		if(sec.HasAncestorFaceOn1(aE,aF))
		{
			TopoDS_Face aFace = TopoDS::Face(aF);
			ssplit.Add(aE,aFace);
		}
	}

	try{
		ssplit.Build();
	}
	catch(Standard_Failure)
	{
		AfxMessageBox("����ָ�ʧ��.");
		return FALSE;
	}

	m_AISContext->Remove(DelObj(0), Standard_True);

	//��ʾ�޸Ĺ�������
	const TopTools_ListOfShape& shapeList = ssplit.Modified(m_aFace1);
	TopTools_ListIteratorOfListOfShape ite(shapeList);
	int ic = 0;
	for(;ite.More();ite.Next())
	{
		TopoDS_Shape aS = ite.Value();
		Display(NewObj(ic),aS);
		ic ++;
	}

	return TRUE;
}

void	ScCmdSurfCut::CutSurf()
{
}

///////////////////////////////////////////////////////////////////////////
//

//�����ͽ�������
int	ScCmdExtraFaces::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	BOOL bSel = TRUE;
	InitSelect();
	if(MoreSelect())
	{
		Handle(AIS_InteractiveObject) aObj = SelectedObj(FALSE);
		TopoDS_Shape aS = SelectedShape();
		if(!aS.IsNull() && 
			((aS.ShapeType() == TopAbs_SHELL) || (aS.ShapeType() == TopAbs_SOLID)))
		{
			bSel = FALSE;
			m_aShape = aS;
			DelObj(0) = aObj;
			
			ExtraFaces();

			Done();
		}
	}
	
	if(bSel)
	{
		SwitchState(S_SHAPE,"ѡ��һ������:");
		NeedSelect(TRUE);
	}

	return 0;
}

int	ScCmdExtraFaces::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdExtraFaces::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SHAPE:
		{
			InitSelect();
			if(MoreSelect())
			{
				Handle(AIS_InteractiveObject) aObj = SelectedObj(FALSE);
				TopoDS_Shape aS = SelectedShape();
				if(!aS.IsNull() && 
					((aS.ShapeType() == TopAbs_SHELL) || (aS.ShapeType() == TopAbs_SOLID)))
				{
					m_aShape = aS;
					DelObj(0) = aObj;

					ExtraFaces();

					Done();
				}
			}
		}
		break;
	default:
		break;
	}
}

void	ScCmdExtraFaces::ExtraFaces()
{
	TopExp_Explorer ex;
	int iC = 0;
	for(ex.Init(m_aShape,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Shape aS = ex.Current();
		Display(NewObj(iC),aS);
		iC ++;
	}

	if(iC > 0)
	{
		m_AISContext->Remove(DelObj(0), Standard_True);
	}
	else
	{
		DelObj(0).Nullify();
	}
}

///////////////////////////////////////////////////////////////////////////
//

ScCmdPlane::ScCmdPlane(int nType)
{
	ASSERT((nType == eTwoPoint) || (nType == eThreePoint));
	this->m_nPlnType = nType;
}


//�����ͽ�������
int	ScCmdPlane::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_POINT1,"��һ��:");
	NeedSnap(TRUE);

	return 0;
}

int	ScCmdPlane::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdPlane::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		RemoveCtxObj(TmpObj(0));
		End();
	}
	else
	{
		switch(GetState())
		{
		case S_POINT1:
			{
				m_pnt1 = this->PixelToCSPlane(point.x,point.y);

				SwitchState(S_POINT2,"�ڶ���");
			}
			break;
		case S_POINT2:
			{
				if(m_nPlnType == eTwoPoint)
				{
					RemoveCtxObj(TmpObj(0));

					TopoDS_Shape aS = BuildPlane(point);
					Display(NewObj(0),aS);

					Done();
				}
				else
				{
					gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
					//Ҫ����̫�ӽ�
					if(pnt.Distance(m_pnt1) < Precision::Confusion())
						return;

					m_pnt2 = pnt;
					SwitchState(S_POINT3,"������:");
				}
			}
			break;
		case S_POINT3:
			{
				RemoveCtxObj(TmpObj(0));

				gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
				gp_Lin aLin(m_pnt1,gp_Vec(m_pnt1,m_pnt2));
				if(aLin.Distance(pnt) < Precision::Confusion())
					return;

				TopoDS_Shape aS = BuildPlane(point);
				Display(NewObj(0),aS);

				Done();
			}
			break;
		}
	}
}
	
//����ƶ�����Ӧ����
void	ScCmdPlane::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_POINT1:
		break;
	case S_POINT2:
		{
			RemoveCtxObj(TmpObj(0));
			if(m_nPlnType == eTwoPoint)
			{
				//��ʾ��
				TopoDS_Shape aS = BuildPlane(point);
				Display(TmpObj(0),aS);
			}
			else
			{
				gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
				//Ҫ����̫�ӽ�
				if(pnt.Distance(m_pnt1) < Precision::Confusion())
					return;
				//��ʾֱ��
				TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_pnt1,pnt);
				Display(TmpObj(0),aE);
			}
		}
		break;
	case S_POINT3:
		{
			RemoveCtxObj(TmpObj(0));
			//��ʾ��
			TopoDS_Shape aS = BuildPlane(point);
			Display(TmpObj(0),aS);
		}
		break;
	}
}

TopoDS_Shape	ScCmdPlane::BuildPlane(const CPoint& point)
{
	TopoDS_Shape aShape;

	if(m_nPlnType == eTwoPoint)
	{
		gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
		//Ҫ����̫�ӽ�
		if(pnt.Distance(m_pnt1) < Precision::Confusion())
			return aShape;

		//�����������ƽ��
		gp_Vec vec(m_pnt1,pnt);
		gp_Ax2 ax = this->m_pScView->CurrentAxis(FALSE);
		if(vec.IsParallel(gp_Vec(ax.XDirection()),0.001) ||
			vec.IsParallel(gp_Vec(ax.YDirection()),0.001))
			return aShape;

		gp_Pln pln = gce_MakePln(m_pnt1,ax.Direction());

		double u,v,u1,v1,u2,v2;
		ElSLib::PlaneParameters(pln.Position(),pnt,u,v);
		u1 = min(0,u);
		u2 = std::max(0.,u);
		v1 = min(0,v);
		v2 = std::max(0.,v);

		BRepBuilderAPI_MakeFace mf(pln,u1,u2,v1,v2);
		aShape = mf.Shape();
	}
	else
	{
		//������ƽ��ķ�������������ķ���?����ķ����Ƚϼ򵥣���������ܲ���plane��
		gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
		gp_Lin aLin(m_pnt1,gp_Vec(m_pnt1,m_pnt2));
		double dist = aLin.Distance(pnt);
		if(dist < Precision::Confusion())
			return aShape;

		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_pnt1,m_pnt2);
		if(!aE.IsNull())
		{
			//�������췽��
			gp_Lin aDLin = aLin.Normal(pnt);
			double x,y,z;
			aDLin.Direction().Coord(x,y,z);

			aShape = BRepPrimAPI_MakePrism(aE,gp_Vec(dist * x,dist * y,dist * z));
		}
	}

	return aShape;
}

/////////////////////////////////////////////////////
// �ܵ��棬ʹ��GeomFill_Pipe��

//�����ͽ�������
int	ScCmdSurfPipe::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_CURVE,"ѡ��һ������:");
	NeedSelect(TRUE);

	return SC_OK;
}

int	ScCmdSurfPipe::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdSurfPipe::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
	}
	else
	{
		switch(GetState())
		{
		case S_CURVE:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape(FALSE);
					if(!aS.IsNull() && 
						(aS.ShapeType() == TopAbs_EDGE))
					{
						TmpObj(0) = SelectedObj();
						NeedSelect(FALSE);

						SwitchState(S_RADIUS,"����ܵ��뾶:");
						NeedDouble("�ܵ��뾶:");
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

//���հ뾶
int		ScCmdSurfPipe::AcceptInput(double dVal)
{
	switch(GetState())
	{
	case S_RADIUS:
		{
			if(dVal > Precision::Confusion())
			{
				double dF,dL;
				TopoDS_Shape aS = GetObjShape(TmpObj(0));
				Handle(Geom_Curve) aC = BRep_Tool::Curve(TopoDS::Edge(aS),dF,dL);
				if(!aC.IsNull())
				{
					Handle(Geom_TrimmedCurve) aTC = new Geom_TrimmedCurve(aC,dF,dL);
					GeomFill_Pipe pipe(aTC,dVal);
					try
					{
						pipe.Perform(TRUE);
					}
					catch(Standard_Failure)
					{
						End();
						return -1;
					}
					Handle(Geom_Surface) aSurf = pipe.Surface();
					if(!aSurf.IsNull())
					{
						TopoDS_Face aF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());//yxk20200104����, Precision::Confusion()
						Display(NewObj(0),aF);

						Done();
					}
					else
					{
						End();
					}
				}
				else
				{
					End();
				}

				TmpObj(0) = NULL;
			}
		}
		break;
	default:
		break;
	}
	return SC_OK;
}

//////////////////////////////////////////////////////
// bsplinecurves���ɵ�surface��
// ʹ���ࣺGeomFill_BSplineCurves

//�����ͽ�������
int	ScCmdSurfBSplineCurves::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_CURVES,"ѡ��2��4����β���ӵ�BSpline���ߣ��Ҽ�����.");
	NeedMultiSelect(TRUE);
	this->m_nCnt = 0;

	return SC_OK;
}


int	ScCmdSurfBSplineCurves::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdSurfBSplineCurves::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		switch(GetState())
		{
		case S_CURVES:
			{
				InitSelect();
				while(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape();
					if(aS.IsNull() || aS.ShapeType() != TopAbs_EDGE)
						continue;

					double dF,dL;
					Handle(Geom_Curve) aC = BRep_Tool::Curve(TopoDS::Edge(aS),dF,dL);
					if(!aC.IsNull() &&
						aC->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
					{
						m_nCnt ++;
						if(m_nCnt == 1)
						{
							m_aC1 = Handle(Geom_BSplineCurve)::DownCast(aC);
						}
						else if(m_nCnt == 2)
						{
							m_aC2 = Handle(Geom_BSplineCurve)::DownCast(aC);
						}
						else if(m_nCnt == 3)
						{
							m_aC3 = Handle(Geom_BSplineCurve)::DownCast(aC);
						}
						else if(m_nCnt == 4)
						{
							m_aC4 = Handle(Geom_BSplineCurve)::DownCast(aC);
						}
						else
						{
							break;
						}
					}
				}

				if(m_nCnt >= 2)
				{
					SwitchState(S_TYPE,"ѡ����������:");
					NeedMultiSelect(FALSE);
					NeedInteger("��������:0 Stretch 1 Coons 2 Curved");
				}
			}
			break;
		default:
			End();
			break;
		}
	}
	else
	{
	}
}


//��������
int		ScCmdSurfBSplineCurves::AcceptInput(int nVal)
{
	switch(GetState())
	{
	case S_TYPE:
		{
			if(nVal < 0 || nVal > 2)
			{
				return -1;
			}

			Handle_Geom_BSplineSurface aNCur;
			try
			{
				if(m_nCnt == 2)
				{
					GeomFill_BSplineCurves bc(m_aC1,m_aC2,(GeomFill_FillingStyle)nVal);
					aNCur = bc.Surface();
				}
				else if(m_nCnt == 3)
				{
					GeomFill_BSplineCurves bc(m_aC1,m_aC2,m_aC3,(GeomFill_FillingStyle)nVal);
					aNCur = bc.Surface();
				}
				else if(m_nCnt == 4)
				{
					GeomFill_BSplineCurves bc(m_aC1,m_aC2,m_aC3,m_aC4,(GeomFill_FillingStyle)nVal);
					aNCur = bc.Surface();
				}
			}
			catch(Standard_Failure)
			{
				End();
				return -1;
			}
			if(aNCur.IsNull())
			{
				End();
			}
			else
			{
				TopoDS_Face aF = BRepBuilderAPI_MakeFace(aNCur, Precision::Confusion());//yxk20200104����, Precision::Confusion());
				Display(NewObj(0),aF);
				Done();
			}
		}
		break;
	default:
		break;
	}
	return 0;
}


//////////////////////////////////////////////////////
// ��������һ���ĳ��ȡ�
// ʹ�ã�GeomLib::ExtendSurfByLength������

//�����ͽ�������
int	ScCmdSurfExtent::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);
	SwitchState(S_SURF,"ѡ��һ����������:");
	NeedSelect(TRUE);
	return 0;
}

int	ScCmdSurfExtent::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdSurfExtent::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
	}
	else
	{
		switch(GetState())
		{
		case S_SURF:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape(FALSE);
					if(!aS.IsNull() && (aS.ShapeType() == TopAbs_FACE))
					{
						Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(aS));
						if(!aSurf.IsNull() &&
							aSurf->IsKind(STANDARD_TYPE(Geom_BoundedSurface)))
						{
							m_aSurf = Handle(Geom_BoundedSurface)::DownCast(aSurf);
							if(!m_aSurf.IsNull())
							{
								TmpObj(0) = SelectedObj();

								NeedSelect(FALSE);
								SwitchState(S_UV,"���ѡ��Ҫ����Ĳ���:");
							}
						}
					}
				}
			}
			break;
		case S_UV:
			{
				TopoDS_Shape aS = GetObjShape(TmpObj(0));
				TopoDS_Face aF = TopoDS::Face(aS);
				gp_Pnt pnt;
				double u,v;
				if(this->m_pScView->GetFacePickPoint(aF,point.x,point.y,u,v,pnt))
				{//��������
					//����uv��Χ��
					double U1,U2,V1,V2;
					BRepTools::UVBounds(aF,U1,U2,V1,V2);
					//�ж�U����V��������Ρ�
					if((u >= U1) && (u <= U2) && (v >= V1) && (v <= V2))
					{
						//��u��v��һ����0,1)�䣬��ֹ��������Χ����̫��
						u = U1 + (u - U1)/(U2 - U1);
						v = V1 + (v - V1)/(V2 - V1);
						U2 = U1 + 1.0;
						V2 = V1 + 1.0;
						double du,dv;
						double um = (U1 + U2)/2.0,vm = (V1 + V2)/2.0;
						if(u > um)
						{
							du = U2 - u;
							if(v > vm)
							{
								dv = V2 - v;
								if(du < dv)
								{
									m_bU = TRUE;
									m_bAfter = TRUE;
								}
								else
								{
									m_bU = FALSE;
									m_bAfter = TRUE;
								}
							}
							else
							{
								dv = v - V1;
								if(du < dv)
								{
									m_bU = TRUE;
									m_bAfter = TRUE;
								}
								else
								{
									m_bU = FALSE;
									m_bAfter = FALSE;
								}
							}
						}
						else
						{// U����ʼ�ࡣ
							double du = u - U1;
							if(v > vm)
							{
								dv = V2 - v;
								if(du < dv)
								{
									m_bU = TRUE;
									m_bAfter = FALSE;
								}
								else
								{
									m_bU = FALSE;
									m_bAfter = TRUE;
								}
							}
							else
							{
								dv = v - V1;
								if(du < dv)
								{
									m_bU = TRUE;
									m_bAfter = FALSE;
								}
								else
								{
									m_bU = FALSE;
									m_bAfter = FALSE;
								}
							}
						}

						SwitchState(S_CONT,"����������:");
						NeedDouble("������:1 2 3");
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

	//�������쳤��
int	ScCmdSurfExtent::AcceptInput(double dVal)
{
	switch(GetState())
	{
	case S_CONT:
		{
			if(dVal != 1.0 && dVal != 2.0 && dVal != 3.0)
				return -1;

			m_nCont = (int)dVal;

			EndInput();
			SwitchState(S_LEN,"�������쳤��:");
			NeedDouble("���쳤��:");
		}
		break;
	case S_LEN:
		{
			if(dVal < Precision::Confusion())
				return -1;

			//��������
			try
			{
				GeomLib::ExtendSurfByLength(m_aSurf,dVal,m_nCont,m_bU,m_bAfter);
			}
			catch(Standard_Failure)
			{
				End();
				return -1;
			}
			if(!m_aSurf.IsNull())
			{
				DelObj(0) = TmpObj(0);
				RemoveCtxObj(TmpObj(0));//ɾ���ɵġ���ʾ�µġ�

				TopoDS_Face aNF = BRepBuilderAPI_MakeFace(m_aSurf, Precision::Confusion());//yxk20200104����, Precision::Confusion()
				Display(NewObj(0),aNF);

				Done();
			}
			else
			{
				End();
			}
		}
		break;
	default:
		break;
	}
	
	return 0;
}

//////////////////////////////////////////////////////////
// ��face����ȡ�ü������ԭʼ����
BOOL	ScCmdRawSurface::BuildShape()//���ɶ�Ӧshape��
{
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(m_aFace);
	if(aSurf.IsNull())
		return FALSE;

	TopoDS_Face aNF;
	if(aSurf->IsKind(STANDARD_TYPE(Geom_BoundedSurface)))
	{
		aNF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());//yxk20200104����, Precision::Confusion()
	}
	else
	{
		double U1,U2,V1,V2;
		BRepTools::UVBounds(m_aFace,U1,U2,V1,V2);

		if(aSurf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
		{
			if(aSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
			{
				aNF = BRepBuilderAPI_MakeFace(aSurf,U1 * 2.0,U2 * 2.0,V1 * 2.0,V2 * 2.0, Precision::Confusion());//yxk20200104����, Precision::Confusion()
			}
			else if(aSurf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)) ||
				aSurf->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
			{
				aNF = BRepBuilderAPI_MakeFace(aSurf,0.0,2.0 * M_PI,V1 * 2.0,V2 * 2.0, Precision::Confusion());//yxk20200104����, Precision::Confusion()
			}
			else 
			{
				aNF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());//yxk20200104����, Precision::Confusion()
			}
		}
		else 
		{
			aNF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());//yxk20200104����, Precision::Confusion()
		}
	}

	if(aNF.IsNull())
		End();
	else
	{
		Display(NewObj(0),aNF,RGB(0,0,200));
		Done();
	}

	return TRUE;
}

////////////////////////////////////////////////////////////
// ��ȡһ�����棬��������Shape��Solid��Compound�е�һ��Face
BOOL	ScCmdExtractFace::BuildShape()//���ɶ�Ӧshape��
{
	TopoDS_Shape aNS;
	BRepBuilderAPI_Copy bcp;
	try
	{
		bcp.Perform(m_aFace);
		aNS = bcp.Shape();
	}
	catch(Standard_Failure)
	{
		return FALSE;
	}

	Display(NewObj(0),aNS,RGB(0,0,128));

	Done();

	return TRUE;
}

////////////////////////////////////////////////////////////////
// �ɶ��ƽ�滷���ɵĲü�ƽ�档�򵥵ķ�����ֱ��ʹ��makeface������
//���ӵķ�������Ҫ�жϻ��Ƿ��ཻ�ȣ�����ͺܸ����ˡ�
//�ȼ򵥵����

//�����ͽ�������
int 	ScCmdMultiLoopFace::	Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		NeedMultiSelect(TRUE);
		SwitchState(S_OUTWIRE,"ѡ��һ���⻷:");

		return 0;
}

 int	ScCmdMultiLoopFace::	End()
 {
		 TmpObj(0) = NULL;
		this->ClearSelObjs();

		return ScCommand::End();
 }

//��ѡ��ʽ�µ���Ϣ��Ӧ����
 void	ScCmdMultiLoopFace::	InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView)
 {
		if(nState == MINPUT_RIGHT)
		{
				if(BuildShape())
						Done();
				else
						End();
		}
		else
		{
				switch(GetState())
				{
				case S_OUTWIRE:
						{
								InitSelect();
								if(MoreSelect())
								{
										TopoDS_Shape aS = SelectedShape(FALSE);
										if(!aS.IsNull() && (aS.ShapeType() == TopAbs_WIRE 
												                    || aS.ShapeType() == TopAbs_EDGE))
										{
												TmpObj(0) = SelectedObj(FALSE);

												SwitchState(S_INWIRES,"ѡ��һ�������ڻ�,�Ҽ�����:");
										}
								}
						}
						break;
				case S_INWIRES:
						{
								SwitchState(S_INWIRES,"ѡ��һ�������ڻ�,�Ҽ�����:");
						}
						break;
				default:
						break;
				}
		}
 }


BOOL		ScCmdMultiLoopFace::	BuildShape()
{
		TopoDS_Shape aOS = GetObjShape(TmpObj(0));
		if(aOS.IsNull())
				return FALSE;

		TopoDS_Wire aOWire;
		if(aOS.ShapeType() == TopAbs_WIRE)
				aOWire = TopoDS::Wire(GetObjShape(TmpObj(0)));
		else
		{
				aOWire = BRepBuilderAPI_MakeWire(TopoDS::Edge(aOS));
		}
		if(aOWire.IsNull())
				return FALSE;

		//aOWire.Reverse();
		InitSelect();
		while(MoreSelect())
		{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull() && (aS.ShapeType() == TopAbs_WIRE ||
						aS.ShapeType() == TopAbs_EDGE))
				{
						if(SelectedObj(FALSE) == TmpObj(0))
						{
								NextSelect();
								continue;
						}

						//��Ҫ�ж�edge��wire�Ƿ�պ�.

						SelObj(-1) = SelectedObj(FALSE);
				}
				NextSelect();
		}

		TopoDS_Face aNF;
		
		try{
				//��ʹ���⻷����һ��ƽ��
				BRepBuilderAPI_MakeFace MF(aOWire,Standard_True);
				if(MF.IsDone())
				{
						aNF = MF.Face();
						if(aNF.IsNull())
								return FALSE;
				}

				//�������ζ��ڻ������ж�,��Ҫ��Ҫ��֤���պ�,���һ��ķ�����ȷ
				BRep_Builder B;
				int nLen = m_seqOfSelObj.Length();
				for(int ix = 0;ix < nLen;ix ++)
				{
						TopoDS_Shape aS = GetObjShape(SelObj(ix));
						TopoDS_Wire aW;
						if(aS.ShapeType() == TopAbs_WIRE)
								aW = TopoDS::Wire(aS);
						else
						{
								aW = BRepBuilderAPI_MakeWire(TopoDS::Edge(aS));
						}

						if(aW.IsNull() || !BRep_Tool::IsClosed(aW))
						{
								DTRACE("\n inner wire make null or not closed.");
								continue;
						}

						//�ж��ڻ��ķ���
						TopoDS_Shape aTS = aNF.EmptyCopied();
						TopoDS_Face aTF = TopoDS::Face(aTS);
						aTF.Orientation(TopAbs_FORWARD);
						B.Add(aTF,aW);
						BRepTopAdaptor_FClass2d fcls(aTF,Precision::PConfusion());
						TopAbs_State sta = fcls.PerformInfinitePoint();
						if(sta == TopAbs_OUT)
								aW.Reverse();

						MF.Add(aW);
				}

				//��ȡ���յ�face
				aNF = MF.Face();

		}catch(Standard_Failure){
				return FALSE;
		}
		
		if(aNF.IsNull())
				return FALSE;

		Display(NewObj(0),aNF);

		return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ʹ������ָ�ƽ�棬ʹ��BrepFeat_splitShape
//

//�����ͽ�������
int		ScCmdSplitPlane::	Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		NeedSelect(TRUE);
		if(HasSelFace())
		{
				SwitchState(S_CURVE,"ѡ��ָ�����:");
		}
		else
		{
				SwitchState(S_FACE,"ѡ��Ҫ�ָ��ƽ������:");
		}

		return 0;
}

int		ScCmdSplitPlane::	End()
{
		SelObj(0) = NULL;
		SelObj(1) = NULL;
		return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
 void		ScCmdSplitPlane::	InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView)
 {
		 if(nState == MINPUT_RIGHT)
		 {
				 End();
		 }
		 else
		 {
				 switch(GetState())
				 {
				 case S_FACE:
						 {
								 if(HasSelFace())
								 {
										 SwitchState(S_CURVE,"ѡ��ָ�����:");
								 }
						 }
						 break;
				 case S_CURVE:
						 {
								 InitSelect();
								 if(MoreSelect())
								 {
										 TopoDS_Shape aS = SelectedShape(FALSE);
										 if(!aS.IsNull() && (aS.ShapeType() == TopAbs_EDGE ||
												 aS.ShapeType() == TopAbs_WIRE))
										 {
												 SelObj(1) = SelectedObj(FALSE);

												 if(DoSplit())
														 Done();
												 else
														 End();

										 }
								 }
						 }
						 break;
				 default:
						 break;
				 }
		 }
 }

BOOL			ScCmdSplitPlane::	HasSelFace()
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape		aShape = SelectedShape(FALSE);
				if(!aShape.IsNull() && aShape.ShapeType() == TopAbs_FACE)
				{
						Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(aShape));
						if(aSurf.IsNull())
								return FALSE;
						GeomLib_IsPlanarSurface ps(aSurf);
						if(ps.IsPlanar())
						{
								SelObj(0) = SelectedObj(FALSE);
								m_pln = ps.Plan();

								return TRUE;
						}
				}
		}
		return FALSE;
}

BOOL			ScCmdSplitPlane::	DoSplit()
{
		TopoDS_Face aFace = TopoDS::Face(GetObjShape(SelObj(0)));
		TopoDS_Shape aCur = GetObjShape(SelObj(1));

		//��aCur��ƽ�淽�����죬�������棬����һ�������󽻣���ý���
		//��ʹ�ý��߽��зָ�
		gp_Dir zD = m_pln.Axis().Direction();
		double dH = 10;//ȡСһЩ

		//�ȶ�profile�����ƶ�
		gp_Trsf trsf;
		trsf.SetTranslation(gp_Vec(-dH * zD.X(),-dH * zD.Y(),-dH * zD.Z()));
		BRepBuilderAPI_Transform brepTrsf(aCur,trsf,TRUE);
		aCur = brepTrsf.Shape();

		gp_Vec aVec(2.0 * dH*zD.X(),2.0 * dH * zD.Y(),2.0 * dH * zD.Z());
		
		TopoDS_Shape aNF;
		try
		{
				aNF = BRepPrimAPI_MakePrism(aCur,aVec);
		}
		catch (Standard_Failure)
		{
				AfxMessageBox("�����м���ʧ�ܡ�");
				return FALSE;
		}

		//��
		BRepAlgoAPI_Section sec(aFace,aNF,Standard_False);
		sec.ComputePCurveOn1(TRUE);
		sec.Approximation(Standard_True);

		BOOL bDone = TRUE;
		try{
				sec.Build();
		}catch(Standard_Failure)
		{
				bDone = FALSE;
		}

		if(!bDone || !sec.IsDone())
		{
				AfxMessageBox("û�н���.");
				return FALSE;
		}

		TopoDS_Shape aInterCurve = sec.Shape();
		if(aInterCurve.IsNull())
		{
				AfxMessageBox("û�н���.");
				return FALSE;
		}

		//�ָ�����
		BRepFeat_SplitShape ssplit(aFace);

		TopExp_Explorer ex;
		for(ex.Init(aInterCurve,TopAbs_EDGE);ex.More();ex.Next())
		{
				TopoDS_Edge aE = TopoDS::Edge(ex.Current());
				TopoDS_Shape aF;
				if(sec.HasAncestorFaceOn1(aE,aF))
				{
						TopoDS_Face aAncFace = TopoDS::Face(aF);
						if(aAncFace.IsSame(aFace))
								ssplit.Add(aE,aFace);
				}
		}

		try{
				ssplit.Build();
		}
		catch(Standard_Failure)
		{
				AfxMessageBox("����ָ�ʧ��.");
				return FALSE;
		}

		//��ʾ�޸Ĺ�������
		const TopTools_ListOfShape& shapeList = ssplit.Modified(aFace);
		TopTools_ListIteratorOfListOfShape ite(shapeList);
		int ic = 0;
		for(;ite.More();ite.Next())
		{
				TopoDS_Shape aS = ite.Value();
				Display(NewObj(ic),aS);
				ic ++;
		}

		if(ic == 0)
				return FALSE;

		m_AISContext->Remove(SelObj(0), Standard_True);

		BeginUndo("split plane");
		UndoAddDelObj(SelObj(0));
		for(int ix = 0;ix < ic;ix ++)
		{
				if(!NewObj(ix).IsNull())
						UndoAddNewObj(NewObj(ix));
		}

		EndUndo(TRUE);

		return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ʹ������ָ����棬ʹ��BRepFeat_splitShape
//�����ͽ�������
int		ScCmdSplitSurf::Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		NeedSelect(TRUE);
		if(HasSelFace())
		{
				SwitchState(S_TOOL,"ѡ��һ����������:");
		}
		else
		{
				SwitchState(S_FACE,"ѡ��һ��Ҫ�ָ������");
		}

		return 0;
}

int		ScCmdSplitSurf::End()
{
		SelObj(0) = NULL;
		SelObj(1) = NULL;
		return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdSplitSurf::InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView)
{
		if(nState == MINPUT_RIGHT)
		{
				End();
		}
		else
		{
				switch(GetState())
				{
				case S_FACE:
						{
								if(HasSelFace())
								{
										SwitchState(S_TOOL,"ѡ��һ����������:");
								}
						}
						break;
				case S_TOOL:
						{
								if(HasSelTool())
								{
										if(DoSplit())
												Done();
										else
												End();
								}
						}
						break;
				default:
						break;
				}
		}
}

BOOL			ScCmdSplitSurf::HasSelFace()
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull() && aS.ShapeType() == TopAbs_FACE)
				{
						SelObj(0) = SelectedObj();

						return TRUE;
				}
		}
		return FALSE;
}

BOOL			ScCmdSplitSurf::HasSelTool()
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull() && aS.ShapeType() == TopAbs_FACE)
				{
						if(SelObj(0) == SelectedObj(FALSE))
						{
								AfxMessageBox("�����治����ͬ.");
								return FALSE;
						}

						SelObj(1) = SelectedObj();

						return TRUE;
				}
		}

		return FALSE;
}

BOOL			ScCmdSplitSurf::DoSplit()
{
		TopoDS_Shape aS1 = GetObjShape(SelObj(0));
		TopoDS_Shape aS2 = GetObjShape(SelObj(1));

		//��
		BRepAlgoAPI_Section sec(aS1,aS2,Standard_False);
		sec.ComputePCurveOn1(TRUE);
		sec.Approximation(Standard_True);

		BOOL bDone = TRUE;
		try{
				sec.Build();
		}catch(Standard_Failure)
		{
				bDone = FALSE;
		}

		if(!bDone || !sec.IsDone())
		{
				AfxMessageBox("û�н���.");
				return FALSE;
		}

		TopoDS_Shape aInterCurve = sec.Shape();
		if(aInterCurve.IsNull())
		{
				AfxMessageBox("û�н���.");
				return FALSE;
		}

		//�ָ�����
		TopoDS_Face aFace = TopoDS::Face(aS1);
		BRepFeat_SplitShape ssplit(aS1);

		TopExp_Explorer ex;
		for(ex.Init(aInterCurve,TopAbs_EDGE);ex.More();ex.Next())
		{
				TopoDS_Edge aE = TopoDS::Edge(ex.Current());
				TopoDS_Shape aF;
				if(sec.HasAncestorFaceOn1(aE,aF))
				{
						TopoDS_Face aAncFace = TopoDS::Face(aF);
						if(aAncFace.IsSame(aFace))
								ssplit.Add(aE,aFace);
				}
		}

		try{
				ssplit.Build();
		}
		catch(Standard_Failure)
		{
				AfxMessageBox("����ָ�ʧ��.");
				return FALSE;
		}

		//��ʾ�޸Ĺ�������
		const TopTools_ListOfShape& shapeList = ssplit.Modified(aFace);
		TopTools_ListIteratorOfListOfShape ite(shapeList);
		int ic = 0;
		for(;ite.More();ite.Next())
		{
				TopoDS_Shape aS = ite.Value();
				Display(NewObj(ic),aS);
				ic ++;
		}

		if(ic == 0)
				return FALSE;

		m_AISContext->Remove(SelObj(0), Standard_True);

		BeginUndo("split plane");
		UndoAddDelObj(SelObj(0));
		for(int ix = 0;ix < ic;ix ++)
		{
				if(!NewObj(ix).IsNull())
						UndoAddNewObj(NewObj(ix));
		}

		EndUndo(TRUE);

		return TRUE;
}
