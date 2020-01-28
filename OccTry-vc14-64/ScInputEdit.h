#pragma once

#include "ScInput.h"

class ScInputBar;

// ScInputEdit

class ScInputEdit : public CEdit
{
	DECLARE_DYNAMIC(ScInputEdit)

public:
	ScInputEdit();
	virtual ~ScInputEdit();

	int					BeginInput(int nVarType,ScInputBar *pInputBar);
	int					EndInput();

protected:
	//��������
	int					AcceptInput();

protected:
	int					m_nVarType;
	ScInputBar			*m_pInputBar;//������bar

	BOOL				m_bInput;//�Ƿ�������

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnSetfocus();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};


