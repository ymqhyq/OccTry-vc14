#pragma once

#include "..\Resource.h"

// ScDlgFillingOptions �Ի���

class ScDlgFillingOptions : public CDialog
{
	DECLARE_DYNAMIC(ScDlgFillingOptions)

public:
	ScDlgFillingOptions(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ScDlgFillingOptions();

// �Ի�������
	enum { IDD = IDD_DIALOG_FILLING_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
