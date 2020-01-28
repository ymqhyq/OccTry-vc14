#include "StdAfx.h"
#include "ScBReplib.h"
#include "ScView.h"

#include <StdSelect_ShapeTypeFilter.hxx>

#include "FFDObj.h"
#include "FFDTool.h"
#include "FFDAlgo.h"

#include ".\sccmdflowbycurve.h"

ScCmdFlowByCurve::ScCmdFlowByCurve(void)
{
}

ScCmdFlowByCurve::~ScCmdFlowByCurve(void)
{
}

//启动和结束命令
int	ScCmdFlowByCurve::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	Handle(StdSelect_ShapeTypeFilter) aFilter = new StdSelect_ShapeTypeFilter(TopAbs_EDGE);
	m_AISContext->AddFilter(aFilter);

	NeedSelect(TRUE);
	if(HasSelObj())
	{
		SwitchState(S_DATUM_CURVE,"选择一个基准曲线,点击靠近端点处:");
	}
	else
	{
		SwitchState(S_OBJ,"选择一个要流动的物体");
	}

	return 0;
}

int	ScCmdFlowByCurve::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdFlowByCurve::InputEvent(const CPoint& point,int nState,//上述minput定义
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
					SwitchState(S_DATUM_CURVE,"选择一个基准曲线,点击靠近端点处:");
				}
			}
			break;
		case S_DATUM_CURVE:
			{
				if(HasSelDatumCurve(point.x,point.y))
				{
					SwitchState(S_TAR_CURVE,"选择一个目标曲线,点击靠近端点处:");
				}
			}
			break;
		case S_TAR_CURVE:
			{
				if(HasSelTarCurve(point.x,point.y))
				{
					if(DoFlow())
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


BOOL	ScCmdFlowByCurve::HasSelObj()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aShape = SelectedShape(FALSE);
		if(ScBRepLib::IsFace(aShape) || ScBRepLib::IsShell(aShape) ||
			ScBRepLib::IsSolid(aShape))
		{
			SelObj(0) = SelectedObj();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL	ScCmdFlowByCurve::HasSelDatumCurve(int nx,int ny)
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aShape = SelectedShape(FALSE);
		if(ScBRepLib::IsEdge(aShape))
		{
			//计算点击点信息
			TopoDS_Edge aE = TopoDS::Edge(aShape);
			if(m_pScView->GetEdgePickPoint(aE,nx,ny,5,m_datumT,m_datumPnt))
			{
				SelObj(1) = SelectedObj();

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL	ScCmdFlowByCurve::HasSelTarCurve(int nx,int ny)
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aShape = SelectedShape(FALSE);
		if(ScBRepLib::IsEdge(aShape))
		{
			//计算点击点信息
			TopoDS_Edge aE = TopoDS::Edge(aShape);
			if(m_pScView->GetEdgePickPoint(aE,nx,ny,5,m_tarT,m_tarPnt))
			{
				SelObj(2) = SelectedObj();

				return TRUE;
			}
		}
	}

	return FALSE;
}

//重点实现在这个函数中.
BOOL	ScCmdFlowByCurve::DoFlow()
{
	TopoDS_Shape aS = GetObjShape(SelObj(0));
	TopoDS_Shape aDS = GetObjShape(SelObj(1));
	TopoDS_Shape aTS = GetObjShape(SelObj(2));

	//初始化对象
	FFDSurfObj surfObj;
	if(!surfObj.InitShape(aS,40,40))
	{
		AfxMessageBox("init shape failed.");
		return FALSE;
	}

	//初始化工具
	FFDCurveTool  curTool;
	double df,dl;
	Handle(Geom_Curve) aDC = BRep_Tool::Curve(TopoDS::Edge(aDS),df,dl);
	curTool.SetDatumCurve(aDC,df,dl);

	Handle(Geom_Curve) aTC = BRep_Tool::Curve(TopoDS::Edge(aTS),df,dl);
	curTool.SetTarCurve(aTC,df,dl);

	if(!curTool.CalcRMF(40,m_datumT,m_tarT))
	{
		AfxMessageBox("init Tool Failed.");
		return FALSE;
	}

	//算法
	FFDCurveAlgo algo;
	algo.Init(&surfObj,&curTool);

	if(algo.Deform())
	{
		TopoDS_Shape aNS = algo.Shape();
		if(!aNS.IsNull())
		{
			Display(NewObj(0),aNS);

			BeginUndo("Flow curve");
			UndoAddNewObj(NewObj(0));
			EndUndo(TRUE);

			return TRUE;
		}
	}
	

	return FALSE;
}
