#pragma once

#include "ScInput.h"

class COccTryView;
class CInputDialog;
class ScCmdMgr;
class ScInputBar;

//实现简单输入的管理

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

	//需要并开始接收输入
	virtual int			NeedInput(LPCTSTR lpszTitle,int nInputType);

	//需要接收输入
	virtual int			NeedInput(const ScInputReq& inputReq);
	//结束结束输入
	virtual int			EndInput();

	//显示信息函数
	void				Show(LPCTSTR lpszVal);
	void				Show(int nVal);
	void				Show(double dVal);
	void				Show(gp_Pnt pnt);

	//鼠标消息处理函数
	virtual int			MouseEvent(const CPoint& point,int nEvent);

	//定时器处理函数,定时器中定时调用
	virtual int			OnTimerProc();
	
	//定时器的周期，ms单位
	void				SetTimerPeriod(DWORD dwTime);
	void				SetWaitTime(DWORD dwTime);
	DWORD				WaitTime() const { return m_dwWaitTime; }

protected:
	ScCmdMgr&			m_scCmdMgr;//关联的命令管理器
	COccTryView			*m_pViewWnd;//关联的view窗口
	CInputDialog		*m_pInputDlg;//输入窗口
	ScInputBar			*m_pInputBar;//输入controlbar.

	ScInputReq			m_inputReq;//当前的输入请求

	BOOL				m_bNeedInput;//需要输入，标识。

protected:
	DWORD				m_dwTimerPeriod;//定时器周期
	DWORD				m_dwWaitTime;//空闲时间

	int					m_nTimerCnt;//计数，以定时器周期计.
	CPoint				m_movePoint;  //移动位置
	
	BOOL				m_bStartTimer;//是否启动定时器
	BOOL				m_bShowDlg; //是否显示窗口
};
