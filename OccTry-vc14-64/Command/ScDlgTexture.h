#pragma once

#include "..\resource.h"

class ScCmdTexture;

// ScDlgTexture 对话框

class ScDlgTexture : public CDialog
{
	DECLARE_DYNAMIC(ScDlgTexture)

public:
	ScDlgTexture(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ScDlgTexture();

// 对话框数据
	enum { IDD = IDD_DIALOG_TEXTURE };

public:
	CString			m_szTexFile;//最终的纹理文件，可能设置的为不可识别的图形文件，需要进行转换。
	                            //

	ScCmdTexture		*m_pCmdTexture;//关联命令

protected:
	void				DoTexture(BOOL bDone = FALSE);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedButtonApply();
	CString m_szTextureFile;
	afx_msg void OnBnClickedButtonSelfile();
};
