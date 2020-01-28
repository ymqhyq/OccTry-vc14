// Command\ScDlgFillingOptions.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "OccTry.h"
#include "Command\ScDlgFillingOptions.h"
#include ".\scdlgfillingoptions.h"


// ScDlgFillingOptions �Ի���

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


// ScDlgFillingOptions ��Ϣ�������

BOOL ScDlgFillingOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void ScDlgFillingOptions::OnOK()
{
	UpdateData(TRUE);

	CDialog::OnOK();
}
