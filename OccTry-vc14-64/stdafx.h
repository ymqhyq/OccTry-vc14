// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning(  disable : 4244 )        // Issue warning 4244
#include "Standard_ShortReal.hxx"
#pragma warning(  default : 4244 )        // Issue warning 4244

#ifdef _DEBUG
#define DTRACE ATLTRACE
#else
#define DTRACE __noop
#endif



#include <Standard.hxx>
#include <Standard_PrimitiveTypes.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Line.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Point.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_PolygonOffsetMode.hxx>
#include <Aspect_DisplayConnection.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRep_TEdge.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Modification.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBndLib.hxx>
#include <BRepAdaptor_HArray1OfCurve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
//yxk20191207
#include <Adaptor3d_IsoCurve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgo_NormalProjection.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepOffset_Mode.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepOffsetAPI_MakeFilling.hxx>
#include <BRepFilletAPI_MakeFillet2d.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepFeat_SplitShape.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRep_GCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Line.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomFill_Trihedron.hxx>
#include <GeomFill_Pipe.hxx>
#include <GeomFill_Frenet.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <GeomFill_FillingStyle.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomPlate_HArray1OfHCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeEllipse.hxx>
#include <gce_MakePln.hxx>
#include <GC_MakeCircle.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <GeomLProp_CLProps.hxx>
#include <Plate_PinpointConstraint.hxx>
#include <Plate_Plate.hxx>
#include <Plate_GtoCConstraint.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_Wireframe.hxx>
//endyxk

#include <GCE2d_MakeSegment.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomLib.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomPlate_Surface.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_BuildAveragePlane.hxx>
#include <Geom_CartesianPoint.hxx>
#include <GeomAbs_IsoType.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Texture1Dsegment.hxx>
#include <gp_Pln.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Circ.hxx>
#include <GProp_GProps.hxx>

#include <OpenGl_GraphicDriver.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_Text.hxx>

#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SequenceOfOwner.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <ShapeBuild_Edge.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_HLRPolyShape.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <StdSelect.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <STANDARD_TYPE.hxx>

#include <TCollection_AsciiString.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include "TopExp.hxx"
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_ListOfShape.hxx>
#include <TopoDS_ListIteratorOfListOfShape.hxx>
#include <TopoDS_Iterator.hxx>
#include "TopoDS_Edge.hxx"
#include "TopoDS_Vertex.hxx"
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V3d_Coordinate.hxx>
#include <WNT_Window.hxx>
#include<IntAna_Quadric.hxx>
#include<IntAna_IntConicQuad.hxx>
#include<GeomAPI_IntCS.hxx>
#include<GeomAPI_ExtremaCurveCurve.hxx>
#include<BRepClass_FaceClassifier.hxx>
#include<GeomLib_IsPlanarSurface.hxx>

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

