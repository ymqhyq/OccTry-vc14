#include "StdAfx.h"
#include ".\sccmdfacebase.h"

ScCmdFaceBase::ScCmdFaceBase(void)
{
}

ScCmdFaceBase::~ScCmdFaceBase(void)
{
}

//
// �����ͽ�������
//
int	ScCmdFaceBase::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	this->m_bLocalCtx = FALSE;

	NeedSelect(TRUE);
	if(HasSelFace())
	{
		BuildShape();
	}
	else if(HasSelShape())
	{
		SwitchState(S_FACE,"ѡ��һ������:");
	}
	else
	{
		SwitchState(S_SHAPE,"ѡ��һ����������.");
	}

	return 0;
}

//
// �������
//
int	ScCmdFaceBase::End()
{
	m_aFace.Nullify();

	this->CloseFaceLocalCtx();
	
	return ScCommand::End();
}

//
// ��ѡ��ʽ�µ���Ϣ��Ӧ����
//
void	ScCmdFaceBase::InputEvent(const CPoint& point,int nState,
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
		case S_SHAPE:
			{
				if(HasSelShape())
				{
					SwitchState(S_FACE,"ѡ��һ������:");
				}
			}
			break;
		case S_FACE:
			{
				if(HasSelFace())
				{
					BuildShape();
				}
			}
			break;
		default:
			break;
		}
	}
}


BOOL	ScCmdFaceBase::HasSelFace()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(!aS.IsNull() && (aS.ShapeType() == TopAbs_FACE))
		{
			m_aFace = TopoDS::Face(aS);

			this->CloseFaceLocalCtx();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL	ScCmdFaceBase::HasSelShape()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(!aS.IsNull() && (aS.ShapeType() == TopAbs_SHELL ||
			aS.ShapeType() == TopAbs_SOLID ||
			aS.ShapeType() == TopAbs_COMPSOLID ||
			aS.ShapeType() == TopAbs_COMPOUND))
		{
			SelObj(0) = SelectedObj(FALSE);

			this->OpenFaceLocalCtx();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL	ScCmdFaceBase::OpenFaceLocalCtx()
{
	if(m_bLocalCtx)
		return TRUE;

	m_bLocalCtx = TRUE;
	//m_AISContext->OpenLocalContext();//yxk20191207
	m_AISContext->Activate(SelObj(0),4);

	return TRUE;
}

BOOL	ScCmdFaceBase::CloseFaceLocalCtx()
{
	if(m_bLocalCtx)
	{
		m_bLocalCtx = FALSE;
		//m_AISContext->CloseLocalContex();//yxk20191207
	}

	return TRUE;
}

BOOL	ScCmdFaceBase::BuildShape()//���ɶ�Ӧshape��
{
	//����face��wire�����ɶ�Ӧ��edge�����ߡ�ÿ��curve�������
	TopExp_Explorer exf,exw;
	int nC = 0;

	for(exf.Init(m_aFace,TopAbs_WIRE);exf.More();exf.Next())
	{
		TopoDS_Wire aW = TopoDS::Wire(exf.Current());
		if(aW.IsNull())
			continue;

		for(exw.Init(aW,TopAbs_EDGE);exw.More();exw.Next())
		{
			TopoDS_Edge aE = TopoDS::Edge(exw.Current());
			if(aE.IsNull())
				continue;

			if(BuildCurve(aE,m_aFace))
				nC ++;
		}
	}

	if(nC > 0)
		Done();
	else
		End();

	return (nC > 0);
}

BOOL	ScCmdFaceBase::BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace)
{
	return FALSE;
}