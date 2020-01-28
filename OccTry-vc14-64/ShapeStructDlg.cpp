// ShapeStructDlg.cpp : 实现文件
//

#include "stdafx.h"

#include <BRepTools.hxx>
#include <BSplCLib.hxx>

#include "OccTry.h"
#include "ShapeStructDlg.h"
#include ".\shapestructdlg.h"

//常用宏描述
char *pAryType[] = {
	" COMPOUND ",
    " COMPSOLID ",
    " SOLID ",
    " SHELL ",
    " FACE ",
    " WIRE ",
    " EDGE ",
    " VERTEX ",
    " SHAPE "
};

char *pAryState[] = {
	" IN ",
    " OUT ",
    " ON ",
    " UNKNOWN ",
    " INOROUT ",
    " INTERSECTED "
};

char *pAryOrit[] = {
	" FORWARD ",
    " REVERSED ",
    " INTERNAL ",
    " EXTERNAL "
};

// CShapeStructDlg 对话框

IMPLEMENT_DYNAMIC(CShapeStructDlg, CDialog)
CShapeStructDlg::CShapeStructDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShapeStructDlg::IDD, pParent)
{
}

CShapeStructDlg::~CShapeStructDlg()
{
}

void CShapeStructDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SHAPE_STRUCT, m_ssTree);
}

//
// 显示一个shape的内部结构。
//
void	CShapeStructDlg::ShowShape(const TopoDS_Shape& aShape)
{
	//清空。
	m_ssTree.DeleteAllItems();
	if(aShape.IsNull())
		return;

	//重新显示
	ShowShape(aShape,TVI_ROOT);
}

HTREEITEM	CShapeStructDlg::ShowShape(const TopoDS_Shape& aShape,
									HTREEITEM hParent)
{
	HTREEITEM hItem = NULL;

	//重新显示
	TopAbs_ShapeEnum st = aShape.ShapeType();
	switch(st)
	{
	case TopAbs_COMPOUND:
		hItem = ShowCompound(aShape,hParent);
		break;
	case TopAbs_COMPSOLID:
		hItem = ShowCompSolid(aShape,hParent);
		break;
	case TopAbs_SOLID:
		hItem = ShowSolid(aShape,hParent);
		break;
	case TopAbs_SHELL:
		hItem = ShowShell(aShape,hParent);
		break;
	case TopAbs_FACE:
		hItem = ShowFace(aShape,hParent);
		break;
	case TopAbs_WIRE:
		hItem = ShowWire(aShape,hParent);
		break;
	case TopAbs_EDGE:
		hItem = ShowEdge(aShape,hParent);
		break;
	case TopAbs_VERTEX:
		hItem = ShowVertex(aShape,hParent);
		break;
	case TopAbs_SHAPE:
		break;
	default:
		break;
	}

	return hItem;
}


//
// 显示Compound。内部可能包含各种类型的shape。
//
HTREEITEM	CShapeStructDlg::ShowCompound(const TopoDS_Shape& aShape,
											 HTREEITEM hParent)
{
	if(aShape.IsNull() ||
		(aShape.ShapeType() != TopAbs_COMPOUND))
		return NULL;
		
	//显示基本属性
	HTREEITEM hRoot = ShowShapeBaseProp(aShape,hParent);
	if(!hRoot)
		return NULL;

	TopoDS_Iterator ite;
	ite.Initialize(aShape);
	for(;ite.More();ite.Next())
	{
		ShowShape(ite.Value(),hRoot);
	}

	return hRoot;
}

//
// 显示CompSolid，即solid的组合。包含一系列的solid。
//
HTREEITEM	CShapeStructDlg::ShowCompSolid(const TopoDS_Shape& aShape,
											 HTREEITEM hParent)
{
	if(aShape.IsNull() ||
		(aShape.ShapeType() != TopAbs_COMPSOLID))
		return NULL;

	//显示基本属性
	HTREEITEM hRoot = ShowShapeBaseProp(aShape,hParent);
	if(!hRoot)
		return NULL;

	TopExp_Explorer Ex;

	//遍历子对象
	Ex.Init(aShape,TopAbs_SOLID);
	for(;Ex.More();Ex.Next())
	{
		TopoDS_Shape aS = Ex.Current();
		ShowSolid(aS,hRoot);
	}

	return hRoot;
}

//
// 显示solid。
//
HTREEITEM	CShapeStructDlg::ShowSolid(const TopoDS_Shape& aShape,
											  HTREEITEM hParent)
{
	if(aShape.IsNull() ||
		(aShape.ShapeType() != TopAbs_SOLID))
		return NULL;

	//显示基本属性
	HTREEITEM hRoot = ShowShapeBaseProp(aShape,hParent);
	if(!hRoot)
		return NULL;

	TopExp_Explorer Ex;

	//遍历子对象
	Ex.Init(aShape,TopAbs_SHELL);
	for(;Ex.More();Ex.Next())
	{
		TopoDS_Shape aS = Ex.Current();
		ShowShell(aS,hRoot);
	}

	return hRoot;
}

//
// 显示shell。
//
HTREEITEM	CShapeStructDlg::ShowShell(const TopoDS_Shape& aShape,
											  HTREEITEM hParent)
{
	if(aShape.IsNull() ||
		(aShape.ShapeType() != TopAbs_SHELL))
		return NULL;

	//显示基本属性
	HTREEITEM hRoot = ShowShapeBaseProp(aShape,hParent);
	if(!hRoot)
		return NULL;

	TopExp_Explorer Ex;

	//遍历子对象
	Ex.Init(aShape,TopAbs_FACE);
	for(;Ex.More();Ex.Next())
	{
		TopoDS_Shape aS = Ex.Current();
		ShowFace(aS,hRoot);
	}

	return hRoot;
}

//
// 需要显示的内容比较多。包括：
//    Face基本信息
//    surface信息
//    裁剪环信息
//
HTREEITEM	CShapeStructDlg::ShowFace(const TopoDS_Shape& aShape,
											 HTREEITEM hParent)
{
	if(aShape.IsNull() ||
		(aShape.ShapeType() != TopAbs_FACE))
		return NULL;

	//显示基本属性
	HTREEITEM hRoot = ShowShapeBaseProp(aShape,hParent);
	if(!hRoot)
		return NULL;

	//显示Geom属性
	ShowFaceGeomProp(aShape,hRoot);
	//显示裁剪环属性
	ShowFaceLoopProp(aShape,hRoot);

	return hRoot;
}

//
// 显示单独的环。非裁剪环。
//
HTREEITEM	CShapeStructDlg::ShowWire(const TopoDS_Shape& aShape,
											 HTREEITEM hParent)
{
	//显示基本属性
	HTREEITEM hRoot = ShowShapeBaseProp(aShape,hParent);
	if(!hRoot)
		return NULL;

	TopExp_Explorer Ex;
	Ex.Init(aShape,TopAbs_EDGE);
	for(;Ex.More();Ex.Next())
	{
		TopoDS_Shape aS = Ex.Current();
		ShowEdge(aS,hRoot);
	}

	return hRoot;
}

//
// 显示裁剪环的wire
//
HTREEITEM	CShapeStructDlg::ShowWire(const TopoDS_Shape& aShape,
							const TopoDS_Face& aFace,
							HTREEITEM hParent)
{
	//显示基本属性
	HTREEITEM hRoot = ShowShapeBaseProp(aShape,hParent);
	if(!hRoot)
		return NULL;

	TopExp_Explorer Ex;
	Ex.Init(aShape,TopAbs_EDGE);
	for(;Ex.More();Ex.Next())
	{
		TopoDS_Shape aS = Ex.Current();
		ShowEdge(aS,aFace,hRoot);
	}

	return hRoot;
}

//
// 显示单独的曲线对应的edge。
//
HTREEITEM	CShapeStructDlg::ShowEdge(const TopoDS_Shape& aShape,
											 HTREEITEM hParent)
{
	//显示基本属性
	HTREEITEM hRoot = ShowShapeBaseProp(aShape,hParent);
	if(!hRoot)
		return NULL;

	HTREEITEM hItem = ShowEdgeGeomProp(aShape,hRoot);

	//显示结点信息
	TopExp_Explorer Ex;
	Ex.Init(aShape,TopAbs_VERTEX);
	for(;Ex.More();Ex.Next())
	{
		TopoDS_Shape aS = Ex.Current();
		ShowVertex(aS,hRoot);
	}

	return hRoot;
}

//
//显示裁剪环中的edge,显示内容：
//    基本属性
//    3d curve
//    2d curve
//
HTREEITEM	CShapeStructDlg::ShowEdge(const TopoDS_Shape& aShape,
							const TopoDS_Face& aFace,
							HTREEITEM hParent)
{
	TopoDS_Edge aEdge = TopoDS::Edge(aShape);
	if(aEdge.IsNull())
		return NULL;

	//显示基本属性
	HTREEITEM hRoot = ShowShapeBaseProp(aShape,hParent);
	if(!hRoot)
		return NULL;

	CString szItem;
	double dFirst,dLast;
	//3d curve
	HTREEITEM hItem = NULL;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge,dFirst,dLast);
	if(!aCurve.IsNull())
	{
		szItem.Format("3D curve,range[%.4f,%.4f]",dFirst,dLast);
		hItem = m_ssTree.InsertItem(szItem,hRoot);
		//显示曲线
		ShowCurve(aCurve,hItem);
	}

	//2d curve
	Handle(Geom2d_Curve) aCur2d = BRep_Tool::CurveOnSurface(aEdge,aFace,dFirst,dLast);
	if(!aCur2d.IsNull())
	{
		szItem.Format("2D curve,range[%.4f,%.4f]",dFirst,dLast);
		hItem = m_ssTree.InsertItem(szItem,hRoot);
		//显示曲线
		Show2dCurve(aCur2d,hItem);
	}

	TopExp_Explorer Ex;
	Ex.Init(aShape,TopAbs_VERTEX);
	for(;Ex.More();Ex.Next())
	{
		TopoDS_Shape aS = Ex.Current();
		ShowVertex(aS,hRoot);
	}

	return hRoot;
}

//
// 显示结点信息。
//
HTREEITEM	CShapeStructDlg::ShowVertex(const TopoDS_Shape& aShape,
											   HTREEITEM hParent)
{
	if(aShape.IsNull()) return NULL;

	TopoDS_Vertex aV = TopoDS::Vertex(aShape);
	if(aV.IsNull())
		return NULL;

	//显示基本属性
	HTREEITEM hRoot = ShowShapeBaseProp(aShape,hParent);
	if(!hRoot)
		return NULL;

	//获取结点信息
	gp_Pnt pnt = BRep_Tool::Pnt(aV);
	CString szItem;
	szItem.Format("point [%.4f,%.4f,%.4f]",pnt.X(),pnt.Y(),pnt.Z());
	m_ssTree.InsertItem(szItem,hRoot);

	return hRoot;
}

//
// 显示shape的基本信息，包括：方向、位置、内部指针等。
// 尽量在一行显示足够多的信息。
//
HTREEITEM	CShapeStructDlg::ShowShapeBaseProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent)
{
	HTREEITEM hItem = NULL;
	int ts = (int)aShape.ShapeType();
	CString szItem = pAryType[ts];
	//方向
	CString szStr;
	szStr.Format("  [Orit:%s]",pAryOrit[(int)aShape.Orientation()]);
	szItem += szStr;
	//TShape指针
	const Handle_TopoDS_TShape& aT = aShape.TShape();
//yxk20200104
	//const Standard_Transient *pT = aT.Access();
	const Standard_Transient *pT = aT.get();
//endyxk
	szStr.Format("  [TShape:0X%8X]",(INT_PTR)pT);
	szItem += szStr;
	//其他
	if(aShape.Closed())
		szItem += "  <Closed>";
	if(aShape.Infinite())
		szItem += "  <Infinite>";
	hItem = m_ssTree.InsertItem(szItem,hParent);
	if(hItem)
	{	
		//位置
		TopLoc_Location loc = aShape.Location();
		if(!loc.IsIdentity())
		{
			HTREEITEM hLocItem = m_ssTree.InsertItem("Location:",hItem);
			gp_Trsf trsf = loc.Transformation();
			for(int ir = 1;ir <= 3;ir ++)
			{
				szItem = "  [";
				for(int ic = 1;ic <= 4;ic ++)
				{
					double dv = trsf.Value(ir,ic);
					CString szt;
					szt.Format("  %.4f",dv);
					szItem += szt;
				}
				szItem += "]";
				m_ssTree.InsertItem(szItem,hLocItem);
			}
		}
	}

	return hItem;
}

//
// 显示face的geom属性。主要是surface的属性。
//
HTREEITEM	CShapeStructDlg::ShowFaceGeomProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent)
{
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(aShape));
	if(aSurf.IsNull())
		return NULL;

	//显示surface的属性
	HTREEITEM hItem = ShowSurface(aSurf,hParent);

	return hItem;
}

//
// 显示surface的属性
//
HTREEITEM	CShapeStructDlg::ShowSurface(const Handle(Geom_Surface)& aSurf,HTREEITEM hParent)
{
	if(aSurf.IsNull())
		return NULL;

	HTREEITEM hRoot = NULL;
	CString szItem;
	if(aSurf->IsKind(STANDARD_TYPE(Geom_BoundedSurface)))
	{
		if(aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
		{
			szItem = "BSpline Surface";
			hRoot = m_ssTree.InsertItem(szItem,hParent);
			//
			this->ShowBSplineSurface(aSurf,hRoot);
		}
		else if(aSurf->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
		{
			szItem = "Bezier Surface";
			hRoot = m_ssTree.InsertItem(szItem,hParent);
			//
			this->ShowBezierSurface(aSurf,hRoot);
		}
		else if(aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
		{
			szItem = "Rectangular trimmed Surface";
			hRoot = m_ssTree.InsertItem(szItem,hParent);
		}
	}
	else if(aSurf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
	{
		if(aSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
		{
			szItem = "Plane Surface";
			hRoot = m_ssTree.InsertItem(szItem,hParent);
			//
			this->ShowPlaneSurface(aSurf,hRoot);
		}
		else if(aSurf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
		{
			szItem = "Cylindrical Surface";
			hRoot = m_ssTree.InsertItem(szItem,hParent);
			//
			this->ShowCylindricalSurface(aSurf,hRoot);
		}
		else if(aSurf->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
		{
			szItem = "Conical Surface";
			hRoot = m_ssTree.InsertItem(szItem,hParent);
			//
			this->ShowConicalSurface(aSurf,hRoot);
		}
		else if(aSurf->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
		{
			szItem = "Spherical Surface";
			hRoot = m_ssTree.InsertItem(szItem,hParent);
			//
			this->ShowSphericalSurface(aSurf,hRoot);
		}
		else if(aSurf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
		{
			szItem = "Toroidal Surface";
			hRoot = m_ssTree.InsertItem(szItem,hParent);
			//
			this->ShowToroidalSurface(aSurf,hRoot);
		}
	}
	else if(aSurf->IsKind(STANDARD_TYPE(Geom_SweptSurface)))
	{
		if(aSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
		{
			szItem = "Linear Extrusion Surface";
			hRoot = m_ssTree.InsertItem(szItem,hParent);
			//
			this->ShowExtrusionSurface(aSurf,hRoot);
		}
		else if(aSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
		{
			szItem = "Revolution Surface";
			hRoot = m_ssTree.InsertItem(szItem,hParent);
			//
			this->ShowRevolutionSurface(aSurf,hRoot);
		}
	}
	else if(aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
	{
		szItem = "Offset Surface";
		hRoot = m_ssTree.InsertItem(szItem,hParent);
	}

	
	return hRoot;
}

HTREEITEM	CShapeStructDlg::ShowBSplineSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent)
{
	return NULL;
}

HTREEITEM	CShapeStructDlg::ShowBezierSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent)
{
	return NULL;
}

HTREEITEM	CShapeStructDlg::ShowElementSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent)
{
	Handle(Geom_ElementarySurface) aEleSurf = Handle(Geom_ElementarySurface)::DownCast(aSurf);
	if(aEleSurf.IsNull())
		return NULL;

	CString szItem;
	HTREEITEM hItem = NULL;
	//局部坐标系
	gp_Ax3 ax3 = aEleSurf->Position();
	if(ax3.Direct())
		szItem = "Right hand axis";
	else
		szItem = "Left hand axis";
	hItem = m_ssTree.InsertItem(szItem,hParent);
	//插入下面各个项
	//loc
	gp_Pnt pnt = ax3.Location();
	szItem.Format("Loc: %.4f,%.4f,%.4f",pnt.X(),pnt.Y(),pnt.Z());
	m_ssTree.InsertItem(szItem,hItem);
	//Dir
	gp_Dir zd = ax3.Direction();
	szItem.Format("Dir: %.4f,%.4f,%.4f",zd.X(),zd.Y(),zd.Z());
	m_ssTree.InsertItem(szItem,hItem);
	//XDir
	gp_Dir xd = ax3.XDirection();
	szItem.Format("XDir: %.4f,%.4f,%.4f",xd.X(),xd.Y(),xd.Z());
	m_ssTree.InsertItem(szItem,hItem);

	//YDir
	gp_Dir yd = ax3.YDirection();
	szItem.Format("YDir: %.4f,%.4f,%.4f",yd.X(),yd.Y(),yd.Z());
	m_ssTree.InsertItem(szItem,hItem);

	return hItem;
}

HTREEITEM	CShapeStructDlg::ShowPlaneSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent)
{
	Handle(Geom_Plane) aPln = Handle(Geom_Plane)::DownCast(aSurf);
	if(aPln.IsNull())
		return NULL;

	//显示基本项
	ShowElementSurface(aSurf,hParent);
	//显示自有项

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowCylindricalSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent)
{
	Handle(Geom_CylindricalSurface) aCyl = Handle(Geom_CylindricalSurface)::DownCast(aSurf);
	if(aCyl.IsNull())
		return NULL;

	//显示基本项
	ShowElementSurface(aSurf,hParent);
	//显示自有项
	CString szItem;
	szItem.Format("Radius: %f",aCyl->Radius());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowConicalSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent)
{
	Handle(Geom_ConicalSurface) aCon = Handle(Geom_ConicalSurface)::DownCast(aSurf);
	if(aCon.IsNull())
		return NULL;

	//显示基本项
	ShowElementSurface(aSurf,hParent);
	//显示自有项
	CString szItem;
	szItem.Format("Radius: %f,SemiAngle: %f",aCon->RefRadius(),aCon->SemiAngle());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowSphericalSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent)
{
	Handle(Geom_SphericalSurface) aSph = Handle(Geom_SphericalSurface)::DownCast(aSurf);
	if(aSph.IsNull())
		return NULL;

	//显示基本项
	ShowElementSurface(aSurf,hParent);
	//显示自有项
	CString szItem;
	szItem.Format("Radius: %f",aSph->Radius());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowToroidalSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent)
{
	Handle(Geom_ToroidalSurface) aTor = Handle(Geom_ToroidalSurface)::DownCast(aSurf);
	if(aTor.IsNull())
		return NULL;

	//显示基本项
	ShowElementSurface(aSurf,hParent);
	//显示自有项
	CString szItem;
	szItem.Format("MajorRadius: %f,MinorRadius: %f",aTor->MajorRadius(),
		aTor->MinorRadius());
	m_ssTree.InsertItem(szItem,hParent);


	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowSweptSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent)
{
	Handle(Geom_SweptSurface) aSweptSurf = Handle(Geom_SweptSurface)::DownCast(aSurf);
	if(aSweptSurf.IsNull())
		return NULL;

	//显示曲线
	Handle(Geom_Curve) aCur = aSweptSurf->BasisCurve();
	ShowCurve(aCur,hParent);
	//显示方向
	gp_Dir d = aSweptSurf->Direction();
	CString szItem;
	szItem.Format("Dir: %.4f,%.4f,%.4f",d.X(),d.Y(),d.Z());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowExtrusionSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent)
{
	Handle(Geom_SurfaceOfLinearExtrusion) aExtSurf = 
		Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(aSurf);
	if(aExtSurf.IsNull())
		return NULL;


	return ShowSweptSurface(aSurf,hParent);
}

HTREEITEM	CShapeStructDlg::ShowRevolutionSurface(const Handle(Geom_Surface)& aSurf,
							HTREEITEM hParent)
{
	Handle(Geom_SurfaceOfRevolution) aRevSurf = 
		Handle(Geom_SurfaceOfRevolution)::DownCast(aSurf);
	if(aRevSurf.IsNull())
		return NULL;

	ShowSweptSurface(aSurf,hParent);
	//显示loc
	gp_Pnt loc = aRevSurf->Location();
	CString szItem;
	szItem.Format("Loc: %.4f,%.4f,%.4f",loc.X(),loc.Y(),loc.Z());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

							
//
// 显示face的裁剪环属性。
//
HTREEITEM	CShapeStructDlg::ShowFaceLoopProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent)
{
	if(aShape.IsNull())
		return NULL;
	
	TopoDS_Face aFace = TopoDS::Face(aShape);
	if(aFace.IsNull())
		return NULL;

	TopoDS_Wire aOutWire = BRepTools::OuterWire(aFace);
	if(!aOutWire.IsNull())
	{
		ShowWire(aOutWire,aFace,hParent);
	}

	//其他wire
	TopExp_Explorer Ex;
	Ex.Init(aShape,TopAbs_WIRE);
	for(;Ex.More();Ex.Next())
	{
		TopoDS_Wire aW = TopoDS::Wire(Ex.Current());
		if(aW.IsEqual(aOutWire))
			continue;

		ShowWire(aW,aFace,hParent);
	}

	return hParent;
}

//
// 显示Edge的geom属性。
//
HTREEITEM	CShapeStructDlg::ShowEdgeGeomProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent)
{
	const Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*) &aShape.TShape());
	const BRep_ListOfCurveRepresentation& lcr = TE->Curves();
  
	HTREEITEM hRoot = m_ssTree.InsertItem("Edge geom prop",hParent);

	CString szItem;
	Handle(BRep_CurveRepresentation) cr;
	Handle(BRep_GCurve) GC;
	Standard_Real f,l;
	int ic = 0;
	BRep_ListIteratorOfListOfCurveRepresentation itcr(lcr);
	while (itcr.More()) 
	{
		Handle(BRep_CurveRepresentation)& cr = itcr.Value();
		GC = Handle(BRep_GCurve)::DownCast(itcr.Value());
		if(!GC.IsNull())
		{
			GC->Range(f,l);
			if(GC->IsCurve3D())
			{
				szItem.Format("3D Curve ,range[%.4f,%.4f]",f,l);
				HTREEITEM hItem = m_ssTree.InsertItem(szItem,hRoot);
				//3d曲线
				const Handle(Geom_Curve)& hcr = GC->Curve3D();
				ShowCurve(hcr,hItem);
			}
			else if(GC->IsCurveOnClosedSurface())
			{
				szItem.Format("Curve On Closed Surface,range[%.4f,%.4f]",f,l);
				HTREEITEM hItem = m_ssTree.InsertItem(szItem,hRoot);
				
				const Handle(Geom_Surface)& hsr = GC->Surface();
				ShowSurface(hsr,hItem);
				const Handle(Geom2d_Curve)& h2cr = GC->PCurve();
				Show2dCurve(h2cr,hItem);
				const Handle(Geom2d_Curve)& h2cr2 = GC->PCurve();
				Show2dCurve(h2cr2,hItem);
			}
			else if(GC->IsCurveOnSurface())
			{
				szItem.Format("Curve On Surface,range[%.4f,%.4f]",f,l);
				HTREEITEM hItem = m_ssTree.InsertItem(szItem,hRoot);

				const Handle(Geom_Surface)& hsr = GC->Surface();
				ShowSurface(hsr,hItem);
				const Handle(Geom2d_Curve)& h2cr = GC->PCurve();
				Show2dCurve(h2cr,hItem);
			}
			else
			{
				szItem.Format("other type curve");
				m_ssTree.InsertItem(szItem,hRoot);
			}
		}
		itcr.Next();
	}

	return hRoot;
}

//
// 显示裁剪环的Edge的属性。
//
HTREEITEM	CShapeStructDlg::ShowLoopEdgeProp(const TopoDS_Shape& aShape,
							HTREEITEM hParent)
{
	return NULL;
}

//
// 显示curve的属性
//
HTREEITEM	CShapeStructDlg::ShowCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent)
{
	if(aCur.IsNull()) return NULL;

	HTREEITEM hItem = NULL;
	CString szItem;
	if (aCur->IsKind(STANDARD_TYPE(Geom_BoundedCurve))) 
	{
		if (aCur->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
		{
			szItem.Format("BSpline Curve.");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowBSplineCurve(aCur,hItem);
		}
		else if (aCur->IsKind(STANDARD_TYPE(Geom_BezierCurve))) 
		{
			szItem.Format("Bezier Curve.");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowBezierCurve(aCur,hItem);
		}
		else if ( aCur->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) 
		{
			szItem.Format("Trimmed Curve.");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowTrimmedCurve(aCur,hItem);
		}
	}
	else if (aCur->IsKind(STANDARD_TYPE(Geom_Conic))) 
	{
		if (aCur->IsKind(STANDARD_TYPE(Geom_Circle))) 
		{
			szItem.Format("Circle Curve");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowCircleCurve(aCur,hItem);
		}
		else if (aCur->IsKind(STANDARD_TYPE(Geom_Ellipse))) 
		{
			szItem.Format("Ellipse Curve");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowEllipseCurve(aCur,hItem);
		}
		else if ( aCur->IsKind(STANDARD_TYPE(Geom_Hyperbola))) 
		{
			szItem.Format("Hyperbola Curve");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowHyperbolaCurve(aCur,hItem);
		}
		else if ( aCur->IsKind(STANDARD_TYPE(Geom_Parabola))) 
		{
			szItem.Format("Parabola Curve");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowParabolaCurve(aCur,hItem);
		}
	}
	else if ( aCur->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) 
	{
		szItem.Format("Offset Curve");
		hItem = m_ssTree.InsertItem(szItem,hParent);

		Handle(Geom_OffsetCurve) aOffCur = Handle(Geom_OffsetCurve)::DownCast(aCur);
		if(!aOffCur.IsNull())
		{
			const Handle(Geom_Curve)& aBasCur = aOffCur->BasisCurve();
			ShowCurve(aBasCur,hItem);
		}
	}
	else if ( aCur->IsKind(STANDARD_TYPE(Geom_Line))) 
	{
		szItem.Format("Line Curve");
		hItem = m_ssTree.InsertItem(szItem,hParent);

		this->ShowLineCurve(aCur,hItem);
	}  

	return hItem;
}

//
// 显示具体类型的curve

//
// 显示样条曲线
// 显示：阶数、属性、控制点、knots、权值。
//
HTREEITEM	CShapeStructDlg::ShowBSplineCurve(const Handle(Geom_Curve)& aCur,
											  HTREEITEM hParent)
{
	Handle(Geom_BSplineCurve) aSplCur = Handle(Geom_BSplineCurve)::DownCast(aCur);
	if(aSplCur.IsNull())
		return NULL;

	CString szItem;
	szItem.Format("Degree : %d",aSplCur->Degree());
	if(aSplCur->IsClosed())
		szItem += ", Closed";
	if(aSplCur->IsPeriodic())
		szItem += ", Periodic";
	if(aSplCur->IsRational())
		szItem += ", Rational";
	m_ssTree.InsertItem(szItem,hParent);

	//控制点和权值
	BOOL bRational = aSplCur->IsRational();
	int nPoles = aSplCur->NbPoles();
	szItem.Format("%d control points:",nPoles);
	HTREEITEM hSubItem = m_ssTree.InsertItem(szItem,hParent);
	for(int ic = 1;ic <= nPoles;ic ++)
	{
		gp_Pnt pt = aSplCur->Pole(ic);
		if(bRational)
			szItem.Format("%3d:  %.4f,%.4f,%.4f,%.4f",ic - 1,pt.X(),pt.Y(),pt.Z(),aSplCur->Weight(ic));
		else
			szItem.Format("%3d:  %.4f,%.4f,%.4f,1.0",ic - 1,pt.X(),pt.Y(),pt.Z());
		m_ssTree.InsertItem(szItem,hSubItem);
	}

	TColStd_Array1OfInteger multis(1,aSplCur->NbKnots());
	aSplCur->Multiplicities(multis);
	//knots,获取knot的个数应当区分周期和非周期的情况。
	int nKnots = BSplCLib::KnotSequenceLength(multis,aSplCur->Degree(),aSplCur->IsPeriodic());
	szItem.Format("%d knots",nKnots);
	hSubItem = m_ssTree.InsertItem(szItem,hParent);
	TColStd_Array1OfReal arKnots(1,nKnots);
	aSplCur->KnotSequence(arKnots);
	CString szStr;
	int nC = 0;
	szItem = "";
	for(int ix = 1;ix <= nKnots;ix ++)
	{
		szStr.Format(" %.4f,",arKnots.Value(ix));
		szItem += szStr;
		nC ++;

		if(nC >= 4)
		{
			m_ssTree.InsertItem(szItem,hSubItem);
			szItem = "";
			nC = 0;
		}
	}
	if(nC && (nC < 4))
	{
		m_ssTree.InsertItem(szItem,hSubItem);
	}

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowBezierCurve(const Handle(Geom_Curve)& aCur,
											 HTREEITEM hParent)
{
	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowTrimmedCurve(const Handle(Geom_Curve)& aCur,
											  HTREEITEM hParent)
{
	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowConicCurve(const Handle(Geom_Curve)& aCur,
											HTREEITEM hParent)
{
	Handle(Geom_Conic) aConic = Handle(Geom_Conic)::DownCast(aCur);
	if(aConic.IsNull())
		return NULL;

	//
	gp_Ax2 ax = aConic->Position();
	CString szItem;
	HTREEITEM hItem = m_ssTree.InsertItem("Axis 3d",hParent);
	if(hItem)
	{
		double x,y,z;
		//loc
		ax.Location().Coord(x,y,z);
		szItem.Format("Loc:  %.4f,%.4f,%.4f",x,y,z);
		m_ssTree.InsertItem(szItem,hItem);
		//dir
		ax.Direction().Coord(x,y,z);
		szItem.Format("Dir:  %.4f,%.4f,%.4f",x,y,z);
		m_ssTree.InsertItem(szItem,hItem);

		//xdir
		ax.XDirection().Coord(x,y,z);
		szItem.Format("XDir:  %.4f,%.4f,%.4f",x,y,z);
		m_ssTree.InsertItem(szItem,hItem);

		//ydir
		ax.YDirection().Coord(x,y,z);
		szItem.Format("YDir:  %.4f,%.4f,%.4f",x,y,z);
		m_ssTree.InsertItem(szItem,hItem);
	}

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowCircleCurve(const Handle(Geom_Curve)& aCur,
											 HTREEITEM hParent)
{
	Handle(Geom_Circle) acCur = Handle(Geom_Circle)::DownCast(aCur);
	if(acCur.IsNull())
		return NULL;

	//显示基本信息
	ShowConicCurve(aCur,hParent);
	//
	CString szItem;
	szItem.Format("Radius : %f",acCur->Radius());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowEllipseCurve(const Handle(Geom_Curve)& aCur,
											  HTREEITEM hParent)
{
	Handle(Geom_Ellipse) acCur = Handle(Geom_Ellipse)::DownCast(aCur);
	if(acCur.IsNull())
		return NULL;

	//显示基本信息
	ShowConicCurve(aCur,hParent);
	//
	CString szItem;
	szItem.Format("MajorRadius : %f,MinorRadius : %f",acCur->MajorRadius(),
		acCur->MinorRadius());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowHyperbolaCurve(const Handle(Geom_Curve)& aCur,
												HTREEITEM hParent)
{
	Handle(Geom_Hyperbola) acCur = Handle(Geom_Hyperbola)::DownCast(aCur);
	if(acCur.IsNull())
		return NULL;

	//显示基本信息
	ShowConicCurve(aCur,hParent);
	//
	CString szItem;
	szItem.Format("MajorRadius : %f,MinorRadius : %f",acCur->MajorRadius(),
		acCur->MinorRadius());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowParabolaCurve(const Handle(Geom_Curve)& aCur,
											   HTREEITEM hParent)
{
	Handle(Geom_Parabola) acCur = Handle(Geom_Parabola)::DownCast(aCur);
	if(acCur.IsNull())
		return NULL;

	//显示基本信息
	ShowConicCurve(aCur,hParent);
	//
	CString szItem;
	szItem.Format("Focal : %f",acCur->Focal());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowLineCurve(const Handle(Geom_Curve)& aCur,
										   HTREEITEM hParent)
{
	Handle(Geom_Line) aLin = Handle(Geom_Line)::DownCast(aCur);
	if(aLin.IsNull())
		return NULL;

	//
	CString szItem;
	gp_Ax1 ax = aLin->Position();
	szItem.Format("Loc: %.4f,%.4f,%.4f",ax.Location().X(),ax.Location().Y(),
		ax.Location().Z());
	m_ssTree.InsertItem(szItem,hParent);
	szItem.Format("Dir: %.4f,%.4f,%.4f",ax.Direction().X(),ax.Direction().Y(),
		ax.Direction().Z());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

//
// 显示2d curve的属性
//
HTREEITEM	CShapeStructDlg::Show2dCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent)
{
	if(aCur.IsNull()) return NULL;

	HTREEITEM hItem = NULL;
	CString szItem = "";
	if (aCur->IsKind(STANDARD_TYPE(Geom2d_BoundedCurve))) 
	{
		if (aCur->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) 
		{
			szItem.Format("2d BSpline Curve.");
			hItem = m_ssTree.InsertItem(szItem,hParent);
			//
			this->ShowBSpline2dCurve(aCur,hItem);
		}
		else if (aCur->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))) 
		{
			szItem.Format("2d Bezier Curve.");
			hItem = m_ssTree.InsertItem(szItem,hParent);
			//
			this->ShowBezier2dCurve(aCur,hItem);
		}
		else if ( aCur->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) 
		{
			szItem.Format("2d Trimmed Curve.");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowTrimmed2dCurve(aCur,hItem);
		}
	}
	else if (aCur->IsKind(STANDARD_TYPE(Geom2d_Conic))) 
	{
		if (aCur->IsKind(STANDARD_TYPE(Geom2d_Circle))) 
		{
			szItem.Format("2d Circle Curve.");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowCircle2dCurve(aCur,hItem);
		}
		else if (aCur->IsKind(STANDARD_TYPE(Geom2d_Ellipse))) 
		{
			szItem.Format("2d Ellipse Curve.");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowEllipse2dCurve(aCur,hItem);
		}
		else if ( aCur->IsKind(STANDARD_TYPE(Geom2d_Hyperbola))) 
		{
			szItem.Format("2d Hyperbola Curve.");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowHyperbola2dCurve(aCur,hItem);
		}
		else if ( aCur->IsKind(STANDARD_TYPE(Geom2d_Parabola))) 
		{
			szItem.Format("2d Parabola Curve.");
			hItem = m_ssTree.InsertItem(szItem,hParent);

			this->ShowParabola2dCurve(aCur,hItem);
		}
	}
	else if ( aCur->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))) 
	{
		szItem.Format("2d Offset Curve.");
		hItem = m_ssTree.InsertItem(szItem,hParent);
		/*const Standard_Transient *pH = aCur.Access();
		szMsg.Format("  Geom2d_OffsetCurve.[Handle:%x]\n",(int)pH);
		Dump(szMsg);
		szMsg.Empty();
		DeclareAndCast(Geom2d_OffsetCurve, OffsetC, aCur);
		const Handle(Geom2d_Curve)& hbc = OffsetC->BasisCurve();
		Dump2dCurve(hbc);*/
	}
	else if ( aCur->IsKind(STANDARD_TYPE(Geom2d_Line))) 
	{
		szItem.Format("2d Line Curve.");
		hItem = m_ssTree.InsertItem(szItem,hParent);

		this->ShowLine2dCurve(aCur,hItem);
	}  

	return hItem;
}

//
// 显示具体类型的curve
//
HTREEITEM	CShapeStructDlg::ShowBSpline2dCurve(const Handle(Geom2d_Curve)& aCur,
											  HTREEITEM hParent)
{
	Handle(Geom2d_BSplineCurve) aSplCur = Handle(Geom2d_BSplineCurve)::DownCast(aCur);
	if(aSplCur.IsNull())
		return NULL;

	CString szItem;
	szItem.Format("Degree : %d",aSplCur->Degree());
	if(aSplCur->IsClosed())
		szItem += ", Closed";
	if(aSplCur->IsPeriodic())
		szItem += ", Periodic";
	if(aSplCur->IsRational())
		szItem += ", Rational";
	m_ssTree.InsertItem(szItem,hParent);

	//控制点和权值
	BOOL bRational = aSplCur->IsRational();
	int nPoles = aSplCur->NbPoles();
	szItem.Format("%d control points:",nPoles);
	HTREEITEM hSubItem = m_ssTree.InsertItem(szItem,hParent);
	for(int ic = 1;ic <= nPoles;ic ++)
	{
		gp_Pnt2d pt = aSplCur->Pole(ic);
		if(bRational)
			szItem.Format("%3d:  %.4f,%.4f,%.4f",ic - 1,pt.X(),pt.Y(),aSplCur->Weight(ic));
		else
			szItem.Format("%3d:  %.4f,%.4f,1.0",ic - 1,pt.X(),pt.Y());
		m_ssTree.InsertItem(szItem,hSubItem);
	}

	TColStd_Array1OfInteger multis(1,aSplCur->NbKnots());
	aSplCur->Multiplicities(multis);
	//knots,获取knot的个数应当区分周期和非周期的情况。
	int nKnots = BSplCLib::KnotSequenceLength(multis,aSplCur->Degree(),aSplCur->IsPeriodic());
	szItem.Format("%d knots",nKnots);
	hSubItem = m_ssTree.InsertItem(szItem,hParent);
	TColStd_Array1OfReal arKnots(1,nKnots);
	aSplCur->KnotSequence(arKnots);
	CString szStr;
	int nC = 0;
	szItem = "";
	for(int ix = 1;ix <= nKnots;ix ++)
	{
		szStr.Format(" %.4f,",arKnots.Value(ix));
		szItem += szStr;
		nC ++;

		if(nC >= 4)
		{
			m_ssTree.InsertItem(szItem,hSubItem);
			szItem = "";
			nC = 0;
		}
	}
	if(nC && (nC < 4))
	{
		m_ssTree.InsertItem(szItem,hSubItem);
	}

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowBezier2dCurve(const Handle(Geom2d_Curve)& aCur,
											 HTREEITEM hParent)
{
	Handle(Geom2d_Line) acCur2d = Handle(Geom2d_Line)::DownCast(aCur);
	if(acCur2d.IsNull())
		return NULL;

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowTrimmed2dCurve(const Handle(Geom2d_Curve)& aCur,
												HTREEITEM hParent)
{
	Handle(Geom2d_Line) acCur2d = Handle(Geom2d_Line)::DownCast(aCur);
	if(acCur2d.IsNull())
		return NULL;

	return hParent;
}

//
// 二次曲线
//
HTREEITEM	CShapeStructDlg::ShowConic2dCurve(const Handle(Geom2d_Curve)& aCur,
											  HTREEITEM hParent)
{
	Handle(Geom2d_Conic) acCur2d = Handle(Geom2d_Conic)::DownCast(aCur);
	if(acCur2d.IsNull())
		return NULL;

	CString szItem;
	HTREEITEM hItem = m_ssTree.InsertItem("Axis 2d",hParent);
	if(hItem)
	{
		double x,y;
		gp_Ax22d ax = acCur2d->Position();
		//Loc
		ax.Location().Coord(x,y);
		szItem.Format("Loc : %.4f,%.4f",x,y);
		m_ssTree.InsertItem(szItem,hItem);
		//XDir
		ax.XDirection().Coord(x,y);
		szItem.Format("XDir : %.4f,%.4f",x,y);
		m_ssTree.InsertItem(szItem,hItem);
		//YDir
		ax.YDirection().Coord(x,y);
		szItem.Format("YDir : %.4f,%.4f",x,y);
		m_ssTree.InsertItem(szItem,hItem);
	}

	return hItem;
}

HTREEITEM	CShapeStructDlg::ShowCircle2dCurve(const Handle(Geom2d_Curve)& aCur,
											   HTREEITEM hParent)
{
	Handle(Geom2d_Circle) acCur2d = Handle(Geom2d_Circle)::DownCast(aCur);
	if(acCur2d.IsNull())
		return NULL;

	//显示基本信息
	this->ShowConic2dCurve(aCur,hParent);
	//显示具体的信息
	CString szItem;
	szItem.Format("Radius : %f",acCur2d->Radius());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowEllipse2dCurve(const Handle(Geom2d_Curve)& aCur,
												HTREEITEM hParent)
{
	Handle(Geom2d_Ellipse) acCur2d = Handle(Geom2d_Ellipse)::DownCast(aCur);
	if(acCur2d.IsNull())
		return NULL;

	//显示基本信息
	this->ShowConic2dCurve(aCur,hParent);
	//显示具体的信息
	CString szItem;
	szItem.Format("MajorRadius : %f,MinorRadius : %f",acCur2d->MajorRadius(),
		acCur2d->MinorRadius());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowHyperbola2dCurve(const Handle(Geom2d_Curve)& aCur,
												  HTREEITEM hParent)
{
	Handle(Geom2d_Hyperbola) acCur2d = Handle(Geom2d_Hyperbola)::DownCast(aCur);
	if(acCur2d.IsNull())
		return NULL;

	//显示基本信息
	this->ShowConic2dCurve(aCur,hParent);
	//显示具体的信息
	CString szItem;
	szItem.Format("MajorRadius : %f,MinorRadius : %f",acCur2d->MajorRadius(),
		acCur2d->MinorRadius());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowParabola2dCurve(const Handle(Geom2d_Curve)& aCur,
												 HTREEITEM hParent)
{
	Handle(Geom2d_Parabola) acCur2d = Handle(Geom2d_Parabola)::DownCast(aCur);
	if(acCur2d.IsNull())
		return NULL;

	//显示基本信息
	this->ShowConic2dCurve(aCur,hParent);
	//显示具体的信息
	CString szItem;
	szItem.Format("Focal : %f",acCur2d->Focal());
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

HTREEITEM	CShapeStructDlg::ShowLine2dCurve(const Handle(Geom2d_Curve)& aCur,
											 HTREEITEM hParent)
{
	Handle(Geom2d_Line) acCur2d = Handle(Geom2d_Line)::DownCast(aCur);
	if(acCur2d.IsNull())
		return NULL;

	gp_Ax2d ax = acCur2d->Position();
	CString szItem;
	double x,y;
	//loc
	ax.Location().Coord(x,y);
	szItem.Format("Loc : %.4f,%.4f",x,y);
	m_ssTree.InsertItem(szItem,hParent);
	//dir
	ax.Direction().Coord(x,y);
	szItem.Format("Dir : %.4f,%.4f",x,y);
	m_ssTree.InsertItem(szItem,hParent);

	return hParent;
}

BEGIN_MESSAGE_MAP(CShapeStructDlg, CDialog)
END_MESSAGE_MAP()


// CShapeStructDlg 消息处理程序

BOOL CShapeStructDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CShapeStructDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnOK();
}
