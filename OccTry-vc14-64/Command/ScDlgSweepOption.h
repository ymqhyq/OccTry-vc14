#pragma once


#include "..\resource.h"

//扫掠类型
enum{
	Sweep_CorrectedFrenet,
	Sweep_Frenet,
	Sweep_Fixed,
	Sweep_ConstantNormal,
	Sweep_GuideAC,
	Sweep_GuidePlan,
	Sweep_GuideACWithContact,
	Sweep_GuidePlanWithContact,
};

// ScDlgSweepOption 对话框

class ScDlgSweepOption : public CDialog
{
	DECLARE_DYNAMIC(ScDlgSweepOption)

public:
	ScDlgSweepOption(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ScDlgSweepOption();

	int			m_nSweepType;//扫掠类型
	gp_Vec		m_vBiNorm;//从法向量
	gp_Ax2		m_ax2;//fix坐标系

// 对话框数据
	enum { IDD = IDD_DIALOG_PIPESHELL };

protected:
	void		EnableBiNorm(BOOL bEnable);
	void		EnableFixed(BOOL bEnable);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnCbnSelchangeComboSweeptype();
};
