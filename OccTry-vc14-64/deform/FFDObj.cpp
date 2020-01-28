#include "StdAfx.h"

#include <ShapeCustom.hxx>
#include <BRepLib.hxx>

#include "ScBrepLib.h"
#include "ScCmdBSpline.h"
#include "FFDAlgo.h"
#include "FFDTool.h"
#include ".\ffdobj.h"

FFDObj::FFDObj(void)
{
}

FFDObj::~FFDObj(void)
{
}

//
// 以aShape进行初始化
//
BOOL		FFDObj::InitShape(const TopoDS_Shape& aShape,
							  int nucpts,int nvcpts,int nudeg,int nvdeg)
{
	return FALSE;
}

//根据新曲面和原来的face，生成新的face。
TopoDS_Face		FFDObj::BuildNewFace(const Handle(Geom_Surface)& aNewSurf,const TopoDS_Face& aFace)
{
	TopoDS_Face aNF;

	TopoDS_Wire aOutW = ScBRepLib::OuterWire(aFace);
	if(aOutW.IsNull())
	{
		ASSERT(FALSE);
		return aNF;
	}

	//生成新的aWire
	TopoDS_Wire aNW = BuildNewWire(aOutW,aNewSurf,aFace);
	if(aNW.IsNull())
	{
		DTRACE("\n build deformed wire failed.");
		return aNF;
	}

	//生成新face
	BRepBuilderAPI_MakeFace mf(aNewSurf,aNW,Standard_True);
	if(!mf.IsDone())
	{
		DTRACE("\n Make new base face error.");
		return aNF;
	}

	TopoDS_Face aTF = mf.Face();

	//转换后续的wire
	BRep_Builder BB;
	TopExp_Explorer Ex;
	for(Ex.Init(aFace,TopAbs_WIRE);Ex.More();Ex.Next())
	{
		TopoDS_Wire aW = TopoDS::Wire(Ex.Current());
		if(aW.IsNull() || aW.IsSame(aOutW))
			continue;
		//内环
		aNW = BuildNewWire(aW,aNewSurf,aFace);
		if(aNW.IsNull())
		{
			DTRACE("\n build deformed inside wire failed.");
			return aNF;
		}

		//判断内环的方向
		TopoDS_Shape aTS = aTF.EmptyCopied();
		TopoDS_Face aTFace = TopoDS::Face(aTS);
		aTF.Orientation(TopAbs_FORWARD);
		BB.Add(aTFace,aNW);
		BRepTopAdaptor_FClass2d fcls(aTFace,Precision::PConfusion());
		TopAbs_State sta = fcls.PerformInfinitePoint();
		if(sta == TopAbs_OUT)
			aNW.Reverse();

		mf.Add(aNW);
	}

	if(mf.IsDone())
	{
		aNF = mf.Face();
		if(aFace.Orientation() == TopAbs_REVERSED)
			aNF.Orientation(aFace.Orientation());//这一步应当是必须的，否则solid可能被破坏。
	}

	return aNF;
}

TopoDS_Wire		FFDObj::BuildNewWire(const TopoDS_Wire& aWire,
									const Handle(Geom_Surface)& aNewSurf,const TopoDS_Face& aFace)
{
	TopoDS_Wire aNW;

	//先对wire进行排序，否则组wire可能失败。或者更简单？直接使用Builder？
	Handle(ShapeFix_Wire) aSFW = 
		new ShapeFix_Wire(aWire,aFace,Precision::Confusion());
	//这里,ShapeExtend_WireData保证将wire作为排序的edge.允许工作于不正确的环.
	aSFW->FixReorder();

	BRepBuilderAPI_MakeWire mW;
	//获取处理后的数据。
	Handle(ShapeExtend_WireData) aSEWD = aSFW->WireData();
	int nE = aSEWD->NbEdges();
	for(int wix = 1;wix <= nE;wix ++)
	{
		TopoDS_Edge aE = aSEWD->Edge(wix);
		if(aE.IsNull())
		{
			ASSERT(FALSE);
			continue;
		}

		//根据旧的aE的参数信息生成新的3D曲线.
		double df,dl;
		Handle(Geom2d_Curve) aCur2d = BRep_Tool::CurveOnSurface(aE,aFace,df,dl);
		if(aCur2d.IsNull())
			continue;

		//根据新surf生成3d曲线
		TopoDS_Edge aNE = BRepBuilderAPI_MakeEdge(aCur2d,aNewSurf,df,dl);
		if(aNE.IsNull())
			continue;

		BRepLib::BuildCurve3d(aNE);
		aNE.Orientation(aE.Orientation());//很关键,需要仔细阅读代码，理解.
		mW.Add(aNE);
		if(!mW.IsDone())
		{
			DTRACE("\n make wire error.");
		}
	}

	if(mW.IsDone())
	{
		aNW = mW.Wire();
	}

	return aNW;
}

TopoDS_Shape	FFDObj::SewNewFaces(const TopoDS_Compound& aC)
{
	TopoDS_Shape aS;
	BRepBuilderAPI_Sewing sew;
	TopExp_Explorer ex;

	for(ex.Init(aC,TopAbs_FACE);ex.More();ex.Next())
	{
		sew.Add(ex.Current());
	}

	try{
		sew.Perform();
		aS = sew.SewedShape();
	}catch(Standard_Failure){
		aS = aC;
	}

	return aS;
}

//////////////////////////////////////////////////////////////////////////
//

FFDSurfObj::FFDSurfObj()
{
	m_bValid = FALSE;
}

FFDSurfObj::~FFDSurfObj()
{

}

//
// 以aShape进行初始化,主要是转换为NURBS曲面，并为每个曲面插入足够的控制点
//
BOOL	FFDSurfObj::InitShape(const TopoDS_Shape& aShape,
							  int nucpts,int nvcpts,int nudeg,int nvdeg)
{
	if(aShape.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//如果是旋转面或基本面等，需要考虑左手坐标系问题。
	TopoDS_Shape aDS = ShapeCustom::DirectFaces(aShape);
	if(aDS.IsNull())
	{
		DTRACE("\n direct faces failed.");
		return FALSE;
	}

	//转换为bsp曲面
	BRepBuilderAPI_NurbsConvert nc;
	nc.Perform(aDS);
	if(!nc.IsDone())
	{
		DTRACE("\n nurbs convert failed.");
		return FALSE;
	}

	m_aShape = aShape;
	m_aNurbsShape = nc.Shape();

	//下面提升每个曲面的阶数，并插入控制点
	TopExp_Explorer ex;
	for(ex.Init(m_aNurbsShape,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(ex.Current());

		TopLoc_Location L;
		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace,L);//注意：这样返回的是内部曲面，否则可能是copy.
		if(aSurf.IsNull())
			continue;

		//转换完成后曲面应当都是nurbs曲面
		Handle(Geom_BSplineSurface) aSplSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
		if(aSplSurf.IsNull())
			continue;

		//如果必要，提升阶数
		int ud = aSplSurf->UDegree(),vd = aSplSurf->VDegree();
		if(ud < nudeg || vd < nvdeg)
		{
			if(ud < nudeg) ud = nudeg;
			if(vd < nvdeg) vd = nvdeg;
			aSplSurf->IncreaseDegree(ud,vd);
			DTRACE("\n Increase Degree.");
		}

		//增加控制点
		TcBSplLib::IncSurfCtrlPnts(aSplSurf,nucpts,nvcpts);
	}

	m_bValid = TRUE;

	return TRUE;
}

//对象变形
int		FFDSurfObj::Deform(FFDTool *pTool,FFDAlgo *pAlgo)
{
	return 0;
}
