// Command\ScDlgTexture.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "OccTry.h"
#include "ScCmdDisplay.h"
#include ".\scdlgtexture.h"


// ScDlgTexture �Ի���

IMPLEMENT_DYNAMIC(ScDlgTexture, CDialog)
ScDlgTexture::ScDlgTexture(CWnd* pParent /*=NULL*/)
	: CDialog(ScDlgTexture::IDD, pParent)
	, m_szTextureFile(_T(""))
{
	this->m_pCmdTexture = NULL;
}

ScDlgTexture::~ScDlgTexture()
{
}

void ScDlgTexture::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEXTURE_FILE, m_szTextureFile);
}


BEGIN_MESSAGE_MAP(ScDlgTexture, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_SELFILE, OnBnClickedButtonSelfile)
END_MESSAGE_MAP()


// ScDlgTexture ��Ϣ�������

BOOL ScDlgTexture::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void ScDlgTexture::OnOK()
{
	DoTexture(TRUE);

	CDialog::OnOK();
}

void ScDlgTexture::OnBnClickedButtonApply()
{
	DoTexture();
}

void	ScDlgTexture::DoTexture(BOOL bDone)
{
	UpdateData();

	if(this->m_szTextureFile.IsEmpty())
	{
		AfxMessageBox("�ļ����Ʋ���Ϊ�ա�");
		return;
	}

	CFileFind ff;
	if(ff.FindFile(this->m_szTextureFile) == FALSE)
	{
		AfxMessageBox("û���ҵ��ļ�.");
		return;
	}

	//
	if(this->m_pCmdTexture)
	{
		this->m_pCmdTexture->SetTexture(this->m_szTextureFile,bDone);
	}
}

void ScDlgTexture::OnBnClickedButtonSelfile()
{
	CFileDialog dlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Bmp Files (*.bmp)|*.bmp|gif Files (*.gif)|*.gif|All Files (*.*)|*.*||", 
		NULL );

	
	if (dlg.DoModal() == IDOK) 
	{
		CString C = dlg.GetPathName();
		if(dlg.GetFileExt().CompareNoCase("bmp") == 0 ||
			dlg.GetFileExt().CompareNoCase("gif") == 0)
		{
			this->m_szTextureFile = C;
			UpdateData(FALSE);
		}
	}
}
