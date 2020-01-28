// MainFrm.h : CMainFrame ��Ľӿ�
//


#pragma once

class COccTryDoc;
class ScInputBar;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// ����
public:

// ����
public:
	//��ʾ��ʾ��Ϣ
	void			ShowPrompt(LPCTSTR lpszText);
	void			ShowMousePos(LPCTSTR lpszText);
	ScInputBar*		GetInputBar() { return m_pInputBar; }

	void			ShowProp(LPCTSTR lpszProp);

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
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

protected:  // �ؼ���Ƕ���Ա
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	ScInputBar		*m_pInputBar;//

// ���ɵ���Ϣӳ�亯��
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


