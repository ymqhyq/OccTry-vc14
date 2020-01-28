#pragma once

#include "..\resource.h"

// ScOffsetShapeDlg 对话框

class ScOffsetShapeDlg : public CDialog
{
	DECLARE_DYNAMIC(ScOffsetShapeDlg)

public:
	ScOffsetShapeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ScOffsetShapeDlg();

	int			m_nOffMode;
	int			m_nJoinType;

// 对话框数据
	enum { IDD = IDD_DIALOG_OFFSETSHAPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
