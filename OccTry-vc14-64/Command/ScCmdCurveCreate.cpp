#include "stdafx.h"
#include <GeomLib.hxx>

#include "ScTools.h"
#include "ScCmdMgr.h"
#include "..\ScView.h"
#include ".\sccmdcurvecreate.h"

ScCmdCurveCreate::ScCmdCurveCreate(void)
{
}

ScCmdCurveCreate::~ScCmdCurveCreate(void)
{
}

/////////////////////////////////////////////////////////////
//
ScCmdLine::ScCmdLine()
{
}

ScCmdLine::~ScCmdLine()
{
}

//启动和结束命令
int		ScCmdLine::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SetState(LINE_POINT1);
	Prompt("输入起始点:");
	NeedSnap(TRUE);

	return SC_OK;
}

int		ScCmdLine::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdLine::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState != MINPUT_LEFT)
		return;

	switch(GetState())
	{
	case LINE_POINT1:
		{
			this->m_pnt1 = PixelToCSPlane(point.x,point.y,TRUE);

			SetState(LINE_POINT2);
			Prompt("输入第二点");
		}
		break;
	case LINE_POINT2:
		{
			NeedSnap(FALSE);
			Done();
		}
		break;
	default:
		break;
	}
}
	
//鼠标移动的响应函数
void	ScCmdLine::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case LINE_POINT1:
		{
//			SnapPoint(point.x,point.y,aView);
		}
		break;
	case LINE_POINT2:
		{//绘制对象
			RemoveCtxObj(m_aAIShape);

			//在删除了旧对象，创建临时对象之间捕捉，避免捕捉到临时对象的点。
//			SnapPoint(point.x,point.y,aView);
			gp_Pnt pnt2 = PixelToCSPlane(point.x,point.y,TRUE);

			//创建直线，显示
			BRepBuilderAPI_MakeEdge me(m_pnt1,pnt2);
			if(me.IsDone())
			{
				m_aAIShape = new AIS_Shape(me);
				m_AISContext->Display(m_aAIShape, Standard_True);
			}

		}
		break;
	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////
//多段线
ScCmdPolyLine::ScCmdPolyLine()
{
}

ScCmdPolyLine::~ScCmdPolyLine()
{
}

//启动和结束命令
int	ScCmdPolyLine::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(LINE_POINT_FIRST,"输入起始点:");
	NeedSnap(TRUE);

	return SC_OK;
}

int	ScCmdPolyLine::End()
{
	return ScCommand::End();
}

//
// 点选方式下的消息响应函数,右键结束。
//
void	ScCmdPolyLine::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		RemoveCtxObj(m_aAIShape);
		RemoveCtxObj(m_aTmpObj);

		TopoDS_Shape aNewShape = BuildNewShape();
		Display(m_aAIShape,aNewShape);
		Done();
	}
	else
	{
		switch(GetState())
		{
		case LINE_POINT_FIRST:
			{
				this->m_pntPrev = this->PixelToCSPlane(point.x,point.y);
				m_seqOfPnt.Append(m_pntPrev);

				SwitchState(LINE_POINT_NEXT,"输入下一点:");
			}
			break;
		case LINE_POINT_NEXT:
			{
				//
				this->m_pntPrev = this->PixelToCSPlane(point.x,point.y);
				m_seqOfPnt.Append(m_pntPrev);
				//
				RemoveCtxObj(m_aAIShape);
				RemoveCtxObj(m_aTmpObj);

				TopoDS_Shape aNewShape = BuildNewShape();
				Display(m_aAIShape,aNewShape);
			}
			break;
		default:
			break;
		}
	}
}

TopoDS_Shape	ScCmdPolyLine::BuildNewShape()
{
	TopoDS_Shape aS;
	int nLen = m_seqOfPnt.Length();
	if(nLen < 2)
		return aS;

	BRepBuilderAPI_MakeWire mw;
	for(int ix = 1;ix < nLen;ix++)
	{
		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_seqOfPnt.Value(ix),m_seqOfPnt.Value(ix + 1));
		if(!aE.IsNull())
		{
			mw.Add(aE);
		}
	}

	aS = mw.Wire();

	return aS;
}
	
//鼠标移动的响应函数
void	ScCmdPolyLine::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case LINE_POINT_FIRST:
		{
			//SnapPoint(point.x,point.y,aView);
		}
		break;
	case LINE_POINT_NEXT:
		{
			RemoveCtxObj(m_aTmpObj);

			//显示临时线
			//SnapPoint(point.x,point.y,aView);
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_pntPrev) < 0.001)
				return;
			
			//创建直线，显示
			BRepBuilderAPI_MakeEdge me(m_pntPrev,pnt);
			if(me.IsDone())
			{
				m_aTmpObj = new AIS_Shape(me);
				m_AISContext->Display(m_aTmpObj, Standard_True);
			}
		}
		break;
	default:
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////////////
//圆
ScCmdCircle::ScCmdCircle()
{
}

ScCmdCircle::~ScCmdCircle()
{
}

//启动和结束命令
int		ScCmdCircle::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SetState(CIRCLE_CENTER);
	Prompt("圆心:");
	NeedSnap(TRUE);

	return SC_OK;
}

int		ScCmdCircle::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdCircle::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case CIRCLE_CENTER:
		{
			m_ptCenter = PixelToCSPlane(point.x,point.y);

			SetState(CIRCLE_RADIUS);
			Prompt("半径:");
			m_bFirst = FALSE;
		}
		break;
	case CIRCLE_RADIUS:
		{
			Done();
		}
		break;
	default:
		break;
	}
}

//鼠标移动的响应函数
void	ScCmdCircle::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case CIRCLE_CENTER:
		{
			SnapPoint(point.x,point.y,aView);
		}
		break;
	case CIRCLE_RADIUS:
		{
			if(m_bFirst)
			{
				m_bFirst = FALSE;
			}
			else
			{
				RemoveCtxObj(m_aShape);
			}

			SnapPoint(point.x,point.y,aView);
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			m_dRadius = pnt.Distance(m_ptCenter);

			gp_Ax2 ax = m_pScView->CurrentAxis(FALSE);
			ax.SetLocation(m_ptCenter);
			gp_Circ circ(ax,m_dRadius);
			TopoDS_Edge aShape = BRepBuilderAPI_MakeEdge(circ);
			m_aShape = new AIS_Shape(aShape);
			m_AISContext->Display(m_aShape, Standard_True);
		}
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////
// 三点圆

//启动和结束命令
 int		ScCmdCircleOf3Point::Begin(Handle(AIS_InteractiveContext) aCtx)
 {
		 ScCommand::Begin(aCtx);

		 SwitchState(S_POINT1,"第一点:");
		 return 0;
 }

 int		ScCmdCircleOf3Point::	End()
 {
		 RemoveCtxObj(TmpObj(0));
		 return ScCommand::End();
 }

//点选方式下的消息响应函数
void		ScCmdCircleOf3Point::	InputEvent(const CPoint& point,int nState,
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
				case S_POINT1:
						{
								m_pnt1 = PixelToCSPlane(point.x,point.y);
								SwitchState(S_POINT2,"第二点:");
						}
						break;
				case S_POINT2:
						{
								gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
								if(pnt.Distance(m_pnt1) < Precision::Confusion())
										return;

								m_pnt2 = pnt;
								SwitchState(S_POINT3,"第三点:");
						}
						break;
				case S_POINT3:
						{
								gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
								if(pnt.Distance(m_pnt1) < Precision::Confusion() ||
										pnt.Distance(m_pnt2) < Precision::Confusion())
										return;

								if(MakeCirc(TRUE))
										Done();
								else
										End();
						}
						break;
				default:
						break;
				}
		}
}

//鼠标移动的响应函数
void		ScCmdCircleOf3Point::	MoveEvent(const CPoint& point,
				const Handle(V3d_View)& aView)
{
		switch(GetState())
		{
		case S_POINT1:
				{
						SnapPoint(point.x,point.y,aView);
				}
				break;
		case S_POINT2:
				{
						RemoveCtxObj(TmpObj(0));
						SnapPoint(point.x,point.y,aView);
						gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
						if(pnt.Distance(m_pnt1) < Precision::Confusion())
								return;

						m_pnt2 = pnt;
						MakeTmpLine();
				}
				break;
		case S_POINT3:
				{
						RemoveCtxObj(TmpObj(0));
						SnapPoint(point.x,point.y,aView);
						gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
						if(pnt.Distance(m_pnt1) < Precision::Confusion() ||
								pnt.Distance(m_pnt2) < Precision::Confusion())
								return;

						m_pnt3 = pnt;
						MakeCirc(FALSE);
				}
				break;
		default:
				break;
		}
}

//接收3维点
int		ScCmdCircleOf3Point::	AcceptInput(gp_Pnt& pnt) 
{
		return SC_OK; 
}

BOOL		ScCmdCircleOf3Point::	MakeTmpLine()
{
		if(m_pnt1.Distance(m_pnt2) < Precision::Confusion())
				return FALSE;

		
		TopoDS_Edge aE;
		try{
				aE = BRepBuilderAPI_MakeEdge(m_pnt1,m_pnt2);
		}catch(Standard_Failure){
				return FALSE;
		}
		if(aE.IsNull())
				return FALSE;

		Display(TmpObj(0),aE);

		return TRUE;
}

BOOL		ScCmdCircleOf3Point::	MakeCirc(BOOL bDone)
{
		Handle(Geom_Curve) aCirc;
		try{
				aCirc = GC_MakeCircle(m_pnt1,m_pnt2,m_pnt3).Value();//yxk20200104
		}catch(Standard_Failure){
				return FALSE;
		}

		TopoDS_Edge aE;
		try{
				aE = BRepBuilderAPI_MakeEdge(aCirc);
		}catch(Standard_Failure){
				return FALSE;
		}
		if(aE.IsNull())
				return FALSE;

		if(bDone)
		{
				Display(NewObj(0),aE);
				BeginUndo("make Circle");
				UndoAddNewObj(NewObj(0));
				EndUndo(TRUE);
		}
		else
		{
				Display(TmpObj(0),aE);
		}

		return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//圆弧
ScCmdArc::ScCmdArc()
{
}

ScCmdArc::~ScCmdArc()
{
}

//启动和结束命令
int		ScCmdArc::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SetState(ARC_CENTER);
	Prompt("输入圆心:");
	NeedSnap(TRUE);

	return 0;
}

int		ScCmdArc::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdArc::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case ARC_CENTER:
		{
			m_ptC = PixelToCSPlane(point.x,point.y);
			
			SwitchState(ARC_RADIUS,"获取半径:");
		}
		break;
	case ARC_RADIUS:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_ptC) < Precision::Confusion())
				return;

			RemoveCtxObj(m_aShape);
			m_pt1 = pnt;
			SwitchState(ARC_ANGLE,"获取角度:");
		}
		break;
	case ARC_ANGLE:
		{
			Done();
		}
		break;
	default:
		break;
	}
}
	
//鼠标移动的响应函数
void	ScCmdArc::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case ARC_CENTER:
		{
			SnapPoint(point.x,point.y,aView);
		}
		break;
	case ARC_RADIUS:
		{
			RemoveCtxObj(m_aShape);

			SnapPoint(point.x,point.y,aView);
			m_pt1 = PixelToCSPlane(point.x,point.y);

			TopoDS_Edge aE = BuildLine();
			Display(m_aShape,aE);
		}
		break;
	case ARC_ANGLE:
		{
			RemoveCtxObj(m_aShape);

			SnapPoint(point.x,point.y,aView);
			m_pt2 = PixelToCSPlane(point.x,point.y);
			TopoDS_Shape aS = BuildArc();
			Display(m_aShape,aS);
		}
		break;
	default:
		break;
	}
}

TopoDS_Edge		ScCmdArc::BuildLine()
{
	TopoDS_Edge aE;
	if(m_pt1.Distance(m_ptC) < Precision::Confusion())
		return aE;

	BRepBuilderAPI_MakeEdge me(m_ptC,m_pt1);
	if(me.IsDone())
	{
		aE = me.Edge();
	}

	return aE;
}

TopoDS_Shape	ScCmdArc::BuildArc()
{
	TopoDS_Shape aS;

	double dR = m_pt1.Distance(m_ptC);
	gp_Ax2 ax = m_pScView->CurrentAxis(FALSE);
	ax.SetLocation(m_ptC);
	gp_Circ circ(ax,dR);

	GC_MakeArcOfCircle marc(circ,m_pt1,m_pt2,TRUE);
	if(marc.IsDone())
	{
		Handle(Geom_TrimmedCurve) aCur = marc.Value();
		aS = BRepBuilderAPI_MakeEdge(aCur);
	}

	return aS;
}

////////////////////////////////////////////////////
// 圆心/两点

//启动和结束命令
int		ScCmdArcOfTwoPoint::Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		SwitchState(S_CENTER,"圆心:");
		NeedSnap(TRUE);

		return 0;
}

int		ScCmdArcOfTwoPoint::End()
{
		RemoveCtxObj(TmpObj(0));
		return ScCommand::End();
}

//点选方式下的消息响应函数
void		ScCmdArcOfTwoPoint::InputEvent(const CPoint& point,int nState,
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
				case S_CENTER:
						{
								m_ptC = PixelToCSPlane(point.x,point.y);
								SwitchState(S_POINT1,"第一点:");
						}
						break;
				case S_POINT1:
						{
								gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
								if(pnt.Distance(m_ptC) < Precision::Confusion())
										return;

								m_pt1 = pnt;
								SwitchState(S_POINT2,"第二点:");
						}
						break;
				case S_POINT2:
						{
								gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
								if(pnt.Distance(m_ptC) < Precision::Confusion() ||
										pnt.Distance(m_pt1) < Precision::Confusion())
										return;

								m_pt2 = pnt;
								if(MakeArc(TRUE))
										Done();
								else
										End();
						}
						break;
				default:
						break;
				}
		}
}

//鼠标移动的响应函数
void		ScCmdArcOfTwoPoint::MoveEvent(const CPoint& point,
				const Handle(V3d_View)& aView)
{
		switch(GetState())
		{
		case S_CENTER:
				{
						SnapPoint(point.x,point.y,aView);
				}
				break;
		case S_POINT1:
				{
						gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
						if(pnt.Distance(m_ptC) < Precision::Confusion())
								return;

						RemoveCtxObj(TmpObj(0));
						SnapPoint(point.x,point.y,aView);
						m_pt1 = pnt;
						MakeLine();
				}
				break;
		case S_POINT2:
				{
						gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
						if(pnt.Distance(m_ptC) < Precision::Confusion() ||
								pnt.Distance(m_pt1) < Precision::Confusion())
								return;

						RemoveCtxObj(TmpObj(0));
						SnapPoint(point.x,point.y,aView);
						m_pt2 = pnt;
						MakeArc(FALSE);
				}
				break;
		default:
				break;
		}
}

//接收浮点数
 int		ScCmdArcOfTwoPoint::AcceptInput(gp_Pnt& pnt) 
 {
		 return SC_OK; 
 }

BOOL				ScCmdArcOfTwoPoint::MakeLine()
{
		TopoDS_Edge aE;
		try{
				aE = BRepBuilderAPI_MakeEdge(m_ptC,m_pt1);
				if(aE.IsNull())
						return FALSE;
		}catch(Standard_Failure){
				return FALSE;
		}

		Display(TmpObj(0),aE);

		return TRUE;
}

BOOL				ScCmdArcOfTwoPoint::MakeArc(BOOL bDone)
{
		TopoDS_Shape aS;

		double dR = m_pt1.Distance(m_ptC);
		gp_Ax2 ax = m_pScView->CurrentAxis(FALSE);
		ax.SetLocation(m_ptC);
		gp_Circ circ(ax,dR);

		try{
				GC_MakeArcOfCircle marc(circ,m_pt1,m_pt2,TRUE);
				if(marc.IsDone())
				{
						Handle(Geom_TrimmedCurve) aCur = marc.Value();
						aS = BRepBuilderAPI_MakeEdge(aCur);
						if(aS.IsNull())
								return FALSE;
				}
				else
						return FALSE;
		}catch(Standard_Failure){
				return FALSE;
		}

		if(bDone)
		{
				Display(NewObj(0),aS);
				BeginUndo("two point arc");
				UndoAddNewObj(NewObj(0));
				EndUndo(TRUE);
		}
		else
		{
				Display(TmpObj(0),aS);
		}

		return TRUE;
}


////////////////////////////////////////////////////
// 椭圆对角

//启动和结束命令
int	ScCmdEllipseOfCorner::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSnap(TRUE);
	SwitchState(S_POINT1,"输入第一个点:");
	return 0;
}

int	ScCmdEllipseOfCorner::End()
{
	RemoveCtxObj(TmpObj(0));
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdEllipseOfCorner::InputEvent(const CPoint& point,int nState,
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
		case S_POINT1:
			{
				m_pnt1 = this->PixelToCSPlane(point.x,point.y);
				SwitchState(S_POINT2,"输入另一个点:");
			}
			break;
		case S_POINT2:
			{
				RemoveCtxObj(TmpObj(0));

				TopoDS_Shape aS;
				if(BuildShape(aS))
				{
					Display(NewObj(0),aS);

					if(!NewObj(0).IsNull())
					{
						BeginUndo("Ellipse by corner");
						UndoAddNewObj(NewObj(0));
						EndUndo(TRUE);
					}

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
	}
}
	
//鼠标移动的响应函数
void	ScCmdEllipseOfCorner::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_POINT1:
		{
			SnapPoint(point.x,point.y,aView);
		}
		break;
	case S_POINT2:
		{
			RemoveCtxObj(TmpObj(0));

			SnapPoint(point.x,point.y,aView);
			gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
			//避免距离太近
			if(pnt.Distance(m_pnt1) < Precision::Confusion())
				return;
			m_pnt2 = pnt;

			TopoDS_Shape aS;
			if(BuildShape(aS))
			{
				Display(TmpObj(0),aS);
			}
		}
		break;
	default:
		break;
	}
}

int		ScCmdEllipseOfCorner::AcceptInput(gp_Pnt& pnt) 
{ 
	return SC_OK; 
}

//创建成功
BOOL	ScCmdEllipseOfCorner::BuildShape(TopoDS_Shape& aShape)
{
	//避免距离太近
	if(m_pnt2.Distance(m_pnt1) < Precision::Confusion())
		return FALSE;

	if(fabs(m_pnt1.X() - m_pnt2.X()) < Precision::Confusion() ||
		fabs(m_pnt1.Y() - m_pnt2.Y()) < Precision::Confusion())
		return FALSE;

	//计算
	gp_Pnt center,s1,s2;
	center.SetX((m_pnt1.X() + m_pnt2.X()) * 0.5);
	center.SetY((m_pnt1.Y() + m_pnt2.Y()) * 0.5);
	center.SetZ((m_pnt1.Z() + m_pnt2.Z()) * 0.5);

	s1 = gp_Pnt(m_pnt1.X(),center.Y(),m_pnt1.Z());
	s2 = gp_Pnt(center.X(),m_pnt2.Y(),m_pnt1.Z());
	Handle(Geom_Curve) aCur;
	if(fabs(s1.X() - center.X()) > fabs(s2.Y() - center.Y()))
	{
		aCur = GC_MakeEllipse(s1,s2,center).Value();//yxk20200104
	}
	else
	{
		aCur = GC_MakeEllipse(s2,s1,center).Value();//yxk20200104
	}

	if(aCur.IsNull())
		return FALSE;

	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(aCur);
	if(aE.IsNull())
		return FALSE;

	aShape = aE;

	return TRUE;
}
////////////////////////////////////////////////
// 矩形

//启动和结束命令
int	ScCmdRectangle::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSnap(TRUE);
	SwitchState(S_POINT1,"输入第一个点:");
	return 0;
}
int	ScCmdRectangle::End()
{
	RemoveCtxObj(TmpObj(0));
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdRectangle::InputEvent(const CPoint& point,int nState,
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
		case S_POINT1:
			{
				m_pnt1 = this->PixelToCSPlane(point.x,point.y);
				SwitchState(S_POINT2,"输入另一个点:");
			}
			break;
		case S_POINT2:
			{
				RemoveCtxObj(TmpObj(0));

				TopoDS_Shape aS;
				if(BuildRectangle(aS))
				{
					Display(NewObj(0),aS);

					if(!NewObj(0).IsNull())
					{
						BeginUndo("Rectangle");
						UndoAddNewObj(NewObj(0));
						EndUndo(TRUE);
					}

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
	}
}
	
//鼠标移动的响应函数
void	ScCmdRectangle::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_POINT1:
		{
			SnapPoint(point.x,point.y,aView);
		}
		break;
	case S_POINT2:
		{
			RemoveCtxObj(TmpObj(0));

			SnapPoint(point.x,point.y,aView);
			gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
			//避免距离太近
			if(pnt.Distance(m_pnt1) < Precision::Confusion())
				return;
			m_pnt2 = pnt;

			TopoDS_Shape aS;
			if(BuildRectangle(aS))
			{
				Display(TmpObj(0),aS);
			}
		}
		break;
	default:
		break;
	}
}

//接收浮点数
int	ScCmdRectangle::AcceptInput(gp_Pnt& pnt) 
{ 
	switch(GetState())
	{
	case S_POINT1:
		break;
	case S_POINT2:
		break;
	default:
		break;
	}
	return SC_OK; 
}

//创建矩形成功
BOOL	ScCmdRectangle::BuildRectangle(TopoDS_Shape& aShape)
{
	//避免距离太近
	if(m_pnt2.Distance(m_pnt1) < Precision::Confusion())
		return FALSE;

	if(fabs(m_pnt1.X() - m_pnt2.X()) < Precision::Confusion() ||
		fabs(m_pnt1.Y() - m_pnt2.Y()) < Precision::Confusion())
		return FALSE;

	gp_Pnt pnt3(m_pnt1.X(),m_pnt2.Y(),m_pnt1.Z());
	gp_Pnt pnt4(m_pnt2.X(),m_pnt1.Y(),m_pnt1.Z());
	TopoDS_Edge aE1 = BRepBuilderAPI_MakeEdge(m_pnt1,pnt3);
	TopoDS_Edge aE2 = BRepBuilderAPI_MakeEdge(pnt3,m_pnt2);
	TopoDS_Edge aE3 = BRepBuilderAPI_MakeEdge(m_pnt2,pnt4);
	TopoDS_Edge aE4 = BRepBuilderAPI_MakeEdge(pnt4,m_pnt1);

	TopoDS_Wire aW = BRepBuilderAPI_MakeWire(aE1,aE2,aE3,aE4);
	if(aW.IsNull())
		return FALSE;
	
	aShape = aW;

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////
//样条曲线
ScCmdSpline::ScCmdSpline()
{

}

ScCmdSpline::~ScCmdSpline()
{
}

void	ScCmdSpline::SetSplType(int nType)
{
	ASSERT(nType == BSpline_Interp || nType == BSpline_Approx);
	this->m_nType = nType;
}

//启动和结束命令
int		ScCmdSpline::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SetState(SPL_FIRST_POINT);
	Prompt("输入起始点");
	NeedSnap(TRUE);

	return 0;
}

int		ScCmdSpline::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdSpline::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		Handle(Geom_BSplineCurve) aCur = BuildBSpline();
		if(!aCur.IsNull())
		{
			BRepBuilderAPI_MakeEdge me(aCur);
			if(me.IsDone())
			{
				TopoDS_Edge aE = me.Edge();
				//显示
				RemoveCtxObj(m_aShape);
				m_aShape = new AIS_Shape(aE);
				m_AISContext->Display(m_aShape, Standard_True);
			}
		}

		Done();
	}
	else
	{
		switch(GetState())
		{
		case SPL_FIRST_POINT:
			{
				gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
				this->m_seqOfPnt.Append(pnt);
				
				SetState(SPL_NEXT_POINT);
				Prompt("输入下一点:");
			}
			break;
		case SPL_NEXT_POINT:
			{
				//
				gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
				this->m_seqOfPnt.Append(pnt);
			}
			break;
		default:
			break;
		}
	}
}
	
//鼠标移动的响应函数
void	ScCmdSpline::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case SPL_FIRST_POINT:
		{
			SnapPoint(point.x,point.y,aView);
		}
		break;
	case SPL_NEXT_POINT:
		{
			RemoveCtxObj(m_aShape);

			SnapPoint(point.x,point.y,aView);
			gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
			//避免距离太近
			gp_Pnt lpnt = m_seqOfPnt.Value(m_seqOfPnt.Length());
			if(pnt.Distance(lpnt) < Precision::Confusion())
				return;

			this->m_seqOfPnt.Append(pnt);

			Handle(Geom_BSplineCurve) aCur = BuildBSpline();
			if(!aCur.IsNull())
			{
				BRepBuilderAPI_MakeEdge me(aCur);
				if(me.IsDone())
				{
					TopoDS_Edge aE = me.Edge();
					//显示
					
					m_aShape = new AIS_Shape(aE);
					m_AISContext->Display(m_aShape, Standard_True);
				}
			}

			this->m_seqOfPnt.Remove(m_seqOfPnt.Length());
		}
		break;
	default:
		break;
	}
}

//
// 根据不同类型，生成样条
//
Handle(Geom_BSplineCurve)	ScCmdSpline::BuildBSpline()
{
	Handle(Geom_BSplineCurve) aCur;
	int nLen = m_seqOfPnt.Length();

	if(m_nType == BSpline_Interp)
	{
		Handle(TColgp_HArray1OfPnt) harray =                                                    
			new TColgp_HArray1OfPnt (1,nLen); // sizing harray
		for(int idx = 1;idx <= nLen;idx++)
		{
			gp_Pnt pt = m_seqOfPnt.Value(idx);
			harray->SetValue(idx,pt);
		}

		GeomAPI_Interpolate anInterp(harray,Standard_False,
			Precision::Approximation());  
		anInterp.Perform();                                                              
	
		if (anInterp.IsDone())   
		{
			aCur = anInterp.Curve(); 
		}
	}
	else
	{
		TColgp_Array1OfPnt harray(1,nLen);
		for(int idx = 1;idx <= nLen;idx++)
		{
			gp_Pnt pt = m_seqOfPnt.Value(idx);
			harray.SetValue(idx,pt);
		}
		GeomAPI_PointsToBSpline ptbspl(harray);
		aCur = ptbspl.Curve();
	}

	return aCur;
}

//////////////////////////////////////////////////////////////////////////
// Bezier曲线的创建

//启动和结束命令
int	ScCmdBezierCurve::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_FIRST_PNT,"输入控制点(个数在2到25之间)，右键结束.");

	return 0;
}

int	ScCmdBezierCurve::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdBezierCurve::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(GetState() == S_NEXT_PNT)
		{
			if(m_seqOfPnt.Length() > 1)
			{
				if(MakeCurve(FALSE))
					Done();
				else
					End();
			}
		}
		else
			End();
	}
	else
	{
		switch(GetState())
		{
		case S_FIRST_PNT:
			{
				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				m_seqOfPnt.Append(pnt);

				SwitchState(S_NEXT_PNT,"输入控制点(个数在2到25之间)，右键结束.");
			}
			break;
		case S_NEXT_PNT:
			{
				if(m_seqOfPnt.Length() >= Geom_BezierCurve::MaxDegree())
				{
					AfxMessageBox("点数太多.");
					End();
				}
				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_seqOfPnt.Value(m_seqOfPnt.Length())) < 1e-6)
					return;
				m_seqOfPnt.Append(pnt);
			}
			break;
		default:
			break;
		}
	}
}

//鼠标移动的响应函数
void	ScCmdBezierCurve::MoveEvent(const CPoint& point,
		const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_NEXT_PNT:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_seqOfPnt.Value(m_seqOfPnt.Length())) < 1e-6)
				return;

			m_seqOfPnt.Append(pnt);//添加临时点

			MakeCurve(TRUE);
			ShowFrame();

			m_seqOfPnt.Remove(m_seqOfPnt.Length());//去除临时点
		}
		break;
	default:
		break;
	}
}

BOOL	ScCmdBezierCurve::MakeCurve(BOOL bTmp)
{
	TColgp_Array1OfPnt arpnt(1,m_seqOfPnt.Length());
	for(int ix = 1;ix <= m_seqOfPnt.Length();ix ++)
	{
		arpnt.SetValue(ix,m_seqOfPnt.Value(ix));
	}

	TopoDS_Edge aE;
	try
	{
		Handle(Geom_BezierCurve) aBCur = new Geom_BezierCurve(arpnt);
		aE = BRepBuilderAPI_MakeEdge(aBCur);
		if(aE.IsNull())
			return FALSE;
	}
	catch (Standard_Failure)
	{
		return FALSE;
	}
	
	RemoveCtxObj(TmpObj(1));
	if(bTmp)
	{
		Display(TmpObj(1),aE);
	}
	else
	{
		Display(NewObj(0),aE);
		BeginUndo("Bezier curve");
		UndoAddNewObj(NewObj(0));
		EndUndo(TRUE);
	}

	return TRUE;
}

BOOL	ScCmdBezierCurve::ShowFrame()
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Wire aW;
	TopoDS_Edge aE;
	BRepBuilderAPI_MakeWire mw;

	for(int ix = 1;ix <= m_seqOfPnt.Length() - 1;ix ++)
	{
		try{
			aE = BRepBuilderAPI_MakeEdge(m_seqOfPnt.Value(ix),m_seqOfPnt.Value(ix + 1));
			if(aE.IsNull())
				continue;
		}catch(Standard_Failure){
			continue;
		}
		mw.Add(aE);
	}

	aW = mw.Wire();
	if(aW.IsNull())
		return FALSE;

	Display(TmpObj(0),aW);

	return TRUE;
}

