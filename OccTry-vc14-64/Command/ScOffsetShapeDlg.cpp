// Command\ScOffsetShapeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "OccTry.h"
#include "Command\ScOffsetShapeDlg.h"
#include ".\scoffsetshapedlg.h"


// ScOffsetShapeDlg �Ի���

IMPLEMENT_DYNAMIC(ScOffsetShapeDlg, CDialog)
ScOffsetShapeDlg::ScOffsetShapeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ScOffsetShapeDlg::IDD, pParent)
{
}

ScOffsetShapeDlg::~ScOffsetShapeDlg()
{
}

void ScOffsetShapeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ScOffsetShapeDlg, CDialog)
END_MESSAGE_MAP()


// ScOffsetShapeDlg ��Ϣ�������

BOOL ScOffsetShapeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox *pOffMode = (CComboBox *)GetDlgItem(IDC_COMBO_OFFMODE);
	if(pOffMode)
	{
		pOffMode->AddString("Skin");
		pOffMode->AddString("Pipe");
		pOffMode->AddString("RectoVerso");
		pOffMode->SetCurSel(0);
	}

	CComboBox *pcbJoinType = (CComboBox *)GetDlgItem(IDC_COMBO_JOINTYPE);
	if(pcbJoinType)
	{
		pcbJoinType->AddString("Arc");
		pcbJoinType->AddString("Tangent");
		pcbJoinType->AddString("Intersection");
		pcbJoinType->SetCurSel(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void ScOffsetShapeDlg::OnOK()
{
	CComboBox *pOffMode = (CComboBox *)GetDlgItem(IDC_COMBO_OFFMODE);
	if(pOffMode)
	{
		m_nOffMode = pOffMode->GetCurSel();
	}

	CComboBox *pcbJoinType = (CComboBox *)GetDlgItem(IDC_COMBO_JOINTYPE);
	if(pcbJoinType)
	{
		m_nJoinType = pcbJoinType->GetCurSel();
	}

	CDialog::OnOK();
}
