// MainFrm.h : CMainFrame 类的接口
//


#pragma once

class COccTryDoc;
class ScInputBar;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 属性
public:

// 操作
public:
	//显示提示信息
	void			ShowPrompt(LPCTSTR lpszText);
	void			ShowMousePos(LPCTSTR lpszText);
	ScInputBar*		GetInputBar() { return m_pInputBar; }

	void			ShowProp(LPCTSTR lpszProp);

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	COccTryDoc *	GetCurrentDoc();

	BOOL			CreateInputBar();

	BOOL			VerifyBarState(LPCTSTR lpszProfileName);

protected:  // 控件条嵌入成员
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	ScInputBar		*m_pInputBar;//

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInputBar();
	afx_msg void OnUpdateInputBar(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	virtual BOOL DestroyWindow();
};


