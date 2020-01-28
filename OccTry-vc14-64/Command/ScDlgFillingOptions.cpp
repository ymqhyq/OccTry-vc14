// Command\ScDlgFillingOptions.cpp : 实现文件
//

#include "stdafx.h"
#include "OccTry.h"
#include "Command\ScDlgFillingOptions.h"
#include ".\scdlgfillingoptions.h"


// ScDlgFillingOptions 对话框

IMPLEMENT_DYNAMIC(ScDlgFillingOptions, CDialog)
ScDlgFillingOptions::ScDlgFillingOptions(CWnd* pParent /*=NULL*/)
	: CDialog(ScDlgFillingOptions::IDD, pParent)
	, m_nDegree(3)
	, m_nNbPntsOnCurve(15)
	, m_nIter(2)
{
}

ScDlgFillingOptions::~ScDlgFillingOptions()
{
}

void ScDlgFillingOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DEGREE, m_nDegree);
	DDX_Text(pDX, IDC_EDIT_NBPNTSONCURVE, m_nNbPntsOnCurve);
	DDX_Text(pDX, IDC_EDIT_NBITER, m_nIter);
}


BEGIN_MESSAGE_MAP(ScDlgFillingOptions, CDialog)
END_MESSAGE_MAP()


// ScDlgFillingOptions 消息处理程序

BOOL ScDlgFillingOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void ScDlgFillingOptions::OnOK()
{
	UpdateData(TRUE);

	CDialog::OnOK();
}
