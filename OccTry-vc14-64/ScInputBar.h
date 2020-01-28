#pragma once

#include "scbarg.h"
#include "ScInputEdit.h"
#include "ScInput.h"

// ScInputBar

class ScInputBar : public CSizingControlBarG
{
	DECLARE_DYNAMIC(ScInputBar)

public:
	ScInputBar();
	virtual ~ScInputBar();

	//需要接收输入
	virtual int			NeedInput(const ScInputReq& inputReq);
	//结束结束输入
	virtual int			EndInput();

	int					AcceptInput(const ScInputVar& inputVar);

	BOOL				IsNeedInput() const { return m_bNeedInput; }

	ScInputEdit&		InputEdit() { return m_inputEdit; }

	//显示信息函数
	void				Show(LPCTSTR lpszVal);
	void				Show(int nVal);
	void				Show(double dVal);
	void				Show(gp_Pnt pnt);

	//显示对象的属性信息。这里是简单的信息。
	void				ShowProp(LPCTSTR lpszProp);

public:

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

protected:
	DECLARE_MESSAGE_MAP()

protected:
	ScInputEdit			m_inputEdit;//输入
	CStatic				m_stPrompt;//提示信息
	CStatic             m_stPropHint;//
	CEdit				m_propEdit;//显示一些属性信息。

	CFont				m_font;

	BOOL				m_bCreated;

	ScInputReq			m_inputReq;
	BOOL				m_bNeedInput;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


