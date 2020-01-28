#pragma once

#include "..\Resource.h"

// ScDlgFillingOptions 对话框

class ScDlgFillingOptions : public CDialog
{
	DECLARE_DYNAMIC(ScDlgFillingOptions)

public:
	ScDlgFillingOptions(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ScDlgFillingOptions();

// 对话框数据
	enum { IDD = IDD_DIALOG_FILLING_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	int m_nDegree;
	int m_nNbPntsOnCurve;
	int m_nIter;
};
