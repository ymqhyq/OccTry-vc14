#pragma once

#include <V3d_View.hxx>
// ScDlgViewAttribs 对话框

class ScDlgViewAttribs : public CDialog
{
	DECLARE_DYNAMIC(ScDlgViewAttribs)

public:
	ScDlgViewAttribs(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ScDlgViewAttribs();

	//显示view的属性
	void			ShowViewAttribs(const Handle(V3d_View)& aView);

// 对话框数据
	enum { IDD = IDD_DIALOG_VIEW_ATTRBS };

protected:
	void			SetText(LPCTSTR lpszText);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
