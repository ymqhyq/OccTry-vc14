// Command\ScDlgSweepOption.cpp : 实现文件
//

#include "stdafx.h"
#include "OccTry.h"
#include "command\ScDlgSweepOption.h"
#include ".\scdlgsweepoption.h"


// ScDlgSweepOption 对话框

IMPLEMENT_DYNAMIC(ScDlgSweepOption, CDialog)
ScDlgSweepOption::ScDlgSweepOption(CWnd* pParent /*=NULL*/)
	: CDialog(ScDlgSweepOption::IDD, pParent)
{
}

ScDlgSweepOption::~ScDlgSweepOption()
{
}

void ScDlgSweepOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ScDlgSweepOption, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_SWEEPTYPE, OnCbnSelchangeComboSweeptype)
END_MESSAGE_MAP()

char *pszType[] = {
	"CorrectedFrenet",
	"Frenet",
	"Fixed",
	"ConstantNormal",
	"GuideAC",
	"GuidePlan",
	"GuideACWithContact",
	"GuidePlanWithContact",
	NULL,
};

// ScDlgSweepOption 消息处理程序

BOOL ScDlgSweepOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox *pBox = (CComboBox *)GetDlgItem(IDC_COMBO_SWEEPTYPE);
	if(pBox)
	{
		int ix = 0;
		while(pszType[ix])
		{
			pBox->AddString(pszType[ix]);
			ix++;
		}
		pBox->SetCurSel(0);
	}

	EnableBiNorm(FALSE);
	EnableFixed(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

static BOOL ParseVec(LPCTSTR lpszVec,gp_Vec& vec)
{
	CString szVec = lpszVec;
	int nPos = szVec.Find(",");
	if(nPos > 0)
	{
		CString szVal,szRest;
		szVal = szVec.Left(nPos);
		szRest = szVec.Right(szVec.GetLength() - nPos - 1);
		vec.SetX(atof(szVal));
		nPos = szRest.Find(",");
		if(nPos > 0)
		{
			szVal = szRest.Left(nPos);
			szRest = szRest.Right(szRest.GetLength() - nPos - 1);
			vec.SetY(atof(szVal));
			vec.SetZ(atof(szRest));

			return TRUE;
		}
	} 

	return FALSE;
}

void ScDlgSweepOption::OnOK()
{
	CComboBox *pBox = (CComboBox *)GetDlgItem(IDC_COMBO_SWEEPTYPE);
	if(pBox)
	{
		EnableFixed(FALSE);
		EnableBiNorm(FALSE);
		m_nSweepType = pBox->GetCurSel();

		if(m_nSweepType == Sweep_Fixed)
		{
			CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_NORMAL);
			ASSERT(pEdit);
			CString szNorm;
			pEdit->GetWindowText(szNorm);
		
			if(!ParseVec(szNorm,m_vBiNorm))
				m_vBiNorm.SetCoord(0,1.0,0);
		}
		else if(m_nSweepType == Sweep_ConstantNormal)
		{
			CString szN,szX,szO;
			CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_NORMAL);
			pEdit->GetWindowText(szN);
	
            CEdit *pEdit1 = (CEdit *)GetDlgItem(IDC_EDIT_XDIR);
			pEdit1->GetWindowText(szX);
	
			CEdit *pEdit2 = (CEdit *)GetDlgItem(IDC_EDIT_LOC);
			pEdit2->GetWindowText(szO);

			gp_Vec vN,vX,vO;
			if(ParseVec(szN,vN) && ParseVec(szX,vX) && 
				ParseVec(szO,vO))
			{
				m_ax2.SetLocation(gp_Pnt(vO.X(),vO.Y(),vO.Z()));
				m_ax2.SetDirection(gp_Dir(vN));
				m_ax2.SetXDirection(gp_Dir(vX));
			}
			else
			{
				m_ax2 = gp_Ax2();
			}
		}
	}

	CDialog::OnOK();
}

void	ScDlgSweepOption::EnableBiNorm(BOOL bEnable)
{
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_NORMAL);
	if(pEdit)
	{
		pEdit->EnableWindow(bEnable);
	}
}

void	ScDlgSweepOption::EnableFixed(BOOL bEnable)
{
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_NORMAL);
	if(pEdit)
	{
		pEdit->EnableWindow(bEnable);
	}
	CEdit *pEdit1 = (CEdit *)GetDlgItem(IDC_EDIT_XDIR);
	if(pEdit1)
	{
		pEdit1->EnableWindow(bEnable);
	}
	CEdit *pEdit2 = (CEdit *)GetDlgItem(IDC_EDIT_LOC);
	if(pEdit2)
	{
		pEdit2->EnableWindow(bEnable);
	}
}

void ScDlgSweepOption::OnCbnSelchangeComboSweeptype()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox *pBox = (CComboBox *)GetDlgItem(IDC_COMBO_SWEEPTYPE);
	if(pBox)
	{
		EnableFixed(FALSE);
		EnableBiNorm(FALSE);
		int nSel = pBox->GetCurSel();
		switch(nSel)
		{
		case Sweep_Fixed:
			EnableFixed(TRUE);
			break;
		case Sweep_ConstantNormal:
			EnableBiNorm(TRUE);
			break;
		default:
			break;
		}
	}
}
