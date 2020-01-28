#include "StdAfx.h"

#include <GeomLib.hxx>

#include "ScTools.h"
#include "ScCmdMgr.h"
#include "ScBRepLib.h"
#include "..\ScView.h"
#include ".\sccmdcurve.h"

ScCmdCurve::ScCmdCurve(void)
{
}

ScCmdCurve::~ScCmdCurve(void)
{
}


///////////////////////////////////////////////////////////////
//

//启动和结束命令
int	ScCmdCurveSewing::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_CURVES,"选择多条连续的曲线，右键结束:");
	NeedMultiSelect(TRUE);

	return 0;
}
	
int	ScCmdCurveSewing::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdCurveSewing::InputEvent(const CPoint& point,int nState,
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
		//可以检查是否是curve。暂不做。
	}
}

TopoDS_Shape	ScCmdCurveSewing::BuildNewShape()
{
	TopoDS_Shape aS;
	TopTools_ListOfShape asList;
	m_AISContext->InitCurrent();
	while(m_AISContext->MoreCurrent())
	{
		Handle(AIS_InteractiveObject) aObj = m_AISContext->Current();
		Handle(AIS_Shape) asObj = Handle(AIS_Shape)::DownCast(aObj);
		if(asObj.IsNull())
			return aS;

		TopoDS_Shape aShape = asObj->Shape();
		if((aShape.ShapeType() != TopAbs_EDGE) && 
			(aShape.ShapeType() != TopAbs_WIRE))
			return aS;

		asList.Append(aShape);
		this->m_seqOfObj.Append(aObj);//记录，以便删除

		m_AISContext->NextCurrent();
	}

	int nLen = asList.Extent();
	if(nLen < 2)
		return aS;

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
		return aS;
//	if(!wireFix.FixReorder() || !wireFix.FixConnected())
//		return aS;

	TopoDS_Wire aWire = wireFix.WireAPIMake();
	if(aWire.IsNull())
		return aS;

	aS = aWire;

	//删除旧的
	int nC = m_seqOfObj.Length();
	for(int ic = 1;ic <= nC;ic++)
	{
		m_AISContext->Remove(m_seqOfObj.Value(ic), Standard_True);
	}

	return aS;
}

//////////////////////////////////////////////////////////////////////////
// 曲线取消组合

//启动和结束命令
int	ScCmdCurveWireToEdge::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);
	NeedSelect(TRUE);

	if(HasSelWire())
	{
		if(DoWireToEdge())
			Done();
		else
			End();
	}

	return 0;
}

int	ScCmdCurveWireToEdge::End()
{
	return ScCommand::End();
}

	//点选方式下的消息响应函数
void	ScCmdCurveWireToEdge::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
		End();
	else
	{
		if(HasSelWire())
		{
			if(DoWireToEdge())
				Done();
			else
				End();
		}
	}
}

BOOL	ScCmdCurveWireToEdge::DoWireToEdge()
{
	TopoDS_Wire aW = TopoDS::Wire(GetObjShape(SelObj(0)));
	if(aW.IsNull())
		return FALSE;

	BeginUndo("Wire To Edge");
	m_AISContext->Remove(SelObj(0),Standard_False);
	UndoAddDelObj(SelObj(0));
	//新的edge
	TopExp_Explorer ex;
	int idx = 0;
	for(ex.Init(aW,TopAbs_EDGE);ex.More();ex.Next())
	{
		TopoDS_Edge aE = TopoDS::Edge(ex.Current());
		if(aE.IsNull())
			continue;

		Display(NewObj(idx),aE,FALSE);
		UndoAddNewObj(NewObj(idx));
	}
	EndUndo(TRUE);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////
//

//启动和结束命令
int	ScCmdBrepProjection::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_CURVE,"选择一条要投影的曲线:");
	NeedSelect(TRUE);
	return 0;
}

int	ScCmdBrepProjection::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdBrepProjection::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
		return;
	}
	//
	switch(GetState())
	{
	case S_CURVE:
		{
			m_AISContext->InitCurrent();
			if(m_AISContext->MoreCurrent())
			{
				Handle(AIS_InteractiveObject) aObj = m_AISContext->Current();
				Handle(AIS_Shape) asObj = Handle(AIS_Shape)::DownCast(aObj);
				if(!asObj.IsNull())
				{
					TopoDS_Shape aShape = asObj->Shape();
					if((aShape.ShapeType() == TopAbs_EDGE) ||
						(aShape.ShapeType() == TopAbs_WIRE))
					{
						m_aCurveObj = aObj;
						//
						m_aProjShape = aShape;
						//
						//m_AISContext->HilightWithColor(aObj,Quantity_NOC_BLUE1);//yxk20191208
						//
						SwitchState(S_SHAPE,"选择要投影到的对象:");
					}

				}
			}
		}
		break;
	case S_SHAPE:
		{
			m_AISContext->InitCurrent();
			if(m_AISContext->MoreCurrent())
			{
				Handle(AIS_InteractiveObject) aObj = m_AISContext->Current();
				if(aObj == m_aCurveObj)
					return;

				Handle(AIS_Shape) asObj = Handle(AIS_Shape)::DownCast(aObj);
				if(!asObj.IsNull())
				{
					TopoDS_Shape aShape = asObj->Shape();
					if((aShape.ShapeType() == TopAbs_FACE) ||
						(aShape.ShapeType() == TopAbs_SHELL) ||
						(aShape.ShapeType() == TopAbs_SOLID) ||
						(aShape.ShapeType() == TopAbs_COMPOUND))
					{
						//
						m_aShape = aShape;
						//
						//m_AISContext->HilightWithColor(aObj,Quantity_NOC_GOLD1);//yxk20191208
						//
						TopoDS_Shape aS = BuildNewShape();
						Display(m_aNewObj,aS);
						Done();
					}

				}
			}
		}
		break;
	default:
		break;
	}
}

TopoDS_Shape	ScCmdBrepProjection::BuildNewShape()
{
	TopoDS_Shape aNewShape;

	//
	BRepAlgo_NormalProjection proj(m_aShape);
	try{
		proj.SetParams(1e-4,1e-4,GeomAbs_C1,14,16);
		proj.Add(m_aProjShape);
		proj.SetLimit(Standard_True);
		proj.Build();

		aNewShape = proj.Projection();

	}catch(Standard_Failure)
	{
		AfxMessageBox("投影失败");
		return aNewShape;
	}
	return aNewShape;
}

//启动和结束命令
int	ScCmdInterCurve::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SHAPE1,"选择第一个对象:");
	NeedMultiSelect(TRUE);

	return 0;
}

int	ScCmdInterCurve::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdInterCurve::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SHAPE1:
		{
			TopoDS_Shape aShape;
			if(SEL_OK == GetCurrentShape(aShape))
			{
				if((aShape.ShapeType() != TopAbs_VERTEX) &&
					(aShape.ShapeType() != TopAbs_EDGE) && 
					(aShape.ShapeType() != TopAbs_WIRE))
				{
					m_aShape1 = aShape;

					SwitchState(S_SHAPE2,"选择第二个对象");
				}
			}
		}
		break;
	case S_SHAPE2:
		{
			InitSelect();
			while(MoreSelect())
			{
				TopoDS_Shape aShape = SelectedShape();
				if(aShape.IsNull() || aShape.IsSame(m_aShape1))	
					continue;
	
				if((aShape.ShapeType() != TopAbs_VERTEX) &&
					(aShape.ShapeType() != TopAbs_EDGE) && 
					(aShape.ShapeType() != TopAbs_WIRE))
				{
					m_aShape2 = aShape;

					BuildInterCurve();
					Done();
				}
			}
		}
		break;
	default:
		break;
	}
}

void	ScCmdInterCurve::BuildInterCurve()
{
	BRepAlgoAPI_Section sec(m_aShape1,m_aShape2,Standard_False);
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
		return;
	}

	TopoDS_Shape aEdgeList = sec.Shape();
	if(aEdgeList.IsNull())
	{
		AfxMessageBox("没有交线.");
		return;
	}

	TopExp_Explorer ex;
	int ic = 0;
	for(ex.Init(aEdgeList,TopAbs_EDGE);ex.More();ex.Next())
	{
		TopoDS_Edge aE = TopoDS::Edge(ex.Current());
		Display(NewObj(ic),aE);
		ic++;
	}
}

////////////////////////////////////////////////
// 点生成

ScCmdPoint::ScCmdPoint(BOOL bPlane)
{
	this->m_bPlanePoint = bPlane;
}

//启动和结束命令
int	ScCmdPoint::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	Prompt("左键点击输入点，右键结束");
	NeedSnap(TRUE);
	m_nIdx = 0;

	return 0;
}

int	ScCmdPoint::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdPoint::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		Done();
	}
	else
	{
		gp_Pnt pnt;
		if(this->m_bPlanePoint)
		{
			pnt = this->PixelToCSPlane(point.x,point.y);
		}
		else
		{
			pnt = this->PixelToModel(point.x,point.y);
		}

		TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(pnt);
		Display(NewObj(m_nIdx),aV);
		m_nIdx ++;
	}
}

/////////////////////////////////////////////////////
// 延伸到点

//启动和结束命令
int	ScCmdExtendToPoint::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_CURVE,"选择一条曲线：");
	NeedSelect(TRUE);

	return 0;
}

int	ScCmdExtendToPoint::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdExtendToPoint::InputEvent(const CPoint& point,int nState,
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
					if(!aS.IsNull() && aS.ShapeType() == TopAbs_EDGE)
					{
						DelObj(0) = SelectedObj(FALSE);

						SwitchState(S_POINT,"输入或选取要延伸到的点:");
						NeedSnap(TRUE);
					}
				}
			}
			break;
		case S_POINT:
			{
				BOOL bSel = FALSE;
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape(FALSE);
					if(!aS.IsNull() && aS.ShapeType() == TopAbs_VERTEX)
					{
						m_extPnt = BRep_Tool::Pnt(TopoDS::Vertex(aS));
						bSel = TRUE;
					}
				}
				if(!bSel)
				{
					m_extPnt = this->PixelToCSPlane(point.x,point.y);
				}

				NeedSnap(FALSE);
				NeedSelect(FALSE);
				SwitchState(S_DIR,"原则要延伸的一端:");
			}
			break;
		case S_DIR:
			{
				gp_Pnt pnt;
				double t;
				TopoDS_Shape aS = GetObjShape(DelObj(0));
				if(this->m_pScView->GetEdgeNearestPoint(TopoDS::Edge(aS),point.x,
					point.y,t,pnt))
				{
					BRepAdaptor_Curve bac(TopoDS::Edge(aS));
					double mt = (bac.FirstParameter() + bac.LastParameter())/2.0;
					if(t < mt)
						m_bAfter = FALSE;
					else
						m_bAfter = TRUE;

					SwitchState(S_CONT,"输入连续性:(1,2,3)");
					NeedInteger("连续性(1,2,3):");
				}
			}
			break;
		default:
			break;
		}
	}
}

//接收整数
int	ScCmdExtendToPoint::AcceptInput(int nVal)
{
	switch(GetState())
	{
	case S_CONT:
		{
			if(nVal < 1 || nVal > 3)
				return -1;

			//进行延伸
			double dF,dL;
			TopoDS_Shape aS = GetObjShape(DelObj(0));
			Handle(Geom_Curve) aC = BRep_Tool::Curve(TopoDS::Edge(aS),dF,dL);
			if(aC->IsPeriodic() || aC->IsClosed())
			{
				DelObj(0) = NULL;
				return 0;
			}
			Handle(Geom_TrimmedCurve) aTC = new Geom_TrimmedCurve(aC,dF,dL);
			GeomLib::ExtendCurveToPoint(aTC,m_extPnt,nVal,m_bAfter);
			//
			TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(aTC);
			m_AISContext->Remove(DelObj(0), Standard_True);
			Display(NewObj(0),aE);

			Done();
		}
		break;
	default:
		break;
	}
	return 0;
}

//////////////////////////////////////////////////////////
// 偏移曲线,使用BRepOffsetAPI_MakeOffset类。
	
//启动和结束命令
int	ScCmdCurveOffset::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_CURVE,"选择一条曲线.");
	NeedSelect(TRUE);

	return 0;
}

int	ScCmdCurveOffset::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdCurveOffset::InputEvent(const CPoint& point,int nState,
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
						((aS.ShapeType() == TopAbs_EDGE) ||
						(aS.ShapeType() == TopAbs_WIRE)))
					{
						TmpObj(0) = SelectedObj(FALSE);

						NeedSelect(FALSE);
						SwitchState(S_OFFVAL,"输入偏移值:");
						NeedDouble("偏移值:");
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

//接收偏移距离
int	ScCmdCurveOffset::AcceptInput(double dVal)
{
	switch(GetState())
	{
	case S_OFFVAL:
		{
			if(fabs(dVal) < Precision::Confusion())
			{
				End();
				return 0;
			}

			//
			TopoDS_Shape aS = GetObjShape(TmpObj(0));
			TmpObj(0) = NULL;
			TopoDS_Wire aW;
			if(aS.ShapeType() == TopAbs_EDGE)
			{
				aW = BRepBuilderAPI_MakeWire(TopoDS::Edge(aS));
			}
			else
			{
				aW = TopoDS::Wire(aS);
			}

			if(aW.IsNull())
			{
				End();
				return 0;
			}

			BRepOffsetAPI_MakeOffset mo(aW);
			try
			{
				mo.Perform(dVal);
			}
			catch(Standard_Failure)
			{
				End();
				return 0;
			}

			TopoDS_Shape aNS = mo.Shape();
			if(!aNS.IsNull())
			{
				Display(NewObj(0),aNS);
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

/////////////////////////////////////////////////////////////
// 简单基类,主要提供共用方法。

//是否选择了face，结果保存在tmpObj(0)中。
BOOL	ScCmdOneFace::HasSelFace()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(!aS.IsNull() && (aS.ShapeType() == TopAbs_FACE))
		{
			TmpObj(0) = SelectedObj();

			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////
// face的边界

//启动和结束命令
int	ScCmdCurveFaceBoundary::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	this->m_nNewCurves = 0;

	NeedSelect(TRUE);

	if(HasSelFace())
	{
		SwitchState(S_CURVE,"选择边界曲线:");
		//m_AISContext->OpenLocalContext();//yxk20191207
		m_AISContext->Activate(TmpObj(0),2);
	}
	else
	{
		SwitchState(S_FACE,"选择一个曲面:");
	}

	return 0;
}

int	ScCmdCurveFaceBoundary::End()
{
	//m_AISContext->CloseLocalContex();//yxk20191207
	TmpObj(0) = NULL;

	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdCurveFaceBoundary::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(this->m_nNewCurves)
			Done();
		else
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
					SwitchState(S_CURVE,"选择边界曲线:");
					//m_AISContext->OpenLocalContext();//yxk20191207
					m_AISContext->Activate(TmpObj(0),2);
				}
			}
			break;
		case S_CURVE:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Edge aE = TopoDS::Edge(SelectedShape(FALSE));
					if(!aE.IsNull())
					{
						//进行检查
						ShapeAnalysis_Edge sae;
						double maxdev;
						sae.CheckSameParameter(aE,maxdev);
						DTRACE("\n edge maxdev %f",maxdev);


						double df,dl;
						Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE,df,dl);
						aCur = Handle(Geom_Curve)::DownCast(aCur->Copy());
						//先不考虑方向
						Handle(Geom_TrimmedCurve) aTC = new Geom_TrimmedCurve(aCur,df,dl);
						TopoDS_Edge aNE = BRepBuilderAPI_MakeEdge(aTC);

						//
						double dm = (df + dl)/2.0;
						gp_Pnt pv = aCur->Value(df);
						gp_Pnt pv1 = aCur->Value(dm);
						gp_Pnt pv2 = aCur->Value(dl);
						DTRACE("\n f[%f,%f,%f],m[%f,%f,%f],l[%f,%f,%f]",pv.X(),pv.Y(),pv.Z(),
							pv1.X(),pv1.Y(),pv1.Z(),pv2.X(),pv2.Y(),pv2.Z());
						

						//使用参数曲线创建曲线
						TopoDS_Edge aNE2;
						TopoDS_Face aF = TopoDS::Face(GetObjShape(TmpObj(0)));
						Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aF);
						Handle(Geom2d_Curve) aCur2d = BRep_Tool::CurveOnSurface(aE,aF,df,dl);
						if(!aCur2d.IsNull() && !aSurf.IsNull())
						{
							aCur2d = Handle(Geom2d_Curve)::DownCast(aCur2d->Copy());
							Handle(Geom2d_TrimmedCurve) aTC2d = new Geom2d_TrimmedCurve(aCur2d,df,dl);
							aNE2 = BRepBuilderAPI_MakeEdge(aTC2d,aSurf);

							gp_Pnt2d ppt;
							ppt = aCur2d->Value(df);
							pv = aSurf->Value(ppt.X(),ppt.Y());
							ppt = aCur2d->Value(dm);
							pv1 = aSurf->Value(ppt.X(),ppt.Y());
							ppt = aCur2d->Value(dl);
							pv2 = aSurf->Value(ppt.X(),ppt.Y());
							DTRACE("\n surf f[%f,%f,%f],m[%f,%f,%f],l[%f,%f,%f]",pv.X(),pv.Y(),pv.Z(),
							pv1.X(),pv1.Y(),pv1.Z(),pv2.X(),pv2.Y(),pv2.Z());
						}

						//m_AISContext->CloseLocalContex();//yxk20191207

						Display(NewObj(0),aNE);
						Display(NewObj(1),aNE2);

						m_nNewCurves = 2;

						TmpObj(0) = NULL;

						Done();
					}
				}
			}
			break;
		default:
			break;
		}
	}
}


//////////////////////////////////////////////////////
//Face的全部3d边界
BOOL	ScCmdFaceBoundary3d::BuildShape()//生成对应shape。
{
	BOOL bRet = ScCmdFaceBase::BuildShape();

	if(bRet)
		Done();
	else
		End();

	return bRet;
}

BOOL	ScCmdFaceBoundary3d::BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace)//生成对应shape。
{
	double df,dl;
	Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE,df,dl);
	if(aCur.IsNull())
		return FALSE;

	aCur = Handle(Geom_Curve)::DownCast(aCur->Copy());
	
	Handle(Geom_TrimmedCurve) aTC = new Geom_TrimmedCurve(aCur,df,dl);
	TopoDS_Edge aNE = BRepBuilderAPI_MakeEdge(aTC);
	
	if(!aNE.IsNull())
	{
		Display(NewObj(-1),aNE);
		return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////
//Face的全部参数曲线生成边界
BOOL	ScCmdFaceBoundary2d::BuildShape()//生成对应shape。
{
	BOOL bRet = ScCmdFaceBase::BuildShape();

	if(bRet)
		Done();
	else
		End();

	return bRet;
}

BOOL	ScCmdFaceBoundary2d::BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace)//生成对应shape。
{
	//使用参数曲线创建曲线
	TopoDS_Edge aNE2;
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);

	double df,dl;
	Handle(Geom2d_Curve) aCur2d = BRep_Tool::CurveOnSurface(aE,aFace,df,dl);
	if(!aCur2d.IsNull() && !aSurf.IsNull())
	{
		aCur2d = Handle(Geom2d_Curve)::DownCast(aCur2d->Copy());
		Handle(Geom2d_TrimmedCurve) aTC2d = new Geom2d_TrimmedCurve(aCur2d,df,dl);
		aNE2 = BRepBuilderAPI_MakeEdge(aTC2d,aSurf);

		if(!aNE2.IsNull())
		{
			Display(NewObj(-1),aNE2);
			return TRUE;
		}
	}

	return FALSE;
}

//Face的全部裁剪环的参数曲线
BOOL	ScCmdFaceParamLoop::BuildShape()//生成对应shape。
{
	BOOL bRet = ScCmdFaceBase::BuildShape();

	if(bRet)
		Done();
	else
		End();

	return bRet;
}

BOOL	ScCmdFaceParamLoop::BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace)
{
	//使用参数曲线创建曲线
	TopoDS_Edge aNE;
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);

	double df,dl;
	Handle(Geom2d_Curve) aCur2d = BRep_Tool::CurveOnSurface(aE,aFace,df,dl);
	if(!aCur2d.IsNull() && !aSurf.IsNull())
	{
		aCur2d = Handle(Geom2d_Curve)::DownCast(aCur2d->Copy());
		Handle(Geom2d_TrimmedCurve) aTC2d = new Geom2d_TrimmedCurve(aCur2d,df,dl);
		//转成3d曲线
		Handle(Geom_Curve) aCur3d = GeomLib::To3d(gp_Ax2(gp_Pnt(0.0,0.0,0.0),gp_Dir(0.0,0.0,1.0)),aTC2d);
		if(!aCur3d.IsNull())
		{
			aNE = BRepBuilderAPI_MakeEdge(aCur3d);

			if(!aNE.IsNull())
			{
				Display(NewObj(-1),aNE);
				return TRUE;
			}
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// 曲线间的交点 BRepExtram_DistShapeShape类

//启动和结束命令
int		ScCmdCurveInterPoint::Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		NeedSelect(TRUE);
		if(!HasSelCurve(TRUE))
		{
				SwitchState(S_CURVE1,"选择第一条曲线:");
		}
		else
		{
				SwitchState(S_CURVE2,"选择第二条曲线:");
		}

		return 0;
}

int		ScCmdCurveInterPoint::End()
{
		return ScCommand::End();
}

//点选方式下的消息响应函数
 void		ScCmdCurveInterPoint::InputEvent(const CPoint& point,int nState,
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
				 case S_CURVE1:
						 {
								 if(HasSelCurve(TRUE))
								 {
										SwitchState(S_CURVE2,"选择第二条曲线:");
								 }
						 }
						 break;
				 case S_CURVE2:
						 {
								 if(HasSelCurve(FALSE))
								 {
										 if(DoInter())
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

BOOL			ScCmdCurveInterPoint::HasSelCurve(BOOL bFirst)
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull() && (aS.ShapeType() == TopAbs_EDGE ||
						aS.ShapeType() == TopAbs_WIRE))
				{
						if(bFirst)
						{
								SelObj(0) = SelectedObj(FALSE);

								return TRUE;
						}
						else
						{
								if(SelObj(0) != SelectedObj(FALSE))
								{
										SelObj(1) = SelectedObj(FALSE);

										return TRUE;
								}
						}
				}
		}
		return FALSE;
}

BOOL			ScCmdCurveInterPoint::DoInter()
{
		TopoDS_Shape aS1 = GetObjShape(SelObj(0));
		TopoDS_Shape aS2 = GetObjShape(SelObj(1));

		BRepExtrema_DistShapeShape inter(aS1,aS2);
		try
		{
				inter.Perform();
		}
		catch (Standard_Failure)
		{
				return FALSE;
		}

		int nPnt = inter.NbSolution();
		if(nPnt == 0)
		{
				AfxMessageBox("没有交点.");
				return FALSE;
		}

		int nC = 0;
		for(int ix = 1;ix <= nPnt;ix ++)
		{
				gp_Pnt p1 = inter.PointOnShape1(ix);
				gp_Pnt p2 = inter.PointOnShape2(ix);
				if(p1.Distance(p2) < 1e-7)
				{
						TopoDS_Vertex  v = BRepBuilderAPI_MakeVertex(p1);
						Display(NewObj(nC),v,FALSE);
						nC ++;
				}
		}

		if(nC == 0)
		{
				AfxMessageBox("没有交点.");
				return FALSE;
		}

		BeginUndo("Curve inter point");
		for(int ix = 0;ix < nC;ix ++)
		{
				if(!NewObj(ix).IsNull())
						UndoAddNewObj(NewObj(ix));
		}
		EndUndo(TRUE);

		return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 曲线活动标架的显式

//启动和结束命令
int	ScCmdCurveFrameBase::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelCurve())
	{
		BuildFrame();
		SwitchState(S_FRAME,"右键退出.");
	}
	else
	{
		SwitchState(S_CURVE,"选择一条曲线");
	}

	return 0;
}

int	ScCmdCurveFrameBase::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdCurveFrameBase::InputEvent(const CPoint& point,int nState,
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
		case S_CURVE:
			{
				if(HasSelCurve())
				{
					BuildFrame();
					SwitchState(S_FRAME,"右键退出.");
				}
			}
			break;
		case S_FRAME:
			break;
		}
	}
}


BOOL	ScCmdCurveFrameBase::HasSelCurve()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsEdge(aS))
		{
			Handle(Geom_Curve) aCur = BRep_Tool::Curve(TopoDS::Edge(aS),m_df,m_dl);
			if(aCur.IsNull())
				return FALSE;

			m_aSelCurve = aCur;
			SelObj(0) = SelectedObj();

			return TRUE;
		}
	}
	return FALSE;
}

BOOL	ScCmdCurveFrameBase::BuildFrame()
{
	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	GeomAdaptor_Curve AC(m_aSelCurve);
	double curlen = GCPnts_AbscissaPoint::Length(AC,m_df,m_dl);
	curlen /= 20.0;
	if(curlen > 20.0) curlen = 20.0;//大小

	int nSamps = 50;
	double t = m_df;
	double dstep = (m_dl - m_df)/nSamps;
	gp_Dir T,N,B;
	gp_Pnt pnt,ptN;
	
	for(int ix = 0;ix <= nSamps;ix ++)
	{
		t = m_df + ix * dstep;
		pnt = m_aSelCurve->Value(t);
		//计算frame
		if(!CalcFrame(t,T,N,B))
			return FALSE;

		//创建对象
		ptN.SetCoord(pnt.X() + curlen * N.X(),pnt.Y() + curlen * N.Y(),pnt.Z() + curlen * N.Z());
		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(pnt,ptN);
		if(!aE.IsNull())
			BB.Add(aC,aE);
		//箭头
		double arSize = curlen / 10.0;
		TopoDS_Shape aCS = BRepPrimAPI_MakeCone(gp_Ax2(ptN,gp_Dir(N)),arSize,0,arSize * 4.0);
		if(!aCS.IsNull())
			BB.Add(aC,aCS);
	}

	Display(TmpObj(0),aC);

	return TRUE;
}

//
BOOL	ScCmdCurveFrenetFrame::CalcFrame(double t,gp_Dir& T,gp_Dir& N,gp_Dir& B)
{
	Handle(Adaptor3d_HCurve) ghc = new GeomAdaptor_HCurve(m_aSelCurve);
	GeomFill_Frenet frenet;
	frenet.SetCurve(ghc);
	gp_Vec vt,vn,vb;

	frenet.D0(t,vt,vn,vb);
	T = gp_Dir(vt);
	N = gp_Dir(vn);
	B = gp_Dir(vb);

	return TRUE;
}

//旋转标架1
BOOL	ScCmdCurveRotateFrame1::BuildFrame()
{
	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	GeomAdaptor_Curve AC(m_aSelCurve);
	double curlen = GCPnts_AbscissaPoint::Length(AC,m_df,m_dl);
	curlen /= 20.0;
	if(curlen > 20.0) curlen = 20.0;//大小

	int nSamps = 50;
	double t = m_df;
	double dstep = (m_dl - m_df)/nSamps;
	gp_Dir T0,N0,B0,T,N,B;
	gp_Pnt pnt,ptN;
	gp_Vec vt,vn,vb;

	for(int ix = 0;ix <= nSamps;ix ++)
	{
		t = m_df + ix * dstep;

		//计算frame
		if(ix == 0)
		{
			Handle(Adaptor3d_HCurve) ghc = new GeomAdaptor_HCurve(m_aSelCurve);
			GeomFill_Frenet frenet;
			frenet.SetCurve(ghc);

			pnt = m_aSelCurve->Value(t);
			frenet.D0(t,vt,vn,vb);
			T = gp_Dir(vt);
			N = gp_Dir(vn);
			B = gp_Dir(vb);	
		}
		else
		{
			//第二种方法:
			m_aSelCurve->D1(t,pnt,vt);
			if(vt.SquareMagnitude() < 1e-10)
				continue;

			T = gp_Dir(vt);
			//求T0×T1=A
			gp_Dir A;
			if(T0.IsParallel(T,0.001))
			{
				T = T0;
				N = N0;
				B = B0;
			}
			else
			{
				A = T0.Crossed(T);
				//计算旋转矩阵
				double sqx = A.X() * A.X(),sqy = A.Y() * A.Y(),sqz = A.Z() * A.Z();
				double cos = T0.Dot(T),cosl = 1 - cos;
				double xycosl = A.X() * A.Y() * cosl;
				double yzcosl = A.Y() * A.Z() * cosl;
				double zxcosl = A.X() * A.Z() * cosl;
				double sin = sqrt(1 - cos * cos),xsin = A.X() * sin,
					ysin = A.Y() * sin,zsin = A.Z() * sin;
				
				gp_Mat m0(sqx + (1 - sqx) * cos,xycosl + zsin,zxcosl - ysin,
					xycosl - zsin,sqy + (1 - sqy) * cos, yzcosl + xsin,
					zxcosl + ysin,yzcosl - xsin,sqz + (1 - sqz) * cos);

				//计算新的N
				N.SetX(m0.Row(1).Dot(N0.XYZ()));
				N.SetY(m0.Row(2).Dot(N0.XYZ()));
				N.SetZ(m0.Row(3).Dot(N0.XYZ()));

				B = T.Crossed(N);
			}
		}

		//创建对象
		ptN.SetCoord(pnt.X() + curlen * N.X(),pnt.Y() + curlen * N.Y(),pnt.Z() + curlen * N.Z());
		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(pnt,ptN);
		if(!aE.IsNull())
			BB.Add(aC,aE);
		//箭头
		double arSize = curlen / 10.0;
		TopoDS_Shape aCS = BRepPrimAPI_MakeCone(gp_Ax2(ptN,gp_Dir(N)),arSize,0,arSize * 4.0);
		if(!aCS.IsNull())
			BB.Add(aC,aCS);

		T0 = T;
		N0 = N;
		B0 = B;
	}

	Display(TmpObj(0),aC);
	return TRUE;
}

//旋转标架2
BOOL	ScCmdCurveRotateFrame2::BuildFrame()
{
	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	GeomAdaptor_Curve AC(m_aSelCurve);
	double curlen = GCPnts_AbscissaPoint::Length(AC,m_df,m_dl);
	curlen /= 20.0;
	if(curlen > 20.0) curlen = 20.0;//大小

	int nSamps = 50;
	double t = m_df;
	double dstep = (m_dl - m_df)/nSamps;
	gp_Dir T0,N0,B0,T,N,B;
	gp_Pnt pnt,ptN;
	gp_Vec vt,vn,vb;

	for(int ix = 0;ix <= nSamps;ix ++)
	{
		t = m_df + ix * dstep;

		//计算frame
		if(ix == 0)
		{
			Handle(Adaptor3d_HCurve) ghc = new GeomAdaptor_HCurve(m_aSelCurve);
			GeomFill_Frenet frenet;
			frenet.SetCurve(ghc);

			pnt = m_aSelCurve->Value(t);
			frenet.D0(t,vt,vn,vb);
			T = gp_Dir(vt);
			N = gp_Dir(vn);
			B = gp_Dir(vb);	
		}
		else
		{
			//第二种方法:
			m_aSelCurve->D1(t,pnt,vt);
			if(vt.SquareMagnitude() < 1e-10)
				continue;

			T = gp_Dir(vt);
			//求T0×T1=A
			gp_Dir A;
			A = T0.Crossed(T);
			if(A.XYZ().Modulus() < 1e-10)
			{
				T = T0;
				N = N0;
				B = B0;
			}
			else
			{
				gp_Dir A0,A1;
				A0 = T0.Crossed(A);
				A1 = T.Crossed(A);
				//计算矩阵
				gp_Mat m0,m1;
				m0.SetCols(T0.XYZ(),A.XYZ(),A0.XYZ());
				m1.SetCols(T.XYZ(),A.XYZ(),A1.XYZ());
				m0.Transpose();
				m0.Multiplied(m1);

				//计算新的N
				N.SetX(m0.Row(1).Dot(N0.XYZ()));
				N.SetY(m0.Row(2).Dot(N0.XYZ()));
				N.SetZ(m0.Row(3).Dot(N0.XYZ()));

				B = T.Crossed(N);
			}
		}

		//创建对象
		ptN.SetCoord(pnt.X() + curlen * N.X(),pnt.Y() + curlen * N.Y(),pnt.Z() + curlen * N.Z());
		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(pnt,ptN);
		if(!aE.IsNull())
			BB.Add(aC,aE);
		//箭头
		double arSize = curlen / 10.0;
		TopoDS_Shape aCS = BRepPrimAPI_MakeCone(gp_Ax2(ptN,gp_Dir(N)),arSize,0,arSize * 4.0);
		if(!aCS.IsNull())
			BB.Add(aC,aCS);

		T0 = T;
		N0 = N;
		B0 = B;
	}

	Display(TmpObj(0),aC);
	return TRUE;
}

//sloan标架
BOOL	ScCmdCurveSloanFrame::BuildFrame()
{
	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	GeomAdaptor_Curve AC(m_aSelCurve);
	double curlen = GCPnts_AbscissaPoint::Length(AC,m_df,m_dl);
	curlen /= 20.0;
	if(curlen > 20.0) curlen = 20.0;//大小

	int nSamps = 50;
	double t = m_df;
	double dstep = (m_dl - m_df)/nSamps;
	gp_Dir T0,N0,B0,T,N,B;
	gp_Pnt pnt,ptN;
	gp_Vec vt,vn,vb;

	for(int ix = 0;ix <= nSamps;ix ++)
	{
		t = m_df + ix * dstep;
		
		//计算frame
		if(ix == 0)
		{
			Handle(Adaptor3d_HCurve) ghc = new GeomAdaptor_HCurve(m_aSelCurve);
			GeomFill_Frenet frenet;
			frenet.SetCurve(ghc);
			
			pnt = m_aSelCurve->Value(t);
			frenet.D0(t,vt,vn,vb);
			T = gp_Dir(vt);
			N = gp_Dir(vn);
			B = gp_Dir(vb);	
		}
		else
		{
			//sloan方法
			m_aSelCurve->D1(t,pnt,vt);
			if(vt.SquareMagnitude() < 1e-10)
				continue;
			T = gp_Dir(vt);
			N = B0.Crossed(T);
			if(N.XYZ().Modulus() < 1e-10)
				continue;
			B = T.Crossed(N);
		}
		
		//创建对象
		ptN.SetCoord(pnt.X() + curlen * N.X(),pnt.Y() + curlen * N.Y(),pnt.Z() + curlen * N.Z());
		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(pnt,ptN);
		if(!aE.IsNull())
			BB.Add(aC,aE);
		//箭头
		double arSize = curlen / 10.0;
		TopoDS_Shape aCS = BRepPrimAPI_MakeCone(gp_Ax2(ptN,gp_Dir(N)),arSize,0,arSize * 4.0);
		if(!aCS.IsNull())
			BB.Add(aC,aCS);

		T0 = T;
		N0 = N;
		B0 = B;
	}

	Display(TmpObj(0),aC);
	return TRUE;
}

//双反射标架
BOOL	ScCmdCurveDblRefFrame::BuildFrame()
{
	return TRUE;
}