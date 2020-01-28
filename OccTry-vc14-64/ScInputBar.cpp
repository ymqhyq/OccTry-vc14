// ScInputBar.cpp : 实现文件
//

#include "stdafx.h"
#include "OccTry.h"
#include "ScInputBar.h"
#include ".\scinputbar.h"
#include "ScCommand.h"


// ScInputBar

IMPLEMENT_DYNAMIC(ScInputBar, CSizingControlBarG)
ScInputBar::ScInputBar()
{
	m_szMinHorz = CSize(33, 32);
    m_szMinVert = CSize(33, 32);
    m_szMinFloat = CSize(37, 32);
    m_szHorz = CSize(200, 200);
    m_szVert = CSize(200, 40);
    m_szFloat = CSize(200, 200);
    m_bTracking = FALSE;
    m_bKeepSize = FALSE;
    m_bParentSizing = FALSE;
    m_cxEdge = 5;
    m_bDragShowContent = FALSE;
    m_nDockBarID = 0;
    m_dwSCBStyle = 0;

	this->m_bCreated = FALSE;

	this->m_bNeedInput = FALSE;
}

ScInputBar::~ScInputBar()
{
}

//需要接收输入
int		ScInputBar::NeedInput(const ScInputReq& inputReq)
{
	if(this->m_bNeedInput)
	{
		EndInput();
	}
	//显示提示信息
	this->m_stPrompt.SetWindowText(inputReq._szDesc);

	this->m_inputReq = inputReq;
	this->m_bNeedInput = TRUE;
	this->m_inputEdit.BeginInput(inputReq._nVarType,this);

	return 0;
}

//结束结束输入
int		ScInputBar::EndInput()
{
	//显示提示信息
	this->m_stPrompt.SetWindowText("输入:");
	this->m_bNeedInput = FALSE;
	this->m_inputEdit.EndInput();

	return 0;
}

int		ScInputBar::AcceptInput(const ScInputVar& inputVar)
{
	ASSERT(this->m_bNeedInput);
	//调用cmd的接收函数
	if(this->m_inputReq._pCmd)
	{
		this->m_inputReq._pCmd->AcceptInput(m_inputReq,inputVar);
	}

	return 0;
}

//显示信息函数
void	ScInputBar::Show(LPCTSTR lpszVal)
{
	if(!lpszVal)
		this->m_inputEdit.SetWindowText(lpszVal);
	else
		this->m_inputEdit.SetWindowText("");
}

void	ScInputBar::Show(int nVal)
{
	CString szV;
	szV.Format("%d",nVal);
	this->m_inputEdit.SetWindowText(szV);
}

void	ScInputBar::Show(double dVal)
{
	CString szV;
	szV.Format("%f",dVal);
	this->m_inputEdit.SetWindowText(szV);
}

void	ScInputBar::Show(gp_Pnt pnt)
{
	CString szV;
	szV.Format("%f,%f,%f",pnt.X(),pnt.Y(),pnt.Z());
	this->m_inputEdit.SetWindowText(szV);
}

//显示对象的属性信息。这里是简单的信息。
void	ScInputBar::ShowProp(LPCTSTR lpszProp)
{
	this->m_propEdit.SetWindowText(lpszProp);
}


BEGIN_MESSAGE_MAP(ScInputBar, CSizingControlBarG)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// ScInputBar 消息处理程序
void ScInputBar::OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/)
{
}

int ScInputBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBarG::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES);
	//
	if(!m_stPrompt.Create(_T("输入框:"), WS_CHILD|WS_VISIBLE|SS_LEFT, 
			CRect(0,0,60,14), this,1))
	{
		return -1;
	}

	if(!m_inputEdit.Create(WS_CHILD|WS_VISIBLE|ES_LEFT ,
		CRect(0,80,100,14), this, 2))
	{
		return -1;
	}
	m_inputEdit.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	if(!m_stPropHint.Create(_T("对象信息:"),WS_CHILD|WS_VISIBLE|SS_LEFT, 
			CRect(0,0,60,14), this,3))
	{
		return -1;
	}

	if(!m_propEdit.Create(WS_CHILD|WS_VISIBLE|ES_LEFT |ES_MULTILINE |ES_WANTRETURN |ES_AUTOVSCROLL ,
		CRect(100,180,100,40), this, 4))
	{
		return -1;
	}

	m_propEdit.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	// older versions of Windows* (NT 3.51 for instance)
	// fail with DEFAULT_GUI_FONT
	if (!m_font.CreateStockObject(DEFAULT_GUI_FONT))
	{
		if (!m_font.CreatePointFont(80, "MS Sans Serif"))
			return -1;
	}

	m_inputEdit.SetFont(&m_font);
	m_stPrompt.SetFont(&m_font);
	m_propEdit.SetFont(&m_font);
	m_stPropHint.SetFont(&m_font);

	this->m_bCreated = TRUE;

	return 0;
}

void ScInputBar::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CSizingControlBarG::OnGetMinMaxInfo(lpMMI);
}

void ScInputBar::OnSize(UINT nType, int cx, int cy)
{
	m_szVert.cx = cx;
	m_szVert.cy = 50;

	CSizingControlBarG::OnSize(nType, cx, cy);

	//调整大小
	if(this->m_bCreated)
	{
		if(cx <= 0 || cy <= 0 || cx > 0x1000 || cy > 0x1000)
			return;

		m_szVert.cx = cx;
		m_szVert.cy = 50;


		int nW1 = 300,nW2 = 300;
		int nH1 = 14,nH2 = 20;

		m_stPrompt.MoveWindow(4,4,nW1,nH1);
		if(cx < nW2)
			nW2 = cx - 8;
		m_inputEdit.MoveWindow(4,nH1 + 8,nW2,nH2);

		//prop的大小
		int nW3 = cx - nW1 - 20;
		m_stPropHint.MoveWindow(nW1 + 16,4,nW3,nH1);
		m_propEdit.MoveWindow(nW1 + 16,nH1 + 8,cx - nW1 - 20,cy - nH1 - 12);
	}
}
