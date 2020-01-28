#pragma once

#include "Resource.h"

class ScInputMgr;
class ScCommand;
// CInputDialog 对话框

class CInputDialog : public CDialog
{
	DECLARE_DYNAMIC(CInputDialog)

public:
	CInputDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInputDialog();

	//设置信息提示和输入类型
	void			NeedInput(LPCTSTR lpszTitle,int nInputType);
	//显示信息
	void			Show(LPCTSTR lpszText);
	void			Show(int nVal);
	void			Show(double dVal);
	void			Show(gp_Pnt pnt);

	//启动定时器
	int				StartTimer(DWORD dwTime);
	//停止定时器
	int				StopTimer();

	void			SetInputMgr(ScInputMgr *pMgr);

	//关联当前执行的命令
	int				AttachScCmd(ScCommand *pScCmd);
	void			DetachScCmd();//去关联

	//为输入做准备
	void			ReadyForInput();

// 对话框数据
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR  nIDEvent);//yxk20200104,UINT->UINTPTR

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();

public:
	CString m_szInput;

protected:
	//解析输入
	int				ParseInput();
	
protected:
	UINT_PTR		m_nTimer;//定时器
	ScInputMgr		*m_pInputMgr;//关联的管理器
	ScCommand		*m_pScCmd;

	CString			m_szTitle;//标题提示信息
	int				m_nInputType;//输入类型,枚举类型
};
