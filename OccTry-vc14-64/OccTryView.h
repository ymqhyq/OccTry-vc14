// OccTryView.h : COccTryView 类的接口
//
#pragma once

#include <V3d_View.hxx>

class CInputDialog;
class ScInputMgr;
class COccTryDoc;
class ScView;
class ScSnapMgr;
class CShapeStructDlg;
class ScDlgViewAttribs;

class COccTryView : public CView
{
protected: // 仅从序列化创建
	COccTryView();
	DECLARE_DYNCREATE(COccTryView)

// 属性
public:
	COccTryDoc* GetDocument() const;

	void FitAll() {   m_occView->FitAll();  m_occView->ZFitAll();  };
	void Redraw() {   m_occView->Redraw(); };

	void ShowShapeStruct(const TopoDS_Shape& aShape);
// 操作
public:

// 重写
	public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~COccTryView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//缩放实现
	void				ViewZoom(int nX,int nY);
	//更新鼠标坐标显示
	void				UpdateMousePos(int nX,int nY);
	//显示更新输入对话框位置
	void				UpdateInputDialogPos(int nX,int nY);

	//更新view属性显示
	void				UpdateViewAttribsDlg();

	//更新自动捕捉点的信息
	void				UpdateSnapPoints();

protected:
	enum eLineStyle { LS_Solid, LS_Dot, LS_ShortDash, LS_LongDash, LS_Default };
	CPen*  m_Pen;

    virtual void		DrawRectangle (const Standard_Integer  MinX  ,
    					        const Standard_Integer  MinY  ,
                                const Standard_Integer  MaxX  ,
				    	        const Standard_Integer  MaxY  ,
					            BOOL  bDraw  ,
                                const eLineStyle aLineStyle = LS_Default);

protected:
	//Add For Cascade.
	Handle_V3d_View		 m_occView; //
	Standard_Integer	 myWidth;
	Standard_Integer   	 myHeight;

	int					 m_nPreX,m_nPreY;//前一个鼠标位置。
	int					 m_nStartX,m_nStartY;//初始的X和Y。
	BOOL				 m_bRButtonMove;

	BOOL				m_bShowSenRegion;//是否显示选择区域

	CInputDialog		*m_pInputDlg;
	ScInputMgr			*m_pInputMgr;
	ScView				*m_pScView;//简单封装V3d_View
	ScSnapMgr			*m_pSnapMgr;
	CShapeStructDlg		*m_pShapeStructDlg;//shape struct dialog.
	ScDlgViewAttribs	*m_pViewAttribsDlg;//

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMenuPlanview();
	afx_msg void OnMenuFrontview();
	afx_msg void OnMenuLeftview();
	afx_msg void OnMenuRightmenu();
	afx_msg void OnMenuTopview();
	afx_msg void OnMenuBottomview();
	afx_msg void OnMenuBackview();
	afx_msg void OnViewAttrs();
	afx_msg void OnToolObjSelectSenregion();
	afx_msg void OnUpdateToolObjSelectSenregion(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // OccTryView.cpp 的调试版本
inline COccTryDoc* COccTryView::GetDocument() const
   { return reinterpret_cast<COccTryDoc*>(m_pDocument); }
#endif

