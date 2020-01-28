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


//������ͼ����
int		ScView::SetProj(int nType)
{
	switch(nType)
	{
	case V3d_Zpos://ǰ��ͼ
		{
			m_nViewType = ScView_Front;
		}
		break;
	case V3d_Zneg://����ͼ
		{
			m_nViewType = ScView_Back;
		}
		break;
	case V3d_Xpos://����ͼ
		{
			m_nViewType = ScView_Left;
		}
		break;
	case V3d_Xneg://����ͼ
		{
			m_nViewType = ScView_Right;
		}
		break;
	case V3d_Ypos://����ͼ
		{
			m_nViewType = ScView_Top;
		}
		break;
	case V3d_Yneg://����ͼ
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

	//����Up
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
// ��Ļ��ת��Ϊ�ռ�(WCS or UCS)�еĵ㡣
//
gp_Pnt	ScView::PixelToModel(Standard_Real x, Standard_Real y,BOOL bWCS)
{
	V3d_Coordinate XEye,YEye,ZEye,XAt,YAt,ZAt;
	m_aView->Eye(XEye,YEye,ZEye);//�ӵ�
	m_aView->At(XAt,YAt,ZAt);//VRCԭ��
	gp_Pnt EyePoint(XEye,YEye,ZEye);
	gp_Pnt AtPoint(XAt,YAt,ZAt);

	gp_Vec EyeVector(EyePoint,AtPoint);
	gp_Dir EyeDir(EyeVector);//����

	gp_Pln PlaneOfTheView = gp_Pln(AtPoint,EyeDir);//��ƽ��

	//��Ļ��ת��Ϊģ�Ϳռ䣨����������ϵ���ĵ㡣
	Standard_Real X,Y,Z;
	m_aView->Convert((int)x,(int)y,X,Y,Z);
	//������ܲ�����ƽ���ϣ����Ҫ����ͶӰ����ƽ�棬�ټ�����ƽ���ϵĵ㡣
	gp_Pnt ConvertedPoint(X,Y,Z);
	gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView,ConvertedPoint);
	
	gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
									ConvertedPointOnPlane.Y(),
									PlaneOfTheView);

#ifdef _VERBOSE_DEBUG
	DTRACE("\n Eye [%.4f,%.4f,%.4f] At [%.4f,%.4f,%.4f],viewpoint [%.4f,%.4f,%.4f],result [%.4f,%.4f,%.4f].",
		XEye,YEye,ZEye,XAt,YAt,ZAt,X,Y,Z,ResultPoint.X(),ResultPoint.Y(),ResultPoint.Z());
#endif

	//Ĭ�ϻ�ȡ����WCS��
	//��WCS������Ҫת����UCS�С�
	if(bWCS == FALSE)
	{
		WCSToUCS(ResultPoint);
	}

	return ResultPoint;
}

//��Ļ��ת��Ϊcs��ƽ���ϵĵ�
gp_Pnt	ScView::PixelToCSPlane(Standard_Real x, Standard_Real y,BOOL bWCS)
{
	//a���Ƚ���Ļ��ת��Ϊ��ƽ���ϵĵ㡣
//yxk20200104
	//double vx,vy;
	//m_aView->Convert((int)x,(int)y,vx,vy);

	//b������ƽ��㰴�ӷ���ͶӰֱ�ߡ�
	//Select3D_Projector prj(m_aView);
	//gp_Lin aL = prj.Shoot(vx,vy);
	gp_Lin aL = GetEyeLine(x,y);
//endyxk
	//c������ҪͶӰƽ�档�����ǰ��UCS������UCS��ƽ�棬������WCS��ƽ��
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
		case ScView_Bottom://XZƽ��
			planDir.SetCoord(0.0,-1.0,0.0);
			break;
		case ScView_Left:
			planDir.SetCoord(1.0,0.0,0.0);
			break;
		case ScView_Right://YZƽ��
			planDir.SetCoord(-1.0,0.0,0.0);
			break;
		default:
			break;
	}

	ScUcs *pScUcs = this->m_pScUcsMgr->CurrentUCS();
	if(pScUcs)
	{//��ΪUCS��ƽ�棬ת��WCS��
		planePnt.Transform(pScUcs->UtowTrsf());
		planDir.Transform(pScUcs->UtowTrsf());
	}

	gp_Pln aPlan = gp_Pln(planePnt,planDir);

	//d������ͶӰ�ߺ�ƽ��Ľ���
	gp_Pnt pntResult(0.0,0.0,0.0);
	this->ComputeLinPlanIntersection(aL,aPlan,pntResult);

#ifdef _VERBOSE_DEBUG
	V3d_Coordinate XEye,YEye,ZEye,XAt,YAt,ZAt;
	m_aView->Eye(XEye,YEye,ZEye);//�ӵ�
	m_aView->At(XAt,YAt,ZAt);//VRCԭ��
	gp_Pnt EyePoint(XEye,YEye,ZEye);
	gp_Pnt AtPoint(XAt,YAt,ZAt);
	DTRACE("\n Eye [%.4f,%.4f,%.4f] At [%.4f,%.4f,%.4f],result [%.4f,%.4f,%.4f].",
		XEye,YEye,ZEye,XAt,YAt,ZAt,pntResult.X(),pntResult.Y(),pntResult.Z());
#endif

	//�����ҪUCS���꣬ת����UCS��
	if(bWCS == FALSE)
	{
		WCSToUCS(pntResult);
	}

	return pntResult;
}

//
// ��Ļ��ת��Ϊ��ǰ����ϵ�еĵ㣬���������ʾ��ͬ�ĵ㡣
// �����ǹ���ƽ���ϵĵ㣬�����ǿռ�㡣
// �������UCS������Ҫת��UCS�С�
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

//����ת������
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
//���߷���Ϊ������ĸ�z����
gp_Lin	ScView::GetEyeLine(int nX,int nY)
{
	//a���Ƚ���Ļ��ת��Ϊ��ƽ���ϵĵ㡣

	//b������ƽ��㰴�ӷ���ͶӰֱ�ߡ�
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

	//yxk ��ͼͶӰ�棬��at������ƽ��
	gp_Pln PlaneOfTheView = gp_Pln(AtPoint, EyeDir);
	Standard_Real X, Y, Z;
	//yxk ����Ļ���꣨x,y)ת��Ϊ�������꣬����zֵȡfar�ü��棬���ڹ�һ������ϵ����1
	aView->Convert(int(x), int(y), X, Y, Z);
	gp_Pnt ConvertedPoint(X, Y, Z);
	//����ͶӰ��ͶӰ���ϣ���������ΪͶӰ������ϵuv����
	gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView, ConvertedPoint);
	//yxk��ƽ���ϵ�ͶӰ����ת��Ϊ��������ϵ������
	gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
		ConvertedPointOnPlane.Y(),
		PlaneOfTheView);
	return ResultPoint;
}




//��ȡcurve�ϵ���ĵ㡣
//�������ѡ��curveʱ����ȡ��ʱ����ĵ㡣
//���WCS�㡣
BOOL	ScView::GetEdgePickPoint(const TopoDS_Edge& aEdge,
								 int nX,int nY,
								 double dTol,
								 double& t,gp_Pnt& pickPnt)
{
	//��ȡ������
	gp_Lin eyeLin = GetEyeLine(nX,nY);
	Handle(Geom_Line) aL = new Geom_Line(eyeLin);

	//��ȡedge����Ϣ
	double dFirst,dLast;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge,dFirst,dLast);
	if(dLast < dFirst)
	{
			double dtmp = dFirst;dFirst = dLast;dLast = dtmp;
	}

	//������㡣
	GeomAPI_ExtremaCurveCurve extCC(aL,aCurve);

	if(extCC.NbExtrema() == 0)
		return FALSE;

	gp_Pnt pt1,pt2;
	extCC.NearestPoints(pt1,pt2);

	//����תΪ��Ļ����,���ڵ�һ��Ϊ�������ϵĵ㣬ת��Ӧ��Ϊnx,ny�����ﲻת����
	int x2,y2;
	this->m_aView->Convert(pt2.X(),pt2.Y(),pt2.Z(),x2,y2);

	double dist = sqrt(double((nX - x2) * (nX - x2) +
		(nY - y2) * (nY - y2)));
	if(dist > dTol)
		return FALSE;

	pickPnt = pt2;
	double t1;
	extCC.LowerDistanceParameters(t1,t);

	//��Ҫ�ж��Ƿ����ڲ�����Χ��
	if(t < dFirst || t > dLast)
			return FALSE;

	return TRUE;
}

//��ȡedge����nx��ny�����ͶӰ�㡣���������Χ�����ض˵㡣
BOOL	ScView::GetEdgeNearestPoint(const TopoDS_Edge& aEdge,
								int nX,int nY,
								double& t,gp_Pnt& pnt)
{
	double dist;
	return this->GetEdgeNearestPoint(aEdge,nX,nY,t,pnt,dist);
}

//��ȡedge����nx��ny�����ͶӰ�㡣���������Χ�����ض˵㡣
BOOL	ScView::GetEdgeNearestPoint(const TopoDS_Edge& aEdge,
								int nX,int nY,
								double& t,gp_Pnt& pnt,double& dist)
{
	//��ȡ������
	gp_Lin eyeLin = GetEyeLine(nX,nY);
	Handle(Geom_Line) aL = new Geom_Line(eyeLin);

	//��ȡedge����Ϣ
	double dFirst,dLast;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge,dFirst,dLast);

	//������㡣
	GeomAPI_ExtremaCurveCurve extCC(aL,aCurve);

	if(extCC.NbExtrema() == 0)
		return FALSE;

	double t1,t2;
	gp_Pnt pt1,pt2;
	extCC.NearestPoints(pt1,pt2);
	extCC.LowerDistanceParameters(t1,t2);
	//��¼����
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


//��ȡface�ϵ���ĵ㡣
//�������ѡ��faceʱ����ȡ��ʱ����ĵ㡣
//���WCS�㡣
BOOL	ScView::GetFacePickPoint(const TopoDS_Face& aFace,
								int nx,int ny,
								double& u,double& v,gp_Pnt& pickPnt)
{
	//��ȡ������
	gp_Lin eyeLin = GetEyeLine(nx,ny);
	Handle(Geom_Line) aL = new Geom_Line(eyeLin);

	//������
	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
	if(aSurf.IsNull())
		return FALSE;

	//��
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
	//Ѱ�������ڲ�������ĵ㡣���ڽ��㶼λ��ֱ���ϣ���w����ֱ���ϲ�����
	//ֱ�߷���Ϊ������ĸ�z����ָ����Ļ�ڲ������wԽС����Խ����
	for(int ix = 1;ix <= intCS.NbPoints();ix++)
	{
		gp_Pnt tp = intCS.Point(ix);
		intCS.Parameters(ix,ut,vt,w);
		//�Ƿ���face�ڲ�
		puv.SetCoord(ut,vt);
		fcls.Perform(aFace,puv,Tol);
		state = fcls.State();
		if(state == TopAbs_ON || state == TopAbs_IN)
		{//��face�ڲ���
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

//��ȡshape�ϵ���ĵ㡣
//�������ѡ��shapeʱ����ȡ��ʱ����ĵ㡣
//���WCS�㡣
gp_Pnt	ScView::GetShapePickPoint(const TopoDS_Shape& aShape,int nx,int ny)
{
	gp_Pnt pickPnt;
	return pickPnt;
}

//
// �����pnt����ǰ����ƽ��ĸ߶�.PntΪWCS�㡣
//
double	ScView::ComputeHeight(const gp_Pnt& pnt)
{
	double dH = 0.0;

	gp_Ax2 ax = this->CurrentAxis(FALSE);
	//ʸ��
	gp_Vec vec(ax.Location(),pnt);
	//pnt��csƽ��ľ��롣
	dH = vec.XYZ().Dot(ax.Direction().XYZ());
	
	return dH;
}

//��ǰ��ax����ϵ������ƽ�治ͬ����ͬ
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
		case ScView_Bottom://XZƽ��
			planDir.SetCoord(0.0,-1.0,0.0);
			XDir.SetCoord(1.0,0.0,0.0);
			break;
		case ScView_Left:
			planDir.SetCoord(1.0,0.0,0.0);
			XDir.SetCoord(0.0,1.0,0.0);
			break;
		case ScView_Right://YZƽ��
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
		{//��ΪUCS��ƽ�棬ת��WCS��
			planePnt.Transform(pScUcs->UtowTrsf());
			planDir.Transform(pScUcs->UtowTrsf());
			XDir.Transform(pScUcs->UtowTrsf());
		}
	}

	gp_Ax2 ax(planePnt,planDir,XDir);
	return ax;
}

//����ֱ�ߺ�ƽ��Ľ���
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
// ���µ�ǰ�ֲ�����.��At������Ϊ�ֲ������ԭ��
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
	////���ӽ��б任���任����ǰ��UCS�С�
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

	////��������
	//this->m_aView->SetAt(pntAt.X(),pntAt.Y(),pntAt.Z());
	//this->m_aView->SetProj(vpn.X(),vpn.Y(),vpn.Z());
	//this->m_aView->SetUp(vup.X(),vup.Y(),vup.Z());
}