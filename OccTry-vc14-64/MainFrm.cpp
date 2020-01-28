// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "OccTry.h"

#include "OccTryDoc.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include ".\mainfrm.h"
#include "ScInputBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(IDM_INPUT_BAR, OnInputBar)
	ON_UPDATE_COMMAND_UI(IDM_INPUT_BAR, OnUpdateInputBar)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	IDS_IND_LOC,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	this->m_pInputBar = NULL;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	//输入bar创建。
	if(!this->m_pInputBar)
		this->m_pInputBar = new ScInputBar();

	
    if (!m_pInputBar->Create(_T("Input bar"), this, IDR_INPUT_SIZECBAR))
    {
        TRACE0("Failed to create input bar\n");
        return -1;      // fail to create
	}

    m_pInputBar->SetBarStyle(m_pInputBar->GetBarStyle() |
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);


	// TODO: 如果不需要工具栏可停靠，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_pInputBar->EnableDocking(CBRS_ALIGN_BOTTOM);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DockControlBar(m_pInputBar,AFX_IDW_DOCKBAR_BOTTOM);

	CString sProfile(_T("BarState"));
    if (VerifyBarState(sProfile))
    {
        m_pInputBar->LoadState(sProfile);
        LoadBarState(sProfile);
    }

	//调整宽度
	UINT nID,nStyle;
	int nW;
	m_wndStatusBar.GetPaneInfo(1,nID,nStyle,nW);
	nW = 240;
	m_wndStatusBar.SetPaneInfo(1,nID,nStyle,nW);

	return 0;
}

void CMainFrame::OnDestroy()
{
	CMDIFrameWnd::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

BOOL CMainFrame::DestroyWindow()
{
	CString sProfile(_T("BarState"));
	m_pInputBar->SaveState(sProfile);
    SaveBarState(sProfile);

	return CMDIFrameWnd::DestroyWindow();
}

BOOL	CMainFrame::CreateInputBar()
{
	return FALSE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return TRUE;
}

//显示提示信息
void	CMainFrame::ShowPrompt(LPCTSTR lpszText)
{
	this->m_wndStatusBar.SetPaneText(0,lpszText);
}

void	CMainFrame::ShowMousePos(LPCTSTR lpszText)
{
	this->m_wndStatusBar.SetPaneText(1,lpszText);
}

void	CMainFrame::ShowProp(LPCTSTR lpszProp)
{
	if(this->m_pInputBar)
		this->m_pInputBar->ShowProp(lpszProp);
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序
COccTryDoc *	CMainFrame::GetCurrentDoc()
{
	COccTryDoc *pDoc = NULL;

	CChildFrame *pChildFrm = (CChildFrame *)this->GetActiveFrame();
	if(pChildFrm)
	{
		pDoc = (COccTryDoc *)pChildFrm->GetActiveDocument();
	}

	return pDoc;
}

//
// 最先截获消息，做一些预处理。
//
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	//Esc取消命令的处理
	if(pMsg->message == WM_KEYUP)
	{
		if(pMsg->wParam == VK_ESCAPE)
		{
			COccTryDoc *pDocument = GetCurrentDoc();
			if(pDocument)
			{
				pDocument->OnCancelEvent();
			}
		}
		else if(VK_DELETE == pMsg->wParam)
		{
			COccTryDoc *pDocument = GetCurrentDoc();
			if(pDocument)
			{
				pDocument->OnDeleteEvent();
			}
		}
	}

	//输入处理
	if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		if(this->m_pInputBar && this->m_pInputBar->IsNeedInput())
		{//需要输入
			if(pMsg->message == WM_KEYDOWN)
			{
				//设置输入焦点
				CWnd *pActiveWnd = this->GetActiveWindow();
				if(pActiveWnd == NULL ||
					(pActiveWnd->GetSafeHwnd() != m_pInputBar->InputEdit().GetSafeHwnd()))
				{
					this->m_pInputBar->InputEdit().SetFocus();
				}
			}
		}
	}

	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWnd::OnSize(nType, cx, cy);

	//if(this->m_pInputBar)
	//{
	//	CRect rc;
	//	GetClientRect(&rc);
	//	this->m_pInputBar->MoveWindow(rc.left,rc.bottom - 30,rc.Width(),30);
	//}
}

void CMainFrame::OnInputBar()
{
	if(this->m_pInputBar)
	{
		BOOL bVisible = this->m_pInputBar->IsVisible();
		ShowControlBar(m_pInputBar,!bVisible,FALSE);
	}
}

void CMainFrame::OnUpdateInputBar(CCmdUI *pCmdUI)
{
	if(this->m_pInputBar)
	{
		pCmdUI->Enable();
		pCmdUI->SetCheck(m_pInputBar->IsVisible());
	}
}

// This function is Copyright (c) 2000, Cristi Posea.
// See www.datamekanix.com for more control bars tips&tricks.
BOOL CMainFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
        ASSERT(pInfo != NULL);
        int nDockedCount = pInfo->m_arrBarID.GetSize();
        if (nDockedCount > 0)
        {
            // dockbar
            for (int j = 0; j < nDockedCount; j++)
            {
                UINT nID = (UINT) pInfo->m_arrBarID[j];
                if (nID == 0) continue; // row separator
                if (nID > 0xFFFF)
                    nID &= 0xFFFF; // placeholder - get the ID
                if (GetControlBar(nID) == NULL)
                    return FALSE;
            }
        }
        
        if (!pInfo->m_bFloating) // floating dockbars can be created later
            if (GetControlBar(pInfo->m_nBarID) == NULL)
                return FALSE; // invalid bar ID
    }

    return TRUE;
}




