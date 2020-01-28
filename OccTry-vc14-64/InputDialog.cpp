// InputDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "OccTry.h"
#include "InputDialog.h"
#include "ScInputMgr.h"
#include "ScCommand.h"
#include ".\inputdialog.h"


// CInputDialog 对话框

IMPLEMENT_DYNAMIC(CInputDialog, CDialog)
CInputDialog::CInputDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInputDialog::IDD, pParent)
	, m_szInput(_T(""))
{
	this->m_nTimer = 0;
	this->m_pScCmd = NULL;
	this->m_pInputMgr = NULL;
}

CInputDialog::~CInputDialog()
{
}

//设置信息提示和输入类型
void	CInputDialog::NeedInput(LPCTSTR lpszTitle,int nInputType)
{
	this->m_szTitle = lpszTitle;
	this->SetWindowText(lpszTitle);

	this->m_nInputType = nInputType;
}

void	CInputDialog::Show(LPCTSTR lpszText)
{
	this->m_szInput = lpszText;
	UpdateData(FALSE);
}

//显示信息
void	CInputDialog::Show(int nVal)
{
	CString szMsg;
	szMsg.Format("%d",nVal);
	Show(szMsg);
}

void	CInputDialog::Show(double dVal)
{
	CString szMsg;
	szMsg.Format("%f",dVal);
	Show(szMsg);
}

void	CInputDialog::Show(gp_Pnt pnt)
{
	CString szMsg;
	szMsg.Format("%f,%f,%f",pnt.X(),pnt.Y(),pnt.Z());
	Show(szMsg);
}

void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INPUT, m_szInput);
}


BEGIN_MESSAGE_MAP(CInputDialog, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CInputDialog 消息处理程序

BOOL CInputDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	this->m_szInput = "13445";
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CInputDialog::OnOK()
{
	UpdateData();

	//解析字符串，看是否合法
	//TODO:输入不合法，返回，重新输入。
	ParseInput();

	CDialog::OnOK();
}

//
// 解析输入
//
int		CInputDialog::ParseInput()
{
	ASSERT(this->m_pScCmd);

	switch(this->m_nInputType)
	{
	case INPUT_INTEGER://输入整形
		break;
	case INPUT_DOUBLE://输入浮点数
		{
			double dVal = ::atof(m_szInput);
			if(this->m_pScCmd)
			{
				this->m_pScCmd->AcceptInput(dVal);

				return SC_OK;
			}
		}
		break;
	case INPUT_POINT3D://3维点
		{
			gp_Pnt pnt;
			int nPos = m_szInput.Find(",");
			if(nPos > 0)
			{
				CString szVal,szRest;
				szVal = m_szInput.Left(nPos);
				szRest = m_szInput.Right(m_szInput.GetLength() - nPos - 1);
				pnt.SetX(atof(szVal));
				nPos = szRest.Find(",");
				if(nPos > 0)
				{
					szVal = szRest.Left(nPos);
					szRest = szRest.Right(szRest.GetLength() - nPos - 1);
					pnt.SetY(atof(szVal));
					pnt.SetZ(atof(szRest));

					this->m_pScCmd->AcceptInput(pnt);

					return SC_OK;
				}
			} 
		}
		break;
	case INPUT_POINT2D://2维点
	case INPUT_VECTOR://输入向量
		break;
	}

	return SC_ERR;
}

void	CInputDialog::SetInputMgr(ScInputMgr *pMgr)
{
	ASSERT(pMgr);
	this->m_pInputMgr = pMgr;
}

//启动定时器
int	CInputDialog::StartTimer(DWORD dwTime)
{
	ASSERT(m_nTimer == 0);
	m_nTimer = SetTimer(1,dwTime,NULL);

	return 0;
}

//停止定时器
int	CInputDialog::StopTimer()
{
	ASSERT(m_nTimer);
	if(m_nTimer)
	{
		this->KillTimer(m_nTimer);
		m_nTimer = 0;
	}

	return 0;
}

void CInputDialog::OnTimer(UINT_PTR nIDEvent)
{
	
	if(this->m_pInputMgr)
	{
		this->m_pInputMgr->OnTimerProc();
	}

	CDialog::OnTimer(nIDEvent);
}

//关联当前执行的命令
int		CInputDialog::AttachScCmd(ScCommand *pScCmd)
{
	ASSERT(pScCmd);
	this->m_pScCmd = pScCmd;

	return 0;
}

void	CInputDialog::DetachScCmd()//去关联
{
	ASSERT(this->m_pScCmd);
	this->m_pScCmd = NULL;
}

//为输入做准备
void	CInputDialog::ReadyForInput()
{
//	this->BringWindowToTop();
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_INPUT);
	ASSERT(pEdit);
	//int nS,nE;
	//pEdit->GetSel(nS,nE);
	//pEdit->SetSel(nS,nE);
	pEdit->SetFocus();
}