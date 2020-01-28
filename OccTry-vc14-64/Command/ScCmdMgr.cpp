#include "StdAfx.h"
#include "..\ScInputMgr.h"
#include "..\ScView.h"
#include "..\ScSnapMgr.h"
#include ".\sccmdmgr.h"

ScCmdMgr::ScCmdMgr(void)
{
	this->m_nState = CMS_IDLE;
	this->m_pScCmd = NULL;
	this->m_pToFreeCmd = NULL;
	this->m_pInputMgr = NULL;
	this->m_pScView = NULL;
	this->m_pScUcsMgr = NULL;
	this->m_pSnapMgr = NULL;

	this->m_bMultiSelect = FALSE;
	this->m_bNeedSelect = FALSE;
}

ScCmdMgr::~ScCmdMgr(void)
{
	
}

//ִ������
int		ScCmdMgr::RunCommand(ScCommand *pScCmd)
{
	if(!pScCmd) return SC_ERR;

	if(this->IsCmdRunning())//�Ѿ�������ִ��
		return SC_ERR;

	//�ͷ��ϴ�Ҫ�ͷŵ�cmd
	if(this->m_pToFreeCmd)
	{
		delete this->m_pToFreeCmd;
		this->m_pToFreeCmd = NULL;
	}

	this->m_nState = CMS_RUNNING;
	this->m_pScCmd = pScCmd;
	//���б�Ҫ������
	this->m_pScCmd->SetScCmdMgr(this);
	this->m_pScCmd->SetScView(this->m_pScView);
	this->m_pScCmd->SetInputMgr(this->m_pInputMgr);
	this->m_pScCmd->SetUcsMgr(this->m_pScUcsMgr);
	this->m_pScCmd->SetSnapMgr(this->m_pSnapMgr);
	this->m_pScCmd->SetUndoMgr(&this->m_undoMgr);

	//��ʼִ������
	this->m_pScCmd->Begin(this->m_AISContext);

	return SC_OK;
}

//��ֹ��ǰ����ִ��
int		ScCmdMgr::StopCommand()
{
	if(!this->IsCmdRunning())
		return SC_OK;

	//��������
	this->m_pScCmd->End();

	//ɾ������
	delete this->m_pScCmd;
	this->m_nState = CMS_IDLE;
	this->m_pScCmd = NULL;

	return SC_OK;
}

//�����������
void	ScCmdMgr::RemoveAll()
{
	ASSERT(!IsCmdRunning());
	if(this->m_scCmdList.IsEmpty())
		return;

	ScCommand *pScCmd = NULL;
	while(!this->m_scCmdList.IsEmpty())
	{
		pScCmd = this->m_scCmdList.RemoveHead();
		delete pScCmd;
	}

	ClearRedoList();
}

//Undo,Redo
int		ScCmdMgr::UndoCommand()
{
	//�ݲ�֧��������undo
	if(this->IsCmdRunning())
		return SC_ERR;

	m_undoMgr.Undo();

	//if(this->m_scCmdList.IsEmpty())
	//	return SC_OK;

	//ScCommand *pScCmd = this->m_scCmdList.GetTail();
	//if(!pScCmd->CanUndo())
	//	return SC_ERR;

	//if(pScCmd->Undo() == SC_OK)
	//{
	//	this->m_scCmdList.RemoveTail();
	//	//this->m_cmdRedoList.AddTail(pScCmd);
	//	//ɾ���ˣ��ݲ���¼��
	//	delete pScCmd;
	//}

	return SC_OK;
}

int		ScCmdMgr::RedoCommand()
{
	return SC_OK;
}

//���redo�б�
void	ScCmdMgr::ClearRedoList()
{
	if(this->m_cmdRedoList.IsEmpty())
		return;

	ScCommand *pScCmd = NULL;
	while(!this->m_cmdRedoList.IsEmpty())
	{
		pScCmd = this->m_cmdRedoList.RemoveHead();
		delete pScCmd;
	}
}

//�ⲿ֪ͨ
int		ScCmdMgr::CmdFinished()
{
	if(this->m_pScCmd)
	{
		if(this->m_pScCmd->IsDone() && this->m_pScCmd->CanUndo())//��¼����undo�����
			this->m_scCmdList.AddTail(this->m_pScCmd);
		else
			this->m_pToFreeCmd = this->m_pScCmd;//��ʱ��¼��������ɾ������ֹ�����⡣

		this->m_nState = CMS_IDLE;
		this->m_pScCmd = NULL;
	}

	return 0;
}

//������ֹ���á�
int		ScCmdMgr::CmdTerminated()
{
	if(this->m_pScCmd)
	{
		this->m_pToFreeCmd = this->m_pScCmd;//��ʱ��¼��������ɾ������ֹ�����⡣

		this->m_nState = CMS_IDLE;
		this->m_pScCmd = NULL;
	}
	return 0;
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdMgr::DragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView)
{
}

//����shift�µĿ�ѡ��ʽ
void	ScCmdMgr::ShiftDragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView)
{
}

//
// ��ѡ��ʽ�µ���Ϣ��Ӧ����.���ﴦ��ѡ�Ͷ�ѡ�������
//
void	ScCmdMgr::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(this->m_pScCmd)
	{
		if(nState == MINPUT_LEFT)
		{
			if(IsMultiSelect())
			{
				m_AISContext->ShiftSelect(Standard_True);
			}
			else
			{
				m_AISContext->Select(Standard_True);
			}
		}

		this->m_pScCmd->InputEvent(point,nState,aView);
	}
	else
	{
		if(nState == MINPUT_LEFT)
		{
			m_AISContext->Select(Standard_True);
		}
	}
}
	
//����shift���µĵ�ѡ��ʽ
void	ScCmdMgr::ShiftInputEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}

//����ƶ�����Ӧ����
void	ScCmdMgr::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	if(this->m_pScCmd)
	{
		//����Ҫѡ��ʱ��������ͳһ�������⵽�����ָô��롣
		if(IsNeedSelect() || IsMultiSelect())
		{
			m_AISContext->MoveTo(point.x,point.y,aView,Standard_True);
		}

		//�Զ���׽��ʵ��
		if(m_pSnapMgr->IsEnable())
		{
			m_pSnapMgr->Snap(point.x,point.y,aView);
		}

		this->m_pScCmd->MoveEvent(point,aView);
	}
}

//����shift���µ�����ƶ�����Ӧ����
void	ScCmdMgr::ShiftMoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}


//�����Ϣ��Ӧ����
void	ScCmdMgr::OnLButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
	if(this->m_pScCmd)
	{
		this->m_pScCmd->OnLButtonDown(nFlags,point,aView);
	}
}

void	ScCmdMgr::OnLButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
	if(this->m_pScCmd)
	{
		this->m_pScCmd->OnLButtonUp(nFlags,point,aView);
	}
}

void	ScCmdMgr::OnMouseMove(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
	if(this->m_pScCmd)
	{
		this->m_pScCmd->OnMouseMove(nFlags,point,aView);
	}
}

void	ScCmdMgr::OnLButtonDblClk(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
	if(this->m_pScCmd)
	{
		this->m_pScCmd->OnLButtonDblClk(nFlags,point,aView);
	}
}

void	ScCmdMgr::OnRButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
	if(this->m_pScCmd)
	{
		this->m_pScCmd->OnRButtonDown(nFlags,point,aView);
	}
}

void	ScCmdMgr::OnRButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
	if(this->m_pScCmd)
	{
		this->m_pScCmd->OnRButtonUp(nFlags,point,aView);
	}
}
