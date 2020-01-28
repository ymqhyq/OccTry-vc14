// InputDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "OccTry.h"
#include "InputDialog.h"
#include "ScInputMgr.h"
#include "ScCommand.h"
#include ".\inputdialog.h"


// CInputDialog �Ի���

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

//������Ϣ��ʾ����������
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

//��ʾ��Ϣ
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


// CInputDialog ��Ϣ�������

BOOL CInputDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	this->m_szInput = "13445";
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CInputDialog::OnOK()
{
	UpdateData();

	//�����ַ��������Ƿ�Ϸ�
	//TODO:���벻�Ϸ������أ��������롣
	ParseInput();

	CDialog::OnOK();
}

//
// ��������
//
int		CInputDialog::ParseInput()
{
	ASSERT(this->m_pScCmd);

	switch(this->m_nInputType)
	{
	case INPUT_INTEGER://��������
		break;
	case INPUT_DOUBLE://���븡����
		{
			double dVal = ::atof(m_szInput);
			if(this->m_pScCmd)
			{
				this->m_pScCmd->AcceptInput(dVal);

				return SC_OK;
			}
		}
		break;
	case INPUT_POINT3D://3ά��
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
	case INPUT_POINT2D://2ά��
	case INPUT_VECTOR://��������
		break;
	}

	return SC_ERR;
}

void	CInputDialog::SetInputMgr(ScInputMgr *pMgr)
{
	ASSERT(pMgr);
	this->m_pInputMgr = pMgr;
}

//������ʱ��
int	CInputDialog::StartTimer(DWORD dwTime)
{
	ASSERT(m_nTimer == 0);
	m_nTimer = SetTimer(1,dwTime,NULL);

	return 0;
}

//ֹͣ��ʱ��
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

//������ǰִ�е�����
int		CInputDialog::AttachScCmd(ScCommand *pScCmd)
{
	ASSERT(pScCmd);
	this->m_pScCmd = pScCmd;

	return 0;
}

void	CInputDialog::DetachScCmd()//ȥ����
{
	ASSERT(this->m_pScCmd);
	this->m_pScCmd = NULL;
}

//Ϊ������׼��
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