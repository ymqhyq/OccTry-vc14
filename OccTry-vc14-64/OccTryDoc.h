// OccTryDoc.h :  COccTryDoc 类的接口
//
#pragma once

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_Trihedron.hxx>
#include "ScCmdMgr.h"

//鼠标的状态。
enum{ MB_DOWN,MB_MOVE,MB_UP };

class COccTryDoc : public CDocument
{
protected: // 仅从序列化创建
	COccTryDoc();
	DECLARE_DYNCREATE(COccTryDoc)

// 属性
public:

// 操作
public:
	Handle_AIS_InteractiveContext& GetAISContext(){ return m_AISContext; };
	Handle_V3d_Viewer GetViewer()  { return m_Viewer; };

	void					ShowTrihedron(BOOL bShow);

	ScCmdMgr&				GetScCmdMgr()  { return m_scCmdMgr; }
	ScUcsMgr*				GetScUcsMgr()  { return m_pScUcsMgr; }

	//
	void					OnCancelEvent();
	void					OnDeleteEvent();

public:
	//框选方式下的消息响应函数
	void					DragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView);
	//按下shift下的框选方式
	void					ShiftDragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView);

	//点选方式下的消息响应函数
	void					InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//按下shift键下的点选方式
	void					ShiftInputEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//鼠标移动的响应函数
	void					MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//按下shift键下的鼠标移动的响应函数
	void					ShiftMoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	void					Fit();

// 重写
	public:
	virtual BOOL			OnNewDocument();
	virtual BOOL			OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL			OnSaveDocument(LPCTSTR lpszPathName);
	virtual void			Serialize(CArchive& ar);

// 实现
public:
	virtual ~COccTryDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL					ReadBRepFile(LPCTSTR lpszFile);
	BOOL					ReadStepFile(LPCTSTR lpszFile);
	BOOL					ReadIgesFile(LPCTSTR lpszFile);
	BOOL					SaveBRepFile(LPCTSTR lpszFile,BOOL bOnlySelected);
	BOOL					SaveStepFile(LPCTSTR lpszFile,BOOL bOnlySelected);
	BOOL					ExportStepFile();

	int						GetFileType(LPCTSTR lpszFile);

	void					DeleteSelectObj();

	//显示对象信息
	void					ShowObjInfo();

protected:
	//Add For Cascade.
    Handle_V3d_Viewer m_Viewer;  
    Handle_AIS_InteractiveContext m_AISContext;

	Handle_AIS_Trihedron m_hTrihedron;

	ScCmdMgr				m_scCmdMgr;//命令管理器
	ScUcsMgr				*m_pScUcsMgr;//

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBrepprimSphere();
	afx_msg void OnBrepprimCyl();
	afx_msg void OnBrepprimCone();
	afx_msg void OnBrepprimBox();
	afx_msg void OnBrepprimTorus();
	afx_msg void OnMenuLine();
	afx_msg void OnMenuPolyline();
	afx_msg void OnMenuCircle();
	afx_msg void OnMenuArc();
	afx_msg void OnMenuSpline();
	afx_msg void OnMenuPolygon();
	afx_msg void OnMenuPreWorkplane();
	afx_msg void OnMenuNextWorkplane();
	afx_msg void OnMenuModelspace();
	afx_msg void OnMenuChangeOrg();
	afx_msg void OnMenuChangeHeight();
	afx_msg void OnMenuChangeRotate();
	afx_msg void OnMenuChange3point();
	afx_msg void OnMenuChangeView();
	afx_msg void OnMenuChangeCustom();
	afx_msg void OnMenuSplineApp();
	afx_msg void OnMenuClosedPolyline();
	afx_msg void OnMenuPrism();
	afx_msg void OnMenuRevol();
	afx_msg void OnMenuThickshell();
	afx_msg void OnFilletConr();
	afx_msg void OnFilletVarRadius();
	afx_msg void OnChamferConstRadius();
	afx_msg void OnMenuBoolCommon();
	afx_msg void OnMenuBoolFuse();
	afx_msg void OnMenuBoolCut();
	afx_msg void OnMenuPointCloud();
	afx_msg void OnMenuImportBrep();
	afx_msg void OnMenuImportStep();
	afx_msg void OnMenuImportIges();
	afx_msg void OnMenuNpatchFill();
	afx_msg void OnMenuBlendSurf();
	afx_msg void OnMenuBlendSurfPoint();
	afx_msg void OnMenuBlendSurfCurve();
	
	afx_msg void OnMenuExportStepfile();
	afx_msg void OnMenuExportIgesfile();
	afx_msg void OnMenuSurfLoft();
	afx_msg void OnMenuSurfPipe();
	afx_msg void OnMenuSurfOffset();
	afx_msg void OnMenuSolidLoft();
	afx_msg void OnMenuSolidPipe();
	afx_msg void OnMenuSolidOffset();
	afx_msg void OnMenuCurveSewing();
	afx_msg void OnMenuCsproj();
	afx_msg void OnMenuSweep();
	afx_msg void OnMenuSewsurf();
	afx_msg void OnMenuCbrepProj();
	afx_msg void OnMenuExttopoint();
	afx_msg void OnMenuBsplFillsurf();
	afx_msg void OnMenuBrepSweep();
	afx_msg void OnMenuFitall();
	afx_msg void OnMenuDelete();
	afx_msg void OnMenuSurfFilling();
	afx_msg void OnMenuSurfClosecurve();
	afx_msg void OnMenuCurveInter();
	afx_msg void OnMenuSurfCut();
	afx_msg void OnMenuExtraFace();
	afx_msg void OnMenuSurfFillintPoint();
	afx_msg void OnMenuSurfFillingCurve();
	afx_msg void OnMenuSurfFillingFace();
	afx_msg void OnMenuUcsCurvePoint();
	afx_msg void OnMenuUcsFacePoint();
	afx_msg void OnMenuUcsObjPoint();
	afx_msg void OnMenuPrismTwodir();
	afx_msg void OnBrepprimSphereAngle();
	afx_msg void OnBrepprimCylAngle();
	afx_msg void OnBrepprimConeTopplane();
	afx_msg void OnMenuPlaneTwopoint();
	afx_msg void OnMenuPlaneThreepoint();
	afx_msg void OnMenuPlaneNormal();
	afx_msg void OnMenuPlaneCurvepoint();
	afx_msg void OnMenuPlaneFacepoint();
	afx_msg void OnMenuPlaneShapepoint();
	afx_msg void OnMenuPoint();
	afx_msg void OnMenuPointInter();
	afx_msg void OnMenuPointVertex();
	afx_msg void OnMenuShapeStruct();
	afx_msg void OnMenuPlanePoint();
	afx_msg void OnMenuMoving();
	afx_msg void OnMenuRotate();
	afx_msg void OnMenuScale();
	afx_msg void OnMenuMirror();
	afx_msg void OnMenuObjClone();
	afx_msg void OnMenuObjMultiClone();
	afx_msg void OnMenuSolidCut();
	afx_msg void OnDispWireframe();
	afx_msg void OnUpdateDispWireframe(CCmdUI *pCmdUI);
	afx_msg void OnDispShade();
	afx_msg void OnUpdateDispShade(CCmdUI *pCmdUI);
	afx_msg void OnDispTexture();
	afx_msg void OnUpdateDispTexture(CCmdUI *pCmdUI);
	afx_msg void OnDispColor();
	afx_msg void OnUpdateDispColor(CCmdUI *pCmdUI);
	afx_msg void OnDispMaterial();
	afx_msg void OnUpdateDispMaterial(CCmdUI *pCmdUI);
	afx_msg void OnDispTransparency();
	afx_msg void OnDispCutplane();
	afx_msg void OnDispMesh();
	afx_msg void OnCurveExtendToPoint();
	afx_msg void OnSurfPointcloud();
	afx_msg void OnCurveOffsetCurve();
	afx_msg void OnSurfOffsetSurf();
	afx_msg void OnMenuSurfPrism();
	afx_msg void OnSurfPrismTwodir();
	afx_msg void OnSurfExtentBylen();
	afx_msg void OnSurfRevol();
	afx_msg void OnSurfSweepCurve();
	afx_msg void OnToolCurveCurvature();
	afx_msg void OnCurveIsocurve();
	afx_msg void OnCurveTobsplinecurve();
	afx_msg void OnCurveCtrlpntEdit();
	afx_msg void OnCurveCurpntEdit();
	afx_msg void OnSolidCompound();
	afx_msg void OnSolidUncompound();
	afx_msg void OnCurveSurfBoundary();
	afx_msg void OnToolFacenorm();
	afx_msg void OnToolCurveTang();
	afx_msg void OnToolSurfTrimloop();
	afx_msg void OnCurve2dallbnd();
	afx_msg void OnCurve3dallbnd();
	afx_msg void OnToolSteptrimloops();
	afx_msg void OnToolStepfileTrmloopinfo();
	afx_msg void OnToolTrmloopDetail();
	afx_msg void OnToolTrmloopErr();
	afx_msg void OnToolTrmloopFatal();
	afx_msg void OnToolTrmloopErrface();
	afx_msg void OnToolTrmloopFatalface();
	afx_msg void OnToolCurve2d();
	afx_msg void OnSurfRawsurf();
	afx_msg void OnToolCurveSingTang();
	afx_msg void OnToolFacebndTang();
	afx_msg void OnSurfExtractFace();
	afx_msg void OnCurveFaceParamloop();
	afx_msg void OnEditUndo();
	afx_msg void OnFaceMultiloop();
	afx_msg void OnCurveBreaktoedge();
	afx_msg void OnWireAnayClosed();
	afx_msg void OnWireFixClosed();
	afx_msg void OnWireToEdge();
	afx_msg void OnWireFixAll();
	afx_msg void OnSurfFaceAnayls();
	afx_msg void OnSurfFaceFix();
	afx_msg void OnOldboolCommon();
	afx_msg void OnOldboolFuse();
	afx_msg void OnOldboolCut();
	afx_msg void OnWireFixgaps();
	afx_msg void OnCheckValid();
	afx_msg void OnCheckTopoValid();
	afx_msg void OnCheckClosed();
	afx_msg void OnBoolCommonSections();
	afx_msg void OnBoolCommonShape1Objs();
	afx_msg void OnBoolCommonShape2Objs();
	afx_msg void OnSurfSplitplane();
	afx_msg void OnSurfSplitsurf();
	afx_msg void OnPlaneFromWire();
	afx_msg void OnCurvetrimSplit();
	afx_msg void OnCurvetrimQuicktrim();
	afx_msg void OnCurveFillet();
	afx_msg void OnCurveChamfer();
	afx_msg void OnCurveSplitBypoint();
	afx_msg void OnCurveSplitBycurve();
	afx_msg void OnCurveRectangle();
	afx_msg void OnElliCorner();
	afx_msg void OnCircleThreePoint();
	afx_msg void OnArcTwoPoint();
	afx_msg void OnTrsfFfd();
	afx_msg void OnSurfTobsplinesurf();
	afx_msg void OnSolidTobspline();
	afx_msg void OnSurfCtrlpntEdit();
	afx_msg void OnCurveBsplineIncdegree();
	afx_msg void OnCurveBezier();
	afx_msg void OnBezierEditCtrlpnt();
	afx_msg void OnBezierIncdegree();
	afx_msg void OnTrsfAxdf();
	afx_msg void OnSurfBsplineIncdegree();
	afx_msg void OnSurfBsplineInsertCtrlpnt();
	afx_msg void OnCurveBsplineInsertCtrlpnt();
	afx_msg void OnCurveFrameFrenet();
	afx_msg void OnCurveFrameRotate1();
	afx_msg void OnCurveFrameRotate2();
	afx_msg void OnCurveFrameSloan();
	afx_msg void OnCurveFrameDblreflection();
	afx_msg void OnPointPrjtoCurve();
	afx_msg void OnPointPrjtoSurf();
	afx_msg void OnFfdFengSurfffd();
	afx_msg void OnCurveBsplineInsertKnots();
	afx_msg void OnSurfBsplineInsertKnots();
	afx_msg void OnCurveBsplineAddCtrlpnts();
	afx_msg void OnSurfBsplineAddCtrlpnts();
	afx_msg void OnTextCurve();
	afx_msg void OnTextSolid();
	afx_msg void OnMeshImportMs3d();
	afx_msg void OnMeshImport3ds();
	afx_msg void OnMeshConvertToMesh();
	afx_msg void OnMeshImportStl();
	afx_msg void OnMeshImportBinStl();
	afx_msg void OnMeshExportAscStl();
	afx_msg void OnMeshExportBinStl();
	afx_msg void OnTrsfInsertCtrlpntsTest();
	afx_msg void OnFfdRectffd();
	afx_msg void On33249();
	afx_msg void OnFfdFlowByCurve();
	afx_msg void OnTextFace();
	afx_msg void OnFfdFlowOnSurf();
	afx_msg void OnFfdShear();
	afx_msg void OnFfdTwist();
	afx_msg void OnAnalyCurveLength();
	afx_msg void OnAnalyFaceArea();
	afx_msg void OnCurveConcatToSpline();
	afx_msg void OnFfdBend();
};


