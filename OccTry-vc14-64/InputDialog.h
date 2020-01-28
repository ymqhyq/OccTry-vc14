#pragma once

#include "Resource.h"

class ScInputMgr;
class ScCommand;
// CInputDialog �Ի���

class CInputDialog : public CDialog
{
	DECLARE_DYNAMIC(CInputDialog)

public:
	CInputDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputDialog();

	//������Ϣ��ʾ����������
	void			NeedInput(LPCTSTR lpszTitle,int nInputType);
	//��ʾ��Ϣ
	void			Show(LPCTSTR lpszText);
	void			Show(int nVal);
	void			Show(double dVal);
	void			Show(gp_Pnt pnt);

	//������ʱ��
	int				StartTimer(DWORD dwTime);
	//ֹͣ��ʱ��
	int				StopTimer();

	void			SetInputMgr(ScInputMgr *pMgr);

	//������ǰִ�е�����
	int				AttachScCmd(ScCommand *pScCmd);
	void			DetachScCmd();//ȥ����

	//Ϊ������׼��
	void			ReadyForInput();

// �Ի�������
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR  nIDEvent);//yxk20200104,UINT->UINTPTR

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();

public:
	CString m_szInput;

protected:
	//��������
	int				ParseInput();
	
protected:
	UINT_PTR		m_nTimer;//��ʱ��
	ScInputMgr		*m_pInputMgr;//�����Ĺ�����
	ScCommand		*m_pScCmd;

	CString			m_szTitle;//������ʾ��Ϣ
	int				m_nInputType;//��������,ö������
};
