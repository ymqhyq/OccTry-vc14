#pragma once

#include "ScInput.h"

class COccTryView;
class CInputDialog;
class ScCmdMgr;
class ScInputBar;

//ʵ�ּ�����Ĺ���

enum{
	IN_MOUSE_MOVE,
	IN_LMOUSE_DOWN,
	IN_RMOUSE_DOWN,
};

class ScInputMgr
{
public:
	ScInputMgr(ScCmdMgr& cmdMgr,COccTryView *pViewWnd,CInputDialog *pDlg);
	~ScInputMgr(void);

	//��Ҫ����ʼ��������
	virtual int			NeedInput(LPCTSTR lpszTitle,int nInputType);

	//��Ҫ��������
	virtual int			NeedInput(const ScInputReq& inputReq);
	//������������
	virtual int			EndInput();

	//��ʾ��Ϣ����
	void				Show(LPCTSTR lpszVal);
	void				Show(int nVal);
	void				Show(double dVal);
	void				Show(gp_Pnt pnt);

	//�����Ϣ������
	virtual int			MouseEvent(const CPoint& point,int nEvent);

	//��ʱ��������,��ʱ���ж�ʱ����
	virtual int			OnTimerProc();
	
	//��ʱ�������ڣ�ms��λ
	void				SetTimerPeriod(DWORD dwTime);
	void				SetWaitTime(DWORD dwTime);
	DWORD				WaitTime() const { return m_dwWaitTime; }

protected:
	ScCmdMgr&			m_scCmdMgr;//���������������
	COccTryView			*m_pViewWnd;//������view����
	CInputDialog		*m_pInputDlg;//���봰��
	ScInputBar			*m_pInputBar;//����controlbar.

	ScInputReq			m_inputReq;//��ǰ����������

	BOOL				m_bNeedInput;//��Ҫ���룬��ʶ��

protected:
	DWORD				m_dwTimerPeriod;//��ʱ������
	DWORD				m_dwWaitTime;//����ʱ��

	int					m_nTimerCnt;//�������Զ�ʱ�����ڼ�.
	CPoint				m_movePoint;  //�ƶ�λ��
	
	BOOL				m_bStartTimer;//�Ƿ�������ʱ��
	BOOL				m_bShowDlg; //�Ƿ���ʾ����
};
