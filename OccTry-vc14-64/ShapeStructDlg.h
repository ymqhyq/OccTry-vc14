#pragma once


// CShapeStructDlg �Ի���

class CShapeStructDlg : public CDialog
{
	DECLARE_DYNAMIC(CShapeStructDlg)

public:
	CShapeStructDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShapeStructDlg();

	//��ʾһ��shape���ڲ��ṹ��
	void				ShowShape(const TopoDS_Shape& aShape);

// �Ի�������
	enum { IDD = IDD_DIALOG_SHAPE_STRUCT };

	CTreeCtrl			m_ssTree; //

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	HTREEITEM			ShowShape(const TopoDS_Shape& aShape,
							HTREEITEM hParent);
	//�������ʾ����
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
	//��ʾ�ü�����wire
	HTREEITEM			ShowWire(const TopoDS_Shape& aShape,
							const TopoDS_Face& aFace,
							HTREEITEM hParent);

	HTREEITEM			ShowEdge(const TopoDS_Shape& aShape,
							HTREEITEM hParent);
	//��ʾ�ü����е�edge
	HTREEITEM			ShowEdge(const TopoDS_Shape& aShape,
							const TopoDS_Face& aFace,
							HTREEITEM hParent);
	HTREEITEM			ShowVertex(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//��ʾshape�Ļ�����Ϣ������������λ�á��ڲ�ָ��ȡ�
	HTREEITEM			ShowShapeBaseProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//��ʾface��geom���ԡ���Ҫ��surface�����ԡ�
	HTREEITEM			ShowFaceGeomProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//��ʾface�Ĳü������ԡ�
	HTREEITEM			ShowFaceLoopProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//��ʾsurface������
	HTREEITEM			ShowSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent);

	//��ʾ����surface����Ϣ
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


	//��ʾEdge��geom���ԡ�
	HTREEITEM			ShowEdgeGeomProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//��ʾ�ü�����Edge�����ԡ�
	HTREEITEM			ShowLoopEdgeProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent);

	//��ʾcurve������
	HTREEITEM			ShowCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	
	//��ʾ�������͵�curve
	HTREEITEM			ShowBSplineCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowBezierCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowTrimmedCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);

	HTREEITEM			ShowConicCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowCircleCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowEllipseCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowHyperbolaCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowParabolaCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM			ShowLineCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	
	//��ʾ2d curve������
	HTREEITEM			Show2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);

	//��ʾ�������͵�curve
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
