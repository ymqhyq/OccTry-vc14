#pragma once


// CShapeStructDlg 对话框

class CShapeStructDlg : public CDialog
{
	DECLARE_DYNAMIC(CShapeStructDlg)

public:
	CShapeStructDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShapeStructDlg();

	//显示一个shape的内部结构。
	void				ShowShape(const TopoDS_Shape& aShape);

// 对话框数据
	enum { IDD = IDD_DIALOG_SHAPE_STRUCT };

	CTreeCtrl			m_ssTree; //

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	HTREEITEM			ShowShape(const TopoDS_Shape& aShape,
							HTREEITEM hParent);
	//具体的显示函数
	HTREEITEM			ShowCompound(const TopoDS_Shape& aShape,
							HTREEITEM hParent);
	HTREEITEM			ShowCompSolid(const TopoDS_Shape& aShape,
							HTREEITEM hParent);
	HTREEITEM			ShowSolid(const TopoDS_Shape& aShape,
							HTREEITEM hParent);
	HTREEITEM			ShowShell(const TopoDS_Shape& aShape,
							HTREEITEM hParent);
	HTREEITEM			ShowFace(const TopoDS_Shape& aShape,
							HTREEITEM hParent);
	HTREEITEM			ShowWire(const TopoDS_Shape& aShape,
							HTREEITEM hParent);
	//显示裁剪环的wire
	HTREEITEM			ShowWire(const TopoDS_Shape& aShape,
							const TopoDS_Face& aFace,
							HTREEITEM hParent);

	HTREEITEM			ShowEdge(const TopoDS_Shape& aShape,
							HTREEITEM hParent);
	//显示裁剪环中的edge
	HTREEITEM			ShowEdge(const TopoDS_Shape& aShape,
							const TopoDS_Face& aFace,
							HTREEITEM hParent);
	HTREEITEM			ShowVertex(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//显示shape的基本信息，包括：方向、位置、内部指针等。
	HTREEITEM			ShowShapeBaseProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//显示face的geom属性。主要是surface的属性。
	HTREEITEM			ShowFaceGeomProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//显示face的裁剪环属性。
	HTREEITEM			ShowFaceLoopProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//显示surface的属性
	HTREEITEM			ShowSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);

	//显示各个surface的信息
	HTREEITEM			ShowBSplineSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);
	HTREEITEM			ShowBezierSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);
	HTREEITEM			ShowElementSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);
	HTREEITEM			ShowPlaneSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);
	HTREEITEM			ShowCylindricalSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);
	HTREEITEM			ShowConicalSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);
	HTREEITEM			ShowSphericalSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);
	HTREEITEM			ShowToroidalSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);
	HTREEITEM			ShowSweptSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);
	HTREEITEM			ShowExtrusionSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);
	HTREEITEM			ShowRevolutionSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);


	//显示Edge的geom属性。
	HTREEITEM			ShowEdgeGeomProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//显示裁剪环的Edge的属性。
	HTREEITEM			ShowLoopEdgeProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//显示curve的属性
	HTREEITEM			ShowCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	
	//显示具体类型的curve
	HTREEITEM			ShowBSplineCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowBezierCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowTrimmedCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);

	HTREEITEM			ShowConicCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowCircleCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowEllipseCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowHyperbolaCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowParabolaCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowLineCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	
	//显示2d curve的属性
	HTREEITEM			Show2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);

	//显示具体类型的curve
	HTREEITEM			ShowBSpline2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowBezier2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowTrimmed2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);

	HTREEITEM			ShowConic2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowCircle2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowEllipse2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowHyperbola2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowParabola2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);

	HTREEITEM			ShowLine2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);
	
protected:
	virtual void OnOK();

};
