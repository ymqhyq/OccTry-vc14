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
// ��aShape���г�ʼ��
//
BOOL		FFDObj::InitShape(const TopoDS_Shape& aShape,
							  int nucpts,int nvcpts,int nudeg,int nvdeg)
{
	return FALSE;
}

//�����������ԭ����face�������µ�face��
TopoDS_Face		FFDObj::BuildNewFace(const Handle(Geom_Surface)& aNewSurf,const TopoDS_Face& aFace)
{
	TopoDS_Face aNF;

	TopoDS_Wire aOutW = ScBRepLib::OuterWire(aFace);
	if(aOutW.IsNull())
	{
		ASSERT(FALSE);
		return aNF;
	}

	//�����µ�aWire
	TopoDS_Wire aNW = BuildNewWire(aOutW,aNewSurf,aFace);
	if(aNW.IsNull())
	{
		DTRACE("\n build deformed wire failed.");
		return aNF;
	}

	//������face
	BRepBuilderAPI_MakeFace mf(aNewSurf,aNW,Standard_True);
	if(!mf.IsDone())
	{
		DTRACE("\n Make new base face error.");
		return aNF;
	}

	TopoDS_Face aTF = mf.Face();

	//ת��������wire
	BRep_Builder BB;
	TopExp_Explorer Ex;
	for(Ex.Init(aFace,TopAbs_WIRE);Ex.More();Ex.Next())
	{
		TopoDS_Wire aW = TopoDS::Wire(Ex.Current());
		if(aW.IsNull() || aW.IsSame(aOutW))
			continue;
		//�ڻ�
		aNW = BuildNewWire(aW,aNewSurf,aFace);
		if(aNW.IsNull())
		{
			DTRACE("\n build deformed inside wire failed.");
			return aNF;
		}

		//�ж��ڻ��ķ���
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
			aNF.Orientation(aFace.Orientation());//��һ��Ӧ���Ǳ���ģ�����solid���ܱ��ƻ���
	}

	return aNF;
}

TopoDS_Wire		FFDObj::BuildNewWire(const TopoDS_Wire& aWire,
									const Handle(Geom_Surface)& aNewSurf,const TopoDS_Face& aFace)
{
	TopoDS_Wire aNW;

	//�ȶ�wire�������򣬷�����wire����ʧ�ܡ����߸��򵥣�ֱ��ʹ��Builder��
	Handle(ShapeFix_Wire) aSFW = 
		new ShapeFix_Wire(aWire,aFace,Precision::Confusion());
	//����,ShapeExtend_WireData��֤��wire��Ϊ�����edge.�������ڲ���ȷ�Ļ�.
	aSFW->FixReorder();

	BRepBuilderAPI_MakeWire mW;
	//��ȡ���������ݡ�
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

		//���ݾɵ�aE�Ĳ�����Ϣ�����µ�3D����.
		double df,dl;
		Handle(Geom2d_Curve) aCur2d = BRep_Tool::CurveOnSurface(aE,aFace,df,dl);
		if(aCur2d.IsNull())
			continue;

		//������surf����3d����
		TopoDS_Edge aNE = BRepBuilderAPI_MakeEdge(aCur2d,aNewSurf,df,dl);
		if(aNE.IsNull())
			continue;

		BRepLib::BuildCurve3d(aNE);
		aNE.Orientation(aE.Orientation());//�ܹؼ�,��Ҫ��ϸ�Ķ����룬���.
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
// ��aShape���г�ʼ��,��Ҫ��ת��ΪNURBS���棬��Ϊÿ����������㹻�Ŀ��Ƶ�
//
BOOL	FFDSurfObj::InitShape(const TopoDS_Shape& aShape,
							  int nucpts,int nvcpts,int nudeg,int nvdeg)
{
	if(aShape.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//�������ת��������ȣ���Ҫ������������ϵ���⡣
	TopoDS_Shape aDS = ShapeCustom::DirectFaces(aShape);
	if(aDS.IsNull())
	{
		DTRACE("\n direct faces failed.");
		return FALSE;
	}

	//ת��Ϊbsp����
	BRepBuilderAPI_NurbsConvert nc;
	nc.Perform(aDS);
	if(!nc.IsDone())
	{
		DTRACE("\n nurbs convert failed.");
		return FALSE;
	}

	m_aShape = aShape;
	m_aNurbsShape = nc.Shape();

	//��������ÿ������Ľ�������������Ƶ�
	TopExp_Explorer ex;
	for(ex.Init(m_aNurbsShape,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(ex.Current());

		TopLoc_Location L;
		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace,L);//ע�⣺�������ص����ڲ����棬���������copy.
		if(aSurf.IsNull())
			continue;

		//ת����ɺ�����Ӧ������nurbs����
		Handle(Geom_BSplineSurface) aSplSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);
		if(aSplSurf.IsNull())
			continue;

		//�����Ҫ����������
		int ud = aSplSurf->UDegree(),vd = aSplSurf->VDegree();
		if(ud < nudeg || vd < nvdeg)
		{
			if(ud < nudeg) ud = nudeg;
			if(vd < nvdeg) vd = nvdeg;
			aSplSurf->IncreaseDegree(ud,vd);
			DTRACE("\n Increase Degree.");
		}

		//���ӿ��Ƶ�
		TcBSplLib::IncSurfCtrlPnts(aSplSurf,nucpts,nvcpts);
	}

	m_bValid = TRUE;

	return TRUE;
}

//�������
int		FFDSurfObj::Deform(FFDTool *pTool,FFDAlgo *pAlgo)
{
	return 0;
}
