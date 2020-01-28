#include "StdAfx.h"

#include "ScBRepLib.h"
#include "FFDTool.h"

#include ".\sccmdshear.h"

ScCmdShear::ScCmdShear(void)
{
}

ScCmdShear::~ScCmdShear(void)
{
}

//启动和结束命令
int	ScCmdShear::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	if(HasSelObj())
	{
		NeedSnap(TRUE);
		SwitchState(S_BASEPNT,"选择基点:");
	}
	else
	{
		NeedSelect(TRUE);
		SwitchState(S_OBJ,"选择对象:");
	}

	return 0;
}

int	ScCmdShear::End()
{
	RemoveCtxObj(TmpObj(0));
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdShear::InputEvent(const CPoint& point,int nState,//上述minput定义
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
					NeedSnap(TRUE);
					SwitchState(S_BASEPNT,"选择基点:");
				}
			}
			break;
		case S_BASEPNT:
			{
				m_basePnt = PixelToCSPlane(point.x,point.y);
				SwitchState(S_BASEDIR,"基准方向:");
			}
			break;
		case S_BASEDIR:
			{
				RemoveCtxObj(TmpObj(0));

				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_basePnt) < 1e-6)
					return;

				m_baseDir = gp_Vec(m_basePnt,pnt);
				SwitchState(S_NEWDIR,"倾斜角度:");
			}
			break;
		case S_NEWDIR:
			{
				RemoveCtxObj(TmpObj(0));

				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_basePnt) < 1e-6)
					return;

				m_newDir = gp_Vec(m_basePnt,pnt);
				if(DoShear(FALSE))
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
void	ScCmdShear::MoveEvent(const CPoint& point,
								  const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_BASEDIR:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_basePnt) < 1e-6)
				return;

			ShowDirLine(pnt);
		}
		break;
	case S_NEWDIR:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_basePnt) < 1e-6)
				return;

			ShowDirLine(pnt);
			m_newDir = gp_Vec(m_basePnt,pnt);
			DoShear(TRUE);
		}
		break;
	default:
		break;
	}
}


BOOL	ScCmdShear::HasSelObj()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aShape = SelectedShape(FALSE);
		if(ScBRepLib::IsWire(aShape) || ScBRepLib::IsEdge(aShape) ||
			ScBRepLib::IsFace(aShape) || ScBRepLib::IsShell(aShape) ||
			ScBRepLib::IsSolid(aShape)|| ScBRepLib::IsCompound(aShape))//范围很广.
		{
			SelObj(0) = SelectedObj();

			return TRUE;
		}
	}
	return FALSE;
}

BOOL	ScCmdShear::ShowDirLine(const gp_Pnt& pnt)
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(m_basePnt,pnt);
	Display(TmpObj(0),aEdge);

	return TRUE;
}

BOOL	ScCmdShear::DoShear(BOOL bTmp)
{
	TopoDS_Shape aS = GetObjShape(SelObj(0));

	RemoveCtxObj(TmpObj(1));

	FFDShearTool shtool;
	shtool.Init(m_basePnt,m_baseDir);
	shtool.Shear(m_newDir);

	gp_GTrsf tsf = shtool.ShearTrsf();

	BRepBuilderAPI_GTransform transf(tsf);
	transf.Perform(aS);

	if(transf.IsDone())
	{
		TopoDS_Shape aNS = transf.Shape();

		if(bTmp)
		{
			Display(TmpObj(1),aNS);
		}
		else
		{
			RemoveCtxObj(TmpObj(0));

			Display(NewObj(0),aNS);
			BeginUndo("Shape Shear");
			UndoAddNewObj(NewObj(0));
			EndUndo(TRUE);
		}

		return TRUE;
	}

	return FALSE;
}
