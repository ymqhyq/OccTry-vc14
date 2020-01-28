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

//鼠标移动的响应函数
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
		//创建face，并显示
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

//启动和结束命令
int		ScCmdSurfSewing::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SURFS,"选择多个邻接曲面，右键结束");
	NeedMultiSelect(TRUE);

	return 0;
}

int		ScCmdSurfSewing::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
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
		this->m_seqOfObj.Append(aObj);//记录，以便删除

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
		AfxMessageBox("缝合失败.");
		return aNewShape;
	}

	if(!aNewShape.IsNull())
	{//删除旧的曲面
		int nC = m_seqOfObj.Length();
		for(int ic = 1;ic <= nC;ic++)
		{
			m_AISContext->Remove(m_seqOfObj.Value(ic), Standard_True);
		}
	}

	return aNewShape;
}

/////////////////////////////////////////////////////////////////////////// 
// 曲面filling功能。
//
ScCmdSurfFilling::ScCmdSurfFilling(int nType)
{
	ASSERT((nType >= eBoundary) && (nType <= eWithFace));
	this->m_nFillType = nType;
	this->m_pApiFill = NULL;
}

//启动和结束命令
int	ScCmdSurfFilling::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SURF,"选择一条曲线或一个曲面,右键结束:");
	NeedSelect(TRUE);

	this->m_pApiFill = new BRepOffsetAPI_MakeFilling(3,10,2);
	this->m_nEdges = 0;

	return 0;
}

int	ScCmdSurfFilling::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdSurfFilling::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		switch(GetState())
		{
		case S_SURF:
			{
				if(m_nEdges == 0)//没有曲线，不构造。
				{
					Done();
					return;
				}

				if(this->m_nFillType == eBoundary)//仅仅边界约束。
				{
					TopoDS_Shape aS = BuildNewShape();
					Display(NewObj(0),aS);
					Done();
					return;
				}

				if(this->m_nFillType == eWithPoints)
				{
					SwitchState(S_POINTS,"选择一个或多个约束点，右键结束.");
					NeedSelect(TRUE);
					NeedMultiSelect(FALSE);
				}
				else if(this->m_nFillType == eWithCurves)
				{
					SwitchState(S_CURVES,"选择一条或多条约束曲线，右键结束.");
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
					TopExp::MapShapesAndAncestors(m_aShape, TopAbs_EDGE, TopAbs_FACE, M);//据此找出edge所归属的face.
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

				SwitchState(S_SURF,"选择一条曲线或一个曲面:");
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
						if((aShape.ShapeType() == TopAbs_FACE))//应当还可以处理壳和solid等。
						{
							m_aFace = TopoDS::Face(aShape);

							SwitchState(S_EDGE,"选择一条或多条边，右键结束");
							NeedSelect(FALSE);
							NeedMultiSelect(TRUE);
							//m_AISContext->OpenLocalContext();//yxk20191207
							m_AISContext->Activate(aObj,2);
						}
						else if(aShape.ShapeType() == TopAbs_EDGE)
						{
							TopoDS_Edge aE = TopoDS::Edge(aShape);

							//非面上的曲线，Occ仅支持到C0（G0）级别，高级别则保存。
							this->m_pApiFill->Add(aE,GeomAbs_C0);
							m_nEdges ++;
						}
						else if(aShape.ShapeType() == TopAbs_WIRE)
						{
							TopExp_Explorer Ex;
							for(Ex.Init(aShape,TopAbs_EDGE);Ex.More();Ex.Next())
							{
								//非面上的曲线，Occ仅支持到C0（G0）级别，高级别则保存。
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

							SwitchState(S_EDGE,"选择一条或多条边，右键结束");
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
								//非面上的曲线，Occ仅支持到C0（G0）级别，高级别则保存。
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
		AfxMessageBox("曲面拼接失败.");
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
		//先创建wire
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

		//排序，并检查连接性
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

	//生成曲面
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

//启动和结束命令
int	ScCmdSurfCut::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SURF1,"选择被裁剪曲面:");
	NeedMultiSelect(TRUE);

	return 0;
}

int	ScCmdSurfCut::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
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
						SwitchState(S_SURF2,"选择裁剪工具曲面:");
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
							SwitchState(S_CUT,"选择要删除的曲面,右键结束");
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
// 求交和分割两个曲面
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
		AfxMessageBox("没有交线.");
		return FALSE;
	}

	TopoDS_Shape aInterCurve = sec.Shape();
	if(aInterCurve.IsNull())
	{
		AfxMessageBox("没有交线.");
		return FALSE;
	}

	//分割曲面
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
		AfxMessageBox("曲面分割失败.");
		return FALSE;
	}

	m_AISContext->Remove(DelObj(0), Standard_True);

	//显示修改过的曲面
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

//启动和结束命令
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
		SwitchState(S_SHAPE,"选择一个对象:");
		NeedSelect(TRUE);
	}

	return 0;
}

int	ScCmdExtraFaces::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
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


//启动和结束命令
int	ScCmdPlane::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_POINT1,"第一点:");
	NeedSnap(TRUE);

	return 0;
}

int	ScCmdPlane::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
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

				SwitchState(S_POINT2,"第二点");
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
					//要避免太接近
					if(pnt.Distance(m_pnt1) < Precision::Confusion())
						return;

					m_pnt2 = pnt;
					SwitchState(S_POINT3,"第三点:");
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
	
//鼠标移动的响应函数
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
				//显示面
				TopoDS_Shape aS = BuildPlane(point);
				Display(TmpObj(0),aS);
			}
			else
			{
				gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
				//要避免太接近
				if(pnt.Distance(m_pnt1) < Precision::Confusion())
					return;
				//显示直线
				TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_pnt1,pnt);
				Display(TmpObj(0),aE);
			}
		}
		break;
	case S_POINT3:
		{
			RemoveCtxObj(TmpObj(0));
			//显示面
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
		//要避免太接近
		if(pnt.Distance(m_pnt1) < Precision::Confusion())
			return aShape;

		//避免和坐标轴平行
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
		//采用找平面的方法。或者拉伸的方法?拉伸的方法比较简单，但结果可能不是plane。
		gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
		gp_Lin aLin(m_pnt1,gp_Vec(m_pnt1,m_pnt2));
		double dist = aLin.Distance(pnt);
		if(dist < Precision::Confusion())
			return aShape;

		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_pnt1,m_pnt2);
		if(!aE.IsNull())
		{
			//计算拉伸方向
			gp_Lin aDLin = aLin.Normal(pnt);
			double x,y,z;
			aDLin.Direction().Coord(x,y,z);

			aShape = BRepPrimAPI_MakePrism(aE,gp_Vec(dist * x,dist * y,dist * z));
		}
	}

	return aShape;
}

/////////////////////////////////////////////////////
// 管道面，使用GeomFill_Pipe类

//启动和结束命令
int	ScCmdSurfPipe::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_CURVE,"选择一条曲线:");
	NeedSelect(TRUE);

	return SC_OK;
}

int	ScCmdSurfPipe::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
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

						SwitchState(S_RADIUS,"输入管道半径:");
						NeedDouble("管道半径:");
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

//接收半径
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
						TopoDS_Face aF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());//yxk20200104增加, Precision::Confusion()
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
// bsplinecurves生成的surface。
// 使用类：GeomFill_BSplineCurves

//启动和结束命令
int	ScCmdSurfBSplineCurves::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_CURVES,"选择2到4条首尾连接的BSpline曲线，右键结束.");
	NeedMultiSelect(TRUE);
	this->m_nCnt = 0;

	return SC_OK;
}


int	ScCmdSurfBSplineCurves::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
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
					SwitchState(S_TYPE,"选择曲面类型:");
					NeedMultiSelect(FALSE);
					NeedInteger("曲面类型:0 Stretch 1 Coons 2 Curved");
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


//接收整数
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
				TopoDS_Face aF = BRepBuilderAPI_MakeFace(aNCur, Precision::Confusion());//yxk20200104增加, Precision::Confusion());
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
// 曲面延伸一定的长度。
// 使用：GeomLib::ExtendSurfByLength方法。

//启动和结束命令
int	ScCmdSurfExtent::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);
	SwitchState(S_SURF,"选择一个样条曲面:");
	NeedSelect(TRUE);
	return 0;
}

int	ScCmdSurfExtent::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
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
								SwitchState(S_UV,"点击选择要延伸的部分:");
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
				{//点中曲面
					//计算uv包围盒
					double U1,U2,V1,V2;
					BRepTools::UVBounds(aF,U1,U2,V1,V2);
					//判断U还算V，方向如何。
					if((u >= U1) && (u <= U2) && (v >= V1) && (v <= V2))
					{
						//将u、v归一到〔0,1)间，防止两参数范围差异太大。
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
						{// U的起始侧。
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

						SwitchState(S_CONT,"输入连续性:");
						NeedDouble("连续性:1 2 3");
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

	//接收延伸长度
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
			SwitchState(S_LEN,"输入延伸长度:");
			NeedDouble("延伸长度:");
		}
		break;
	case S_LEN:
		{
			if(dVal < Precision::Confusion())
				return -1;

			//进行延伸
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
				RemoveCtxObj(TmpObj(0));//删除旧的。显示新的。

				TopoDS_Face aNF = BRepBuilderAPI_MakeFace(m_aSurf, Precision::Confusion());//yxk20200104增加, Precision::Confusion()
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
// 对face，获取裁剪曲面的原始曲面
BOOL	ScCmdRawSurface::BuildShape()//生成对应shape。
{
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(m_aFace);
	if(aSurf.IsNull())
		return FALSE;

	TopoDS_Face aNF;
	if(aSurf->IsKind(STANDARD_TYPE(Geom_BoundedSurface)))
	{
		aNF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());//yxk20200104增加, Precision::Confusion()
	}
	else
	{
		double U1,U2,V1,V2;
		BRepTools::UVBounds(m_aFace,U1,U2,V1,V2);

		if(aSurf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
		{
			if(aSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
			{
				aNF = BRepBuilderAPI_MakeFace(aSurf,U1 * 2.0,U2 * 2.0,V1 * 2.0,V2 * 2.0, Precision::Confusion());//yxk20200104增加, Precision::Confusion()
			}
			else if(aSurf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)) ||
				aSurf->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
			{
				aNF = BRepBuilderAPI_MakeFace(aSurf,0.0,2.0 * M_PI,V1 * 2.0,V2 * 2.0, Precision::Confusion());//yxk20200104增加, Precision::Confusion()
			}
			else 
			{
				aNF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());//yxk20200104增加, Precision::Confusion()
			}
		}
		else 
		{
			aNF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());//yxk20200104增加, Precision::Confusion()
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
// 获取一个曲面，该曲面是Shape、Solid或Compound中的一个Face
BOOL	ScCmdExtractFace::BuildShape()//生成对应shape。
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
// 由多个平面环构成的裁剪平面。简单的方法，直接使用makeface构建。
//复杂的方法，需要判断环是否相交等，情况就很复杂了。
//先简单的情况

//启动和结束命令
int 	ScCmdMultiLoopFace::	Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		NeedMultiSelect(TRUE);
		SwitchState(S_OUTWIRE,"选择一个外环:");

		return 0;
}

 int	ScCmdMultiLoopFace::	End()
 {
		 TmpObj(0) = NULL;
		this->ClearSelObjs();

		return ScCommand::End();
 }

//点选方式下的消息响应函数
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

												SwitchState(S_INWIRES,"选择一个或多个内环,右键结束:");
										}
								}
						}
						break;
				case S_INWIRES:
						{
								SwitchState(S_INWIRES,"选择一个或多个内环,右键结束:");
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

						//需要判断edge或wire是否闭合.

						SelObj(-1) = SelectedObj(FALSE);
				}
				NextSelect();
		}

		TopoDS_Face aNF;
		
		try{
				//先使用外环创建一个平面
				BRepBuilderAPI_MakeFace MF(aOWire,Standard_True);
				if(MF.IsDone())
				{
						aNF = MF.Face();
						if(aNF.IsNull())
								return FALSE;
				}

				//下面依次对内环进行判断,主要是要保证环闭合,并且环的方向正确
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

						//判断内环的方向
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

				//获取最终的face
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
// 使用曲面分割平面，使用BrepFeat_splitShape
//

//启动和结束命令
int		ScCmdSplitPlane::	Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		NeedSelect(TRUE);
		if(HasSelFace())
		{
				SwitchState(S_CURVE,"选择分割曲线:");
		}
		else
		{
				SwitchState(S_FACE,"选择要分割的平面曲面:");
		}

		return 0;
}

int		ScCmdSplitPlane::	End()
{
		SelObj(0) = NULL;
		SelObj(1) = NULL;
		return ScCommand::End();
}

//点选方式下的消息响应函数
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
										 SwitchState(S_CURVE,"选择分割曲线:");
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

		//将aCur沿平面方向拉伸，生成曲面，和另一个曲面求交，获得交线
		//再使用交线进行分割
		gp_Dir zD = m_pln.Axis().Direction();
		double dH = 10;//取小一些

		//先对profile进行移动
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
				AfxMessageBox("生成中间面失败。");
				return FALSE;
		}

		//求交
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
				AfxMessageBox("没有交线.");
				return FALSE;
		}

		TopoDS_Shape aInterCurve = sec.Shape();
		if(aInterCurve.IsNull())
		{
				AfxMessageBox("没有交线.");
				return FALSE;
		}

		//分割曲面
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
				AfxMessageBox("曲面分割失败.");
				return FALSE;
		}

		//显示修改过的曲面
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
// 使用曲面分割曲面，使用BRepFeat_splitShape
//启动和结束命令
int		ScCmdSplitSurf::Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		NeedSelect(TRUE);
		if(HasSelFace())
		{
				SwitchState(S_TOOL,"选择一个工具曲面:");
		}
		else
		{
				SwitchState(S_FACE,"选择一个要分割的曲面");
		}

		return 0;
}

int		ScCmdSplitSurf::End()
{
		SelObj(0) = NULL;
		SelObj(1) = NULL;
		return ScCommand::End();
}

//点选方式下的消息响应函数
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
										SwitchState(S_TOOL,"选择一个工具曲面:");
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
								AfxMessageBox("两曲面不能相同.");
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

		//求交
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
				AfxMessageBox("没有交线.");
				return FALSE;
		}

		TopoDS_Shape aInterCurve = sec.Shape();
		if(aInterCurve.IsNull())
		{
				AfxMessageBox("没有交线.");
				return FALSE;
		}

		//分割曲面
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
				AfxMessageBox("曲面分割失败.");
				return FALSE;
		}

		//显示修改过的曲面
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
