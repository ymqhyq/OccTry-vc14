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

//�����ͽ�������
int	ScCmdShear::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	if(HasSelObj())
	{
		NeedSnap(TRUE);
		SwitchState(S_BASEPNT,"ѡ�����:");
	}
	else
	{
		NeedSelect(TRUE);
		SwitchState(S_OBJ,"ѡ�����:");
	}

	return 0;
}

int	ScCmdShear::End()
{
	RemoveCtxObj(TmpObj(0));
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdShear::InputEvent(const CPoint& point,int nState,//����minput����
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
					SwitchState(S_BASEPNT,"ѡ�����:");
				}
			}
			break;
		case S_BASEPNT:
			{
				m_basePnt = PixelToCSPlane(point.x,point.y);
				SwitchState(S_BASEDIR,"��׼����:");
			}
			break;
		case S_BASEDIR:
			{
				RemoveCtxObj(TmpObj(0));

				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_basePnt) < 1e-6)
					return;

				m_baseDir = gp_Vec(m_basePnt,pnt);
				SwitchState(S_NEWDIR,"��б�Ƕ�:");
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

//����ƶ�����Ӧ����
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
			ScBRepLib::IsSolid(aShape)|| ScBRepLib::IsCompound(aShape))//��Χ�ܹ�.
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
