// OccTryView.cpp : COccTryView 类的实现
//

#include "stdafx.h"
#include "OccTry.h"

#include <WNT_Window.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
//#include <SelectBasics_ListOfBox2d.hxx>//yxk
//#include <SelectBasics_ListIteratorOfListOfBox2d.hxx>//yxk
#include "MainFrm.h"
#include "OccTryDoc.h"
#include "OccTryView.h"
#include "ScTools.h"
#include "InputDialog.h"
#include "ScInputMgr.h"
#include "ScView.h"
#include "ScUcsMgr.h"
#include ".\occtryview.h"
#include "ScSnapMgr.h"
#include "ShapeStructDlg.h"
#include "ScDlgViewAttribs.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#endif




// COccTryView

IMPLEMENT_DYNCREATE(COccTryView, CView)

BEGIN_MESSAGE_MAP(COccTryView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_MENU_PLANVIEW, OnMenuPlanview)
	ON_COMMAND(ID_MENU_FRONTVIEW, OnMenuFrontview)
	ON_COMMAND(ID_MENU_LEFTVIEW, OnMenuLeftview)
	ON_COMMAND(ID_MENU_RIGHTMENU, OnMenuRightmenu)
	ON_COMMAND(ID_MENU_TOPVIEW, OnMenuTopview)
	ON_COMMAND(ID_MENU_BOTTOMVIEW, OnMenuBottomview)
	ON_COMMAND(ID_MENU_BACKVIEW, OnMenuBackview)
	ON_COMMAND(IDM_VIEW_ATTRS, OnViewAttrs)
	ON_COMMAND(IDM_TOOL_OBJ_SELECT_SENREGION, OnToolObjSelectSenregion)
	ON_UPDATE_COMMAND_UI(IDM_TOOL_OBJ_SELECT_SENREGION, OnUpdateToolObjSelectSenregion)
END_MESSAGE_MAP()

// COccTryView 构造/析构

COccTryView::COccTryView()
{
	// TODO: 在此处添加构造代码
	myWidth=0;
	myHeight=0;

	this->m_pInputDlg = NULL;
	this->m_pInputMgr = NULL;
	this->m_pScView = NULL;
	this->m_pSnapMgr = NULL;
	this->m_pShapeStructDlg = NULL;
	this->m_pViewAttribsDlg = NULL;

	this->m_bShowSenRegion = FALSE;
}

COccTryView::~COccTryView()
{
}

BOOL COccTryView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return CView::PreCreateWindow(cs);
}


// COccTryView 绘制

void COccTryView::OnDraw(CDC* /*pDC*/)
{
	COccTryDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	//Add For Cascade.
	//实践证明:不添加如下Post消息代码,当窗口大小改变时,可能会导致刷新问题.
	CRect aRect;
	GetWindowRect(aRect);
	if(myWidth != aRect.Width() || myHeight != aRect.Height()) {
		myWidth = aRect.Width();
		myHeight = aRect.Height();
		::PostMessage ( GetSafeHwnd () , WM_SIZE , SW_SHOW , myWidth + myHeight*65536 );
	}
	m_occView->Redraw();
}


// COccTryView 打印

BOOL COccTryView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void COccTryView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印前添加额外的初始化
}

void COccTryView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印后添加清除过程
}


// COccTryView 诊断

#ifdef _DEBUG
void COccTryView::AssertValid() const
{
	CView::AssertValid();
}

void COccTryView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COccTryDoc* COccTryView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COccTryDoc)));
	return (COccTryDoc*)m_pDocument;
}
#endif //_DEBUG

void COccTryView::ShowShapeStruct(const TopoDS_Shape& aShape)
{
	if(aShape.IsNull())
		return;

	if(this->m_pShapeStructDlg)
	{
		this->m_pShapeStructDlg->ShowShape(aShape);
		this->m_pShapeStructDlg->ShowWindow(SW_NORMAL);
	}
}

// COccTryView 消息处理程序

void COccTryView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	//Add For Cascade.
	m_occView = GetDocument()->GetViewer()->CreateView();

    // set the default mode in wireframe ( not hidden line ! )
//    myView->SetDegenerateModeOn();
    // store for restore state after rotation (witch is in Degenerated mode)
//    myDegenerateModeIsOn = Standard_True;
//yxk20200104
	//Handle(Graphic3d_WNTGraphicDevice) theGraphicDevice = 
	//	((COccTryApp*)AfxGetApp())->GetGraphicDevice3D();
	Handle(Graphic3d_GraphicDriver)theGraphicDevice =
		((COccTryApp*)AfxGetApp())->GetGraphicDevice3D();
//endyxk
    
    Handle(WNT_Window) aWNTWindow = new WNT_Window(/*theGraphicDevice,*/GetSafeHwnd ());//yxk
    m_occView->SetWindow(aWNTWindow);
    if (!aWNTWindow->IsMapped()) aWNTWindow->Map();

	//支持纹理
//yxk20200104	
//https://dev.opencascade.org/doc/overview/html/occt_dev_guides__upgrade.html
//现在，纹理对象比环境映射具有更高的优先级。
//冗余枚举V3d_TypeOfSurface和Graphic3d_TypeOfSurface，类OpenGl_SurfaceDetailState，Graphic3d_CView，
//OpenGl_ShaderManager，OpenGl_View，V3d_View和V3d_Viewer中的相应方法已删除。绘制命令VSetTextureMode已删除。
	//m_occView->SetSurfaceDetail(V3d_TEX_ALL);
//endyxk
	//
	m_occView->SetBackgroundColor(Quantity_TOC_RGB,0.1,0.0,0.1);

	//GetDocument()->ShowTrihedron(TRUE);
	GetDocument()->GetScUcsMgr()->ShowWCSTrihedron();

	//创建工具对话框
	this->m_pInputDlg = new CInputDialog(this);
	this->m_pInputDlg->Create(IDD_INPUT_DIALOG);

	//创建对话框
	this->m_pShapeStructDlg = new CShapeStructDlg(this);
	this->m_pShapeStructDlg->Create(IDD_DIALOG_SHAPE_STRUCT);


	//创建输入管理
	this->m_pInputMgr = new ScInputMgr(GetDocument()->GetScCmdMgr(),this,
		this->m_pInputDlg);

	this->m_pScView = new ScView(m_occView);
	this->m_pScView->SetUcsMgr(GetDocument()->GetScUcsMgr());
	this->m_pScView->m_pViewWnd = this;//记录

	//进行必要的设置
	GetDocument()->GetScCmdMgr().SetScView(m_pScView);

	//自动捕捉管理
	this->m_pSnapMgr = new ScSnapMgr(GetDocument()->GetAISContext());
	this->m_pSnapMgr->SetScView(this->m_pScView);
	GetDocument()->GetScCmdMgr().SetSnapMgr(m_pSnapMgr);
}

void COccTryView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (!m_occView.IsNull())
		m_occView->MustBeResized();
}

void COccTryView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CView::OnLButtonDblClk(nFlags, point);
}

//
// 鼠标左键按下。可能的后续动作：
// a、点选  b、框选.点选要到鼠标抬起时才可判断。
//
void COccTryView::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->m_nPreX = point.x;
	this->m_nPreY = point.y;
	this->m_nStartX = point.x;
	this->m_nStartY = point.y;

	if(nFlags & MK_CONTROL)
	{
	}
	else
	{
		//判断是否框选
		if(nFlags & MK_SHIFT)
		{
			GetDocument()->ShiftDragEvent(point,MB_DOWN,this->m_occView);
		}
		else
		{
			GetDocument()->DragEvent(point,MB_DOWN,this->m_occView);
		}
	}

	CView::OnLButtonDown(nFlags, point);
}

void COccTryView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(nFlags & MK_CONTROL)
	{
	}
	else
	{
		if((point.x == this->m_nStartX) && 
			(point.y == this->m_nStartY))//和初始点进行比较，没有移动
		{
			this->m_nPreX = point.x;
			this->m_nPreY = point.y;

			if(nFlags & MK_SHIFT)
			{
				GetDocument()->ShiftInputEvent(point,this->m_occView);
			}
			else
			{
				GetDocument()->InputEvent(point,MINPUT_LEFT,this->m_occView);
			}
		}
		else
		{//框选
			this->m_nPreX = point.x;
			this->m_nPreY = point.y;
			//隐藏选择框
			this->DrawRectangle(this->m_nStartX,this->m_nStartY,point.x,point.y,FALSE);

			if(nFlags & MK_SHIFT)
			{
				GetDocument()->ShiftDragEvent(point,MB_UP,this->m_occView);
			}
			else
			{
				GetDocument()->DragEvent(point,MB_UP,this->m_occView);
			}
		}

	}

	CView::OnLButtonUp(nFlags, point);
}

//这里主要实现旋转功能
void COccTryView::OnMButtonDown(UINT nFlags, CPoint point)
{
	if(nFlags & MK_SHIFT)
	{
		
	}
	else
	{
		//m_occView->SetDegenerateModeOn();
		this->m_occView->StartRotation(point.x,point.y);
		this->m_nPreX = point.x;
		this->m_nPreY = point.y;
	}


	CView::OnMButtonDown(nFlags, point);
}

void COccTryView::OnMButtonUp(UINT nFlags, CPoint point)
{
	//视图变换，需要更新snap点
	UpdateSnapPoints();

	CView::OnMButtonUp(nFlags, point);
}

void COccTryView::OnMouseMove(UINT nFlags, CPoint point)
{
	//如果没有焦点，设置交点
	CWnd *pActiveWnd = this->GetActiveWindow();
	if(pActiveWnd)
	{
		if((pActiveWnd->GetSafeHwnd() != this->GetSafeHwnd()) &&
			(pActiveWnd->GetSafeHwnd() != NULL))
		{
			if((point.x != this->m_nPreX) ||
				(point.y != this->m_nPreY))//鼠标移动，不再原处，则设置焦点.
			{
				//DTRACE("\nOnMouseMove SetFocus...");
				SetFocus();
			}
		}
	}
	//按着左键
	if(nFlags & MK_LBUTTON)
	{
		if(nFlags & MK_CONTROL)
		{
		}
		else
		{
			//框选操作
			
			//隐藏选择框
			this->DrawRectangle(this->m_nStartX,this->m_nStartY,point.x,point.y,FALSE);

			if(nFlags & MK_SHIFT)
			{
				GetDocument()->ShiftDragEvent(point,MB_MOVE,this->m_occView);
			}
			else
			{
				GetDocument()->DragEvent(point,MB_MOVE,this->m_occView);
				//GetDocument()->MoveEvent(point,this->m_occView);
				//GetDocument()->ShiftInputEvent(point,this->m_occView);
			}

			//显示选择框选
			this->DrawRectangle(this->m_nStartX,this->m_nStartY,point.x,point.y,TRUE);

		}
	}
	else if ( nFlags & MK_MBUTTON)
	{
		//中键，旋转，缩放，平移
		if(nFlags & MK_SHIFT)
		{
			this->ViewZoom(point.x,point.y);
		}
		else if(nFlags & MK_CONTROL)
		{
			this->m_occView->Pan(point.x - this->m_nPreX,
				  this->m_nPreY - point.y);

			//记录当前点
			this->m_nPreX = point.x;
			this->m_nPreY = point.y;
		}
		else
		{
			this->m_occView->Rotation(point.x,point.y);
			this->m_occView->Redraw();
		}

		//this->UpdateViewAttribsDlg();
		//重新绘制
		//this->m_occView->Redraw();
	}
	else if ( nFlags & MK_RBUTTON )
	{
		this->m_occView->Pan(point.x - this->m_nPreX,
			this->m_nPreY - point.y);

		this->UpdateViewAttribsDlg();

		this->m_bRButtonMove = TRUE;
		//记录当前点
		this->m_nPreX = point.x;
		this->m_nPreY = point.y;
	}
	else
	{
		//鼠标移动的消息
		if(nFlags & MK_SHIFT)
		{
			GetDocument()->ShiftMoveEvent(point,this->m_occView);
		}
		else
		{
			GetDocument()->MoveEvent(point,this->m_occView);
		}

		//输入管理器更新
		if(this->m_pInputMgr)
		{
			//DTRACE("\n input mouse move....");
			this->m_pInputMgr->MouseEvent(point,IN_MOUSE_MOVE);
		}

		//更新鼠标值,竟然会影响到旋转!!!
		this->UpdateMousePos(point.x,point.y);
		this->UpdateInputDialogPos(point.x,point.y);

		//记录当前点
		this->m_nPreX = point.x;
		this->m_nPreY = point.y;
	}

	CView::OnMouseMove(nFlags, point);
}

//缩放实现
void	COccTryView::ViewZoom(int nX,int nY)
{
	Standard_Real dViewWith,dViewHeight;//yxk202000104 Quantity_Length->Standard_Real
	this->m_occView->Size(dViewWith,dViewHeight);
	DTRACE("\nview with is %.4f,height is %.4f.",dViewWith,dViewHeight);

	//太小或太大禁止缩放
	//if((dViewWith > 10.0) && (dViewWith < 10000.0) &&
	//	(dViewHeight > 10.0) && (dViewHeight < 10000.0))
	{
		this->m_occView->Zoom(this->m_nPreX,this->m_nPreY,nX,nY);
	}

	this->m_nPreX = nX;
	this->m_nPreY = nY;
}

//前滚放大，后滚缩小
BOOL COccTryView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	Standard_Real dViewWith,dViewHeight;
	this->m_occView->Size(dViewWith,dViewHeight);
//	DTRACE("\n Mouse Wheel....");

	double dCoef = 1.0;
	BOOL bZoom = TRUE;
	if(zDelta > 0.0)//放大
	{
		if(dViewWith < 0.01 || dViewHeight < 0.01)
			bZoom = FALSE;
		else
			dCoef = 5.0/4.0;
	}
	else//缩小
	{
		if(dViewWith > 10000000.0 || dViewHeight > 10000000.0)
			bZoom = FALSE;
		else
			dCoef = 4.0/5.0;
	}

	//太小或太大禁止缩放
	if(bZoom)
	{
		//以当前鼠标位置为中心，进行缩放
		//this->m_occView->SetCenter(pt.x,pt.y);

		this->m_occView->SetZoom(dCoef,TRUE);
		this->m_occView->Redraw();

		this->UpdateViewAttribsDlg();
		//视图变换，需要更新snap点
		UpdateSnapPoints();
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void COccTryView::OnRButtonDown(UINT nFlags, CPoint point)
{
	this->m_bRButtonMove = FALSE;
	this->m_nPreX = point.x;
	this->m_nPreY = point.y;

	CView::OnRButtonDown(nFlags, point);
}

void COccTryView::OnRButtonUp(UINT nFlags, CPoint point)
{
	
	if(this->m_bRButtonMove)
	{
		this->m_bRButtonMove = FALSE;
		//视图变换，需要更新snap点
		UpdateSnapPoints();
	}
	else
	{
		//
		GetDocument()->InputEvent(point,MINPUT_RIGHT,this->m_occView);
	}

	CView::OnRButtonUp(nFlags, point);
}

//更新鼠标坐标显示
void	COccTryView::UpdateMousePos(int nX,int nY)
{
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if(!pFrm)
		return;

	gp_Pnt pos;
	if(this->m_pScView)
	{
		pos = this->m_pScView->PixelToObjCS(nX,nY);
	}
	
	CString szText;
	szText.Format("%f,%f,%f",pos.X(),pos.Y(),pos.Z());

	pFrm->ShowMousePos(szText);
}

//显示更新输入对话框位置
void	COccTryView::UpdateInputDialogPos(int nX,int nY)
{
	static int nPreX = 0,nPreY = 0;

	if(this->m_pInputDlg)
	{
		//移动一定距离才更新显示
		if(abs(nX - nPreX) > 10 || abs(nY - nPreY) > 10)
		{
			CPoint cp(nX,nY);
			this->ClientToScreen(&cp);

			CRect rectWnd;
			this->m_pInputDlg->GetWindowRect(&rectWnd);

			this->m_pInputDlg->MoveWindow(cp.x + 2,cp.y + 2,rectWnd.Width(),rectWnd.Height());

			nPreX = nX;nPreY = nY;
		}
	}
}

//正视于
void COccTryView::OnMenuPlanview()
{
	if(!this->m_pScView)
		return;

	switch(this->m_pScView->m_nViewType)
	{
		case ScView_Front:
			this->m_pScView->SetProj(V3d_Zpos);
			break;
		case ScView_Back://XY平面
			this->m_pScView->SetProj(V3d_Zneg);
			break;
		case ScView_Top:
			this->m_pScView->SetProj(V3d_Ypos);
			break;
		case ScView_Bottom://XZ平面
			this->m_pScView->SetProj(V3d_Yneg);
			break;
		case ScView_Left:
			this->m_pScView->SetProj(V3d_Xpos);
			break;
		case ScView_Right://YZ平面
			this->m_pScView->SetProj(V3d_Xneg);
			break;
		default:
			break;
	}
	//视图变换，需要更新snap点
	UpdateSnapPoints();
}

//前视图
void COccTryView::OnMenuFrontview()
{
	// TODO: 在此添加命令处理程序代码
	this->m_pScView->SetProj(V3d_Zpos);
	//视图变换，需要更新snap点
	UpdateSnapPoints();
}

//后视图
void COccTryView::OnMenuBackview()
{
	// TODO: 在此添加命令处理程序代码
	this->m_pScView->SetProj(V3d_Zneg);
	//视图变换，需要更新snap点
	UpdateSnapPoints();
}

//左视图
void COccTryView::OnMenuLeftview()
{
	// TODO: 在此添加命令处理程序代码
	this->m_pScView->SetProj(V3d_Xpos);
	//视图变换，需要更新snap点
	UpdateSnapPoints();
}

//右视图
void COccTryView::OnMenuRightmenu()
{
	// TODO: 在此添加命令处理程序代码
	this->m_pScView->SetProj(V3d_Xneg);
	//视图变换，需要更新snap点
	UpdateSnapPoints();
}

//上视图
void COccTryView::OnMenuTopview()
{
	// TODO: 在此添加命令处理程序代码
	this->m_pScView->SetProj(V3d_Ypos);
	//视图变换，需要更新snap点
	UpdateSnapPoints();
}

//下视图
void COccTryView::OnMenuBottomview()
{
	// TODO: 在此添加命令处理程序代码
	this->m_pScView->SetProj(V3d_Yneg);
	//视图变换，需要更新snap点
	UpdateSnapPoints();
}

void COccTryView::DrawRectangle(const Standard_Integer  MinX    ,
					                    const Standard_Integer  MinY    ,
                                        const Standard_Integer  MaxX ,
					                    const Standard_Integer  MaxY ,
					                    BOOL  bDraw , 
                                        const eLineStyle aLineStyle)
{
    static int m_DrawMode = LS_Default;

    if  (!m_Pen && aLineStyle == LS_Solid )
    {
		m_Pen = new CPen(PS_SOLID, 1, RGB(0,0,0)); 
		m_DrawMode = R2_MERGEPENNOT;
	}
    else if (!m_Pen && aLineStyle == LS_Dot )
    {
		m_Pen = new CPen(PS_DOT, 1, RGB(0,0,0));   
		m_DrawMode = R2_XORPEN;
	}
    else if (!m_Pen && aLineStyle == LS_ShortDash)
    {
		m_Pen = new CPen(PS_DASH, 1, RGB(255,0,0));	
		m_DrawMode = R2_XORPEN;
	}
    else if (!m_Pen && aLineStyle == LS_LongDash)
    {
		m_Pen = new CPen(PS_DASH, 1, RGB(0,0,0));	
		m_DrawMode = R2_NOTXORPEN;
	}
    else if (aLineStyle == LS_Default) 
    { 
		m_Pen = NULL;	
		m_DrawMode = R2_MERGEPENNOT;
	}

    CPen* pOldPen = NULL;
    CClientDC clientDC(this);
    if (m_Pen) 
		pOldPen = clientDC.SelectObject(m_Pen);
    clientDC.SetROP2(m_DrawMode);

    static	int nStoredMinX, nStoredMaxX, nStoredMinY, nStoredMaxY;
    static	BOOL m_bVisible = FALSE;

    if (m_bVisible && !bDraw) // move or up  : erase at the old position 
    {
		clientDC.MoveTo(nStoredMinX,nStoredMinY); 
		clientDC.LineTo(nStoredMinX,nStoredMaxY); 
		clientDC.LineTo(nStoredMaxX,nStoredMaxY); 
		clientDC.LineTo(nStoredMaxX,nStoredMinY); 
		clientDC.LineTo(nStoredMinX,nStoredMinY);
		m_bVisible = FALSE;
    }

    nStoredMinX = min ( MinX, MaxX );
    nStoredMinY = min ( MinY, MaxY );
    nStoredMaxX = std::max ( MinX, MaxX );
    nStoredMaxY = std::max ( MinY, MaxY);

    if (bDraw) // move : draw
    {
		clientDC.MoveTo(nStoredMinX,nStoredMinY); 
		clientDC.LineTo(nStoredMinX,nStoredMaxY); 
		clientDC.LineTo(nStoredMaxX,nStoredMaxY); 
		clientDC.LineTo(nStoredMaxX,nStoredMinY); 
		clientDC.LineTo(nStoredMinX,nStoredMinY);
		m_bVisible = TRUE;
	}

    if (m_Pen) clientDC.SelectObject(pOldPen);
}


void COccTryView::OnViewAttrs()
{
	if(this->m_pViewAttribsDlg == NULL)
	{
		this->m_pViewAttribsDlg = new ScDlgViewAttribs(this);
		this->m_pViewAttribsDlg->Create(IDD_DIALOG_VIEW_ATTRBS);
	}
	this->m_pViewAttribsDlg->ShowViewAttribs(this->m_occView);
	this->m_pViewAttribsDlg->ShowWindow(SW_SHOW);
}

//更新view属性显示
void	COccTryView::UpdateViewAttribsDlg()
{
	if(this->m_pViewAttribsDlg)
	{
		if(this->m_pViewAttribsDlg->IsWindowVisible())
			this->m_pViewAttribsDlg->ShowViewAttribs(this->m_occView);
	}
}

//更新自动捕捉点的信息
void	COccTryView::UpdateSnapPoints()
{
	//当需要自动捕捉时，更新捕获点信息
	if(this->m_pSnapMgr && this->m_pSnapMgr->IsEnable())
	{
		this->m_pSnapMgr->UpdateSnapPoints();
	}
}

void COccTryView::OnToolObjSelectSenregion()
{
	this->m_bShowSenRegion = !this->m_bShowSenRegion;
}

void COccTryView::OnUpdateToolObjSelectSenregion(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(this->m_bShowSenRegion);
}
