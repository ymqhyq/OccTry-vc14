#pragma once

#include "..\resource.h"

// ScOffsetShapeDlg �Ի���

class ScOffsetShapeDlg : public CDialog
{
	DECLARE_DYNAMIC(ScOffsetShapeDlg)

public:
	ScOffsetShapeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ScOffsetShapeDlg();

	int			m_nOffMode;
	int			m_nJoinType;

// �Ի�������
	enum { IDD = IDD_DIALOG_OFFSETSHAPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
