// Command\ScDlgTexture.cpp : 实现文件
//

#include "stdafx.h"
#include "OccTry.h"
#include "ScCmdDisplay.h"
#include ".\scdlgtexture.h"


// ScDlgTexture 对话框

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


// ScDlgTexture 消息处理程序

BOOL ScDlgTexture::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
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
		AfxMessageBox("文件名称不能为空。");
		return;
	}

	CFileFind ff;
	if(ff.FindFile(this->m_szTextureFile) == FALSE)
	{
		AfxMessageBox("没有找到文件.");
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
