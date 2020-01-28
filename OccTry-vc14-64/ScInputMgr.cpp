#include "StdAfx.h"
#include "OccTryView.h"
#include "mainFrm.h"
#include "InputDialog.h"
#include "ScCmdMgr.h"
#include ".\scinputmgr.h"
#include "ScInput.h"
#include "ScInputBar.h"

#define MOUSE_RANGE  5

ScInputMgr::ScInputMgr(ScCmdMgr& cmdMgr,COccTryView *pViewWnd,CInputDialog *pDlg)
:m_scCmdMgr(cmdMgr),m_pViewWnd(pViewWnd),m_pInputDlg(pDlg)
{
	ASSERT(pViewWnd && pDlg);

	this->m_dwTimerPeriod = 100;//
	this->m_dwWaitTime = 1000;//0.5秒空闲，则弹出窗口。

	this->m_nTimerCnt = 0;
	this->m_bNeedInput = FALSE;
	this->m_bStartTimer = FALSE;
	this->m_bShowDlg = FALSE;
	this->m_pInputBar = NULL;

	m_scCmdMgr.SetInputMgr(this);
}

ScInputMgr::~ScInputMgr(void)
{
}

//需要并开始接收输入
int		ScInputMgr::NeedInput(LPCTSTR lpszTitle,int nInputType)
{
	if(!this->m_pInputDlg)
		return SC_ERR;

	this->m_pInputDlg->NeedInput(lpszTitle,nInputType);

	//启动inputdialg的定时器。
	this->m_pInputDlg->SetInputMgr(this);
	this->m_pInputDlg->StartTimer(this->m_dwTimerPeriod);
	this->m_bStartTimer = TRUE;
	
	this->m_nTimerCnt = 0;

	//将当前命令关联到inputdialog
	ASSERT(this->m_scCmdMgr.IsCmdRunning());
	this->m_pInputDlg->AttachScCmd(this->m_scCmdMgr.CurrentCmd());

	this->m_bNeedInput = TRUE;

	return 0;
}

int		ScInputMgr::NeedInput(const ScInputReq& inputReq)
{
	if(!inputReq.IsValid())
		return -1;

	this->m_inputReq = inputReq;

	if(inputReq._nInputMode == ScInputReq::eInputBar)
	{
		if(this->m_pInputBar == NULL)
		{
			CMainFrame *pFrm = (CMainFrame *)::AfxGetMainWnd();
			if(pFrm)
			{
				this->m_pInputBar = pFrm->GetInputBar();
			}
		}

		if(this->m_pInputBar == NULL)
		{
			ASSERT(FALSE);
			return SC_ERR;
		}

		this->m_pInputBar->NeedInput(inputReq);
		this->m_bNeedInput = TRUE;
	}
	else
	{
		NeedInput(inputReq._szDesc,inputReq._nVarType);
	}

	return 0;
}

//
// 结束接收输入.
// 停止定时器。隐藏输入框。
//
int		ScInputMgr::EndInput()
{
	if(!this->m_bNeedInput)
		return SC_OK;

	this->m_bNeedInput = FALSE;

	if(this->m_inputReq._nInputMode == ScInputReq::eInputDlg)
	{
		//停止定时器
		if(this->m_bStartTimer)
		{
			this->m_pInputDlg->StopTimer();
			this->m_bStartTimer = FALSE;
		}

		if(this->m_bShowDlg)
		{
			this->m_pInputDlg->ShowWindow(SW_HIDE);
			this->m_bShowDlg = FALSE;
		}

		//命令去关联
		this->m_pInputDlg->DetachScCmd();
	}
	else
	{
		if(this->m_pInputBar)
		{
			this->m_pInputBar->EndInput();
		}
	}

	return 0;
}

//显示信息函数
void	ScInputMgr::Show(LPCTSTR lpszVal)
{
	if(this->m_pInputDlg)
	{
		this->m_pInputDlg->Show(lpszVal);
	}
}

void	ScInputMgr::Show(int nVal)
{
	if(this->m_inputReq._nInputMode == ScInputReq::eInputDlg)
	{
		if(this->m_pInputDlg)
		{
			this->m_pInputDlg->Show(nVal);
		}
	}
	else if(this->m_inputReq._nInputMode == ScInputReq::eInputBar)
	{
		if(this->m_pInputBar)
		{
			this->m_pInputBar->Show(nVal);
		}
	}
}

void	ScInputMgr::Show(double dVal)
{
	if(this->m_inputReq._nInputMode == ScInputReq::eInputDlg)
	{
		if(this->m_pInputDlg)
		{
			this->m_pInputDlg->Show(dVal);
		}
	}
	else if(this->m_inputReq._nInputMode == ScInputReq::eInputBar)
	{
		if(this->m_pInputBar)
		{
			this->m_pInputBar->Show(dVal);
		}
	}
}

void	ScInputMgr::Show(gp_Pnt pnt)
{
	if(this->m_inputReq._nInputMode == ScInputReq::eInputDlg)
	{
		if(this->m_pInputDlg)
		{
			this->m_pInputDlg->Show(pnt);
		}
	}
	else if(this->m_inputReq._nInputMode == ScInputReq::eInputBar)
	{
		if(this->m_pInputBar)
		{
			this->m_pInputBar->Show(pnt);
		}
	}
}

//
// 鼠标消息处理函数
// 需要处理的消息包括：鼠标移动、鼠标点击。
//
int		ScInputMgr::MouseEvent(const CPoint& point,int nEvent)
{
	if(!this->m_bNeedInput)
		return 0;

	switch(nEvent)
	{
	case IN_MOUSE_MOVE:
		{
			int dx = point.x - m_movePoint.x;
			int dy = point.y - m_movePoint.y;
			if((abs(dx) <= MOUSE_RANGE) && (abs(dy) <= MOUSE_RANGE))
				return 0;

			BOOL bHide = ((dx > 0) && (dy > 0)) ? FALSE : TRUE;
			
			//显示窗口，鼠标向窗口外移动，隐藏窗口。重启定时器，否则
			//可能用户希望输入信息，显示窗口。
			if(this->m_bShowDlg && bHide)
			{
				DTRACE("\n restart timer.");
				this->m_pInputDlg->ShowWindow(SW_HIDE);
				this->m_pInputDlg->StartTimer(this->m_dwTimerPeriod);
				this->m_bShowDlg = FALSE;
				this->m_bStartTimer = TRUE;
			}

			//重置计数
			this->m_nTimerCnt = 0;
			
			//记录移动位置
			this->m_movePoint = point;
		}
		break;
	case IN_LMOUSE_DOWN:
		break;
	case IN_RMOUSE_DOWN:
		break;
	default:
		break;
	}

	return 0;
}

//定时器处理函数,定时器中定时调用
int		ScInputMgr::OnTimerProc()
{
	this->m_nTimerCnt ++;
	DTRACE("\n timer count %d...",this->m_nTimerCnt);
	if(this->m_nTimerCnt * this->m_dwTimerPeriod >= this->m_dwWaitTime)
	{
		//显示输入框
		CRect rectWnd;
		this->m_pInputDlg->GetWindowRect(&rectWnd);
		CPoint pnt(this->m_movePoint);
		this->m_pViewWnd->ClientToScreen(&pnt);
		//
		this->m_pInputDlg->MoveWindow(pnt.x + MOUSE_RANGE,pnt.y + MOUSE_RANGE,
										rectWnd.Width(),rectWnd.Height());
		this->m_pInputDlg->ShowWindow(SW_SHOW);
		this->m_pInputDlg->ReadyForInput();
		//
		this->m_nTimerCnt = 0;
		this->m_bShowDlg = TRUE;

		//简单处理
		//应当暂停定时器
		this->m_pInputDlg->StopTimer();
		this->m_bStartTimer = FALSE;
	}

	return 0;
}

//定时器的周期，ms单位
void	ScInputMgr::SetTimerPeriod(DWORD dwTime)
{
	this->m_dwTimerPeriod = dwTime;
}

void	ScInputMgr::SetWaitTime(DWORD dwTime)
{
	this->m_dwWaitTime = dwTime;
}