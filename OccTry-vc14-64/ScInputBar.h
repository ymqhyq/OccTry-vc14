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

	//��Ҫ��������
	virtual int			NeedInput(const ScInputReq& inputReq);
	//������������
	virtual int			EndInput();

	int					AcceptInput(const ScInputVar& inputVar);

	BOOL				IsNeedInput() const { return m_bNeedInput; }

	ScInputEdit&		InputEdit() { return m_inputEdit; }

	//��ʾ��Ϣ����
	void				Show(LPCTSTR lpszVal);
	void				Show(int nVal);
	void				Show(double dVal);
	void				Show(gp_Pnt pnt);

	//��ʾ�����������Ϣ�������Ǽ򵥵���Ϣ��
	void				ShowProp(LPCTSTR lpszProp);

public:

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

protected:
	DECLARE_MESSAGE_MAP()

protected:
	ScInputEdit			m_inputEdit;//����
	CStatic				m_stPrompt;//��ʾ��Ϣ
	CStatic             m_stPropHint;//
	CEdit				m_propEdit;//��ʾһЩ������Ϣ��

	CFont				m_font;

	BOOL				m_bCreated;

	ScInputReq			m_inputReq;
	BOOL				m_bNeedInput;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


