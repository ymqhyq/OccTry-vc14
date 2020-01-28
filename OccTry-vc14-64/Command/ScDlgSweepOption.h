#pragma once


#include "..\resource.h"

//ɨ������
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

// ScDlgSweepOption �Ի���

class ScDlgSweepOption : public CDialog
{
	DECLARE_DYNAMIC(ScDlgSweepOption)

public:
	ScDlgSweepOption(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ScDlgSweepOption();

	int			m_nSweepType;//ɨ������
	gp_Vec		m_vBiNorm;//�ӷ�����
	gp_Ax2		m_ax2;//fix����ϵ

// �Ի�������
	enum { IDD = IDD_DIALOG_PIPESHELL };

protected:
	void		EnableBiNorm(BOOL bEnable);
	void		EnableFixed(BOOL bEnable);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnCbnSelchangeComboSweeptype();
};
