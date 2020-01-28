#pragma once

#include <V3d_View.hxx>
// ScDlgViewAttribs �Ի���

class ScDlgViewAttribs : public CDialog
{
	DECLARE_DYNAMIC(ScDlgViewAttribs)

public:
	ScDlgViewAttribs(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ScDlgViewAttribs();

	//��ʾview������
	void			ShowViewAttribs(const Handle(V3d_View)& aView);

// �Ի�������
	enum { IDD = IDD_DIALOG_VIEW_ATTRBS };

protected:
	void			SetText(LPCTSTR lpszText);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
