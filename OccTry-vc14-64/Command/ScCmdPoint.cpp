#include "StdAfx.h"
#include "ScBRepLib.h"
#include ".\sccmdpoint.h"

//ScCmdPoint::ScCmdPoint(void)
//{
//}
//
//ScCmdPoint::~ScCmdPoint(void)
//{
//}

//////////////////////////////////////////////////////////////////////////
// 点投影到曲线

//启动命令，开始执行。
int	ScCmdPointPrjToCurve::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelCurve())
	{
		SwitchState(S_POINT,"输入点或选择点进行投影:");
	}
	else
	{
		SwitchState(S_CURVE,"选择一条曲线:");
	}

	return 0;
}

//结束命令，通常用于终止命令。
int	ScCmdPointPrjToCurve::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdPointPrjToCurve::InputEvent(const CPoint& point,int nState,//上述minput定义
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(DoPrj(FALSE))
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
					SwitchState(S_POINT,"输入点或选择点进行投影:");
				}
			}
			break;
		case S_POINT:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape(FALSE);
					if(ScBRepLib::IsVertex(aS))
					{
						m_selPnt = BRep_Tool::Pnt(TopoDS::Vertex(aS));
						DoPrj(TRUE);
					}
				}
				else
				{
					m_selPnt = PixelToModel(point.x,point.y);
					DoPrj(TRUE);
				}
			}
			break;
		default:
			break;
		}
	}
}


BOOL	ScCmdPointPrjToCurve::HasSelCurve()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsEdge(aS))
		{
			SelObj(0) = SelectedObj();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL	ScCmdPointPrjToCurve::DoPrj(BOOL bTmp)
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Edge aE = TopoDS::Edge(GetObjShape(SelObj(0)));
	double df,dl;
	Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE,df,dl);
	if(aCur.IsNull())
		return FALSE;

	GeomAPI_ProjectPointOnCurve prjC(m_selPnt,aCur,df,dl);
	if(prjC.NbPoints() == 0)
	{
		AfxMessageBox("no projection point");
		return FALSE;
	}

	gp_Pnt pnt;
	if(bTmp)
	{
		TopoDS_Compound aC;
		BRep_Builder BB;
		BB.MakeCompound(aC);

		for(int ix = 1;ix <= prjC.NbPoints();ix ++)
		{
			pnt = prjC.Point(ix);
			aE = BRepBuilderAPI_MakeEdge(m_selPnt,pnt);
			if(!aE.IsNull())
				BB.Add(aC,aE);
		}

		Display(TmpObj(0),aC);
	}
	else
	{
		BeginUndo("Point project to curve");
		for(int ix = 1;ix <= prjC.NbPoints();ix ++)
		{
			pnt = prjC.Point(ix);
			TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(pnt);
			if(!aV.IsNull())
			{
				Display(NewObj(ix - 1),aV);
				UndoAddNewObj(NewObj(ix - 1));
			}
		}
		EndUndo(TRUE);
	}

	return TRUE;
}
