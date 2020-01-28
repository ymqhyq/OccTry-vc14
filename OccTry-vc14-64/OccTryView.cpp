// OccTryView.cpp : COccTryView ���ʵ��
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
	// ��׼��ӡ����
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

// COccTryView ����/����

COccTryView::COccTryView()
{
	// TODO: �ڴ˴���ӹ������
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
	// TODO: �ڴ˴�ͨ���޸� CREATESTRUCT cs ���޸Ĵ������
	// ��ʽ

	return CView::PreCreateWindow(cs);
}


// COccTryView ����

void COccTryView::OnDraw(CDC* /*pDC*/)
{
	COccTryDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	//Add For Cascade.
	//ʵ��֤��:���������Post��Ϣ����,�����ڴ�С�ı�ʱ,���ܻᵼ��ˢ������.
	CRect aRect;
	GetWindowRect(aRect);
	if(myWidth != aRect.Width() || myHeight != aRect.Height()) {
		myWidth = aRect.Width();
		myHeight = aRect.Height();
		::PostMessage ( GetSafeHwnd () , WM_SIZE , SW_SHOW , myWidth + myHeight*65536 );
	}
	m_occView->Redraw();
}


// COccTryView ��ӡ

BOOL COccTryView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void COccTryView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��ӡǰ��Ӷ���ĳ�ʼ��
}

void COccTryView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��ӡ������������
}


// COccTryView ���

#ifdef _DEBUG
void COccTryView::AssertValid() const
{
	CView::AssertValid();
}

void COccTryView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COccTryDoc* COccTryView::GetDocument() const // �ǵ��԰汾��������
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

// COccTryView ��Ϣ�������

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

	//֧������
//yxk20200104	
//https://dev.opencascade.org/doc/overview/html/occt_dev_guides__upgrade.html
//���ڣ��������Ȼ���ӳ����и��ߵ����ȼ���
//����ö��V3d_TypeOfSurface��Graphic3d_TypeOfSurface����OpenGl_SurfaceDetailState��Graphic3d_CView��
//OpenGl_ShaderManager��OpenGl_View��V3d_View��V3d_Viewer�е���Ӧ������ɾ������������VSetTextureMode��ɾ����
	//m_occView->SetSurfaceDetail(V3d_TEX_ALL);
//endyxk
	//
	m_occView->SetBackgroundColor(Quantity_TOC_RGB,0.1,0.0,0.1);

	//GetDocument()->ShowTrihedron(TRUE);
	GetDocument()->GetScUcsMgr()->ShowWCSTrihedron();

	//�������߶Ի���
	this->m_pInputDlg = new CInputDialog(this);
	this->m_pInputDlg->Create(IDD_INPUT_DIALOG);

	//�����Ի���
	this->m_pShapeStructDlg = new CShapeStructDlg(this);
	this->m_pShapeStructDlg->Create(IDD_DIALOG_SHAPE_STRUCT);


	//�����������
	this->m_pInputMgr = new ScInputMgr(GetDocument()->GetScCmdMgr(),this,
		this->m_pInputDlg);

	this->m_pScView = new ScView(m_occView);
	this->m_pScView->SetUcsMgr(GetDocument()->GetScUcsMgr());
	this->m_pScView->m_pViewWnd = this;//��¼

	//���б�Ҫ������
	GetDocument()->GetScCmdMgr().SetScView(m_pScView);

	//�Զ���׽����
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CView::OnLButtonDblClk(nFlags, point);
}

//
// ���������¡����ܵĺ���������
// a����ѡ  b����ѡ.��ѡҪ�����̧��ʱ�ſ��жϡ�
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
		//�ж��Ƿ��ѡ
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
			(point.y == this->m_nStartY))//�ͳ�ʼ����бȽϣ�û���ƶ�
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
		{//��ѡ
			this->m_nPreX = point.x;
			this->m_nPreY = point.y;
			//����ѡ���
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

//������Ҫʵ����ת����
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
	//��ͼ�任����Ҫ����snap��
	UpdateSnapPoints();

	CView::OnMButtonUp(nFlags, point);
}

void COccTryView::OnMouseMove(UINT nFlags, CPoint point)
{
	//���û�н��㣬���ý���
	CWnd *pActiveWnd = this->GetActiveWindow();
	if(pActiveWnd)
	{
		if((pActiveWnd->GetSafeHwnd() != this->GetSafeHwnd()) &&
			(pActiveWnd->GetSafeHwnd() != NULL))
		{
			if((point.x != this->m_nPreX) ||
				(point.y != this->m_nPreY))//����ƶ�������ԭ���������ý���.
			{
				//DTRACE("\nOnMouseMove SetFocus...");
				SetFocus();
			}
		}
	}
	//�������
	if(nFlags & MK_LBUTTON)
	{
		if(nFlags & MK_CONTROL)
		{
		}
		else
		{
			//��ѡ����
			
			//����ѡ���
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

			//��ʾѡ���ѡ
			this->DrawRectangle(this->m_nStartX,this->m_nStartY,point.x,point.y,TRUE);

		}
	}
	else if ( nFlags & MK_MBUTTON)
	{
		//�м�����ת�����ţ�ƽ��
		if(nFlags & MK_SHIFT)
		{
			this->ViewZoom(point.x,point.y);
		}
		else if(nFlags & MK_CONTROL)
		{
			this->m_occView->Pan(point.x - this->m_nPreX,
				  this->m_nPreY - point.y);

			//��¼��ǰ��
			this->m_nPreX = point.x;
			this->m_nPreY = point.y;
		}
		else
		{
			this->m_occView->Rotation(point.x,point.y);
			this->m_occView->Redraw();
		}

		//this->UpdateViewAttribsDlg();
		//���»���
		//this->m_occView->Redraw();
	}
	else if ( nFlags & MK_RBUTTON )
	{
		this->m_occView->Pan(point.x - this->m_nPreX,
			this->m_nPreY - point.y);

		this->UpdateViewAttribsDlg();

		this->m_bRButtonMove = TRUE;
		//��¼��ǰ��
		this->m_nPreX = point.x;
		this->m_nPreY = point.y;
	}
	else
	{
		//����ƶ�����Ϣ
		if(nFlags & MK_SHIFT)
		{
			GetDocument()->ShiftMoveEvent(point,this->m_occView);
		}
		else
		{
			GetDocument()->MoveEvent(point,this->m_occView);
		}

		//�������������
		if(this->m_pInputMgr)
		{
			//DTRACE("\n input mouse move....");
			this->m_pInputMgr->MouseEvent(point,IN_MOUSE_MOVE);
		}

		//�������ֵ,��Ȼ��Ӱ�쵽��ת!!!
		this->UpdateMousePos(point.x,point.y);
		this->UpdateInputDialogPos(point.x,point.y);

		//��¼��ǰ��
		this->m_nPreX = point.x;
		this->m_nPreY = point.y;
	}

	CView::OnMouseMove(nFlags, point);
}

//����ʵ��
void	COccTryView::ViewZoom(int nX,int nY)
{
	Standard_Real dViewWith,dViewHeight;//yxk202000104 Quantity_Length->Standard_Real
	this->m_occView->Size(dViewWith,dViewHeight);
	DTRACE("\nview with is %.4f,height is %.4f.",dViewWith,dViewHeight);

	//̫С��̫���ֹ����
	//if((dViewWith > 10.0) && (dViewWith < 10000.0) &&
	//	(dViewHeight > 10.0) && (dViewHeight < 10000.0))
	{
		this->m_occView->Zoom(this->m_nPreX,this->m_nPreY,nX,nY);
	}

	this->m_nPreX = nX;
	this->m_nPreY = nY;
}

//ǰ���Ŵ󣬺����С
BOOL COccTryView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	Standard_Real dViewWith,dViewHeight;
	this->m_occView->Size(dViewWith,dViewHeight);
//	DTRACE("\n Mouse Wheel....");

	double dCoef = 1.0;
	BOOL bZoom = TRUE;
	if(zDelta > 0.0)//�Ŵ�
	{
		if(dViewWith < 0.01 || dViewHeight < 0.01)
			bZoom = FALSE;
		else
			dCoef = 5.0/4.0;
	}
	else//��С
	{
		if(dViewWith > 10000000.0 || dViewHeight > 10000000.0)
			bZoom = FALSE;
		else
			dCoef = 4.0/5.0;
	}

	//̫С��̫���ֹ����
	if(bZoom)
	{
		//�Ե�ǰ���λ��Ϊ���ģ���������
		//this->m_occView->SetCenter(pt.x,pt.y);

		this->m_occView->SetZoom(dCoef,TRUE);
		this->m_occView->Redraw();

		this->UpdateViewAttribsDlg();
		//��ͼ�任����Ҫ����snap��
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
		//��ͼ�任����Ҫ����snap��
		UpdateSnapPoints();
	}
	else
	{
		//
		GetDocument()->InputEvent(point,MINPUT_RIGHT,this->m_occView);
	}

	CView::OnRButtonUp(nFlags, point);
}

//�������������ʾ
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

//��ʾ��������Ի���λ��
void	COccTryView::UpdateInputDialogPos(int nX,int nY)
{
	static int nPreX = 0,nPreY = 0;

	if(this->m_pInputDlg)
	{
		//�ƶ�һ������Ÿ�����ʾ
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

//������
void COccTryView::OnMenuPlanview()
{
	if(!this->m_pScView)
		return;

	switch(this->m_pScView->m_nViewType)
	{
		case ScView_Front:
			this->m_pScView->SetProj(V3d_Zpos);
			break;
		case ScView_Back://XYƽ��
			this->m_pScView->SetProj(V3d_Zneg);
			break;
		case ScView_Top:
			this->m_pScView->SetProj(V3d_Ypos);
			break;
		case ScView_Bottom://XZƽ��
			this->m_pScView->SetProj(V3d_Yneg);
			break;
		case ScView_Left:
			this->m_pScView->SetProj(V3d_Xpos);
			break;
		case ScView_Right://YZƽ��
			this->m_pScView->SetProj(V3d_Xneg);
			break;
		default:
			break;
	}
	//��ͼ�任����Ҫ����snap��
	UpdateSnapPoints();
}

//ǰ��ͼ
void COccTryView::OnMenuFrontview()
{
	// TODO: �ڴ���������������
	this->m_pScView->SetProj(V3d_Zpos);
	//��ͼ�任����Ҫ����snap��
	UpdateSnapPoints();
}

//����ͼ
void COccTryView::OnMenuBackview()
{
	// TODO: �ڴ���������������
	this->m_pScView->SetProj(V3d_Zneg);
	//��ͼ�任����Ҫ����snap��
	UpdateSnapPoints();
}

//����ͼ
void COccTryView::OnMenuLeftview()
{
	// TODO: �ڴ���������������
	this->m_pScView->SetProj(V3d_Xpos);
	//��ͼ�任����Ҫ����snap��
	UpdateSnapPoints();
}

//����ͼ
void COccTryView::OnMenuRightmenu()
{
	// TODO: �ڴ���������������
	this->m_pScView->SetProj(V3d_Xneg);
	//��ͼ�任����Ҫ����snap��
	UpdateSnapPoints();
}

//����ͼ
void COccTryView::OnMenuTopview()
{
	// TODO: �ڴ���������������
	this->m_pScView->SetProj(V3d_Ypos);
	//��ͼ�任����Ҫ����snap��
	UpdateSnapPoints();
}

//����ͼ
void COccTryView::OnMenuBottomview()
{
	// TODO: �ڴ���������������
	this->m_pScView->SetProj(V3d_Yneg);
	//��ͼ�任����Ҫ����snap��
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

//����view������ʾ
void	COccTryView::UpdateViewAttribsDlg()
{
	if(this->m_pViewAttribsDlg)
	{
		if(this->m_pViewAttribsDlg->IsWindowVisible())
			this->m_pViewAttribsDlg->ShowViewAttribs(this->m_occView);
	}
}

//�����Զ���׽�����Ϣ
void	COccTryView::UpdateSnapPoints()
{
	//����Ҫ�Զ���׽ʱ�����²������Ϣ
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
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(this->m_bShowSenRegion);
}
