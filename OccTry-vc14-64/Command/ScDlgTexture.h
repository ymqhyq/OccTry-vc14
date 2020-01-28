#pragma once

#include "..\resource.h"

class ScCmdTexture;

// ScDlgTexture �Ի���

class ScDlgTexture : public CDialog
{
	DECLARE_DYNAMIC(ScDlgTexture)

public:
	ScDlgTexture(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ScDlgTexture();

// �Ի�������
	enum { IDD = IDD_DIALOG_TEXTURE };

public:
	CString			m_szTexFile;//���յ������ļ����������õ�Ϊ����ʶ���ͼ���ļ�����Ҫ����ת����
	                            //

	ScCmdTexture		*m_pCmdTexture;//��������

protected:
	void				DoTexture(BOOL bDone = FALSE);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
