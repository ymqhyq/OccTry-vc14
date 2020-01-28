// MainFrm.cpp : CMainFrame ���ʵ��
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
	ID_SEPARATOR,           // ״̬��ָʾ��
	IDS_IND_LOC,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
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
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}

	//����bar������
	if(!this->m_pInputBar)
		this->m_pInputBar = new ScInputBar();

	
    if (!m_pInputBar->Create(_T("Input bar"), this, IDR_INPUT_SIZECBAR))
    {
        TRACE0("Failed to create input bar\n");
        return -1;      // fail to create
	}

    m_pInputBar->SetBarStyle(m_pInputBar->GetBarStyle() |
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);


	// TODO: �������Ҫ��������ͣ������ɾ��������
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

	//�������
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

	// TODO: �ڴ˴������Ϣ����������
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
	// TODO: �ڴ˴�ͨ���޸� CREATESTRUCT cs ���޸Ĵ������
	// ��ʽ

	return TRUE;
}

//��ʾ��ʾ��Ϣ
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

// CMainFrame ���

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


// CMainFrame ��Ϣ�������
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
// ���Ƚػ���Ϣ����һЩԤ����
//
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	//Escȡ������Ĵ���
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

	//���봦��
	if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		if(this->m_pInputBar && this->m_pInputBar->IsNeedInput())
		{//��Ҫ����
			if(pMsg->message == WM_KEYDOWN)
			{
				//�������뽹��
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




