#include "StdAfx.h"

#include <ProjLib.hxx>
#include <ElsLib.hxx>
//yxk20200128
//#include <Select3D_Projector.hxx>
#include<V3d.hxx>
//endyxk
#include <V3d_TypeOfOrientation.hxx>

#include "ScUcsMgr.h"
#include ".\scview.h"

ScView::ScView(const Handle(V3d_View)& aView)
:m_aView(aView),m_nViewType(ScView_Front)
{
	this->m_pScUcsMgr = NULL;
	this->m_bNeedModelPoint = FALSE;
	this->m_pViewWnd = NULL;
}

ScView::~ScView(void)
{
}


//设置视图类型
int		ScView::SetProj(int nType)
{
	switch(nType)
	{
	case V3d_Zpos://前视图
		{
			m_nViewType = ScView_Front;
		}
		break;
	case V3d_Zneg://后视图
		{
			m_nViewType = ScView_Back;
		}
		break;
	case V3d_Xpos://左视图
		{
			m_nViewType = ScView_Left;
		}
		break;
	case V3d_Xneg://右视图
		{
			m_nViewType = ScView_Right;
		}
		break;
	case V3d_Ypos://上视图
		{
			m_nViewType = ScView_Top;
		}
		break;
	case V3d_Yneg://下视图
		{
			m_nViewType = ScView_Bottom;
		}
		break;
	default:
		return -1;
		break;
	}
//yxk20200104
	//Graphic3d_Vector gv = V3d::GetProjAxis((V3d_TypeOfOrientation)nType);
	gp_Dir gv = V3d::GetProjAxis((V3d_TypeOfOrientation)nType);
//endyxk
	double x,y,z;
	gv.Coord(x,y,z);

	gp_Vec vec(x,y,z);
	UCSToWCS(vec);

	this->m_aView->SetProj(vec.X(),vec.Y(),vec.Z());

	//更新Up
	ScUcs *pScUcs = this->m_pScUcsMgr->CurrentUCS();
	gp_Dir vup(0.0,1.0,0.0);
	gp_Pnt org(0.0,0.0,0.0);
	if(pScUcs)
	{
		org = pScUcs->m_ax2.Location();
		vup = pScUcs->m_ax2.YDirection();
	}
	this->m_aView->SetAt(org.X(),org.Y(),org.Z());
	this->m_aView->SetUp(vup.X(),vup.Y(),vup.Z());
	return 0;
}

//
// 屏幕点转换为空间(WCS or UCS)中的点。
//
gp_Pnt	ScView::PixelToModel(Standard_Real x, Standard_Real y,BOOL bWCS)
{
	V3d_Coordinate XEye,YEye,ZEye,XAt,YAt,ZAt;
	m_aView->Eye(XEye,YEye,ZEye);//视点
	m_aView->At(XAt,YAt,ZAt);//VRC原点
	gp_Pnt EyePoint(XEye,YEye,ZEye);
	gp_Pnt AtPoint(XAt,YAt,ZAt);

	gp_Vec EyeVector(EyePoint,AtPoint);
	gp_Dir EyeDir(EyeVector);//视线

	gp_Pln PlaneOfTheView = gp_Pln(AtPoint,EyeDir);//视平面

	//屏幕点转换为模型空间（或世界坐标系）的点。
	Standard_Real X,Y,Z;
	m_aView->Convert((int)x,(int)y,X,Y,Z);
	//但点可能不在视平面上，因此要将点投影到视平面，再计算视平面上的点。
	gp_Pnt ConvertedPoint(X,Y,Z);
	gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView,ConvertedPoint);
	
	gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
									ConvertedPointOnPlane.Y(),
									PlaneOfTheView);

#ifdef _VERBOSE_DEBUG
	DTRACE("\n Eye [%.4f,%.4f,%.4f] At [%.4f,%.4f,%.4f],viewpoint [%.4f,%.4f,%.4f],result [%.4f,%.4f,%.4f].",
		XEye,YEye,ZEye,XAt,YAt,ZAt,X,Y,Z,ResultPoint.X(),ResultPoint.Y(),ResultPoint.Z());
#endif

	//默认获取点是WCS。
	//非WCS，还需要转换到UCS中。
	if(bWCS == FALSE)
	{
		WCSToUCS(ResultPoint);
	}

	return ResultPoint;
}

//屏幕点转换为cs的平面上的点
gp_Pnt	ScView::PixelToCSPlane(Standard_Real x, Standard_Real y,BOOL bWCS)
{
	//a、先将屏幕点转换为视平面上的点。
//yxk20200104
	//double vx,vy;
	//m_aView->Convert((int)x,(int)y,vx,vy);

	//b、从视平面点按视方向投影直线。
	//Select3D_Projector prj(m_aView);
	//gp_Lin aL = prj.Shoot(vx,vy);
	gp_Lin aL = GetEyeLine(x,y);
//endyxk
	//c、计算要投影平面。如果当前是UCS，则是UCS中平面，否则是WCS中平面
	gp_Pnt	planePnt(0,0,0);
	gp_Dir	planDir;
	switch(m_nViewType){
		case ScView_Front:
			planDir.SetCoord(0.0,0.0,1.0);
			break;
		case ScView_Back:
			planDir.SetCoord(0.0,0.0,-1.0);
			break;
		case ScView_Top:
			planDir.SetCoord(0.0,1.0,0.0);
			break;
		case ScView_Bottom://XZ平面
			planDir.SetCoord(0.0,-1.0,0.0);
			break;
		case ScView_Left:
			planDir.SetCoord(1.0,0.0,0.0);
			break;
		case ScView_Right://YZ平面
			planDir.SetCoord(-1.0,0.0,0.0);
			break;
		default:
			break;
	}

	ScUcs *pScUcs = this->m_pScUcsMgr->CurrentUCS();
	if(pScUcs)
	{//则为UCS中平面，转入WCS中
		planePnt.Transform(pScUcs->UtowTrsf());
		planDir.Transform(pScUcs->UtowTrsf());
	}

	gp_Pln aPlan = gp_Pln(planePnt,planDir);

	//d、计算投影线和平面的交线
	gp_Pnt pntResult(0.0,0.0,0.0);
	this->ComputeLinPlanIntersection(aL,aPlan,pntResult);

#ifdef _VERBOSE_DEBUG
	V3d_Coordinate XEye,YEye,ZEye,XAt,YAt,ZAt;
	m_aView->Eye(XEye,YEye,ZEye);//视点
	m_aView->At(XAt,YAt,ZAt);//VRC原点
	gp_Pnt EyePoint(XEye,YEye,ZEye);
	gp_Pnt AtPoint(XAt,YAt,ZAt);
	DTRACE("\n Eye [%.4f,%.4f,%.4f] At [%.4f,%.4f,%.4f],result [%.4f,%.4f,%.4f].",
		XEye,YEye,ZEye,XAt,YAt,ZAt,pntResult.X(),pntResult.Y(),pntResult.Z());
#endif

	//如果需要UCS坐标，转换到UCS中
	if(bWCS == FALSE)
	{
		WCSToUCS(pntResult);
	}

	return pntResult;
}

//
// 屏幕点转换为当前坐标系中的点，根据情况显示不同的点。
// 可能是工作平面上的点，可能是空间点。
// 如果存在UCS，则需要转到UCS中。
//
gp_Pnt	ScView::PixelToObjCS(int x,int y)
{
	gp_Pnt pntRes;

	if(this->m_bNeedModelPoint)
	{
		pntRes = this->PixelToModel(x,y,FALSE);
	}
	else
	{
		pntRes = this->PixelToCSPlane(x,y,FALSE);
	}

	return pntRes;
}

//坐标转换函数
int		ScView::WCSToUCS(gp_Pnt& pnt)
{
	ASSERT(this->m_pScUcsMgr);

	ScUcs *pScUcs = this->m_pScUcsMgr->CurrentUCS();
	if(pScUcs)
	{
		pnt.Transform(pScUcs->WtouTrsf());
	}

	return 0;
}

int		ScView::UCSToWCS(gp_Pnt& pnt)
{
	ASSERT(this->m_pScUcsMgr);

	ScUcs *pScUcs = this->m_pScUcsMgr->CurrentUCS();
	if(pScUcs)
	{
		pnt.Transform(pScUcs->UtowTrsf());
	}

	return 0;
}

int		ScView::WCSToUCS(gp_Vec& vec)
{
	ASSERT(this->m_pScUcsMgr);

	ScUcs *pScUcs = this->m_pScUcsMgr->CurrentUCS();
	if(pScUcs)
	{
		vec.Transform(pScUcs->WtouTrsf());
	}

	return 0;
}

int		ScView::UCSToWCS(gp_Vec& vec)
{
	ASSERT(this->m_pScUcsMgr);

	ScUcs *pScUcs = this->m_pScUcsMgr->CurrentUCS();
	if(pScUcs)
	{
		vec.Transform(pScUcs->UtowTrsf());
	}

	return 0;
}

gp_Pnt ConvertClickToPoint(Standard_Real x, Standard_Real y, Handle(V3d_View) aView);
//视线方向为眼坐标的负z方向
gp_Lin	ScView::GetEyeLine(int nX,int nY)
{
	//a、先将屏幕点转换为视平面上的点。

	//b、从视平面点按视方向投影直线。
//yxk20200104
	//double vx,vy;
	//m_aView->Convert(nX,nY,vx,vy);
	//Select3D_Projector prj(m_aView);
	//return prj.Shoot(vx,vy);
	Standard_Real XEye, YEye, ZEye;
	m_aView->Eye(XEye, YEye, ZEye);
	gp_Pnt EyePoint(XEye, YEye, ZEye);
	gp_Pnt PickPoint = ConvertClickToPoint(nX, nY, m_aView);
	gp_Lin eyeLine(EyePoint, gp_Vec(EyePoint, PickPoint));
	return eyeLine;
//endyxk
}

gp_Pnt ConvertClickToPoint(Standard_Real x, Standard_Real y, Handle(V3d_View) aView)
{
	Standard_Real XEye, YEye, ZEye, XAt, YAt, ZAt;
	aView->Eye(XEye, YEye, ZEye);
	aView->At(XAt, YAt, ZAt);
	gp_Pnt EyePoint(XEye, YEye, ZEye);
	gp_Pnt AtPoint(XAt, YAt, ZAt);

	gp_Vec EyeVector(EyePoint, AtPoint);
	gp_Dir EyeDir(EyeVector);

	//yxk 视图投影面，即at点所在平面
	gp_Pln PlaneOfTheView = gp_Pln(AtPoint, EyeDir);
	Standard_Real X, Y, Z;
	//yxk 将屏幕坐标（x,y)转换为世界坐标，其中z值取far裁剪面，即在归一化坐标系下是1
	aView->Convert(int(x), int(y), X, Y, Z);
	gp_Pnt ConvertedPoint(X, Y, Z);
	//将点投影到投影面上，返回坐标为投影面坐标系uv坐标
	gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView, ConvertedPoint);
	//yxk将平面上的投影坐标转换为世界坐标系下坐标
	gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
		ConvertedPointOnPlane.Y(),
		PlaneOfTheView);
	return ResultPoint;
}




//获取curve上点击的点。
//即当点击选中curve时，获取此时点击的点。
//结果WCS点。
BOOL	ScView::GetEdgePickPoint(const TopoDS_Edge& aEdge,
								 int nX,int nY,
								 double dTol,
								 double& t,gp_Pnt& pickPnt)
{
	//获取眼射线
	gp_Lin eyeLin = GetEyeLine(nX,nY);
	Handle(Geom_Line) aL = new Geom_Line(eyeLin);

	//获取edge的信息
	double dFirst,dLast;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge,dFirst,dLast);
	if(dLast < dFirst)
	{
			double dtmp = dFirst;dFirst = dLast;dLast = dtmp;
	}

	//求最近点。
	GeomAPI_ExtremaCurveCurve extCC(aL,aCurve);

	if(extCC.NbExtrema() == 0)
		return FALSE;

	gp_Pnt pt1,pt2;
	extCC.NearestPoints(pt1,pt2);

	//两点转为屏幕坐标,由于第一点为眼射线上的点，转换应当为nx,ny，这里不转换。
	int x2,y2;
	this->m_aView->Convert(pt2.X(),pt2.Y(),pt2.Z(),x2,y2);

	double dist = sqrt(double((nX - x2) * (nX - x2) +
		(nY - y2) * (nY - y2)));
	if(dist > dTol)
		return FALSE;

	pickPnt = pt2;
	double t1;
	extCC.LowerDistanceParameters(t1,t);

	//需要判断是否落在参数范围内
	if(t < dFirst || t > dLast)
			return FALSE;

	return TRUE;
}

//获取edge上离nx，ny最近的投影点。如果超出范围，返回端点。
BOOL	ScView::GetEdgeNearestPoint(const TopoDS_Edge& aEdge,
								int nX,int nY,
								double& t,gp_Pnt& pnt)
{
	double dist;
	return this->GetEdgeNearestPoint(aEdge,nX,nY,t,pnt,dist);
}

//获取edge上离nx，ny最近的投影点。如果超出范围，返回端点。
BOOL	ScView::GetEdgeNearestPoint(const TopoDS_Edge& aEdge,
								int nX,int nY,
								double& t,gp_Pnt& pnt,double& dist)
{
	//获取眼射线
	gp_Lin eyeLin = GetEyeLine(nX,nY);
	Handle(Geom_Line) aL = new Geom_Line(eyeLin);

	//获取edge的信息
	double dFirst,dLast;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge,dFirst,dLast);

	//求最近点。
	GeomAPI_ExtremaCurveCurve extCC(aL,aCurve);

	if(extCC.NbExtrema() == 0)
		return FALSE;

	double t1,t2;
	gp_Pnt pt1,pt2;
	extCC.NearestPoints(pt1,pt2);
	extCC.LowerDistanceParameters(t1,t2);
	//记录距离
	dist = extCC.LowerDistance();

	if(dFirst > dLast)
	{
		double dtmp = dLast;
		dLast = dFirst;
		dFirst = dtmp;
	}
	if(t2 < dFirst)
	{
		t = dFirst;
		BRepAdaptor_Curve baCur(aEdge);
		baCur.D0(t,pnt);
	}
	else if(t2 > dLast)
	{
		t = dLast;
		BRepAdaptor_Curve baCur(aEdge);
		baCur.D0(t,pnt);
	}
	else
	{
		t = t2;
		pnt = pt2;
	}

	return TRUE;
}


//获取face上点击的点。
//即当点击选中face时，获取此时点击的点。
//结果WCS点。
BOOL	ScView::GetFacePickPoint(const TopoDS_Face& aFace,
								int nx,int ny,
								double& u,double& v,gp_Pnt& pickPnt)
{
	//获取眼射线
	gp_Lin eyeLin = GetEyeLine(nx,ny);
	Handle(Geom_Line) aL = new Geom_Line(eyeLin);

	//求曲面
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
	if(aSurf.IsNull())
		return FALSE;

	//求交
	GeomAPI_IntCS intCS;
	try
	{
		intCS.Perform(aL,aSurf);
	}
	catch(Standard_Failure)
	{
		return FALSE;
	}

	if(!intCS.IsDone())
		return FALSE;

	BRepClass_FaceClassifier fcls;
	gp_Pnt2d puv;
	TopAbs_State state;
	double Tol = BRep_Tool::Tolerance(aFace);
	double ut,vt,w;
	double minW = 1e10;
	BOOL   bOk = FALSE;
	//寻找曲面内部的最近的点。由于交点都位于直线上，而w返回直线上参数。
	//直线方向为眼坐标的负z方向，指向屏幕内部，因此w越小，则越近。
	for(int ix = 1;ix <= intCS.NbPoints();ix++)
	{
		gp_Pnt tp = intCS.Point(ix);
		intCS.Parameters(ix,ut,vt,w);
		//是否在face内部
		puv.SetCoord(ut,vt);
		fcls.Perform(aFace,puv,Tol);
		state = fcls.State();
		if(state == TopAbs_ON || state == TopAbs_IN)
		{//在face内部。
			if(w < minW)
			{
				minW = w;
				u = ut;v = vt;
				pickPnt = tp;
				bOk = TRUE;
			}
		}
	}

	return bOk;
}

//获取shape上点击的点。
//即当点击选中shape时，获取此时点击的点。
//结果WCS点。
gp_Pnt	ScView::GetShapePickPoint(const TopoDS_Shape& aShape,int nx,int ny)
{
	gp_Pnt pickPnt;
	return pickPnt;
}

//
// 计算点pnt到当前工作平面的高度.Pnt为WCS点。
//
double	ScView::ComputeHeight(const gp_Pnt& pnt)
{
	double dH = 0.0;

	gp_Ax2 ax = this->CurrentAxis(FALSE);
	//矢量
	gp_Vec vec(ax.Location(),pnt);
	//pnt和cs平面的距离。
	dH = vec.XYZ().Dot(ax.Direction().XYZ());
	
	return dH;
}

//当前的ax坐标系，根据平面不同而不同
gp_Ax2	ScView::CurrentAxis(BOOL bUCS)
{
	gp_Pnt	planePnt(0,0,0);
	gp_Dir	planDir,XDir;
	switch(m_nViewType){
		case ScView_Front:
			planDir.SetCoord(0.0,0.0,1.0);
			XDir.SetCoord(1.0,0.0,0.0);
			break;
		case ScView_Back:
			planDir.SetCoord(0.0,0.0,-1.0);
			XDir.SetCoord(1.0,0.0,0.0);
			break;
		case ScView_Top:
			planDir.SetCoord(0.0,1.0,0.0);
			XDir.SetCoord(1.0,0.0,0.0);
			break;
		case ScView_Bottom://XZ平面
			planDir.SetCoord(0.0,-1.0,0.0);
			XDir.SetCoord(1.0,0.0,0.0);
			break;
		case ScView_Left:
			planDir.SetCoord(1.0,0.0,0.0);
			XDir.SetCoord(0.0,1.0,0.0);
			break;
		case ScView_Right://YZ平面
			planDir.SetCoord(-1.0,0.0,0.0);
			XDir.SetCoord(0.0,1.0,0.0);
			break;
		default:
			break;
	}

	if(!bUCS)
	{
		ScUcs *pScUcs = this->m_pScUcsMgr->CurrentUCS();
		if(pScUcs)
		{//则为UCS中平面，转入WCS中
			planePnt.Transform(pScUcs->UtowTrsf());
			planDir.Transform(pScUcs->UtowTrsf());
			XDir.Transform(pScUcs->UtowTrsf());
		}
	}

	gp_Ax2 ax(planePnt,planDir,XDir);
	return ax;
}

//计算直线和平面的交点
BOOL	ScView::ComputeLinPlanIntersection(const gp_Lin& aL,
							const gp_Pln& aPlan,gp_Pnt& aInterPnt)
{
	static IntAna_Quadric aQuad;
	aQuad.SetQuadric(aPlan);
	static IntAna_IntConicQuad QQ;

	QQ.Perform(aL,aQuad);
	if(QQ.IsDone())
	{
		if(QQ.NbPoints() > 0)
		{
			aInterPnt = QQ.Point(1);
			return TRUE;
		}
	}

	return FALSE;
}

//
// 更新当前局部坐标.将At点设置为局部坐标的原点
//
void	ScView::UpdateUCS()
{
	//ScUcs *pScUcs = this->m_pScUcsMgr->CurrentUCS();
	//if(pScUcs)
	//{
	//	gp_Pnt pntAt = pScUcs->m_ax2.Location();
	//	this->m_aView->SetAt(pntAt.X(),pntAt.Y(),pntAt.Z());
	//}
	//ScUcs *pPreUcs = this->m_pScUcsMgr->PrevUCS();
	//ScUcs *pScUcs = this->m_pScUcsMgr->CurrentUCS();
	//gp_Ax2 fromAx,toAx;
	//if(pPreUcs)
	//	fromAx = pPreUcs->m_ax2;
	//else
	//	fromAx = gp::XOY();

	//if(pScUcs)
	//	toAx = pScUcs->m_ax2;
	//else
	//	toAx = gp::XOY();

	//gp_Trsf trsf;
	//trsf.SetTransformation(gp_Ax3(fromAx),gp_Ax3(toAx));
	////对视进行变换，变换到当前的UCS中。
	//double x,y,z;
	//this->m_aView->At(x,y,z);
	//gp_Pnt pntAt(x,y,z);
	//pntAt.Transform(trsf);

	//this->m_aView->Proj(x,y,z);
	//gp_Dir vpn(x,y,z);
	//vpn.Transform(trsf);

	//this->m_aView->Up(x,y,z);
	//gp_Dir vup(x,y,z);
	//vup.Transform(trsf);

	////重新设置
	//this->m_aView->SetAt(pntAt.X(),pntAt.Y(),pntAt.Z());
	//this->m_aView->SetProj(vpn.X(),vpn.Y(),vpn.Z());
	//this->m_aView->SetUp(vup.X(),vup.Y(),vup.Z());
}