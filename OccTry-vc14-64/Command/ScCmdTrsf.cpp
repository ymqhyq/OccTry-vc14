#include "StdAfx.h"
#include ".\sccmdtrsf.h"

ScCmdTrsf::ScCmdTrsf(void)
{
}

ScCmdTrsf::~ScCmdTrsf(void)
{
}

////////////////////////////////////////////////////////////////
// ����ƽ�ơ�
//

//�����ͽ�������
int	ScCmdObjMoving::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SELECT,"ѡ��Ҫ�ƶ��Ķ���.");
	NeedSelect(TRUE);

	return 0;
}

int	ScCmdObjMoving::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
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

						SwitchState(S_MOVING,"�ƶ�����.");
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

//����ƶ�����Ӧ����
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

	//����ƫ�ƾ��룬���ɱ任��
	gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
	gp_Trsf trsf;
	trsf.SetTranslation(m_pntStart,pnt);

	BRepBuilderAPI_Transform objTrsf(m_aSelObj,trsf,TRUE);
	aShape = objTrsf.Shape();

	return aShape;
}

//////////////////////////////////////////////////////////////////
//
//����clone.

ScCmdObjClone::ScCmdObjClone(BOOL bMulti)
{
	this->m_bMulti = bMulti;
}

	//�����ͽ�������
int		ScCmdObjClone::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SELECT,"ѡ��Ҫ�ƶ��Ķ���.");
	NeedSelect(TRUE);

	return 0;
}

int		ScCmdObjClone::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
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

						SwitchState(S_MOVING,"�ƶ�����.");
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

//����ƶ�����Ӧ����
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

	//����ƫ�ƾ��룬���ɱ任��
	gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
	gp_Trsf trsf;
	trsf.SetTranslation(m_pntStart,pnt);

	BRepBuilderAPI_Transform objTrsf(m_aSelObj,trsf,TRUE);
	aShape = objTrsf.Shape();

	return aShape;
}

