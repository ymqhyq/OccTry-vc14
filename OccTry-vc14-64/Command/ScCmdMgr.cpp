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

//执行命令
int		ScCmdMgr::RunCommand(ScCommand *pScCmd)
{
	if(!pScCmd) return SC_ERR;

	if(this->IsCmdRunning())//已经有命令执行
		return SC_ERR;

	//释放上次要释放的cmd
	if(this->m_pToFreeCmd)
	{
		delete this->m_pToFreeCmd;
		this->m_pToFreeCmd = NULL;
	}

	this->m_nState = CMS_RUNNING;
	this->m_pScCmd = pScCmd;
	//进行必要的设置
	this->m_pScCmd->SetScCmdMgr(this);
	this->m_pScCmd->SetScView(this->m_pScView);
	this->m_pScCmd->SetInputMgr(this->m_pInputMgr);
	this->m_pScCmd->SetUcsMgr(this->m_pScUcsMgr);
	this->m_pScCmd->SetSnapMgr(this->m_pSnapMgr);
	this->m_pScCmd->SetUndoMgr(&this->m_undoMgr);

	//开始执行命令
	this->m_pScCmd->Begin(this->m_AISContext);

	return SC_OK;
}

//终止当前命令执行
int		ScCmdMgr::StopCommand()
{
	if(!this->IsCmdRunning())
		return SC_OK;

	//结束处理。
	this->m_pScCmd->End();

	//删除对象
	delete this->m_pScCmd;
	this->m_nState = CMS_IDLE;
	this->m_pScCmd = NULL;

	return SC_OK;
}

//清除所有命令
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
	//暂不支持命令中undo
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
	//	//删除了，暂不记录。
	//	delete pScCmd;
	//}

	return SC_OK;
}

int		ScCmdMgr::RedoCommand()
{
	return SC_OK;
}

//清除redo列表
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

//外部通知
int		ScCmdMgr::CmdFinished()
{
	if(this->m_pScCmd)
	{
		if(this->m_pScCmd->IsDone() && this->m_pScCmd->CanUndo())//记录可以undo的命令。
			this->m_scCmdList.AddTail(this->m_pScCmd);
		else
			this->m_pToFreeCmd = this->m_pScCmd;//临时记录，不立即删除，防止出问题。

		this->m_nState = CMS_IDLE;
		this->m_pScCmd = NULL;
	}

	return 0;
}

//命令中止调用。
int		ScCmdMgr::CmdTerminated()
{
	if(this->m_pScCmd)
	{
		this->m_pToFreeCmd = this->m_pScCmd;//临时记录，不立即删除，防止出问题。

		this->m_nState = CMS_IDLE;
		this->m_pScCmd = NULL;
	}
	return 0;
}

//框选方式下的消息响应函数
void	ScCmdMgr::DragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView)
{
}

//按下shift下的框选方式
void	ScCmdMgr::ShiftDragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView)
{
}

//
// 点选方式下的消息响应函数.这里处理单选和多选的情况。
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
	
//按下shift键下的点选方式
void	ScCmdMgr::ShiftInputEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}

//鼠标移动的响应函数
void	ScCmdMgr::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	if(this->m_pScCmd)
	{
		//当需要选择时，在这里统一处理，避免到处出现该代码。
		if(IsNeedSelect() || IsMultiSelect())
		{
			m_AISContext->MoveTo(point.x,point.y,aView,Standard_True);
		}

		//自动捕捉的实现
		if(m_pSnapMgr->IsEnable())
		{
			m_pSnapMgr->Snap(point.x,point.y,aView);
		}

		this->m_pScCmd->MoveEvent(point,aView);
	}
}

//按下shift键下的鼠标移动的响应函数
void	ScCmdMgr::ShiftMoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}


//鼠标消息响应函数
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
