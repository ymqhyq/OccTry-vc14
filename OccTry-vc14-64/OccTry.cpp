// OccTry.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "OccTry.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "OccTryDoc.h"
#include "OccTryView.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#endif


// COccTryApp

BEGIN_MESSAGE_MAP(COccTryApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

// COccTryApp ����

COccTryApp::COccTryApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	//Add For Cascade.
	try
	{
		//yxk20191207
		//m_graphicDevice3D = new Graphic3d_WNTGraphicDevice();
		Handle(Aspect_DisplayConnection) aDisplayConnection;
		m_graphicDevice3D = new OpenGl_GraphicDriver(aDisplayConnection);

	}
	catch(Standard_Failure)
	{
		AfxMessageBox("Fatal error during graphic initialization",MB_ICONSTOP);
		ExitProcess(1);
	}


	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� COccTryApp ����

COccTryApp theApp;

// COccTryApp ��ʼ��

BOOL COccTryApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_OccTryTYPE,
		RUNTIME_CLASS(COccTryDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(COccTryView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// ������ MDI ��ܴ���
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� MDI Ӧ�ó����У���Ӧ������ m_pMainWnd ֮����������
	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// �������ѳ�ʼ���������ʾ����������и���
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	return TRUE;
}



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void COccTryApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// COccTryApp ��Ϣ�������

