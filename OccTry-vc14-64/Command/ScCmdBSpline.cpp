#include "StdAfx.h"
#include "ScBRepLib.h"
#include "sccmdbspline.h"

ScCmdBSpline::ScCmdBSpline(void)
{
}

ScCmdBSpline::~ScCmdBSpline(void)
{
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// 工具类,提供一些基本的功能

//增加曲线的控制点到一定数目
BOOL	TcBSplLib::IncCurveCtrlPnts(Handle(Geom_BSplineCurve) &aCur,int ncpts,
									double df,double dl)
{
	if(aCur.IsNull())
		return FALSE;

	if(aCur->NbPoles() >= ncpts)
		return TRUE;

	int nInc = ncpts - aCur->NbPoles();
	double u,dstep = (dl - df)/(nInc + 1);
	TColStd_Array1OfReal knots(1,nInc);
	TColStd_Array1OfInteger mults(1,nInc);

	for(int ix = 1;ix <= nInc;ix ++)
	{
		u = df + ix * dstep;
		knots.SetValue(ix,u);
		mults.SetValue(ix,1);
	}

	aCur->InsertKnots(knots,mults,0.0,Standard_True);

	return TRUE;
}

//增加曲面的控制点到一定数目
BOOL	TcBSplLib::IncSurfCtrlPnts(Handle(Geom_BSplineSurface) &aSurf,int nucpts,int nvcpts)
{
	if(aSurf.IsNull())
		return FALSE;

	if(aSurf->NbUPoles() >= nucpts && aSurf->NbVPoles() >= nvcpts)
		return TRUE;

	double u,v,u1,u2,v1,v2,dstep;
	aSurf->Bounds(u1,u2,v1,v2);

	int nUInc = nucpts - aSurf->NbUPoles(),nVInc = nvcpts - aSurf->NbVPoles();
	if(nUInc > 0)
	{
		dstep = (u2 - u1)/(nUInc + 1);
		TColStd_Array1OfReal knots(1,nUInc);
		TColStd_Array1OfInteger mults(1,nUInc);

		for(int ix = 1;ix <= nUInc;ix ++)
		{
			u = u1 + ix * dstep;
			knots.SetValue(ix,u);
			mults.SetValue(ix,1);
		}

		aSurf->InsertUKnots(knots,mults);
	}
	if(nVInc > 0)
	{
		dstep = (v2 - v1)/(nVInc + 1);
		TColStd_Array1OfReal knots(1,nVInc);
		TColStd_Array1OfInteger mults(1,nVInc);

		for(int ix = 1;ix <= nVInc;ix ++)
		{
			v = v1 + ix * dstep;
			knots.SetValue(ix,v);
			mults.SetValue(ix,1);
		}

		aSurf->InsertVKnots(knots,mults);
	}

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////
// 曲线转为BSpline

//启动和结束命令
int		ScCmdCurveToBSpline::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelEdgeOrWire())
	{
		if(DoConvert())
			Done();
		else
			End();
	}

	return 0;
}

int		ScCmdCurveToBSpline::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdCurveToBSpline::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
	}
	else
	{
		if(HasSelEdgeOrWire())
		{
			if(DoConvert())
				Done();
			else
				End();
		}
	}
}

BOOL	ScCmdCurveToBSpline::DoConvert()
{
	TopoDS_Shape aS = GetObjShape(SelObj(0));
	if(aS.IsNull())
		return FALSE;

	//转换
	BRepBuilderAPI_NurbsConvert ns;
	ns.Perform(aS);
	if(!ns.IsDone())
		return FALSE;

	TopoDS_Shape aNS = ns.Shape();
	//
	m_AISContext->Remove(SelObj(0),Standard_False);
	Display(NewObj(0),aNS);
	BeginUndo("Curve to bspline");
	UndoAddDelObj(SelObj(0));
	UndoAddNewObj(NewObj(0));
	EndUndo(TRUE);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// 曲面转为BSpline

//启动和结束命令
int	ScCmdSurfToBSpline::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelSurf())
	{
		if(DoConvert())
			Done();
		else
			End();
	}
	return 0;
}

int	ScCmdSurfToBSpline::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdSurfToBSpline::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
	}
	else
	{
		if(HasSelSurf())
		{
			if(DoConvert())
				Done();
			else
				End();
		}
	}
}

BOOL	ScCmdSurfToBSpline::HasSelSurf()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(!aS.IsNull() && (ScBRepLib::IsFace(aS) || ScBRepLib::IsShell(aS)))
		{
			SelObj(0) = SelectedObj(FALSE);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL	ScCmdSurfToBSpline::DoConvert()
{
	TopoDS_Shape aS = GetObjShape(SelObj(0));
	if(aS.IsNull())
		return FALSE;

	//转换
	BRepBuilderAPI_NurbsConvert ns;
	ns.Perform(aS);
	if(!ns.IsDone())
		return FALSE;

	TopoDS_Shape aNS = ns.Shape();
	//
	m_AISContext->Remove(SelObj(0),Standard_False);
	Display(NewObj(0),aNS);
	BeginUndo("Curve to bspline");
	UndoAddDelObj(SelObj(0));
	UndoAddNewObj(NewObj(0));
	EndUndo(TRUE);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// 实体转为BSpline
//启动和结束命令
int	ScCmdSolidToBSpline::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelSolid())
	{
		if(DoConvert())
			Done();
		else
			End();
	}

	return 0;
}
int	ScCmdSolidToBSpline::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdSolidToBSpline::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
	}
	else
	{
		if(HasSelSolid())
		{
			if(DoConvert())
				Done();
			else
				End();
		}
	}
}

BOOL	ScCmdSolidToBSpline::HasSelSolid()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(!aS.IsNull() && (ScBRepLib::IsSolid(aS) || 
			ScBRepLib::IsCompSolid(aS) || ScBRepLib::IsCompound(aS)))
		{
			SelObj(0) = SelectedObj(FALSE);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL	ScCmdSolidToBSpline::DoConvert()
{
	TopoDS_Shape aS = GetObjShape(SelObj(0));
	if(aS.IsNull())
		return FALSE;

	//转换
	BRepBuilderAPI_NurbsConvert ns;
	ns.Perform(aS);
	if(!ns.IsDone())
		return FALSE;

	TopoDS_Shape aNS = ns.Shape();
	//
	m_AISContext->Remove(SelObj(0),Standard_False);
	Display(NewObj(0),aNS);
	BeginUndo("Curve to bspline");
	UndoAddDelObj(SelObj(0));
	UndoAddNewObj(NewObj(0));
	EndUndo(TRUE);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 曲面控制点编辑

//bezier曲线的控制点编辑

//启动和结束命令
int	ScCmdBezierCurveEditByCP::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelCurve())
	{
		ShowCtrlPnts();
		SwitchState(S_SELPNT,"选择控制点进行编辑:");
	}
	else
	{
		SwitchState(S_CURVE,"选择一条要编辑的Bezier曲线:");
	}

	return 0;
}

int	ScCmdBezierCurveEditByCP::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdBezierCurveEditByCP::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		RemoveCtxObj(TmpObj(0));
		RemoveCtxObj(TmpObj(1));
		//
		if(DoEdit(FALSE,TRUE))
			Done();
		else
		{
			End();
		}
	}
	else
	{
		switch(GetState())
		{
		case S_CURVE:
			{
				if(HasSelCurve())
				{
					ShowCtrlPnts();
					SwitchState(S_SELPNT,"选择控制点进行编辑:");
				}
			}
			break;
		case S_SELPNT:
			{
				InitSelect();
				if(MoreSelect())
				{
					m_selPnt = BRep_Tool::Pnt(TopoDS::Vertex(SelectedShape(FALSE)));
					//m_AISContext->CloseLocalContex();//yxk20191207
					SwitchState(S_CHGPNT,"修改控制点:");
				}
			}
			break;
		case S_CHGPNT:
			{
				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_selPnt) < 1e-6)
					return;

				m_chgPnt = pnt;
				DoEdit(FALSE,FALSE);
				ShowCtrlPnts();
				SwitchState(S_SELPNT,"选择控制点进行编辑:");
			}
			break;
		default:
			break;
		}
	}
}

//鼠标移动的响应函数
void	ScCmdBezierCurveEditByCP::MoveEvent(const CPoint& point,
		const Handle(V3d_View)& aView)
{
	if(GetState() == S_CHGPNT)
	{
		gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
		if(pnt.Distance(m_selPnt) < 1e-6)
			return;

		m_chgPnt = pnt;
		DoEdit(TRUE,FALSE);
		ShowCtrlPnts();
		//m_AISContext->CloseLocalContex();//yxk20191207
	}
}

BOOL	ScCmdBezierCurveEditByCP::HasSelCurve()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsEdge(aS))
		{
			double df,dl;
			Handle(Geom_Curve) aCur = BRep_Tool::Curve(TopoDS::Edge(aS),df,dl);
			if(aCur.IsNull())
				return FALSE;

			if(!aCur->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
				return FALSE;

			//重要注意：如果是trimmed curve，需要取出片段，转换为对应完整curve进行编辑。

			SelObj(0) = SelectedObj();
			m_tmpCur = Handle(Geom_BezierCurve)::DownCast(aCur->Copy());
			m_midCur = Handle(Geom_BezierCurve)::DownCast(aCur->Copy());

			return TRUE;
		}
	}
	return FALSE;
}

BOOL	ScCmdBezierCurveEditByCP::ShowCtrlPnts()
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	gp_Pnt p1,p2;
	TopoDS_Edge aE;
	int nLen = m_tmpCur->NbPoles();
	for(int ix = 1;ix < nLen;ix ++)
	{
		p1 = m_tmpCur->Pole(ix);
		p2 = m_tmpCur->Pole(ix + 1);
		aE = BRepBuilderAPI_MakeEdge(p1,p2);
		if(aE.IsNull())
			continue;
		BB.Add(aC,aE);
	}

	Display(TmpObj(0),aC);

	//m_AISContext->OpenLocalContext();//yxk20191207
	m_AISContext->Activate(TmpObj(0),1);

	return TRUE;
}

BOOL	ScCmdBezierCurveEditByCP::DoEdit(BOOL bTmp,BOOL bDone)
{
	if(m_chgPnt.Distance(m_selPnt) < 1e-6)
		return FALSE;

	RemoveCtxObj(TmpObj(1));

	gp_Pnt pnt;
	for(int ix = 1;ix <= m_tmpCur->NbPoles();ix ++)
	{
		pnt = m_midCur->Pole(ix);
		if(pnt.Distance(m_selPnt) < 1e-6)
		{
			m_tmpCur->SetPole(ix,m_chgPnt);
			if(!bTmp)
				m_midCur->SetPole(ix,m_chgPnt);

			break;
		}
	}
	
	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_tmpCur);
	if(bDone)
	{
		m_AISContext->Remove(SelObj(0),Standard_False);
		Display(NewObj(0),aE);
		BeginUndo("Bezier curve edit.");
		UndoAddNewObj(NewObj(0));
		UndoAddDelObj(SelObj(0));
		EndUndo(TRUE);
	}
	else
	{
		Display(TmpObj(1),aE);
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//启动和结束命令
int	ScCmdBSplSurfEditByCP::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	m_bChged = FALSE;
	NeedMultiSelect(TRUE);
	if(HasSelSurf())
	{
		ShowControlPnts(TRUE);
		SwitchState(S_SELPNT,"选择要修改的控制点,右键结束选择:");
	}
	else
	{
		SwitchState(S_SURF,"选择一个BSpline曲面:");
	}

	return 0;
}

int	ScCmdBSplSurfEditByCP::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdBSplSurfEditByCP::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(GetState() == S_SELPNT)
		{
			InitSelect();
			if(MoreSelect())
			{
				//
				m_seqOfV.Clear();
				while(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape();
					m_seqOfV.Append(aS);
				}

				//m_AISContext->CloseLocalContex();//yxk20191207
				SwitchState(S_BASPNT,"选择改变基点:");
			}
			else
			{
				//
				//m_AISContext->CloseLocalContex();//yxk20191207
				if(m_bChged)
				{
					RemoveCtxObj(TmpObj(1));
					RemoveCtxObj(TmpObj(0));

					m_AISContext->Remove(SelObj(0),Standard_False);
					TopoDS_Face aF = BRepBuilderAPI_MakeFace(m_splSurf, Precision::Confusion());//yxk20200104
					Display(NewObj(1),aF);
					
					BeginUndo("BSpline Surface Edit by control points");
					UndoAddDelObj(SelObj(0));
					UndoAddNewObj(NewObj(0));
					EndUndo(TRUE);
					
					Done();
				}
				else
					End();
			}
		}
		else
		{
			End();
		}
	}
	else
	{
		//
		switch(GetState())
		{
		case S_SURF:
			{
				if(HasSelSurf())
				{
					ShowControlPnts(TRUE);
					SwitchState(S_SELPNT,"选择要修改的控制点,右键结束选择:");
				}
			}
			break;
		case S_SELPNT:
			break;
		case S_BASPNT:
			{
				m_basPnt = PixelToModel(point.x,point.y);
				SwitchState(S_CHGPNT,"输入改变后位置:");
			}
			break;
		case S_CHGPNT:
			{
				gp_Pnt pnt = PixelToModel(point.x,point.y);
				if(pnt.Distance(m_basPnt) < 1e-6)
					return;

				m_chgPnt = pnt;
				ChangeSurf(FALSE);

				ShowControlPnts(TRUE);
				SwitchState(S_SELPNT,"选择要修改的控制点,右键结束选择:");
			}
			break;
		default:
			break;
		}
	}
}

//鼠标移动的响应函数
void	ScCmdBSplSurfEditByCP::MoveEvent(const CPoint& point,
								  const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SURF:
		break;
	case S_SELPNT:
		break;
	case S_BASPNT:
		break;
	case S_CHGPNT:
		{
			gp_Pnt pnt = PixelToModel(point.x,point.y);
			if(pnt.Distance(m_basPnt) < 1e-6)
				return;

			m_chgPnt = pnt;
			ChangeSurf(TRUE);

			ShowControlPnts(FALSE);
		}
		break;
	default:
		break;
	}
}


BOOL	ScCmdBSplSurfEditByCP::HasSelSurf()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsFace(aS))
		{
			Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(aS));
			if(aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
			{
				SelObj(0) = SelectedObj(FALSE);

				//记录临时的
				m_splSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf->Copy());
				m_midSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf->Copy());

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL	ScCmdBSplSurfEditByCP::ShowControlPnts(BOOL bSel)
{
	//显示临时曲面的控制点
	if(m_splSurf.IsNull())
		return FALSE;

	//m_AISContext->CloseLocalContex();//yxk20191207
	RemoveCtxObj(TmpObj(0));

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	int ix,jx;
	for(ix = 1;ix <= m_splSurf->NbUPoles();ix ++)
	{
		for(jx = 1;jx <= m_splSurf->NbVPoles();jx ++)
		{
			gp_Pnt pnt = m_splSurf->Pole(ix,jx);

			TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(pnt);
			BB.Add(aC,aV);
		}
	}

	Display(TmpObj(0),aC);

	//允许选择点
	if(bSel)
	{
		//m_AISContext->OpenLocalContext();//yxk20191207
		m_AISContext->Activate(TmpObj(0),1);//选择点
	}

	return TRUE;
}

BOOL	ScCmdBSplSurfEditByCP::InChange(const gp_Pnt& pnt)
{
	gp_Pnt tp;
	for(int ix = 1;ix <= m_seqOfV.Length();ix ++)
	{
		tp = BRep_Tool::Pnt(TopoDS::Vertex(m_seqOfV.Value(ix)));
		if(tp.Distance(pnt) < 1e-10)
			return TRUE;
	}
	return FALSE;
}


BOOL	ScCmdBSplSurfEditByCP::ChangeSurf(BOOL bTmp)
{
	if(m_seqOfV.IsEmpty())
		return FALSE;

	gp_Vec vchg(m_chgPnt.X() - m_basPnt.X(),m_chgPnt.Y() - m_basPnt.Y(),
		m_chgPnt.Z() - m_basPnt.Z());

	int ix,jx;
	for(ix = 1;ix <= m_splSurf->NbUPoles();ix ++)
	{
		for(jx = 1;jx <= m_splSurf->NbVPoles();jx ++)
		{
			gp_Pnt pnt = m_midSurf->Pole(ix,jx);//中间曲面
			
			if(InChange(pnt))
			{
				pnt.SetCoord(pnt.X() + vchg.X(),pnt.Y() + vchg.Y(),
					pnt.Z() + vchg.Z());
				
				m_splSurf->SetPole(ix,jx,pnt);
				if(!bTmp)
				{
					m_bChged = TRUE;
					m_midSurf->SetPole(ix,jx,pnt);
				}
			}
			else
			{
				m_splSurf->SetPole(ix,jx,pnt);
			}
		}
	}

	RemoveCtxObj(TmpObj(1));
	TopoDS_Face aF = BRepBuilderAPI_MakeFace(m_splSurf, Precision::Confusion());//yxk20200104
	Display(TmpObj(1),aF);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// BSpline曲线升阶

//启动和结束命令
int	ScCmdBSplCurveIncDegree::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelObj())
	{
		SwitchState(S_DEGREE,"输入要提升到的阶数[3-25]:");
		CString szMsg;
		szMsg.Format("新的阶数(当前阶数%d):",this->m_nOldDegree);
		NeedDouble(szMsg);
	}
	else
	{
		SwitchState(S_OBJ,"选择一个BSpline曲线:");
	}

	return 0;
}

int	ScCmdBSplCurveIncDegree::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void		ScCmdBSplCurveIncDegree::InputEvent(const CPoint& point,int nState,
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
		case S_OBJ:
			{
				if(HasSelObj())
				{
					SwitchState(S_DEGREE,"输入要提升到的阶数[3-25]:");
					CString szMsg;
					szMsg.Format("新的阶数(当前阶数%d):",this->m_nOldDegree);
					NeedInteger(szMsg);
				}
			}
			break;
		case S_DEGREE:
			break;
		default:
			break;
		}
	}
}

//接收浮点数
int		ScCmdBSplCurveIncDegree::AcceptInput(int nVal)
{
	if(GetState() == S_DEGREE)
	{
		if(nVal <= m_nOldDegree || nVal >= 25)
		{
			End();
			return 0;
		}

		if(DoIncDegree(nVal))
			Done();
		else
			End();
	}

	return 0;
}

BOOL	ScCmdBSplCurveIncDegree::HasSelObj()
{
	if(HasSelEdge())
	{
		TopoDS_Edge aE = TopoDS::Edge(GetObjShape(SelObj(0)));
		double df,dl;
		Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE,df,dl);
		if(aCur.IsNull())
			return FALSE;

		if(!aCur->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
			return FALSE;

		Handle(Geom_BSplineCurve) abCur = Handle(Geom_BSplineCurve)::DownCast(aCur);
		m_nOldDegree = abCur->Degree();

		return TRUE;
	}

	return FALSE;
}

BOOL	ScCmdBSplCurveIncDegree::DoIncDegree(int nDegree)
{
	TopoDS_Edge aE = TopoDS::Edge(GetObjShape(SelObj(0)));
	double df,dl;
	Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE,df,dl);
	if(aCur.IsNull())
		return FALSE;

	Handle(Geom_BSplineCurve) abCur = Handle(Geom_BSplineCurve)::DownCast(aCur->Copy());
	abCur->IncreaseDegree(nDegree);

	TopoDS_Edge aNE = BRepBuilderAPI_MakeEdge(abCur);
	if(aNE.IsNull())
		return FALSE;

	m_AISContext->Remove(SelObj(0),Standard_False);
	Display(NewObj(0),aNE);

	BeginUndo("BSpline Curve Increase Degree");
	UndoAddDelObj(SelObj(0));
	UndoAddNewObj(NewObj(0));
	EndUndo(TRUE);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// bspline曲面升阶

//启动和结束命令
int	ScCmdBSplSurfIncDegree::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelObj())
	{
		SwitchState(S_UDEGREE,"输入提升的U的阶数:");
		CString szMsg;
		szMsg.Format("输入新的U阶数(当前阶数%d):",m_nOldUDegree);
		NeedInteger(szMsg);
	}
	else
	{
		SwitchState(S_OBJ,"选择一个BSpline曲面:");
	}

	return 0;
}

int	ScCmdBSplSurfIncDegree::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdBSplSurfIncDegree::InputEvent(const CPoint& point,int nState,
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
		case S_OBJ:
			{
				if(HasSelObj())
				{
					SwitchState(S_UDEGREE,"输入提升的U的阶数:");
					CString szMsg;
					szMsg.Format("输入新的U阶数(当前阶数%d):",m_nOldUDegree);
					NeedInteger(szMsg);
				}
			}
			break;
		default:
			break;
		}
	}
}

//接收浮点数
int		ScCmdBSplSurfIncDegree::AcceptInput(int nVal)
{
	if(nVal <= 0 || nVal > 25)
		return -1;

	switch(GetState())
	{
	case S_UDEGREE:
		{
			if(nVal <= m_nOldUDegree)
				return -1;

			m_nUDegree = nVal;
			EndInput();

			CString szMsg;
			szMsg.Format("输入新的V阶数(当前阶数%d):",m_nOldVDegree);
			NeedInteger(szMsg);
			SwitchState(S_VDEGREE,"输入新的V阶数:");
		}
		break;
	case S_VDEGREE:
		{
			if(nVal <= m_nOldVDegree)
				return -1;

			m_nVDegree = nVal;

			if(DoIncDegree())
				Done();
			else
				End();
		}
		break;
	default:
		break;
	}
	return 0;
}

BOOL	ScCmdBSplSurfIncDegree::HasSelObj()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsFace(aS))
		{
			TopoDS_Face aF = TopoDS::Face(aS);

			Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aF);
			if(aSurf.IsNull())
				return FALSE;

			if(!aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
				return FALSE;

			Handle(Geom_BSplineSurface) abSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
			m_nOldUDegree = abSurf->UDegree();
			m_nOldVDegree = abSurf->VDegree();

			SelObj(0) = SelectedObj(FALSE);

			return TRUE;
		}
	}
	return FALSE;
}

BOOL	ScCmdBSplSurfIncDegree::DoIncDegree()
{
	TopoDS_Face aFace = TopoDS::Face(GetObjShape(SelObj(0)));

	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
	if(aSurf.IsNull())
		return FALSE;

	Handle(Geom_BSplineSurface) abSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf->Copy());
	abSurf->IncreaseDegree(m_nUDegree,m_nVDegree);

	TopoDS_Face aNF = BRepBuilderAPI_MakeFace(abSurf, Precision::Confusion());//yxk20200104
	if(aNF.IsNull())
		return FALSE;

	m_AISContext->Remove(SelObj(0),Standard_False);
	Display(NewObj(0),aNF);
	BeginUndo("BSpline Surface Increase Degree");
	UndoAddNewObj(NewObj(0));
	UndoAddDelObj(SelObj(0));
	EndUndo(TRUE);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//

//启动和结束命令
int	ScCmdBSplCurveInsertKnots::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	m_nFlag = 0;
	if(HasSelCurve())
	{
		ShowCtrlPnts();
		SwitchState(S_INSERT,"点击曲线插入结点:");
	}
	else
	{
		NeedSelect(TRUE);
		SwitchState(S_CURVE,"选择一个BSpline曲线:");
	}

	return 0;
}

int	ScCmdBSplCurveInsertKnots::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdBSplCurveInsertKnots::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(DoInsert(TRUE))
			Done();
		else
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
					ShowCtrlPnts();
					SwitchState(S_INSERT,"点击曲线插入结点:");
				}
			}
			break;
		case S_INSERT:
			{
				m_newPt = PixelToModel(point.x,point.y);
				DoInsert(FALSE);
			}
			break;
		default:
			break;
		}
	}
}

//接收浮点数
int	ScCmdBSplCurveInsertKnots::AcceptInput(int nVal)
{
	return 0;
}


BOOL	ScCmdBSplCurveInsertKnots::HasSelCurve()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsEdge(aS))
		{
			double df,dl;
			TopoDS_Edge aE = TopoDS::Edge(aS);
			Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE,df,dl);
			if(aCur.IsNull())
				return FALSE;

			if(!aCur->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
				return FALSE;

			m_cpyCur = Handle(Geom_BSplineCurve)::DownCast(aCur->Copy());
			SelObj(0) = SelectedObj();
			m_df = df;
			m_dl = dl;

			return TRUE;
		}
	}
	return FALSE;
}

BOOL	ScCmdBSplCurveInsertKnots::ShowCtrlPnts()
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	gp_Pnt pnt;
	TopoDS_Vertex aVS;
	for(int iu = 1;iu <= m_cpyCur->NbPoles();iu ++)
	{
		pnt = m_cpyCur->Pole(iu);
		aVS = BRepBuilderAPI_MakeVertex(pnt);
		BB.Add(aC,aVS);
	}

	Display(TmpObj(0),aC);

	return TRUE;
}

BOOL	ScCmdBSplCurveInsertKnots::DoInsert(BOOL bDone)
{
	//计算点到曲线的最近点
	GeomAPI_ProjectPointOnCurve pprj;
	pprj.Init(m_cpyCur,m_df,m_dl);
	pprj.Perform(m_newPt);
	if(pprj.NbPoints() == 0)
		return FALSE;

	double u = pprj.LowerDistanceParameter();
	//插入knot
	m_cpyCur->InsertKnot(u);

	if(bDone)
	{
		m_AISContext->Remove(SelObj(0), Standard_True);
		RemoveCtxObj(TmpObj(0));

		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_cpyCur);
		if(aE.IsNull())
			return FALSE;

		Display(NewObj(0),aE);
		BeginUndo("Spline curve insert knots");
		UndoAddDelObj(SelObj(0));
		UndoAddNewObj(NewObj(0));
		EndUndo(TRUE);
	}
	else
	{
		//更新控制点显式
		ShowCtrlPnts();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//

//启动和结束命令
int	ScCmdBSplSurfInsertKnots::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	if(HasSelSurf())
	{
		ShowCtrlPnts();
		SwitchState(S_FLAG,"选择插入方式:");
		NeedInteger("插入方式<1: U向 2:V向 3:双向>:");
	}
	else
	{
		NeedSelect(TRUE);
		SwitchState(S_SURF,"选择一个BSpline曲面:");
	}

	return 0;
}

int	ScCmdBSplSurfInsertKnots::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void		ScCmdBSplSurfInsertKnots::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(DoInsert(TRUE))
			Done();
		else
			End();
	}
	else
	{
		switch(GetState())
		{
		case S_SURF:
			{
				if(HasSelSurf())
				{
					ShowCtrlPnts();
					SwitchState(S_FLAG,"选择插入方式:");
					NeedInteger("插入方式<1: U向 2:V向 3:双向>:");
				}
			}
			break;
		case S_FLAG:
			break;
		case S_INSERT:
			{
				m_clkPnt = PixelToModel(point.x,point.y);
				DoInsert(FALSE);
			}
			break;
		default:
			break;
		}
	}
}

int	ScCmdBSplSurfInsertKnots::AcceptInput(int nVal)
{
	if(GetState() == S_FLAG)
	{
		if(nVal < 0 || nVal > 3)
			return -1;

		m_nFlag = nVal;
		SwitchState(S_INSERT,"点击插入结点:");
	}

	return 0;
}

BOOL	ScCmdBSplSurfInsertKnots::HasSelSurf()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsFace(aS))
		{
			TopoDS_Face aF = TopoDS::Face(aS);
			Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aF);
			if(aSurf.IsNull())
				return FALSE;

			if(!aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
				return FALSE;

			m_cpySurf = Handle(Geom_BSplineSurface)::DownCast(aSurf->Copy());
			SelObj(0) = SelectedObj();

			return TRUE;
		}
	}
	return FALSE;
}

BOOL	ScCmdBSplSurfInsertKnots::ShowCtrlPnts()
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	gp_Pnt pnt;
	TopoDS_Vertex aVS;
	for(int iu = 1;iu <= m_cpySurf->NbUPoles();iu ++)
	{
		for(int iv = 1;iv <= m_cpySurf->NbVPoles();iv ++)
		{
			pnt = m_cpySurf->Pole(iu,iv);
			aVS = BRepBuilderAPI_MakeVertex(pnt);
			BB.Add(aC,aVS);
		}
	}

	Display(TmpObj(0),aC);

	return TRUE;
}

BOOL	ScCmdBSplSurfInsertKnots::DoInsert(BOOL bDone)
{
	//计算最近点
	GeomAPI_ProjectPointOnSurf psp(m_clkPnt,m_cpySurf);
	if(psp.NbPoints() == 0)
		return FALSE;

	double u,v;
	psp.LowerDistanceParameters(u,v);

	if(m_nFlag == 1)
	{
		m_cpySurf->InsertUKnot(u,1,0.0);
	}
	else if(m_nFlag == 2)
	{
		m_cpySurf->InsertVKnot(v,1,0.0);
	}
	else
	{	
		m_cpySurf->InsertUKnot(u,1,0.0);
		m_cpySurf->InsertVKnot(v,1,0.0);
	}

	if(bDone)
	{
		TopoDS_Face aF = BRepBuilderAPI_MakeFace(m_cpySurf, Precision::Confusion());//yxk20200104
		if(aF.IsNull())
			return FALSE;

		m_AISContext->Remove(SelObj(0), Standard_True);
		RemoveCtxObj(TmpObj(0));
		Display(NewObj(0),aF);
		BeginUndo("BSpline surface insert knots");
		UndoAddNewObj(NewObj(0));
		UndoAddDelObj(SelObj(0));
		EndUndo(TRUE);
	}
	else
	{
		ShowCtrlPnts();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// 增加控制点

//启动和结束命令
int	ScCmdBSplCurveAddCtrlPnts::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	if(HasSelCurve())
	{
		ShowCtrlPnts();
		SwitchState(S_INPUT,"输入控制点个数:");
		NeedInteger("新控制点的个数:");
	}
	else
	{
		NeedSelect(TRUE);
		SwitchState(S_CURVE,"选择样条曲线:");
	}

	return 0;
}

int	ScCmdBSplCurveAddCtrlPnts::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdBSplCurveAddCtrlPnts::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		RemoveCtxObj(TmpObj(0));
		if(DoAddCtrlPnts(TRUE))
			Done();
		else
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
					ShowCtrlPnts();
					SwitchState(S_INPUT,"输入控制点个数:");
					NeedInteger("新控制点的个数:");
				}
			}
			break;
		case S_INPUT:
			break;
		default:
			break;
		}
	}
}

//接收浮点数
int	ScCmdBSplCurveAddCtrlPnts::AcceptInput(int nVal)
{
	if(GetState() == S_INPUT)
	{
		if(nVal <= 0 || nVal > 500)
			return -1;

		m_ncpts = nVal;
		DoAddCtrlPnts(FALSE);
		ShowCtrlPnts();
	}

	return 0;
}


BOOL	ScCmdBSplCurveAddCtrlPnts::HasSelCurve()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsEdge(aS))
		{
			double df,dl;
			TopoDS_Edge aE = TopoDS::Edge(aS);
			Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE,df,dl);
			if(aCur.IsNull())
				return FALSE;

			if(!aCur->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
				return FALSE;

			m_cpyCur = Handle(Geom_BSplineCurve)::DownCast(aCur->Copy());
			m_tmpCur = Handle(Geom_BSplineCurve)::DownCast(aCur->Copy());
			SelObj(0) = SelectedObj();
			m_df = df;
			m_dl = dl;

			return TRUE;
		}
	}
	return FALSE;
}

void	ScCmdBSplCurveAddCtrlPnts::ShowCtrlPnts()
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	gp_Pnt pnt;
	TopoDS_Vertex aVS;
	for(int iu = 1;iu <= m_tmpCur->NbPoles();iu ++)
	{
		pnt = m_tmpCur->Pole(iu);
		aVS = BRepBuilderAPI_MakeVertex(pnt);
		BB.Add(aC,aVS);
	}

	Display(TmpObj(0),aC);

}

BOOL	ScCmdBSplCurveAddCtrlPnts::DoAddCtrlPnts(BOOL bDone)
{
	if(m_cpyCur->NbPoles() >= m_ncpts)
		return FALSE;

	int noff = m_ncpts - m_cpyCur->NbPoles();
	//通过插入knot来增加点，插入noff个knots。
	TColStd_Array1OfReal kns(1,noff);
	TColStd_Array1OfInteger  mul(1,noff);
	double u;
	double dstep = (m_dl - m_df)/(noff + 1);
	for(int ix = 1;ix <= noff;ix ++)
	{
		u = m_df + ix * dstep;
		kns.SetValue(ix,u);
		mul.SetValue(ix,1);
	}

	m_tmpCur = Handle(Geom_BSplineCurve)::DownCast(m_cpyCur->Copy());
	m_tmpCur->InsertKnots(kns,mul,0.0,Standard_True);

	if(bDone)
	{
		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_tmpCur,m_df,m_dl);
		if(aE.IsNull())
			return FALSE;

		m_AISContext->Remove(SelObj(0), Standard_True);
		Display(NewObj(0),aE);

		BeginUndo("BSpline curve add ctrlpnts");
		UndoAddDelObj(SelObj(0));
		UndoAddNewObj(NewObj(0));
		EndUndo(TRUE);
	}

	return TRUE;
}

	
///////////////////////////////////////////////////////////////////////////
//

//启动和结束命令
int	ScCmdBSplSurfAddCtrlPnts::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	if(HasSelSurf())
	{
		ShowCtrlPnts();
		SwitchState(S_UINPUT,"输入新的U向控制点个数:");
		CString szMsg;
		szMsg.Format("U向新控制点个数(当前%d):",m_cpySurf->NbUPoles());
		NeedInteger(szMsg);
	}
	else
	{
		SwitchState(S_SURF,"选择一个bspline曲面:");
		NeedSelect(TRUE);
	}

	return 0;
}

int	ScCmdBSplSurfAddCtrlPnts::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdBSplSurfAddCtrlPnts::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(DoAddCtrlPnts(TRUE))
			Done();
		else
			End();
	}
	else
	{
		switch(GetState())
		{
		case S_SURF:
			{
				//
				if(HasSelSurf())
				{
					ShowCtrlPnts();
					SwitchState(S_UINPUT,"输入新的U向控制点个数:");
					CString szMsg;
					szMsg.Format("U向新控制点个数(当前%d):",m_cpySurf->NbUPoles());
					NeedInteger(szMsg);
				}
			}
			break;
		default:
			break;
		}
	}
}

//接收浮点数
int	ScCmdBSplSurfAddCtrlPnts::AcceptInput(int nVal)
{
	switch(GetState())
	{
	case S_UINPUT:
		{
			m_nucpts = nVal;

			DoAddCtrlPnts(FALSE);
			ShowCtrlPnts();
			SwitchState(S_VINPUT,"输入新的V向控制点个数:");
			CString szMsg;
			szMsg.Format("V向新控制点个数(当前%d):",m_cpySurf->NbVPoles());
			NeedInteger(szMsg);
		}
		break;
	case S_VINPUT:
		{
			m_nvcpts = nVal;

			DoAddCtrlPnts(FALSE);
			ShowCtrlPnts();
			SwitchState(S_UINPUT,"输入新的U向控制点个数:");
			CString szMsg;
			szMsg.Format("U向新控制点个数(当前%d):",m_cpySurf->NbUPoles());
			NeedInteger(szMsg);
		}
		break;
	default:
		break;
	}
	return 0;
}


BOOL	ScCmdBSplSurfAddCtrlPnts::HasSelSurf()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsFace(aS))
		{
			TopoDS_Face aF = TopoDS::Face(aS);
			Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aF);
			if(aSurf.IsNull())
				return FALSE;

			if(!aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
				return FALSE;

			m_cpySurf = Handle(Geom_BSplineSurface)::DownCast(aSurf->Copy());
			m_tmpSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf->Copy());
			SelObj(0) = SelectedObj();

			return TRUE;
		}
	}
	return FALSE;
}

void	ScCmdBSplSurfAddCtrlPnts::ShowCtrlPnts()
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	gp_Pnt pnt;
	TopoDS_Vertex aVS;
	for(int iu = 1;iu <= m_tmpSurf->NbUPoles();iu ++)
	{
		for(int iv = 1;iv <= m_tmpSurf->NbVPoles();iv ++)
		{
			pnt = m_tmpSurf->Pole(iu,iv);
			aVS = BRepBuilderAPI_MakeVertex(pnt);
			BB.Add(aC,aVS);
		}
	}

	Display(TmpObj(0),aC);
}

BOOL	ScCmdBSplSurfAddCtrlPnts::DoAddCtrlPnts(BOOL bDone)
{
	if(!bDone)
	{
		if(GetState() == S_UINPUT)
		{
			double u1,u2,v1,v2,u,dstep;
			m_cpySurf->Bounds(u1,u2,v1,v2);

			int nOff = m_nucpts - m_cpySurf->NbUPoles();
			if(nOff <= 0)
				return FALSE;

			TColStd_Array1OfReal knt(1,nOff);
			TColStd_Array1OfInteger mul(1,nOff);
			
			dstep = (u2 - u1)/(nOff + 1);
			for(int ix = 1;ix <= nOff;ix ++)
			{
				u = u1 + ix * dstep;
				knt.SetValue(ix,u);
				mul.SetValue(ix,1);
			}
	
			m_tmpSurf = Handle(Geom_BSplineSurface)::DownCast(m_cpySurf->Copy());
			m_tmpSurf->InsertUKnots(knt,mul,0.0);
		}
		else
		{
			double u1,u2,v1,v2,v,dstep;
			m_cpySurf->Bounds(u1,u2,v1,v2);

			int nOff = m_nvcpts - m_cpySurf->NbVPoles();
			if(nOff <= 0)
				return FALSE;

			TColStd_Array1OfReal knt(1,nOff);
			TColStd_Array1OfInteger mul(1,nOff);

			dstep = (v2 - v1)/(nOff + 1);
			for(int ix = 1;ix <= nOff;ix ++)
			{
				v = v1 + ix * dstep;
				knt.SetValue(ix,v);
				mul.SetValue(ix,1);
			}

			m_tmpSurf->InsertVKnots(knt,mul,0.0);
		}
	}

	if(bDone)
	{
		TopoDS_Face aNF = BRepBuilderAPI_MakeFace(m_tmpSurf, Precision::Confusion());//yxk20200104
		if(aNF.IsNull())
			return FALSE;

		m_AISContext->Remove(SelObj(0),Standard_False);
		Display(NewObj(0),aNF);
		BeginUndo("BSpline Surface add ctrl pnts");
		UndoAddNewObj(NewObj(0));
		UndoAddDelObj(SelObj(0));
		EndUndo(TRUE);
	}

	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 测试一个或多个曲面插入控制点后的情形，重点观察边界处的情况。

int	ScCmdBSplCtrlPntsTest::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	if(HasSelObj())
	{
		SwitchState(S_UPNTS,"输入U向控制点个数:");
		NeedInteger("U向控制点个数:");
	}
	else
	{
		NeedSelect(TRUE);
		SwitchState(S_OBJ,"选择一个对象:");
	}

	return 0;
}

int	ScCmdBSplCtrlPntsTest::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdBSplCtrlPntsTest::InputEvent(const CPoint& point,int nState,
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
		case S_OBJ:
			{
				if(HasSelObj())
				{
					SwitchState(S_UPNTS,"输入U向控制点个数:");
					NeedInteger("U向控制点个数:");
				}
			}
			break;
		default:
			break;
		}
	}
}

//接收浮点数
int	ScCmdBSplCtrlPntsTest::AcceptInput(int nVal)
{
	if(GetState() == S_UPNTS)
	{
		m_nUPnts = nVal;
		
		SwitchState(S_VPNTS,"输入V向控制点个数:");
		NeedInteger("V向控制点个数:");
	}
	else if(GetState() == S_VPNTS)
	{
		m_nVPnts = nVal;

		if(DoInsert())
			Done();
		else
			End();
	}
	return 0;
}


BOOL	ScCmdBSplCtrlPntsTest::HasSelObj()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsSolid(aS) || ScBRepLib::IsShell(aS) || ScBRepLib::IsFace(aS))
		{
			SelObj(0) = SelectedObj();

			return TRUE;
		}
	}

	return FALSE;
}

void	ScCmdBSplCtrlPntsTest::ShowCtrlPnts(const TopoDS_Shape& aShape)
{
	TopoDS_Face aFace = TopoDS::Face(aShape);
	Handle(Geom_Surface) aSurf = Handle(Geom_Surface)::DownCast(BRep_Tool::Surface(aFace)->Copy());
	Handle(Geom_BSplineSurface) aSplSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);

	//如果必要，提升阶数
	int ud = aSplSurf->UDegree(),vd = aSplSurf->VDegree();
	if(ud < 3 || vd < 3)
	{
		if(ud < 3) ud = 3;
		if(vd < 3) vd = 3;
		aSplSurf->IncreaseDegree(ud,vd);
		DTRACE("\n Increase Degree.");
	}

	//增加控制点
	TcBSplLib::IncSurfCtrlPnts(aSplSurf,m_nUPnts,m_nVPnts);

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	double dMax = 0;
	gp_Pnt pnt;
	TopoDS_Vertex aVS;
	for(int iu = 1;iu <= aSplSurf->NbUPoles();iu ++)
	{
		for(int iv = 1;iv <= aSplSurf->NbVPoles();iv ++)
		{
			pnt = aSplSurf->Pole(iu,iv);
			aVS = BRepBuilderAPI_MakeVertex(pnt);
			BB.Add(aC,aVS);

			GeomAPI_ProjectPointOnSurf prj(pnt,aSplSurf);
			if(prj.IsDone() && prj.NbPoints() > 0)
			{
				if(prj.LowerDistance() > dMax)
					dMax = prj.LowerDistance();
			}
		}
	}

	Display(NewObj(0),aC);
	UndoAddNewObj(NewObj(0));
	DTRACE("\n surf ctrl dist max is %f",dMax);
}

BOOL	ScCmdBSplCtrlPntsTest::DoInsert()
{
	TopoDS_Shape aS = GetObjShape(SelObj(0));
	BRepBuilderAPI_NurbsConvert nc;
	nc.Perform(aS);
	if(!nc.IsDone())
	{
		AfxMessageBox("转换失败.");
		return FALSE;
	}

	BeginUndo("test insert ctrlpnts");
	TopoDS_Shape aNS = nc.Shape();
	TopExp_Explorer ex;
	for(ex.Init(aNS,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Face aF = TopoDS::Face(ex.Current());
		//
		ShowCtrlPnts(aF);

	}
	EndUndo(TRUE);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 组合曲线转换为一条BSpline曲线
int	ScCmdCurveConcatToSpline::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);
	return 0;
}

int	ScCmdCurveConcatToSpline::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdCurveConcatToSpline::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{

}

BOOL	ScCmdCurveConcatToSpline::DoConcat()
{
	return FALSE;
}