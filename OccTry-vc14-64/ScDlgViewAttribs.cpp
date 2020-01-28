// ScDlgViewAttribs.cpp : 实现文件
//

#include "stdafx.h"
#include "OccTry.h"
//yxk20191208
//#include <Visual3d_View.hxx>
//#include <Visual3d_ViewMapping.hxx>
//endyxk
#include "ScDlgViewAttribs.h"
#include ".\scdlgviewattribs.h"


// ScDlgViewAttribs 对话框

IMPLEMENT_DYNAMIC(ScDlgViewAttribs, CDialog)
ScDlgViewAttribs::ScDlgViewAttribs(CWnd* pParent /*=NULL*/)
	: CDialog(ScDlgViewAttribs::IDD, pParent)
{
}

ScDlgViewAttribs::~ScDlgViewAttribs()
{
}

void ScDlgViewAttribs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ScDlgViewAttribs, CDialog)
END_MESSAGE_MAP()


// ScDlgViewAttribs 消息处理程序

BOOL ScDlgViewAttribs::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetText("");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//
// 显示view的属性
//
void	ScDlgViewAttribs::ShowViewAttribs(const Handle(V3d_View)& aView)
{
	CString szText,szStr;
	//VRC属性
	szText.Format("VRC坐标系:\r\n");
	//眼坐标
	double x,y,z;
	aView->Eye(x,y,z);
	szStr.Format("EYE: %.4f,%.4f,%.4f\r\n",x,y,z);
	szText += szStr;
	//VRP
	aView->At(x,y,z);
	szStr.Format("VRP: %.4f,%.4f,%.4f\r\n",x,y,z);
	szText += szStr;
	//VPN
	aView->Proj(x,y,z);
	szStr.Format("VPN: %.4f,%.4f,%.4f\r\n",x,y,z);
	szText += szStr;
	//VUP
	aView->Up(x,y,z);
	szStr.Format("VUP: %.4f,%.4f,%.4f\r\n",x,y,z);
	szText += szStr;

	//view window
	szText += "\r\nView window:\r\n";
//yxk20200104
	////center
	//aView->Center(x,y);
	//szStr.Format("Center: %.4f,%.4f\r\n",x,y);
	//szText += szStr;
//endyxk
	//size
	aView->Size(x,y);
	szStr.Format("Size: %.4f,%.4f\r\n",x,y);
	szText += szStr;

//yxk20200104
	//	//ZCueing
	//aView->ZCueing(x,y);
	//szStr.Format("ZCueing: depth %.4f,width %.4f\r\n",x,y);
	//szText += szStr;
	//aView->ZClipping(x,y);
	//szStr.Format("ZClipping: depth %.4f,width %.4f\r\n",x,y);
	szText += szStr;

	//NPC
//	szText += "\r\nNPC:\r\n";
////https://dev.opencascade.org/doc/overview/html/occt_dev_guides__upgrade.html
//
//	//Handle(Visual3d_View) aVisView = aView->View();
//	Handle(Graphic3d_CView) aVisView = aView->View();
//	Visual3d_ViewMapping vmap = aVisView->ViewMapping();
//	//PRP
//	vmap.ProjectionReferencePoint().Coord(x,y,z);
//	szStr.Format("PRP: %.4f,%.4f\r\n",x,y);
//	szText += szStr;
//	szStr.Format("VPD: %.4f\r\n",vmap.ViewPlaneDistance());
//	szText += szStr;
//	szStr.Format("BPD: %.4f\r\n",vmap.BackPlaneDistance());
//	szText += szStr;
//	szStr.Format("FPD: %.4f\r\n",vmap.FrontPlaneDistance());
//	szText += szStr;
//
//	double u1,u2,v1,v2;
//	vmap.WindowLimit(u1,v1,u2,v2);
//	szStr.Format("WinLimit: U [%.4f,%.4f],V [%.4f,%.4f]\r\n",u1,u2,v1,v2);
//  szText += szStr;
//endyxk

	SetText(szText);
}

void	ScDlgViewAttribs::SetText(LPCTSTR lpszText)
{
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_VIEW_ATTRIBS);
	if(pEdit)
	{
		pEdit->SetWindowText(lpszText);
	}
}
