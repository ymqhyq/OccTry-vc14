#include "StdAfx.h"

#include <BRepBndLib.hxx>
#include "ScBRepLib.h"
#include "ScTools.h"
#include "FFDTool.h"
#include "FFDAlgo.h"
#include "FFDObj.h"

#include ".\sccmdffdbend.h"

ScCmdFFDBend::ScCmdFFDBend(void)
{
	m_pObj = NULL;
	m_pTool = NULL;
	m_pAlgo = NULL;

	m_bChanged = FALSE;
}

ScCmdFFDBend::~ScCmdFFDBend(void)
{
}

//启动和结束命令
int	ScCmdFFDBend::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	if(HasSelObj())
	{
		NeedSnap(TRUE);
		SwitchState(S_AX_BEGIN,"骨干线起点:");
	}
	else
	{
		SwitchState(S_OBJ,"选择对象:");
	}

	return 0;
}

int	ScCmdFFDBend::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void		ScCmdFFDBend::InputEvent(const CPoint& point,int nState,//上述minput定义
											   const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(m_bChanged)
		{
			if(Deform(FALSE))
				Done();
			else
				End();
		}
		else
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
					NeedSnap(TRUE);
					SwitchState(S_AX_BEGIN,"骨干线起点:");
				}
			}
			break;
		case S_AX_BEGIN:
			{
				m_axPnt1 = PixelToCSPlane(point.x,point.y);
				SwitchState(S_AX_END,"骨干线终点:");
			}
			break;
		case S_AX_END:
			{
				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_axPnt1) < 1e-6)
					return;

				m_axPnt2 = pnt;
				if(!InitBend())
				{
					ASSERT(FALSE);
					return;
				}
				SwitchState(S_PASS_PNT,"选择通过点弯曲对象:");
			}
			break;
		case S_PASS_PNT:
			{
				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_axPnt1) < 1e-6 ||
					pnt.Distance(m_axPnt2) < 1e-6)
					return;

				m_passPnt = pnt;
				ShowArcCurve();
				Deform(TRUE);
				//End();
			}
			break;
		default:
			break;
		}
	}
}

//鼠标移动的响应函数
void	ScCmdFFDBend::MoveEvent(const CPoint& point,
								  const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_OBJ:
		break;
	case S_AX_BEGIN:
		break;
	case S_AX_END:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_axPnt1) < 1e-6)
				return;

			ShowAxLine(pnt);
		}
		break;
	case S_PASS_PNT:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_axPnt1) < 1e-6 ||
				pnt.Distance(m_axPnt2) < 1e-6)
				return;

			m_passPnt = pnt;
			ShowArcCurve();
		}
		break;
	default:
		break;
	}
}

BOOL	ScCmdFFDBend::HasSelObj()
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

void	ScCmdFFDBend::ShowAxLine(const gp_Pnt& pnt)
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_axPnt1,pnt);
	Display(TmpObj(0),aE);
}

void	ScCmdFFDBend::ShowArcCurve()
{
	if(!m_pTool->BuildCurveTool(m_passPnt,TRUE,TRUE))
		return;

	Handle(Geom_Curve) aC1 = m_pTool->GetTarCurve();
	Handle(Geom_Curve) aC2 = m_pTool->GetArcCurve();

	RemoveCtxObj(TmpObj(1));
	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(aC2);
	Display(TmpObj(1),aE);
}

BOOL	ScCmdFFDBend::InitBend()
{
	TopoDS_Shape aS = GetObjShape(SelObj(0));
	Bnd_Box bb;
	BRepBndLib::Add(aS,bb);

	TColgp_SequenceOfPnt pnts;
	ScTools::GetBoxCorner(bb,pnts);

	//将所有点投影到骨干上,获得最大的两个点
	Handle(Geom_Line) aLin = new Geom_Line(m_axPnt1,gp_Dir(gp_Vec(m_axPnt1,m_axPnt2)));
	gp_Pnt p1,p2,pnt;
	double t,t1 = 1e10,t2 = -1e10;
	for(int ix = 1;ix <= pnts.Length();ix ++)
	{
		pnt = pnts.Value(ix);
		GeomAPI_ProjectPointOnCurve ppc(pnt,aLin);
		if(ppc.NbPoints() > 0)
		{
			t = ppc.LowerDistanceParameter();
			if(t < t1)
			{
				t1 = t;
				p1 = pnt;
			}
			if(t > t2)
			{
				t2 = t;
				p2 = pnt;
			}
		}
	}
	
	m_pTool = new FFDBendTool;
	if(!m_pTool->Init(m_axPnt1,m_axPnt2,p1,p2))
		return FALSE;

	m_pObj = new FFDSurfObj;
	if(!m_pObj->InitShape(aS,40,40))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_pAlgo = new FFDBendAlgo;
	if(!m_pAlgo->Init(m_pObj,m_pTool))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	return TRUE;
}

BOOL	ScCmdFFDBend::Deform(BOOL bTmp)
{
	RemoveCtxObj(TmpObj(2));

	if(m_pAlgo->Deform() && m_pAlgo->IsDone())
	{
		TopoDS_Shape aNS = m_pAlgo->Shape();
		//
		if(bTmp)
		{
			Display(TmpObj(2),aNS);
			m_bChanged = TRUE;
		}
		else
		{
			Display(NewObj(0),aNS);
			BeginUndo("FFD Bend");
			UndoAddNewObj(NewObj(0));
			EndUndo(TRUE);
		}

		return TRUE;
	}

	return FALSE;
}