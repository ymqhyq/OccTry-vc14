// OccTryDoc.cpp :  COccTryDoc 类的实现
//

#include "stdafx.h"
#include "OccTry.h"

#include <STEPControl_Reader.hxx>
#include <TopTools_HSequenceOfShape.hxx>
//#include <Interface_TraceFile.hxx>//yxk20200104
#include <IGESControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <BRepTools.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <STEPControl_StepModelType.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <BRepBndLib.hxx>

#include "MainFrm.h"
#include "OccTryView.h"
#include "OccTryDoc.h"
#include ".\occtrydoc.h"
#include "ScUcsMgr.h"
#include "ScCmdBRepPrim.h"
#include "ScCmdCurve.h"
#include "ScCmdSurface.h"
#include "ScCmdUcs.h"
#include "ScCmdPlate.h"
#include "ScCmdOffset.h"
#include "ScCmdFillet.h"
#include "ScCmdEdit.h"
#include "ScCmdTrsf.h"
#include "ScTools.h"
#include "ScCmdGeom.h"
#include "ScCmdDisplay.h"
#include "ScCmdTool.h"
#include "ScTools.h"
#include "ScCmdWire.h"
#include "ScCmdFace.h"
#include "ScCmdBoolean.h"
#include "ScCmdCurveCreate.h"
#include "ScCmdPlane.h"
#include "ScCmdCurveEdit.h"
#include "ScCmdFFD.h"
#include "ScCmdBSpline.h"
#include "ScCmdAxDF.h"
#include "ScCmdPoint.h"
#include "ScCmdSurfFFD.h"
#include "ScCmdText.h"
#include "ScCmdMeshImport.h"
#include "ScCmdMeshGen.h"
#include "ScCmdFFDLine.h"
#include "ScCmdFlowByCurve.h"
#include "ScCmdFlowBySurf.h"
#include "ScCmdShear.h"
#include "ScCmdFFDTwist.h"
#include "ScCmdAnalysis.h"
#include "ScCmdFFDBend.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#endif

enum{
	BREP_FILE,
	STEP_FILE,
	IGES_FILE,
	UNKNOWN_FILE,
};

// COccTryDoc

IMPLEMENT_DYNCREATE(COccTryDoc, CDocument)

BEGIN_MESSAGE_MAP(COccTryDoc, CDocument)
	ON_COMMAND(ID_BREPPRIM_SPHERE, OnBrepprimSphere)
	ON_COMMAND(ID_BREPPRIM_CYL, OnBrepprimCyl)
	ON_COMMAND(ID_BREPPRIM_CONE, OnBrepprimCone)
	ON_COMMAND(ID_BREPPRIM_BOX, OnBrepprimBox)
	ON_COMMAND(ID_BREPPRIM_TORUS, OnBrepprimTorus)
	ON_COMMAND(ID_MENU_LINE, OnMenuLine)
	ON_COMMAND(ID_MENU_POLYLINE, OnMenuPolyline)
	ON_COMMAND(ID_MENU_CIRCLE, OnMenuCircle)
	ON_COMMAND(ID_MENU_ARC, OnMenuArc)
	ON_COMMAND(ID_MENU_SPLINE, OnMenuSpline)
	ON_COMMAND(ID_MENU_POLYGON, OnMenuPolygon)
	ON_COMMAND(ID_MENU_PRE_WORKPLANE, OnMenuPreWorkplane)
	ON_COMMAND(ID_MENU_NEXT_WORKPLANE, OnMenuNextWorkplane)
	ON_COMMAND(ID_MENU_MODELSPACE, OnMenuModelspace)
	ON_COMMAND(ID_MENU_CHANGE_ORG, OnMenuChangeOrg)
	ON_COMMAND(ID_MENU_CHANGE_HEIGHT, OnMenuChangeHeight)
	ON_COMMAND(ID_MENU_CHANGE_ROTATE, OnMenuChangeRotate)
	ON_COMMAND(ID_MENU_CHANGE_3POINT, OnMenuChange3point)
	ON_COMMAND(ID_MENU_CHANGE_VIEW, OnMenuChangeView)
	ON_COMMAND(ID_MENU_CHANGE_CUSTOM, OnMenuChangeCustom)
	ON_COMMAND(ID_MENU_SPLINE_APP, OnMenuSplineApp)
	ON_COMMAND(ID_MENU_CLOSED_POLYLINE, OnMenuClosedPolyline)
	ON_COMMAND(ID_MENU_PRISM, OnMenuPrism)
	ON_COMMAND(ID_MENU_REVOL, OnMenuRevol)
	ON_COMMAND(ID_MENU_THICKSHELL, OnMenuThickshell)
	ON_COMMAND(ID_FILLET_CONR, OnFilletConr)
	ON_COMMAND(ID_FILLET_VARR, OnFilletVarRadius)
	ON_COMMAND(ID_CHAMFER_CONR, OnChamferConstRadius)
	ON_COMMAND(ID_MENU_BOOL_COMMON, OnMenuBoolCommon)
	ON_COMMAND(ID_MENU_BOOL_FUSE, OnMenuBoolFuse)
	ON_COMMAND(ID_MENU_BOOL_CUT, OnMenuBoolCut)
	ON_COMMAND(ID_MENU_POINT_CLOUD, OnMenuPointCloud)
	ON_COMMAND(ID_MENU_IMPORT_BREP, OnMenuImportBrep)
	ON_COMMAND(ID_MENU_IMPORT_STEP, OnMenuImportStep)
	ON_COMMAND(ID_MENU_IMPORT_IGES, OnMenuImportIges)
	ON_COMMAND(ID_MENU_NPATCH_FILL, OnMenuNpatchFill)
	ON_COMMAND(ID_MENU_BLEND_SURF, OnMenuBlendSurf)
	ON_COMMAND(ID_MENU_BLEND_SURF_POINT, OnMenuBlendSurfPoint)
	ON_COMMAND(ID_MENU_BLEND_SURF_CURVE, OnMenuBlendSurfCurve)
	ON_COMMAND(ID_MENU_EXPORT_STEPFILE, OnMenuExportStepfile)
	ON_COMMAND(ID_MENU_EXPORT_IGESFILE, OnMenuExportIgesfile)
	ON_COMMAND(ID_MENU_SURF_LOFT, OnMenuSurfLoft)
	ON_COMMAND(ID_MENU_SURF_PIPE, OnMenuSurfPipe)
	ON_COMMAND(ID_MENU_SURF_OFFSET, OnMenuSurfOffset)
	ON_COMMAND(ID_MENU_SOLID_LOFT, OnMenuSolidLoft)
	ON_COMMAND(ID_MENU_SOLID_PIPE, OnMenuSolidPipe)
	ON_COMMAND(ID_MENU_SOLID_OFFSET, OnMenuSolidOffset)
	ON_COMMAND(ID_MENU_CURVE_SEWING, OnMenuCurveSewing)
	ON_COMMAND(ID_MENU_CSPROJ, OnMenuCsproj)
	ON_COMMAND(ID_MENU_SWEEP, OnMenuSweep)
	ON_COMMAND(ID_MENU_SEWSURF, OnMenuSewsurf)
	ON_COMMAND(ID_MENU_CBREP_PROJ, OnMenuCbrepProj)
	ON_COMMAND(ID_MENU_EXTTOPOINT, OnMenuExttopoint)
	ON_COMMAND(ID_MENU_BSPL_FILLSURF, OnMenuBsplFillsurf)
	ON_COMMAND(ID_MENU_BREP_SWEEP, OnMenuBrepSweep)
	ON_COMMAND(ID_MENU_FITALL, OnMenuFitall)
	ON_COMMAND(ID_MENU_DELETE, OnMenuDelete)
	ON_COMMAND(ID_MENU_SURF_FILLING, OnMenuSurfFilling)
	ON_COMMAND(ID_MENU_SURF_CLOSECURVE, OnMenuSurfClosecurve)
	ON_COMMAND(ID_MENU_CURVE_INTER, OnMenuCurveInter)
	ON_COMMAND(ID_MENU_SURF_CUT, OnMenuSurfCut)
	ON_COMMAND(ID_MENU_EXTRA_FACE, OnMenuExtraFace)
	ON_COMMAND(ID_MENU_SURF_FILLINT_POINT, OnMenuSurfFillintPoint)
	ON_COMMAND(ID_MENU_SURF_FILLING_CURVE, OnMenuSurfFillingCurve)
	ON_COMMAND(ID_MENU_SURF_FILLING_FACE, OnMenuSurfFillingFace)
	ON_COMMAND(ID_MENU_UCS_CURVE_POINT, OnMenuUcsCurvePoint)
	ON_COMMAND(ID_MENU_UCS_FACE_POINT, OnMenuUcsFacePoint)
	ON_COMMAND(ID_MENU_UCS_OBJ_POINT, OnMenuUcsObjPoint)
	ON_COMMAND(ID_MENU_PRISM_TWODIR, OnMenuPrismTwodir)
	ON_COMMAND(ID_BREPPRIM_SPHERE_ANGLE, OnBrepprimSphereAngle)
	ON_COMMAND(ID_BREPPRIM_CYL_ANGLE, OnBrepprimCylAngle)
	ON_COMMAND(ID_BREPPRIM_CONE_TOPPLANE, OnBrepprimConeTopplane)
	ON_COMMAND(ID_MENU_PLANE_TWOPOINT, OnMenuPlaneTwopoint)
	ON_COMMAND(ID_MENU_PLANE_THREEPOINT, OnMenuPlaneThreepoint)
	ON_COMMAND(ID_MENU_PLANE_NORMAL, OnMenuPlaneNormal)
	ON_COMMAND(ID_MENU_PLANE_CURVEPOINT, OnMenuPlaneCurvepoint)
	ON_COMMAND(ID_MENU_PLANE_FACEPOINT, OnMenuPlaneFacepoint)
	ON_COMMAND(ID_MENU_PLANE_SHAPEPOINT, OnMenuPlaneShapepoint)
	ON_COMMAND(ID_MENU_POINT, OnMenuPoint)
	ON_COMMAND(ID_MENU_POINT_INTER, OnMenuPointInter)
	ON_COMMAND(ID_MENU_POINT_VERTEX, OnMenuPointVertex)
	ON_COMMAND(ID_MENU_SHAPE_STRUCT, OnMenuShapeStruct)
	ON_COMMAND(ID_MENU_PLANE_POINT, OnMenuPlanePoint)
	ON_COMMAND(ID_MENU_MOVING, OnMenuMoving)
	ON_COMMAND(ID_MENU_ROTATE, OnMenuRotate)
	ON_COMMAND(ID_MENU_SCALE, OnMenuScale)
	ON_COMMAND(ID_MENU_MIRROR, OnMenuMirror)
	ON_COMMAND(ID_MENU_OBJ_CLONE, OnMenuObjClone)
	ON_COMMAND(ID_MENU_OBJ_MULTI_CLONE, OnMenuObjMultiClone)
	ON_COMMAND(ID_MENU_SOLID_CUT, OnMenuSolidCut)
	ON_COMMAND(IDM_DISP_WIREFRAME, OnDispWireframe)
	ON_UPDATE_COMMAND_UI(IDM_DISP_WIREFRAME, OnUpdateDispWireframe)
	ON_COMMAND(IDM_DISP_SHADE, OnDispShade)
	ON_UPDATE_COMMAND_UI(IDM_DISP_SHADE, OnUpdateDispShade)
	ON_COMMAND(IDM_DISP_TEXTURE, OnDispTexture)
	ON_UPDATE_COMMAND_UI(IDM_DISP_TEXTURE, OnUpdateDispTexture)
	ON_COMMAND(IDM_DISP_COLOR, OnDispColor)
	ON_UPDATE_COMMAND_UI(IDM_DISP_COLOR, OnUpdateDispColor)
	ON_COMMAND(IDM_DISP_MATERIAL, OnDispMaterial)
	ON_UPDATE_COMMAND_UI(IDM_DISP_MATERIAL, OnUpdateDispMaterial)
	ON_COMMAND(IDM_DISP_TRANSPARENCY, OnDispTransparency)
	ON_COMMAND(IDM_DISP_CUTPLANE, OnDispCutplane)
	ON_COMMAND(IDM_DISP_MESH, OnDispMesh)
	ON_COMMAND(IDM_CURVE_EXTEND_TO_POINT, OnCurveExtendToPoint)
	ON_COMMAND(IDM_SURF_POINTCLOUD, OnSurfPointcloud)
	ON_COMMAND(IDM_CURVE_OFFSET_CURVE, OnCurveOffsetCurve)
	ON_COMMAND(IDM_SURF_OFFSET_SURF, OnSurfOffsetSurf)
	ON_COMMAND(ID_MENU_SURF_PRISM, OnMenuSurfPrism)
	ON_COMMAND(IDM_SURF_PRISM_TWODIR, OnSurfPrismTwodir)
	ON_COMMAND(IDM_SURF_EXTENT_BYLEN, OnSurfExtentBylen)
	ON_COMMAND(IDM_SURF_REVOL, OnSurfRevol)
	ON_COMMAND(IDM_SURF_SWEEP_CURVE, OnSurfSweepCurve)
	ON_COMMAND(IDM_TOOL_CURVE_CURVATURE, OnToolCurveCurvature)
	ON_COMMAND(IDM_CURVE_ISOCURVE, OnCurveIsocurve)
	ON_COMMAND(IDM_CURVE_TOBSPLINECURVE, OnCurveTobsplinecurve)
	ON_COMMAND(IDM_CURVE_CTRLPNT_EDIT, OnCurveCtrlpntEdit)
	ON_COMMAND(IDM_CURVE_CURPNT_EDIT, OnCurveCurpntEdit)
	ON_COMMAND(IDM_SOLID_COMPOUND, OnSolidCompound)
	ON_COMMAND(IDM_SOLID_UNCOMPOUND, OnSolidUncompound)
	ON_COMMAND(IDM_CURVE_SURF_BOUNDARY, OnCurveSurfBoundary)
	ON_COMMAND(IDM_TOOL_FACENORM, OnToolFacenorm)
	ON_COMMAND(IDM_TOOL_CURVE_TANG, OnToolCurveTang)
	ON_COMMAND(IDM_TOOL_SURF_TRIMLOOP, OnToolSurfTrimloop)
	ON_COMMAND(IDM_CURVE_2DALLBND, OnCurve2dallbnd)
	ON_COMMAND(IDM_CURVE_3DALLBND, OnCurve3dallbnd)
	ON_COMMAND(IDM_TOOL_STEPTRIMLOOPS, OnToolSteptrimloops)
	ON_COMMAND(IDM_TOOL_STEPFILE_TRMLOOPINFO, OnToolStepfileTrmloopinfo)
	ON_COMMAND(IDM_TOOL_TRMLOOP_DETAIL, OnToolTrmloopDetail)
	ON_COMMAND(IDM_TOOL_TRMLOOP_ERR, OnToolTrmloopErr)
	ON_COMMAND(IDM_TOOL_TRMLOOP_FATAL, OnToolTrmloopFatal)
	ON_COMMAND(IDM_TOOL_TRMLOOP_ERRFACE, OnToolTrmloopErrface)
	ON_COMMAND(IDM_TOOL_TRMLOOP_FATALFACE, OnToolTrmloopFatalface)
	ON_COMMAND(IDM_TOOL_CURVE2D, OnToolCurve2d)
	ON_COMMAND(IDM_SURF_RAWSURF, OnSurfRawsurf)
	ON_COMMAND(IDM_TOOL_CURVE_SING_TANG, OnToolCurveSingTang)
	ON_COMMAND(IDM_TOOL_FACEBND_TANG, OnToolFacebndTang)
	ON_COMMAND(IDM_SURF_EXTRACT_FACE, OnSurfExtractFace)
	ON_COMMAND(IDM_CURVE_FACE_PARAMLOOP, OnCurveFaceParamloop)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(IDM_FACE_MULTILOOP, OnFaceMultiloop)
	ON_COMMAND(IDM_WIRE_ANAY_CLOSED, OnWireAnayClosed)
	ON_COMMAND(IDM_WIRE_FIX_CLOSED, OnWireFixClosed)
	ON_COMMAND(IDM_WIRE_TO_EDGE, OnWireToEdge)
	ON_COMMAND(IDM_WIRE_FIX_ALL, OnWireFixAll)
	ON_COMMAND(IDM_SURF_FACE_ANAYLS, OnSurfFaceAnayls)
	ON_COMMAND(IDM_SURF_FACE_FIX, OnSurfFaceFix)
	ON_COMMAND(IDM_OLDBOOL_COMMON, OnOldboolCommon)
	ON_COMMAND(IDM_OLDBOOL_FUSE, OnOldboolFuse)
	ON_COMMAND(IDM_OLDBOOL_CUT, OnOldboolCut)
	ON_COMMAND(IDM_WIRE_FIXGAPS, OnWireFixgaps)
	ON_COMMAND(IDM_CHECK_VALID, OnCheckValid)
	ON_COMMAND(IDM_CHECK_TOPO_VALID, OnCheckTopoValid)
	ON_COMMAND(IDM_CHECK_CLOSED, OnCheckClosed)
	ON_COMMAND(IDM_BOOL_COMMON_SECTIONS, OnBoolCommonSections)
	ON_COMMAND(IDM_BOOL_COMMON_SHAPE1_OBJS, OnBoolCommonShape1Objs)
	ON_COMMAND(IDM_BOOL_COMMON_SHAPE2_OBJS, OnBoolCommonShape2Objs)
	ON_COMMAND(IDM_SURF_SPLITPLANE, OnSurfSplitplane)
	ON_COMMAND(IDM_SURF_SPLITSURF, OnSurfSplitsurf)
	ON_COMMAND(IDM_PLANE_FROM_WIRE, OnPlaneFromWire)
	ON_COMMAND(IDM_CURVETRIM_SPLIT, OnCurvetrimSplit)
	ON_COMMAND(IDM_CURVETRIM_QUICKTRIM, OnCurvetrimQuicktrim)
	ON_COMMAND(IDM_CURVE_FILLET, OnCurveFillet)
	ON_COMMAND(IDM_CURVE_CHAMFER, OnCurveChamfer)
	ON_COMMAND(IDM_CURVE_SPLIT_BYPOINT, OnCurveSplitBypoint)
	ON_COMMAND(IDM_CURVE_SPLIT_BYCURVE, OnCurveSplitBycurve)
	ON_COMMAND(IDM_CURVE_RECTANGLE, OnCurveRectangle)
	ON_COMMAND(IDM_ELLI_CORNER, OnElliCorner)
	ON_COMMAND(IDM_CIRCLE_THREE_POINT, OnCircleThreePoint)
	ON_COMMAND(IDM_ARC_TWO_POINT, OnArcTwoPoint)
	ON_COMMAND(ID_TRSF_FFD, OnTrsfFfd)
	ON_COMMAND(IDM_SURF_TOBSPLINESURF, OnSurfTobsplinesurf)
	ON_COMMAND(IDM_SOLID_TOBSPLINE, OnSolidTobspline)
	ON_COMMAND(IDM_SURF_CTRLPNT_EDIT, OnSurfCtrlpntEdit)
	ON_COMMAND(IDM_CURVE_BSPLINE_INCDEGREE, OnCurveBsplineIncdegree)
	ON_COMMAND(IDM_CURVE_BEZIER, OnCurveBezier)
	ON_COMMAND(IDM_BEZIER_EDIT_CTRLPNT, OnBezierEditCtrlpnt)
	ON_COMMAND(IDM_BEZIER_INCDEGREE, OnBezierIncdegree)
	ON_COMMAND(IDM_TRSF_AXDF, OnTrsfAxdf)
	ON_COMMAND(IDM_SURF_BSPLINE_INCDEGREE, OnSurfBsplineIncdegree)
	ON_COMMAND(IDM_SURF_BSPLINE_INSERT_CTRLPNT, OnSurfBsplineInsertCtrlpnt)
	ON_COMMAND(IDM_CURVE_BSPLINE_INSERT_CTRLPNT, OnCurveBsplineInsertCtrlpnt)
	ON_COMMAND(IDM_CURVE_FRAME_FRENET, OnCurveFrameFrenet)
	ON_COMMAND(IDM_CURVE_FRAME_ROTATE1, OnCurveFrameRotate1)
	ON_COMMAND(IDM_CURVE_FRAME_ROTATE2, OnCurveFrameRotate2)
	ON_COMMAND(IDM_CURVE_FRAME_SLOAN, OnCurveFrameSloan)
	ON_COMMAND(IDM_CURVE_FRAME_DBLREFLECTION, OnCurveFrameDblreflection)
	ON_COMMAND(IDM_POINT_PRJTO_CURVE, OnPointPrjtoCurve)
	ON_COMMAND(IDM_POINT_PRJTO_SURF, OnPointPrjtoSurf)
	ON_COMMAND(IDM_FFD_FENG_SURFFFD, OnFfdFengSurfffd)
	ON_COMMAND(IDM_CURVE_BSPLINE_INSERT_KNOTS, OnCurveBsplineInsertKnots)
	ON_COMMAND(IDM_SURF_BSPLINE_INSERT_KNOTS, OnSurfBsplineInsertKnots)
	ON_COMMAND(IDM_CURVE_BSPLINE_ADD_CTRLPNTS, OnCurveBsplineAddCtrlpnts)
	ON_COMMAND(IDM_SURF_BSPLINE_ADD_CTRLPNTS, OnSurfBsplineAddCtrlpnts)
	ON_COMMAND(IDM_TEXT_CURVE, OnTextCurve)
	ON_COMMAND(IDM_TEXT_SOLID, OnTextSolid)
	ON_COMMAND(IDM_MESH_IMPORT_MS3D, OnMeshImportMs3d)
	ON_COMMAND(IDM_MESH_IMPORT_3DS, OnMeshImport3ds)
	ON_COMMAND(IDM_MESH_CONVERT_TO_MESH, OnMeshConvertToMesh)
	ON_COMMAND(IDM_MESH_IMPORT_STL, OnMeshImportStl)
	ON_COMMAND(IDM_MESH_IMPORT_BIN_STL, OnMeshImportBinStl)
	ON_COMMAND(IDM_MESH_EXPORT_ASC_STL, OnMeshExportAscStl)
	ON_COMMAND(IDM_MESH_EXPORT_BIN_STL, OnMeshExportBinStl)
	ON_COMMAND(IDM_TRSF_INSERT_CTRLPNTS_TEST, OnTrsfInsertCtrlpntsTest)
	ON_COMMAND(IDM_FFD_RECTFFD, OnFfdRectffd)
	ON_COMMAND(ID_33249, On33249)
	ON_COMMAND(IDM_FFD_FLOW_BY_CURVE, OnFfdFlowByCurve)
	ON_COMMAND(IDM_TEXT_FACE, OnTextFace)
	ON_COMMAND(IDM_FFD_FLOW_ON_SURF, OnFfdFlowOnSurf)
	ON_COMMAND(IDM_FFD_SHEAR, OnFfdShear)
	ON_COMMAND(IDM_FFD_TWIST, OnFfdTwist)
	ON_COMMAND(IDM_ANALY_CURVE_LENGTH, OnAnalyCurveLength)
	ON_COMMAND(IDM_ANALY_FACE_AREA, OnAnalyFaceArea)
	ON_COMMAND(IDM_CURVE_CONCAT_TO_SPLINE, OnCurveConcatToSpline)
	ON_COMMAND(IDM_FFD_BEND, OnFfdBend)
	END_MESSAGE_MAP()


// COccTryDoc 构造/析构

COccTryDoc::COccTryDoc()
{
	// TODO: 在此添加一次性构造代码
//yxk20200104
	//Handle(Graphic3d_WNTGraphicDevice) theGraphicDevice = 
	//	((COccTryApp*)AfxGetApp())->GetGraphicDevice3D();
	Handle(Graphic3d_GraphicDriver)theGraphicDevice =
		((COccTryApp*)AfxGetApp())->GetGraphicDevice3D();
	m_Viewer = new V3d_Viewer(theGraphicDevice);
//endyxk
	m_Viewer->SetDefaultLights();
	m_Viewer->SetLightOn();
	m_AISContext =new AIS_InteractiveContext(m_Viewer);

	//渲染模式.
	m_AISContext->SetDisplayMode(AIS_Shaded,Standard_False);

	//
//	m_AISContext->SetToHilightSelected(Standard_True);

	this->m_scCmdMgr.SetAISContext(this->m_AISContext);

	this->m_pScUcsMgr = new ScUcsMgr();
	this->m_scCmdMgr.SetUcsMgr(this->m_pScUcsMgr);
	this->m_pScUcsMgr->SetAISCtx(m_AISContext);
}

COccTryDoc::~COccTryDoc()
{
	m_AISContext->RemoveAll(Standard_True);
}

void	COccTryDoc::ShowTrihedron(BOOL bShow)
{
	Handle(Geom_Axis2Placement) myTrihedronAxis=new Geom_Axis2Placement(gp::XOY());
	m_hTrihedron=new AIS_Trihedron(myTrihedronAxis);
	m_AISContext->SetTrihedronSize(40, Standard_True);
	m_AISContext->Display(m_hTrihedron, Standard_True);//yxk
}

BOOL COccTryDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}

BOOL COccTryDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	int nType = GetFileType(lpszPathName);
	if(nType == UNKNOWN_FILE)
	{
		AfxMessageBox("未知文件类型",MB_ICONERROR);
		return FALSE;
	}

	BOOL bRet = TRUE;
	switch(nType)
	{
	case BREP_FILE:
		bRet = ReadBRepFile(lpszPathName);
		break;
	case STEP_FILE:
		bRet = ReadStepFile(lpszPathName);
		break;
	case IGES_FILE:
		bRet = ReadIgesFile(lpszPathName);
		break;
	default:
		break;
	}

	return bRet;
}

BOOL COccTryDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类
	this->SaveBRepFile(lpszPathName,FALSE);
	this->SetModifiedFlag(FALSE);

	return TRUE;
	//return CDocument::OnSaveDocument(lpszPathName);//会破坏已经保存的结果。
}

int		COccTryDoc::GetFileType(LPCTSTR lpszFile)
{
	int nType = UNKNOWN_FILE;
	if(!lpszFile)
		return nType;

	CString szFile = lpszFile;
	int nPos = szFile.ReverseFind('.');
	CString szExt = szFile.Right(szFile.GetLength() - nPos - 1);
	if(::stricmp(szExt,"brep") == 0 )
	{
		nType = BREP_FILE;
	}
	else if(::stricmp(szExt,"Step") == 0 ||
		    ::stricmp(szExt,"stp") == 0)
	{
		nType = STEP_FILE;
	}
	else if(::stricmp(szExt,"IGES") == 0 ||
		    ::stricmp(szExt,"igs") == 0)
	{
		nType = IGES_FILE;
	}

	return nType;
}

// COccTryDoc 序列化

void COccTryDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

void COccTryDoc::Fit()
{
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	COccTryView *pView = (COccTryView *) pChild->GetActiveView();
	pView->FitAll();
}

//
void	COccTryDoc::OnCancelEvent()
{
	if(this->m_scCmdMgr.IsCmdRunning())
	{
		this->m_scCmdMgr.StopCommand();
	}
}

void	COccTryDoc::OnDeleteEvent()
{
	this->DeleteSelectObj();
}

void	COccTryDoc::DeleteSelectObj()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	m_AISContext->InitCurrent();
	if(m_AISContext->MoreCurrent())
	{
		//启动命令
		ScCmdDelete *pScCmd = new ScCmdDelete();
		this->m_scCmdMgr.RunCommand(pScCmd);		
	}
}

/////////////////////////////////////////////////////////////////////
//鼠标消息处理函数

//框选方式下的消息响应函数
void	COccTryDoc::DragEvent(const CPoint& point,int nMbState,
										  const Handle(V3d_View)& aView)
{
	static int nPreX = 0;
	static int nPreY = 0;

	if(nMbState == MB_DOWN)
	{
		nPreX = point.x;
		nPreY = point.y;
	}

	if(nMbState == MB_MOVE)
	{
		m_AISContext->Select(nPreX,nPreY,point.x,point.y,aView, Standard_True);//yxk
	}
}

//按下shift下的框选方式
void	COccTryDoc::ShiftDragEvent(const CPoint& point,int nMbState,
												   const Handle(V3d_View)& aView)
{
	static int nPreX = 0;
	static int nPreY = 0;

	if(nMbState == MB_DOWN)
	{
		nPreX = point.x;
		nPreY = point.y;
	}

	if(nMbState == MB_MOVE)
	{
		m_AISContext->ShiftSelect(nPreX,nPreY,point.x,point.y,aView, Standard_True);
	}
}

//点选方式下的消息响应函数
void	COccTryDoc::InputEvent(const CPoint& point,int nState,
											   const Handle(V3d_View)& aView)
{
	if(this->m_scCmdMgr.IsCmdRunning())
	{
		this->m_scCmdMgr.InputEvent(point,nState,aView);
	}
	else
	{
		if(nState == MINPUT_LEFT)
		{
			m_AISContext->Select(Standard_True);
			//m_AISContext->Select(Standard_False);
			//m_AISContext->UnhilightSelected();
			//
			ShowObjInfo();
		}
	}
}

//显示对象信息
void	COccTryDoc::ShowObjInfo()
{
	CString szProp = "";
	m_AISContext->InitSelected();
	if(m_AISContext->MoreSelected())
	{
		TopoDS_Shape aS = m_AISContext->SelectedShape();
		if(!aS.IsNull())
		{

			//类型
			szProp = ScTools::ShapeTypeName(aS.ShapeType());
			szProp += " Shape,BoundBox :";

			//包围盒大小
			Bnd_Box B;
			BRepBndLib::Add(aS,B);
			CString szV;
			double x1,y1,z1,x2,y2,z2;
			B.Get(x1,y1,z1,x2,y2,z2);
			szV.Format("[%.4f,%.4f,%.4f] - [%.4f,%.4f,%.4f].",x1,y1,z1,x2,y2,z2);
			szProp += szV;
		}
	}
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if(pFrm)
	{
		pFrm->ShowProp(szProp);
	}
}

//按下shift键下的点选方式
void	COccTryDoc::ShiftInputEvent(const CPoint& point,
													const Handle(V3d_View)& aView)
{
	if(this->m_scCmdMgr.IsCmdRunning())
	{
		this->m_scCmdMgr.ShiftInputEvent(point,aView);
	}
	else
	{
		m_AISContext->ShiftSelect(Standard_True);
	}
}

//鼠标移动的响应函数
void	COccTryDoc::MoveEvent(const CPoint& point,
											  const Handle(V3d_View)& aView)
{
	if(this->m_scCmdMgr.IsCmdRunning())
	{
		this->m_scCmdMgr.MoveEvent(point,aView);
	}
	else
	{
		//遍历一下端点
		m_AISContext->MoveTo(point.x,point.y,aView, Standard_True);
	}
}

//按下shift键下的鼠标移动的响应函数
void	COccTryDoc::ShiftMoveEvent(const CPoint& point,
												   const Handle(V3d_View)& aView)
{
	if(this->m_scCmdMgr.IsCmdRunning())
	{
		this->m_scCmdMgr.ShiftInputEvent(point,aView);
	}
	else
	{
		m_AISContext->MoveTo(point.x,point.y,aView, Standard_True);
	}
	//
	////m_AISContext->OpenLocalContext();//yxk20191207
	//m_AISContext->ActivateStandardMode(TopAbs_VERTEX);
	//m_AISContext->ShiftSelect(point.x - 20,point.y - 20,point.x + 20,point.y + 20,aView);
	////m_AISContext->CloseLocalContex();//yxk20191207

//	m_AISContext->MoveTo(point.x,point.y,aView);	
}



// COccTryDoc 诊断

#ifdef _DEBUG
void COccTryDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COccTryDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// COccTryDoc 命令

void COccTryDoc::OnBrepprimSphere()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSphere *pScCmd = new ScCmdSphere();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnBrepprimSphereAngle()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnBrepprimCyl()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdCyl *pScCmd = new ScCmdCyl(FALSE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnBrepprimCylAngle()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdCyl *pScCmd = new ScCmdCyl(TRUE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnBrepprimCone()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnBrepprimConeTopplane()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnBrepprimBox()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdBox *pScCmd = new ScCmdBox();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnBrepprimTorus()
{
	// TODO: 在此添加命令处理程序代码
}

//////////////////////////////////////////////////////////////
// 曲线创建命令
void COccTryDoc::OnMenuLine()
{
	// TODO: 在此添加命令处理程序代码
	if(this->m_scCmdMgr.IsCmdRunning())
		return;
//yxk杨绪坤的测试，绘制10000条直线
	//gp_Pnt pnt1(0.,0.,0.), pnt2(0.0,100.0,0);
	//for (int i = 0; i < 10000; i++) {
	//	pnt1.SetX(pnt1.X() + 1.0);
	//	pnt2.SetX(pnt1.X() + 1.0);
	//	BRepBuilderAPI_MakeEdge me(pnt1, pnt2);
	//	if (me.IsDone())
	//	{
	//		Handle(AIS_Shape) aAIShape = new AIS_Shape(me);
	//		m_AISContext->Display(aAIShape, Standard_False);
	//	}

	//}
	//m_AISContext->UpdateCurrentViewer();
	//return;
//endyxk
	ScCmdLine *pScCmd = new ScCmdLine();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuPolyline()
{
	// TODO: 在此添加命令处理程序代码
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPolyLine *pScCmd = new ScCmdPolyLine();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuCircle()
{
	// TODO: 在此添加命令处理程序代码
	if(this->m_scCmdMgr.IsCmdRunning())
		return;
//yxk杨绪坤的测试，绘制10000条直线
	gp_Pnt ptCenter(0.,0.,0.);
	gp_Dir zDir(0, 0, 1);
	gp_Ax2 ax(ptCenter, zDir);
	double dRadius = 100.;
	for (int i = 0; i < 10000; i++) {
		ptCenter.SetZ(ptCenter.Z() + 1.0);
		ax.SetLocation(ptCenter);
		gp_Circ circ(ax, dRadius);
		BRepBuilderAPI_MakeEdge me(circ);
		if (me.IsDone())
		{
			Handle(AIS_Shape) aAIShape = new AIS_Shape(me);
			m_AISContext->Display(aAIShape, Standard_False);
		}

	}
	m_AISContext->UpdateCurrentViewer();
	return;

//endyxk

	ScCmdCircle *pScCmd = new ScCmdCircle();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuArc()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdArc *pScCmd = new ScCmdArc();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSpline()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSpline *pScCmd = new ScCmdSpline();
	pScCmd->SetSplType(ScCmdSpline::BSpline_Interp);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSplineApp()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSpline *pScCmd = new ScCmdSpline();
	pScCmd->SetSplType(ScCmdSpline::BSpline_Approx);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuPolygon()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuPreWorkplane()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdUcsPrev *pScCmd = new ScCmdUcsPrev();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuNextWorkplane()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdUcsNext *pScCmd = new ScCmdUcsNext();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuModelspace()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdUcsModel *pScCmd = new ScCmdUcsModel();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuChangeOrg()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdUcsOrg *pScCmd = new ScCmdUcsOrg();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuChangeHeight()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdUcsHeight *pScCmd = new ScCmdUcsHeight();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuChangeRotate()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuChange3point()
{
	// TODO: 在此添加命令处理程序代码
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdUcs3Point *pScCmd = new ScCmdUcs3Point();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuChangeView()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuChangeCustom()
{
	// TODO: 在此添加命令处理程序代码
}



void COccTryDoc::OnMenuClosedPolyline()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdClosedPolyLine *pScCmd = new ScCmdClosedPolyLine();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//拉伸
void COccTryDoc::OnMenuPrism()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPrism *pScCmd = new ScCmdPrism(FALSE,FALSE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//双向拉伸
void COccTryDoc::OnMenuPrismTwodir()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPrism *pScCmd = new ScCmdPrism(TRUE,FALSE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}


void COccTryDoc::OnMenuRevol()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdRevol *pScCmd = new ScCmdRevol(TRUE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuThickshell()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdThickSolid *pScCmd = new ScCmdThickSolid();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnFilletConr()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdFillet *pScCmd = new ScCmdFillet(TRUE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnFilletVarRadius()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdFillet *pScCmd = new ScCmdFillet(FALSE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnChamferConstRadius()
{
	// TODO: 在此添加命令处理程序代码
}

///////////////////////////////////////////////////////////////////////////////////////////////
// BOOL运算

void COccTryDoc::OnMenuBoolCommon()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdBoolCommon *pScCmd = new ScCmdBoolCommon();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuBoolFuse()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdBoolFuse *pScCmd = new ScCmdBoolFuse();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuBoolCut()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdBoolCut *pScCmd = new ScCmdBoolCut();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//旧算法

void COccTryDoc::OnOldboolCommon()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdBoolCommonOld *pScCmd = new ScCmdBoolCommonOld();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnOldboolFuse()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdBoolFuseOld *pScCmd = new ScCmdBoolFuseOld();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnOldboolCut()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdBoolCutOld *pScCmd = new ScCmdBoolCutOld();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnBoolCommonSections()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdBoolCommonSections *pScCmd = new ScCmdBoolCommonSections();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnBoolCommonShape1Objs()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdBoolCommonGenObjs *pScCmd = new ScCmdBoolCommonGenObjs(TRUE);
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnBoolCommonShape2Objs()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdBoolCommonGenObjs *pScCmd = new ScCmdBoolCommonGenObjs(FALSE);
		this->m_scCmdMgr.RunCommand(pScCmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void COccTryDoc::OnMenuPointCloud()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuBlendSurf()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPlateBlend *pScCmd = new ScCmdPlateBlend(FALSE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuBlendSurfPoint()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuBlendSurfCurve()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPlateBlend *pScCmd = new ScCmdPlateBlend(TRUE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}


void COccTryDoc::OnMenuNpatchFill()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPlateFillHole *pScCmd = new ScCmdPlateFillHole();
	this->m_scCmdMgr.RunCommand(pScCmd);
}


////////////////////////////////////////////////////////////////
// 导入外部文件
void COccTryDoc::OnMenuImportBrep()
{
	CFileDialog dlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"BREP Files (*.brep , *.rle)|*.brep;  *.BREP; *.rle; *.RLE; |All Files (*.*)|*.*||", 
		NULL ); 

	if (dlg.DoModal() == IDOK) 
	{
		CString filename = dlg.GetPathName();
		if(ReadBRepFile(filename))
		{
			this->SetPathName(filename);
			this->SetTitle(filename);
		}
		
	}
}

void COccTryDoc::OnMenuImportStep()
{
	CFileDialog dlg(TRUE,
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  "STEP Files (*.stp;*.step)|*.stp; *.step|All Files (*.*)|*.*||", 
                  NULL );

	if (dlg.DoModal() == IDOK) 
	{
		CString C = dlg.GetPathName();
		if(ReadStepFile(C))
		{
			this->SetPathName(C);
			this->SetTitle(C);
		}
	}
}

void COccTryDoc::OnMenuImportIges()
{
	CFileDialog dlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"IGES Files (*.iges , *.igs)|*.iges; *.igs|All Files (*.*)|*.*||", 
		NULL );

	
	if (dlg.DoModal() == IDOK) 
	{
		CString C = dlg.GetPathName();
		if(ReadIgesFile(C))
		{
			this->SetPathName(C);
			this->SetTitle(C);
		}
	}
}

BOOL	COccTryDoc::ReadBRepFile(LPCTSTR lpszFile)
{
	Standard_CString aFileName = (Standard_CString)(LPCTSTR)lpszFile;

	TopoDS_Shape aShape;
	BRep_Builder aBuilder;
	Standard_Boolean result = BRepTools::Read(aShape,aFileName,aBuilder);
	if(!result)
	{
		return FALSE;
	}

	//如果是compound，分离成一个一个的显示
	if(aShape.ShapeType() == TopAbs_COMPOUND)
	{
		TopoDS_Iterator ite(aShape);
		for(;ite.More();ite.Next())
		{
			TopoDS_Shape aS = ite.Value();
			if(!aS.IsNull())
			{
				Handle(AIS_Shape) aObj = new AIS_Shape(aS);
				m_AISContext->Display(aObj,Standard_True);//yxk
			}
		}
	}
	else
	{
		Handle(AIS_Shape) aObj = new AIS_Shape(aShape);
		m_AISContext->Display(aObj, Standard_True);//yxk
	}

	Fit();
	return TRUE;
}

static Handle(TopTools_HSequenceOfShape) BuildSequenceFromContext(
	const Handle(AIS_InteractiveContext)& anInteractiveContext,BOOL bOnlySelected) 
{
    Handle(TopTools_HSequenceOfShape) aSequence = new TopTools_HSequenceOfShape();
    Handle(AIS_InteractiveObject) picked;

	if(bOnlySelected)
	{
		for(anInteractiveContext->InitCurrent();anInteractiveContext->MoreCurrent();anInteractiveContext->NextCurrent())
		{
			picked = anInteractiveContext->Current();
			if (anInteractiveContext->Current()->IsKind(STANDARD_TYPE(AIS_Shape)))
			{
				TopoDS_Shape aShape = Handle(AIS_Shape)::DownCast(picked)->Shape();
				aSequence->Append(aShape);
			}
		}
	}
	else
	{
		AIS_ListOfInteractive aList;
		anInteractiveContext->DisplayedObjects(aList/*,Standard_True*/);//yxk
		AIS_ListIteratorOfListOfInteractive aListIterator;
		for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next())
		{
			Handle(AIS_InteractiveObject) aObj = aListIterator.Value();
			if(aObj->IsKind(STANDARD_TYPE(AIS_Shape)))
			{
				TopoDS_Shape aShape = Handle(AIS_Shape)::DownCast(aObj)->Shape();
				aSequence->Append(aShape);
			}
		}
	}
     
	return aSequence;
}

BOOL	COccTryDoc::SaveBRepFile(LPCTSTR lpszFile,BOOL bOnlySelected)
{
	BOOL bRet = TRUE;
	Standard_CString aFileName = (Standard_CString)(LPCTSTR)lpszFile;

	Handle(TopTools_HSequenceOfShape) aHSequenceOfShape;
	aHSequenceOfShape = BuildSequenceFromContext(m_AISContext,bOnlySelected);
	int aLength = aHSequenceOfShape->Length();
	if (aLength == 1)
	{
		TopoDS_Shape RES = aHSequenceOfShape->Value(1);
		bRet = BRepTools::Write(RES,aFileName);
	} 
	else 
	{
		TopoDS_Compound RES;
		BRep_Builder MKCP;
		MKCP.MakeCompound(RES);
		for (Standard_Integer i=1;i<=aLength;i++)
		{
			TopoDS_Shape aShape= aHSequenceOfShape->Value(i);
			if ( aShape.IsNull() ) 
			{
				continue;
			}
			MKCP.Add(RES, aShape);
		}
		bRet = BRepTools::Write(RES,aFileName);
	}
	return bRet;
}

BOOL	COccTryDoc::ReadStepFile(LPCTSTR lpszFile)
{
	Standard_CString aFileName = (Standard_CString)(LPCTSTR)lpszFile;

	STEPControl_Reader aReader;
	IFSelect_ReturnStatus status = aReader.ReadFile(aFileName);
	if (status != IFSelect_RetDone)
	{
		AfxMessageBox("读文件失败。");
		return FALSE;
	}
//yxk20200104
	//Standard_CString LogFileName = "ReadStepFile.log";
	//Interface_TraceFile::SetDefault(2,LogFileName,Standard_True);
//endyxk
	Standard_Boolean failsonly = Standard_False;
	aReader.PrintCheckLoad(failsonly, IFSelect_ItemsByEntity);

	// Root transfers
	Standard_Integer nbr = aReader.NbRootsForTransfer();
	aReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);
	for ( Standard_Integer n = 1; n<=nbr; n++) {
		Standard_Boolean ok = aReader.TransferRoot(n);
	}

	// Collecting resulting entities
	Standard_Integer nbs = aReader.NbShapes();
	if (nbs == 0) {
		return FALSE;
	}

	Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = new TopTools_HSequenceOfShape();
	for (Standard_Integer i=1; i<=nbs; i++) {
		aHSequenceOfShape->Append(aReader.Shape(i));
	}

	//将所有shape进行转换，并显示
	for(int ix = 1;ix <= aHSequenceOfShape->Length();ix++)
	{
		TopoDS_Shape aShape = aHSequenceOfShape->Value(ix);
		Handle(AIS_Shape) aObj = new AIS_Shape(aShape);
		m_AISContext->Display(aObj, Standard_True);//yxk
	}

	Fit();
	return TRUE;
}

BOOL	COccTryDoc::SaveStepFile(LPCTSTR lpszFile,BOOL bOnlySelected)
{
	BOOL bRet = TRUE;
	Standard_CString aFileName = (Standard_CString)(LPCTSTR)lpszFile;

	Handle(TopTools_HSequenceOfShape) aHSequenceOfShape;
	aHSequenceOfShape = BuildSequenceFromContext(m_AISContext,bOnlySelected);
	int aLength = aHSequenceOfShape->Length();
	if(aLength <= 0)
		return TRUE;

	STEPControl_Writer aWriter;
	STEPControl_StepModelType aValue = STEPControl_AsIs;

	IFSelect_ReturnStatus status;
	for (Standard_Integer i=1;i<=aHSequenceOfShape->Length();i++)  
	{
		status =  aWriter.Transfer(aHSequenceOfShape->Value(i), aValue);
		if ( status != IFSelect_RetDone ) return FALSE;
	}     
	status = aWriter.Write(aFileName);

	return (status == IFSelect_RetDone);
}

BOOL	COccTryDoc::ExportStepFile()
{
	CFileDialog dlg(FALSE,_T("*.step"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	              "STEP Files (*.step )|*.step;|STEP Files (*.stp )| *.stp;||", NULL ); 

	BOOL bRet = TRUE;
	if (dlg.DoModal() == IDOK)  
	{ 
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT)); 
		CString C = dlg.GetPathName(); 
		bRet = SaveStepFile(C,FALSE);
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
	return bRet;
}

BOOL	COccTryDoc::ReadIgesFile(LPCTSTR lpszFile)
{
	ASSERT(lpszFile);

	Standard_CString aFileName = (Standard_CString)(LPCTSTR)lpszFile;
	IGESControl_Reader Reader;

	Standard_Integer status = Reader.ReadFile(aFileName);
	if (status != IFSelect_RetDone)
	{
		return FALSE;
	}

	Reader.TransferRoots();
	for(int ix = 1;ix <= Reader.NbShapes();ix++)
	{
		TopoDS_Shape aShape = Reader.Shape(ix);
		Handle(AIS_Shape) aObj = new AIS_Shape(aShape);
		m_AISContext->Display(aObj, Standard_True);//yxk
	}

	Fit();
	return TRUE;
}



void COccTryDoc::OnMenuExportStepfile()
{
	if(!ExportStepFile())
		AfxMessageBox("导出失败.");
}

void COccTryDoc::OnMenuExportIgesfile()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuSurfLoft()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdLoft *pScCmd = new ScCmdLoft(FALSE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSurfPipe()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSurfPipe *pScCmd = new ScCmdSurfPipe();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSurfOffset()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdOffsetShape *pScCmd = new ScCmdOffsetShape(TRUE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSolidLoft()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdLoft *pScCmd = new ScCmdLoft(TRUE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSolidPipe()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuSolidOffset()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdOffsetShape *pScCmd = new ScCmdOffsetShape(FALSE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuCurveSewing()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdCurveSewing *pScCmd = new ScCmdCurveSewing();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//
// 曲线到曲面的投影
//
void COccTryDoc::OnMenuCsproj()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuSweep()
{
	
}

void COccTryDoc::OnMenuSewsurf()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSurfSewing *pScCmd = new ScCmdSurfSewing();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//曲线沿法线投影到实体上
void COccTryDoc::OnMenuCbrepProj()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdBrepProjection *pScCmd = new ScCmdBrepProjection();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuExttopoint()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuBsplFillsurf()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSurfBSplineCurves *pScCmd = new ScCmdSurfBSplineCurves();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuBrepSweep()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdBRepSweep *pScCmd = new ScCmdBRepSweep();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuFitall()
{
	Fit();
}

void COccTryDoc::OnMenuDelete()
{
	this->DeleteSelectObj();
}

/////////////////////////////////////////////////////////////////
// 以下四个曲面拼接。
void COccTryDoc::OnMenuSurfFilling()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSurfFilling *pScCmd = new ScCmdSurfFilling(ScCmdSurfFilling::eBoundary);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSurfFillintPoint()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSurfFilling *pScCmd = new ScCmdSurfFilling(ScCmdSurfFilling::eWithPoints);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSurfFillingCurve()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSurfFilling *pScCmd = new ScCmdSurfFilling(ScCmdSurfFilling::eWithCurves);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSurfFillingFace()
{
	// TODO: 在此添加命令处理程序代码
}

//////////////////////////////////////////////////////////////////////

void COccTryDoc::OnMenuSurfClosecurve()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSurfPlanClosedCurves *pScCmd = new ScCmdSurfPlanClosedCurves();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//交线
void COccTryDoc::OnMenuCurveInter()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdInterCurve *pScCmd = new ScCmdInterCurve();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSurfCut()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSurfCut *pScCmd = new ScCmdSurfCut();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//shell、solid炸开成面
void COccTryDoc::OnMenuExtraFace()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdExtraFaces *pScCmd = new ScCmdExtraFaces();
	this->m_scCmdMgr.RunCommand(pScCmd);
}



void COccTryDoc::OnMenuUcsCurvePoint()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdUcsCurvePoint *pScCmd = new ScCmdUcsCurvePoint();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuUcsFacePoint()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdUcsFacePoint *pScCmd = new ScCmdUcsFacePoint();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuUcsObjPoint()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdUcsShapePoint *pScCmd = new ScCmdUcsShapePoint();
	this->m_scCmdMgr.RunCommand(pScCmd);
}




////////////////////////////////////////////////////////
// 平面生成
void COccTryDoc::OnMenuPlaneTwopoint()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPlane *pScCmd = new ScCmdPlane(ScCmdPlane::eTwoPoint);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuPlaneThreepoint()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPlane *pScCmd = new ScCmdPlane(ScCmdPlane::eThreePoint);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuPlaneNormal()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuPlaneCurvepoint()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuPlaneFacepoint()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuPlaneShapepoint()
{
	// TODO: 在此添加命令处理程序代码
}
///////////////////////////////////////////

/////////////////////////////////////////
// 点生成

//空间点
void COccTryDoc::OnMenuPoint()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPoint *pScCmd = new ScCmdPoint(FALSE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//平面点
void COccTryDoc::OnMenuPlanePoint()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPoint *pScCmd = new ScCmdPoint(TRUE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//两条曲线的交点
void COccTryDoc::OnMenuPointInter()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdCurveInterPoint *pScCmd = new ScCmdCurveInterPoint;
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuPointVertex()
{
	// TODO: 在此添加命令处理程序代码
}

////////////////////////////////////////////////
void COccTryDoc::OnMenuShapeStruct()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	m_AISContext->InitCurrent();
	if(m_AISContext->MoreCurrent())
	{
		Handle(AIS_Shape) aShapeObj = Handle(AIS_Shape)::DownCast(
			m_AISContext->Current());
		if(aShapeObj.IsNull())
			return;

		TopoDS_Shape aShape = aShapeObj->Shape();

		CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
		COccTryView *pView = (COccTryView *) pChild->GetActiveView();
		if(pView)
		{
			pView->ShowShapeStruct(aShape);
		}
	}
}

////////////////////////////////////////////////////////////
// 变换的处理。
//
void COccTryDoc::OnMenuMoving()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdObjMoving *pScCmd = new ScCmdObjMoving();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuRotate()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuScale()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuMirror()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMenuObjClone()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdObjClone *pScCmd = new ScCmdObjClone(FALSE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuObjMultiClone()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdObjClone *pScCmd = new ScCmdObjClone(TRUE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSolidCut()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSolidCut *pScCmd = new ScCmdSolidCut();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

////////////////////////////////////////////////////////////////
// 显示功能

void COccTryDoc::OnDispWireframe()
{
	for(m_AISContext->InitCurrent();m_AISContext->MoreCurrent();m_AISContext->NextCurrent())
	{
		m_AISContext->SetDisplayMode(m_AISContext->Current(),0,Standard_True);//yxk
	}
}

void COccTryDoc::OnUpdateDispWireframe(CCmdUI *pCmdUI)
{
	BOOL bInShading = FALSE;
	for(m_AISContext->InitCurrent();m_AISContext->MoreCurrent();m_AISContext->NextCurrent())
	{
		if(m_AISContext->IsDisplayed(m_AISContext->Current(),1))
			bInShading = TRUE;
	}

	pCmdUI->Enable(bInShading);
}

void COccTryDoc::OnDispShade()
{
	for(m_AISContext->InitCurrent();m_AISContext->MoreCurrent();m_AISContext->NextCurrent())
	{
		m_AISContext->SetDisplayMode(m_AISContext->Current(),1,Standard_True);//yxk
	}
}

void COccTryDoc::OnUpdateDispShade(CCmdUI *pCmdUI)
{
	BOOL bInWF = FALSE;
	for(m_AISContext->InitCurrent();m_AISContext->MoreCurrent();m_AISContext->NextCurrent())
	{
		if(m_AISContext->IsDisplayed(m_AISContext->Current(),0))
			bInWF = TRUE;
	}

	pCmdUI->Enable(bInWF);
}

//
// 纹理
//
void COccTryDoc::OnDispTexture()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdTexture *pScCmd = new ScCmdTexture();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnUpdateDispTexture(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}

void COccTryDoc::OnDispColor()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnUpdateDispColor(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}

void COccTryDoc::OnDispMaterial()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnUpdateDispMaterial(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}

void COccTryDoc::OnDispTransparency()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnDispCutplane()
{
	// TODO: 在此添加命令处理程序代码
}

//
// 显示对象的网格，必须是face以上的对象。
//
void COccTryDoc::OnDispMesh()
{
	for(m_AISContext->InitCurrent();m_AISContext->MoreCurrent();m_AISContext->NextCurrent())
	{
		Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(m_AISContext->Current());
		if(aObj.IsNull())
			continue;
		TopoDS_Shape aS = aObj->Shape();
		if(aS.ShapeType() == TopAbs_VERTEX || 
			aS.ShapeType() == TopAbs_EDGE ||
			aS.ShapeType() == TopAbs_WIRE)
			continue;

		TopoDS_Shape aMeshS = ScTools::BuildShapeMesh(aS,0.2);
		if(!aMeshS.IsNull())
		{
			Handle(AIS_Shape) aMObj = new AIS_Shape(aMeshS);
			m_AISContext->Display(aMObj, Standard_True);//yxk
		}
	}
}



void COccTryDoc::OnCurveExtendToPoint()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdExtendToPoint *pScCmd = new ScCmdExtendToPoint();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//
// 从点生成面。
//
void COccTryDoc::OnSurfPointcloud()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPlatePointCloud *pScCmd = new ScCmdPlatePointCloud();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveOffsetCurve()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdCurveOffset *pScCmd = new ScCmdCurveOffset();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnSurfOffsetSurf()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdOffsetShape *pScCmd = new ScCmdOffsetShape(TRUE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnMenuSurfPrism()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPrism *pScCmd = new ScCmdPrism(FALSE,TRUE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnSurfPrismTwodir()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdPrism *pScCmd = new ScCmdPrism(TRUE,TRUE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnSurfExtentBylen()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSurfExtent *pScCmd = new ScCmdSurfExtent();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnSurfRevol()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdRevol *pScCmd = new ScCmdRevol(FALSE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnSurfSweepCurve()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdSweepOnePath *pScCmd = new ScCmdSweepOnePath();
	this->m_scCmdMgr.RunCommand(pScCmd);
	
}

//
// 显示曲线的曲率圆
//
void COccTryDoc::OnToolCurveCurvature()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdCurveCurvatureCirc *pScCmd = new ScCmdCurveCurvatureCirc();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//
// 从曲面的流线生成曲线
//
void COccTryDoc::OnCurveIsocurve()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdIsoCurve *pScCmd = new ScCmdIsoCurve();
	this->m_scCmdMgr.RunCommand(pScCmd);
}


// 组合命令
//
void COccTryDoc::OnSolidCompound()
{
	// TODO: 在此添加命令处理程序代码
}

// 取消组合命令
//
void COccTryDoc::OnSolidUncompound()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdDeCompound *pScCmd = new ScCmdDeCompound();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveSurfBoundary()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdCurveFaceBoundary *pScCmd = new ScCmdCurveFaceBoundary();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//face的有参数曲线生成的曲线。
void COccTryDoc::OnCurve2dallbnd()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdFaceBoundary2d *pScCmd = new ScCmdFaceBoundary2d();
	this->m_scCmdMgr.RunCommand(pScCmd);
	
}

//face的有3d曲线生成的曲线。
void COccTryDoc::OnCurve3dallbnd()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdFaceBoundary3d *pScCmd = new ScCmdFaceBoundary3d();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//曲面法矢
void COccTryDoc::OnToolFacenorm()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdFaceNormal *pScCmd = new ScCmdFaceNormal();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//曲线的切矢
void COccTryDoc::OnToolCurveTang()
{
	
}

//曲面的裁剪环信息
void COccTryDoc::OnToolSurfTrimloop()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdFaceTrmLoopInfo *pScCmd = new ScCmdFaceTrmLoopInfo();
	this->m_scCmdMgr.RunCommand(pScCmd);
	
}

CString		GetFolder(HWND hWnd)
{
	CString strResult =""; 
    LPMALLOC lpMalloc;  // pointer to IMalloc 
    if (::SHGetMalloc(&lpMalloc) != NOERROR) 
	{ 
		AfxMessageBox("Path operation error!"); 
		return ""; 
	} 
    char szDisplayName[_MAX_PATH]; 
    char szBuffer[_MAX_PATH]; 
    BROWSEINFO browseInfo; 
    browseInfo.hwndOwner = hWnd; 
    // set root at Desktop 
    browseInfo.pidlRoot = NULL; 
    browseInfo.pszDisplayName = szDisplayName; 
    browseInfo.lpszTitle = "请选择目录";  // Dialog title 
    browseInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS; 
    browseInfo.lpfn = NULL;      // not used 
    browseInfo.lParam = 0;      // not used 
    LPITEMIDLIST lpItemIDList; 
    if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) 
        != NULL) 
    { 
        // Get the path of the selected folder from the    item ID list. 
        if (::SHGetPathFromIDList(lpItemIDList, szBuffer)) 
        { 
            // At this point, szBuffer contains the path the user chose. 
            if (szBuffer[0] == '\0') 
            { 
                // SHGetPathFromIDList failed, or SHBrowseForFolder failed. 
                AfxMessageBox("Fail to get directory!", 
                    MB_ICONSTOP|MB_OK); 
                return ""; 
            } 
            // We have a path in szBuffer! Return it. 
            strResult = szBuffer; 
        } 
        else 
        { 
            // The thing referred to by lpItemIDList 
            // might not have been a file system object. 
            // For whatever reason, SHGetPathFromIDList didn't work! 
            AfxMessageBox("Fail to get directory!", 
                MB_ICONSTOP|MB_OK); 
            return ""; 
        } 
        lpMalloc->Free(lpItemIDList); 
        lpMalloc->Release(); 
    } 

	return strResult;
}

//
// 根据不同的level，生成不同的文件的信息，并写入到文件。
//
static void  StepFileTrmLoopInfo(LPCTSTR lpszFile,CStdioFile& sf,int nLevel)
{
	CString szInfo;

	STEPControl_Reader aReader;
	IFSelect_ReturnStatus status = aReader.ReadFile((Standard_CString)lpszFile);
	if (status != IFSelect_RetDone)
	{
		szInfo.Format("Read step file %s failed.\n",lpszFile);
		sf.WriteString(szInfo);
		return;
	}

	szInfo.Format("[STEP FILE]:%s\n",lpszFile);
	sf.WriteString(szInfo);

	// Root transfers
	Standard_Integer nbr = aReader.NbRootsForTransfer();
	for ( Standard_Integer n = 1; n<=nbr; n++) {
		Standard_Boolean ok = aReader.TransferRoot(n);
	}

	Standard_Integer nbs = aReader.NbShapes();
	for (Standard_Integer i=1; i<=nbs; i++) 
	{
		TopoDS_Shape aS = aReader.Shape(i);
		TopExp_Explorer ex;
		for(ex.Init(aS,TopAbs_FACE);ex.More();ex.Next())
		{
			TopoDS_Face aFace = TopoDS::Face(ex.Current());
			//
			CStringList list;
			int iret = ScTools::FaceTrmLoopInfo(aFace,list,nLevel);
			//记录信息到文件中
			if(nLevel == 0 || ((nLevel > 0) && !iret) || 
				((nLevel == 2) && (iret >= FACE_TRMLOOP_FATAL_ERR)))
			{
				POSITION pos = list.GetHeadPosition();
				while(pos)
				{
					CString szLine = list.GetNext(pos);
					szLine += "\n";
					sf.WriteString(szLine);
				}
			}

			//是否弥合gap
			if((nLevel > 0) && (iret >= FACE_TRMLOOP_FATAL_ERR))
			{
				//弥合gap
				Handle(ShapeFix_Wireframe) fixwf = new ShapeFix_Wireframe(aFace);
				fixwf->SetPrecision(0.001);
				fixwf->FixWireGaps();
				TopoDS_Shape aR = fixwf->Shape();

				if(aR.ShapeType() != TopAbs_FACE)
				{
					sf.WriteString("----------fixwiregaps result is not a face----\n");
				}
				else
				{
					sf.WriteString("\n----------after fixwiregaps----------\n");
					CStringList list;
					iret = ScTools::FaceTrmLoopInfo(TopoDS::Face(aR),list,0);
					if(iret != 0)
					{
						POSITION pos = list.GetHeadPosition();
						while(pos)
						{
							CString szLine = list.GetNext(pos);
							szLine += "\n";
							sf.WriteString(szLine);
						}
					}
					sf.WriteString("----------after fixwiregaps end----------\n\n");
				}
			}
		}
	}
}

//
// 指定路径下的所有step文件的分析信息
//
void COccTryDoc::OnToolSteptrimloops()
{
	CString szPath = GetFolder(AfxGetMainWnd()->GetSafeHwnd());
	if(szPath.IsEmpty())
		return;

	//保存文件名称和路径
	CFileDialog dlg(FALSE,_T("*.txt"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	              "Text Files (*.txt )|*.txt;||", NULL ); 

	if(dlg.DoModal())
	{
		CString szSavFile = dlg.GetFileName();
		CStdioFile sf;

		if(!sf.Open(szSavFile,CFile::modeCreate | CFile::modeWrite))
		{
			AfxMessageBox("Open filefailed.");
			return;
		}

		CWaitCursor wait;

		CFileFind ff;
		if(szPath.Right(1) != "\\")
			szPath += "\\";
		CString szFlt = szPath + "\\*.stp";
		BOOL bFound = ff.FindFile(szFlt);
		while(bFound)
		{
			bFound = ff.FindNextFile();

			CString szStepFile = ff.GetFilePath();
			DTRACE("\n step file: %s",szStepFile);
			StepFileTrmLoopInfo(szStepFile,sf,1);
		}

		ff.Close();

		szFlt = szPath + "\\*.step";
		bFound = ff.FindFile(szFlt);
		while(bFound)
		{
			bFound = ff.FindNextFile();

			CString szStepFile = ff.GetFilePath();
			DTRACE("\n step file: %s",szStepFile);
			StepFileTrmLoopInfo(szStepFile,sf,1);
		}

		ff.Close();

		sf.Close();

		//打开文件
		ShellExecute(NULL, "open", szSavFile, NULL, NULL, SW_SHOWNORMAL);
	}
}

static void  OnTrmLoopInfo(int nLevel)
{
	CFileDialog dlg(TRUE,
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  "STEP Files (*.stp;*.step)|*.stp; *.step|All Files (*.*)|*.*||", 
                  NULL );

	if (dlg.DoModal() != IDOK) 
		return;

	CString szStepFile = dlg.GetFileName();

	//保存文件名称和路径
	CFileDialog savdlg(FALSE,_T("*.txt"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	              "Text Files (*.txt )|*.txt;||", NULL ); 

	if(savdlg.DoModal())
	{
		CString szSavFile = savdlg.GetFileName();
		CStdioFile sf;

		if(!sf.Open(szSavFile,CFile::modeCreate | CFile::modeWrite))
		{
			AfxMessageBox("Open filefailed.");
			return;
		}

		StepFileTrmLoopInfo(szStepFile,sf,nLevel);

		sf.Close();
	}
}

void COccTryDoc::OnToolStepfileTrmloopinfo()
{
	OnTrmLoopInfo(0);
}

void COccTryDoc::OnToolTrmloopDetail()
{
	OnTrmLoopInfo(0);
}

void COccTryDoc::OnToolTrmloopErr()
{
	OnTrmLoopInfo(1);
}

void COccTryDoc::OnToolTrmloopFatal()
{
	OnTrmLoopInfo(2);
}

void COccTryDoc::OnToolTrmloopErrface()
{
	
}

void COccTryDoc::OnToolTrmloopFatalface()
{
	
}

void COccTryDoc::OnToolCurve2d()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdToolCurve2d *pScCmd = new ScCmdToolCurve2d();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnSurfRawsurf()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdRawSurface *pScCmd = new ScCmdRawSurface();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//曲线的切矢显示
void COccTryDoc::OnToolCurveSingTang()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdCurveTang *pScCmd = new ScCmdCurveTang();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

//曲面边界的切矢显示.
void COccTryDoc::OnToolFacebndTang()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdFaceBndTang *pScCmd = new ScCmdFaceBndTang();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//
// 从实体等中抽取出一个面。
//
void COccTryDoc::OnSurfExtractFace()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdExtractFace *pScCmd = new ScCmdExtractFace();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//
// 显示face的裁剪环，2d曲线显示。
//
void COccTryDoc::OnCurveFaceParamloop()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdFaceParamLoop *pScCmd = new ScCmdFaceParamLoop();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

//
// Undo的实现，重要的功能。
//
void COccTryDoc::OnEditUndo()
{
	if(this->m_scCmdMgr.IsCmdRunning())//目前不支持命令执行中的undo，以后可能会添加。
		return;

	//调用CmdMgr的undo功能。
	this->m_scCmdMgr.UndoCommand();
}

void COccTryDoc::OnFaceMultiloop()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdMultiLoopFace *pScCmd = new ScCmdMultiLoopFace();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveBreaktoedge()
{
		// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnWireAnayClosed()
{
		// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnWireFixClosed()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdWireCloseFix *pScCmd = new ScCmdWireCloseFix();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnWireToEdge()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdCurveWireToEdge();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnWireFixAll()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdWireFix *pScCmd = new ScCmdWireFix();
		this->m_scCmdMgr.RunCommand(pScCmd);
		
}

void COccTryDoc::OnSurfFaceAnayls()
{
		// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnSurfFaceFix()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdFaceFixAll *pScCmd = new ScCmdFaceFixAll();
		this->m_scCmdMgr.RunCommand(pScCmd);
}


void COccTryDoc::OnWireFixgaps()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdFaceFixGaps *pScCmd = new ScCmdFaceFixGaps();
		this->m_scCmdMgr.RunCommand(pScCmd);
		
}

void COccTryDoc::OnCheckValid()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdCheckValid *pScCmd = new ScCmdCheckValid();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCheckTopoValid()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdCheckTopoValid *pScCmd = new ScCmdCheckTopoValid();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCheckClosed()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdCheckClosed *pScCmd = new ScCmdCheckClosed();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

//使用曲线分配平面
void COccTryDoc::OnSurfSplitplane()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdSplitPlane *pScCmd = new ScCmdSplitPlane();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

//使用曲面分割曲面
void COccTryDoc::OnSurfSplitsurf()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdSplitSurf *pScCmd = new ScCmdSplitSurf();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnPlaneFromWire()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdPlaneFromWire *pScCmd = new ScCmdPlaneFromWire();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurvetrimSplit()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdCurveTrimSplit *pScCmd = new ScCmdCurveTrimSplit();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurvetrimQuicktrim()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdCurveQuickTrim *pScCmd = new ScCmdCurveQuickTrim();
		this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveFillet()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdCurveFillet *pScCmd = new ScCmdCurveFillet();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveChamfer()
{
		// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnCurveSplitBypoint()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdCurveSplitByPoint *pScCmd = new ScCmdCurveSplitByPoint();
		this->m_scCmdMgr.RunCommand(pScCmd);		
}

void COccTryDoc::OnCurveSplitBycurve()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdCurveSplitByCurve *pScCmd = new ScCmdCurveSplitByCurve();
		this->m_scCmdMgr.RunCommand(pScCmd);	
}

void COccTryDoc::OnCurveRectangle()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdRectangle *pScCmd = new ScCmdRectangle();
		this->m_scCmdMgr.RunCommand(pScCmd);	
}

void COccTryDoc::OnElliCorner()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdEllipseOfCorner *pScCmd = new ScCmdEllipseOfCorner();
		this->m_scCmdMgr.RunCommand(pScCmd);	
}

void COccTryDoc::OnCircleThreePoint()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdCircleOf3Point *pScCmd = new ScCmdCircleOf3Point();
		this->m_scCmdMgr.RunCommand(pScCmd);	
}

void COccTryDoc::OnArcTwoPoint()
{
		if(this->m_scCmdMgr.IsCmdRunning())
				return;

		ScCmdArcOfTwoPoint *pScCmd = new ScCmdArcOfTwoPoint();
		this->m_scCmdMgr.RunCommand(pScCmd);	
}

void COccTryDoc::OnTrsfFfd()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdFFD *pScCmd = new ScCmdFFD();
	this->m_scCmdMgr.RunCommand(pScCmd);	
}

void COccTryDoc::OnCurveTobsplinecurve()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdCurveToBSpline();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveCtrlpntEdit()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdBSplCurveEditByCtrlPnt *pScCmd = new ScCmdBSplCurveEditByCtrlPnt();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveCurpntEdit()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCmdBSplCurveEditByCurvPnt *pScCmd = new ScCmdBSplCurveEditByCurvPnt();
	this->m_scCmdMgr.RunCommand(pScCmd);
}


void COccTryDoc::OnSurfTobsplinesurf()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdSurfToBSpline();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnSolidTobspline()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdSolidToBSpline();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnSurfCtrlpntEdit()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdBSplSurfEditByCP();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveBsplineIncdegree()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdBSplCurveIncDegree();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveBezier()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdBezierCurve();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnBezierEditCtrlpnt()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdBezierCurveEditByCP();
	this->m_scCmdMgr.RunCommand(pScCmd);
	
}

void COccTryDoc::OnBezierIncdegree()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnTrsfAxdf()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdAxDF();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnSurfBsplineIncdegree()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdBSplSurfIncDegree();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnSurfBsplineInsertCtrlpnt()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnCurveBsplineInsertCtrlpnt()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnCurveFrameFrenet()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdCurveFrenetFrame();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveFrameRotate1()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdCurveRotateFrame1();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveFrameRotate2()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdCurveRotateFrame2();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveFrameSloan()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdCurveSloanFrame();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveFrameDblreflection()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnPointPrjtoCurve()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdPointPrjToCurve();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnPointPrjtoSurf()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnFfdFengSurfffd()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdSurfFFD();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveBsplineInsertKnots()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdBSplCurveInsertKnots();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnSurfBsplineInsertKnots()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdBSplSurfInsertKnots();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnCurveBsplineAddCtrlpnts()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdBSplCurveAddCtrlPnts();
	this->m_scCmdMgr.RunCommand(pScCmd);
	
}

void COccTryDoc::OnSurfBsplineAddCtrlpnts()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdBSplSurfAddCtrlPnts();
	this->m_scCmdMgr.RunCommand(pScCmd);
	
}



void COccTryDoc::OnMeshImportMs3d()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMeshImport3ds()
{
	// TODO: 在此添加命令处理程序代码
	if(this->m_scCmdMgr.IsCmdRunning())
		return;
//yxk20200104
	//ScCommand *pScCmd = new ScCmdMeshImportMs3d();
	//this->m_scCmdMgr.RunCommand(pScCmd);
//endyxk	
}

void COccTryDoc::OnMeshConvertToMesh()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

//yxk20200104
	//ScCommand *pScCmd = new ScCmdMeshConvertToMesh();
	//this->m_scCmdMgr.RunCommand(pScCmd);
//endyxk	
}

void COccTryDoc::OnMeshImportStl()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMeshImportBinStl()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMeshExportAscStl()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnMeshExportBinStl()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnTrsfInsertCtrlpntsTest()
{
	// TODO: 在此添加命令处理程序代码
	if(this->m_scCmdMgr.IsCmdRunning())
			return;

	ScCommand *pScCmd = new ScCmdBSplCtrlPntsTest();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnFfdRectffd()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::On33249()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdFFDLine();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnFfdFlowByCurve()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdFlowByCurve();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnTextCurve()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdOutLineText(ScCmdOutLineText::TEXT_WIRE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnTextSolid()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdOutLineText(ScCmdOutLineText::TEXT_SOLID);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnTextFace()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdOutLineText(ScCmdOutLineText::TEXT_FACE);
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnFfdFlowOnSurf()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdFlowBySurf();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnFfdShear()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdShear();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnFfdTwist()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdFFDTwist();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnAnalyCurveLength()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdCurveLength();
	this->m_scCmdMgr.RunCommand(pScCmd);
}

void COccTryDoc::OnAnalyFaceArea()
{
	// TODO: 在此添加命令处理程序代码
	//
	

}

void COccTryDoc::OnCurveConcatToSpline()
{
	// TODO: 在此添加命令处理程序代码
}

void COccTryDoc::OnFfdBend()
{
	if(this->m_scCmdMgr.IsCmdRunning())
		return;

	ScCommand *pScCmd = new ScCmdFFDBend();
	this->m_scCmdMgr.RunCommand(pScCmd);
}
