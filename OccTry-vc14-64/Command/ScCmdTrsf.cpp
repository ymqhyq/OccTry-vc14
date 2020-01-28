#include "StdAfx.h"
#include ".\sccmdtrsf.h"

ScCmdTrsf::ScCmdTrsf(void)
{
}

ScCmdTrsf::~ScCmdTrsf(void)
{
}

////////////////////////////////////////////////////////////////
// 对象平移。
//

//启动和结束命令
int	ScCmdObjMoving::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SELECT,"选择要移动的对象.");
	NeedSelect(TRUE);

	return 0;
}

int	ScCmdObjMoving::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdObjMoving::InputEvent(const CPoint& point,int nState,
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
		case S_SELECT:
			{
				InitSelect();
				if(MoreSelect())
				{
					m_aSelObj = SelectedShape(FALSE);
					if(!m_aSelObj.IsNull())
					{
						NeedSelect(FALSE);
						m_nX = point.x;
						m_nY = point.y;
						DelObj(0) = SelectedObj();
						m_pntStart = this->PixelToCSPlane(point.x,point.y);

						SwitchState(S_MOVING,"移动对象.");
					}
				}
			}
			break;
		case S_MOVING:
			{
				RemoveCtxObj(TmpObj(0));

				TopoDS_Shape aS = BuildCopy(point);
				if(!aS.IsNull())
				{
					m_AISContext->Remove(DelObj(0), Standard_True);
					Display(NewObj(0),aS);
				}
				Done();
			}
			break;
		default:
			break;
		}
	}
}

//鼠标移动的响应函数
void	ScCmdObjMoving::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SELECT:
		break;
	case S_MOVING:
		{
			RemoveCtxObj(TmpObj(0));
			TopoDS_Shape aS = BuildCopy(point);
			WFDisplay(TmpObj(0),aS);
		}
		break;
	default:
		break;
	}
}

TopoDS_Shape	ScCmdObjMoving::BuildCopy(const CPoint& point)
{
	TopoDS_Shape aShape;
	if((m_nX == point.x) && (m_nY == point.y))
		return aShape;

	//计算偏移距离，生成变换。
	gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
	gp_Trsf trsf;
	trsf.SetTranslation(m_pntStart,pnt);

	BRepBuilderAPI_Transform objTrsf(m_aSelObj,trsf,TRUE);
	aShape = objTrsf.Shape();

	return aShape;
}

//////////////////////////////////////////////////////////////////
//
//对象clone.

ScCmdObjClone::ScCmdObjClone(BOOL bMulti)
{
	this->m_bMulti = bMulti;
}

	//启动和结束命令
int		ScCmdObjClone::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SELECT,"选择要移动的对象.");
	NeedSelect(TRUE);

	return 0;
}

int		ScCmdObjClone::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdObjClone::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		switch(GetState())
		{
		case S_SELECT:
			{
				RemoveCtxObj(TmpObj(0));
				End();
			}
			break;
		case S_MOVING:
			{
				RemoveCtxObj(TmpObj(0));
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
		case S_SELECT:
			{
				InitSelect();
				if(MoreSelect())
				{
					m_aSelObj = SelectedShape(FALSE);
					if(!m_aSelObj.IsNull())
					{
						NeedSelect(FALSE);
						m_nX = point.x;
						m_nY = point.y;
						m_pntStart = this->PixelToCSPlane(point.x,point.y);

						SwitchState(S_MOVING,"移动对象.");
					}
				}
			}
			break;
		case S_MOVING:
			{
				RemoveCtxObj(TmpObj(0));

				TopoDS_Shape aS = BuildClone(point);
				if(!aS.IsNull())
				{
					Display(NewObj(0),aS);
				}
				if(!this->m_bMulti)
					Done();
			}
			break;
		default:
			break;
		}
	}
}

//鼠标移动的响应函数
void	ScCmdObjClone::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SELECT:
		break;
	case S_MOVING:
		{
			RemoveCtxObj(TmpObj(0));
			TopoDS_Shape aS = BuildClone(point);
			WFDisplay(TmpObj(0),aS);
		}
		break;
	default:
		break;
	}
}


TopoDS_Shape	ScCmdObjClone::BuildClone(const CPoint& point)
{
	TopoDS_Shape aShape;
	if((m_nX == point.x) && (m_nY == point.y))
		return aShape;

	//计算偏移距离，生成变换。
	gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
	gp_Trsf trsf;
	trsf.SetTranslation(m_pntStart,pnt);

	BRepBuilderAPI_Transform objTrsf(m_aSelObj,trsf,TRUE);
	aShape = objTrsf.Shape();

	return aShape;
}

