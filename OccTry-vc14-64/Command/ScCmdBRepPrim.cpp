#include "StdAfx.h"

#include <ElSLib.hxx>
#include <BRepAlgo_Common.hxx>
#include <Precision.hxx>

#include "ScTools.h"
#include "ScCmdMgr.h"
#include "..\ScView.h"
#include ".\sccmdbrepprim.h"


ScCmdBRepPrim::ScCmdBRepPrim(void)
{
}

ScCmdBRepPrim::~ScCmdBRepPrim(void)
{
}

//�����ͽ�������
int	ScCmdBRepPrim::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	this->m_AISContext = aCtx;
	return 0;
}

int	ScCmdBRepPrim::End()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
ScCmdSphere::ScCmdSphere()
{
}

ScCmdSphere::~ScCmdSphere()
{
}

//�����ͽ�������
int	ScCmdSphere::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCmdBRepPrim::Begin(aCtx);

	SetState(SPH_GET_CENTER);
	Prompt("��ȡ����:");
	NeedInput("��������:",INPUT_POINT3D);

	return 0;
}

int	ScCmdSphere::End()
{
	if(!IsDone())
	{//û���������
	}

	ScCmdBRepPrim::End();

	return 0;
}

//���ո�����
int		ScCmdSphere::AcceptInput(double dVal) 
{
	if(GetState() == SPH_GET_RADIUS)
	{
		m_dRadius = dVal;

		if(m_bFirst)
		{
			m_bFirst = FALSE;
		}
		else
		{
			//ȥ���ɵ���ʾ
			RemoveCtxObj(m_aShape);
		}
		//��ʾ�µ�
		TopoDS_Shape aS = BRepPrimAPI_MakeSphere(this->m_ptCenter,m_dRadius);
		m_aShape = new AIS_Shape(aS);
		m_AISContext->Display(m_aShape, Standard_True);

		SetState(SPH_END);
		Done();
	}
	return SC_OK; 
}
//����3ά��
int		ScCmdSphere::AcceptInput(gp_Pnt& pnt) 
{ 
	if(GetState() == SPH_GET_CENTER)
	{
		this->m_ptCenter = pnt;
		UCSToWCS(m_ptCenter);

		EndInput();
		SetState(SPH_GET_RADIUS);
		Prompt("��ȡ�뾶:");
		NeedInput("��뾶:",INPUT_DOUBLE);
	}
	return SC_OK; 
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdSphere::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState != MINPUT_LEFT)
		return;

	switch(GetState())
	{
	case SPH_GET_CENTER:
		{
			this->m_ptCenter = PixelToCSPlane(point.x,point.y);

			EndInput();
			SetState(SPH_GET_RADIUS);
			Prompt("��ȡ�뾶:");
			NeedInput("��뾶:",INPUT_DOUBLE);
		}
		break;
	case SPH_GET_RADIUS:
		{
			SetState(SPH_END);
			Done();
		}
		break;
	default:
		break;
	}
}


//����ƶ�����Ӧ����
void	ScCmdSphere::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case SPH_GET_CENTER:
		{
		}
		break;
	case SPH_GET_RADIUS:
		{
			//��ȡ�뾶
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			this->m_dRadius = pnt.Distance(this->m_ptCenter);

			Show(this->m_dRadius);

			if(m_bFirst)
			{
				m_bFirst = FALSE;
			}
			else
			{
				//ȥ���ɵ���ʾ
				RemoveCtxObj(m_aShape);
			}
			//��ʾ�µ�
			TopoDS_Shape aS = BRepPrimAPI_MakeSphere(this->m_ptCenter,m_dRadius);
			m_aShape = new AIS_Shape(aS);
			m_AISContext->Display(m_aShape, Standard_True);
		}
		break;
	};
}

///////////////////////////////////////////////////////////////////////////
//
ScCmdBox::ScCmdBox()
{
}

ScCmdBox::~ScCmdBox()
{
}

//�����ͽ�������
int	ScCmdBox::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SetState(BOX_POINT1);
	Prompt("�����һ��:");

	return 0;
}

int	ScCmdBox::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdBox::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState != MINPUT_LEFT)
		return;

	gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
	DoAcceptPoint(pnt);
}

//����ƶ�����Ӧ����
void	ScCmdBox::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case BOX_POINT2:
		{
			RemoveCtxObj(this->m_aShape);
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			m_pnt2 = pnt;

			TopoDS_Face aFace = BuildBottom();
			if(!aFace.IsNull())
			{
				m_aShape = new AIS_Shape(aFace);
				m_AISContext->Display(m_aShape, Standard_True);
			}
		}
		break;
	case BOX_HEIGHT:
		{
			RemoveCtxObj(this->m_aShape);

			gp_Pnt pnt = PixelToModel(point.x,point.y);
			m_dH = this->m_pScView->ComputeHeight(pnt);

			TopoDS_Shape aS = BuildBox();
			if(!aS.IsNull())
			{
				m_aShape = new AIS_Shape(aS);
				m_AISContext->Display(m_aShape, Standard_True);
			}
		}
		break;
	default:
		break;
	}
}

//����3ά��
int		ScCmdBox::AcceptInput(gp_Pnt& pnt)
{
	DoAcceptPoint(pnt);
	return 0;
}

int		ScCmdBox::DoAcceptPoint(gp_Pnt& pnt)
{
	switch(GetState())
	{
	case BOX_POINT1:
		{
			m_pnt1 = pnt;

			SetState(BOX_POINT2);
			Prompt("����ڶ���:");
		}
		break;
	case BOX_POINT2:
		{
			
			if(m_pnt1.Distance(pnt) < Precision::Confusion())
				return -1;
	
			gp_Vec vec(m_pnt1,pnt);
			gp_Ax2 ax = this->m_pScView->CurrentAxis(FALSE);
			if(vec.IsParallel(gp_Vec(ax.XDirection()),0.001) ||
				vec.IsParallel(gp_Vec(ax.YDirection()),0.001))
				return -1;

			m_pnt2 = pnt;

			SetState(BOX_HEIGHT);
			Prompt("����߶�:");
			this->m_pScView->NeedModelPoint(TRUE);
		}
		break;
	case BOX_HEIGHT:
		{
			this->m_pScView->NeedModelPoint(FALSE);
			Done();
		}
		break;
	default:
		break;
	}

	return 0;
}

//
// �����Խǵ㴴��һ��face��
//
TopoDS_Face		ScCmdBox::BuildBottom()
{
	TopoDS_Face aFace;
	if(m_pnt1.Distance(m_pnt2) < Precision::Confusion())
		return aFace;
	
	gp_Vec vec(m_pnt1,m_pnt2);
	gp_Ax2 ax = this->m_pScView->CurrentAxis(FALSE);
	if(vec.IsParallel(gp_Vec(ax.XDirection()),0.001) ||
		vec.IsParallel(gp_Vec(ax.YDirection()),0.001))
		return aFace;

	gp_Pln pln = gce_MakePln(m_pnt1,ax.Direction());

	double u,v,u1,v1,u2,v2;
	ElSLib::PlaneParameters(pln.Position(),m_pnt2,u,v);
	u1 = min(0.,u);
	u2 = std::max(0.0,u);
	v1 = min(0.,v);
	v2 = std::max(0.0,v);

	BRepBuilderAPI_MakeFace mf(pln,u1,u2,v1,v2);
	if(mf.IsDone())
	{
		aFace = mf.Face();
	}

	return aFace;
}

TopoDS_Shape	ScCmdBox::BuildBox()
{
	TopoDS_Shape aShape;
	if(fabs(m_dH) < Precision::Confusion())
		return aShape;

	double dx = m_pnt2.X() - m_pnt1.X();
	double dy = m_pnt2.Y() - m_pnt1.Y();
	double dz = m_dH;
	if(fabs(dx) < Precision::Confusion() || fabs(dy) < Precision::Confusion()
		|| fabs(dz) < Precision::Confusion())
		return aShape;

	aShape = BRepPrimAPI_MakeBox(m_pnt1,dx,dy,dz);

	return aShape;
}

//////////////////////////////////////////////////////////////////////////////
//
ScCmdCyl::ScCmdCyl(BOOL bWithAngle)
{
	this->m_bWithAngle = bWithAngle;
}

ScCmdCyl::~ScCmdCyl()
{
}

//�����ͽ�������
int	  ScCmdCyl::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(CYL_CENTER,"Բ��:");

	return 0;
}

int	  ScCmdCyl::End()
{
	RemoveCtxObj(TmpObj(0));

	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdCyl::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case CYL_CENTER:
		{
			m_ptC = PixelToCSPlane(point.x,point.y);

			SwitchState(CYL_RADIUS,"�뾶:");
		}
		break;
	case CYL_RADIUS:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_ptC) < Precision::Confusion())
				return;

			m_pt1 = pnt;
			if(this->m_bWithAngle)
			{
				SwitchState(CYL_ANGLE,"�Ƕ�");
			}
			else
			{
				SwitchState(CYL_HEIGHT,"�߶�",TRUE);
			}
		}
		break;
	case CYL_ANGLE:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_pt1) < Precision::Confusion())
				return;

			m_pt2 = pnt;

			SwitchState(CYL_HEIGHT,"�߶�",TRUE);
		}
		break;
	case CYL_HEIGHT:
		{
			RemoveCtxObj(TmpObj(0));
			TopoDS_Shape aS = BuildCyl();
			Display(NewObj(0),aS);

			Done();
		}
		break;
	default:
		break;
	}
}

//����ƶ�����Ӧ����
void	ScCmdCyl::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case CYL_RADIUS:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_ptC) < Precision::Confusion())
				return;

			if(this->m_bWithAngle)
			{
				RemoveCtxObj(TmpObj(0));
				TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_ptC,pnt);
				Display(TmpObj(0),aE);
			}
			else
			{
				m_pt1 = pnt;
				RemoveCtxObj(TmpObj(0));
				TopoDS_Shape aS = BuildCyl();
				Display(TmpObj(0),aS);		
			}
		}
		break;
	case CYL_ANGLE:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_pt1) < Precision::Confusion())
				return;

			m_pt2 = pnt;

			RemoveCtxObj(TmpObj(0));
			TopoDS_Shape aS = BuildCyl();//BuildBottom();// //
			Display(TmpObj(0),aS);
		}
		break;
	case CYL_HEIGHT:
		{
			gp_Pnt pnt = PixelToModel(point.x,point.y);
			double dH = this->m_pScView->ComputeHeight(pnt);

			if(dH < Precision::Confusion())
				return;

			m_pt3 = pnt;
			
			RemoveCtxObj(TmpObj(0));
			TopoDS_Shape aS = BuildCyl();
			Display(TmpObj(0),aS);
		}
		break;
	default:
		break;
	}
}

//����3ά��
int		ScCmdCyl::AcceptInput(gp_Pnt& pnt)
{
	return 0;
}

//
// �ɻ�������
//
TopoDS_Shape	ScCmdCyl::BuildBottom()
{
	TopoDS_Face aS;

	if(this->m_bWithAngle)
	{
		TopoDS_Edge aE1 = BRepBuilderAPI_MakeEdge(m_ptC,m_pt1);
		TopoDS_Edge aE3 = BRepBuilderAPI_MakeEdge(m_pt2,m_ptC);
		//����Բ��
		double dR = m_pt1.Distance(m_ptC);
		gp_Ax2 ax = m_pScView->CurrentAxis(FALSE);
		ax.SetLocation(m_ptC);
		gp_Circ circ(ax,dR);

		GC_MakeArcOfCircle marc(circ,m_pt1,m_pt2,TRUE);
		if(marc.IsDone())
		{
			Handle(Geom_TrimmedCurve) aCur = marc.Value();
			TopoDS_Edge aE2 = BRepBuilderAPI_MakeEdge(aCur);

			TopoDS_Wire aW = BRepBuilderAPI_MakeWire(aE1,aE2,aE3);
			aS = BRepBuilderAPI_MakeFace(aW,TRUE);
		}
	}
	else
	{
		double dR = m_pt1.Distance(m_ptC);
		gp_Ax2 ax = m_pScView->CurrentAxis(FALSE);
		ax.SetLocation(m_ptC);
		gp_Circ circ(ax,dR);

		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(circ);
		TopoDS_Wire aW = BRepBuilderAPI_MakeWire(aE);
		aS = BRepBuilderAPI_MakeFace(aW,TRUE);
	}

	return aS;
}

TopoDS_Shape	ScCmdCyl::BuildCyl()
{
	TopoDS_Shape aS;
	double dR = m_pt1.Distance(m_ptC);
	gp_Ax2 ax = m_pScView->CurrentAxis(FALSE);
	double dH = this->m_pScView->ComputeHeight(m_pt3);
	if(dH < Precision::Confusion())
		dH =  10.0;

	if(dH > 0.0)
	{
		ax.SetLocation(m_ptC);
	}
	else
	{
		gp_Pnt loc = ax.Location();
		loc.SetX(loc.X() + dH * ax.Direction().X());
		loc.SetY(loc.Y() + dH * ax.Direction().Y());
		loc.SetZ(loc.Z() + dH * ax.Direction().Z());
		ax.SetLocation(loc);
	}
	
	if(this->m_bWithAngle)
	{
		double dA = ComputeAngle();
		//����X��.
		gp_Dir xD(m_pt1.X() - m_ptC.X(),m_pt1.Y() - m_ptC.Y(),m_pt1.Z() - m_ptC.Z());
		ax.SetXDirection(xD);

		try{
			aS = BRepPrimAPI_MakeCylinder(ax,dR,dH,dA);
		}
		catch(Standard_Failure)
		{
		}
	}
	else
	{
		try{
			aS = BRepPrimAPI_MakeCylinder(ax,dR,dH);
		}
		catch(Standard_Failure)
		{
		}
	}

	return aS;
}

double		ScCmdCyl::ComputeAngle()
{
	gp_Ax2 ax = m_pScView->CurrentAxis(FALSE);
	gp_Vec vec1(m_pt1.X() - m_ptC.X(),m_pt1.Y() - m_ptC.Y(),m_pt1.Z() - m_ptC.Z());
	gp_Vec vec2(m_pt2.X() - m_ptC.X(),m_pt2.Y() - m_ptC.Y(),m_pt2.Z() - m_ptC.Z());
	double dA = vec1.AngleWithRef(vec2,gp_Vec(ax.Direction()));
	if(dA < 0.0)
		dA += M_PI * 2.0;

	return dA;
}

///////////////////////////////////////////////////////////////
//
ScCmdCone::ScCmdCone()
{
}

ScCmdCone::~ScCmdCone()
{
}

//�����ͽ�������
int	ScCmdCone::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	return 0;
}

int	ScCmdCone::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdCone::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
}

//����ƶ�����Ӧ����
void	ScCmdCone::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}

//����3ά��
int		ScCmdCone::AcceptInput(gp_Pnt& pnt)
{
	return 0;
}

/////////////////////////////////////////////////////////////////
//
ScCmdPrism::ScCmdPrism(BOOL bTwoDir,BOOL bGenSurf)
{
	this->m_bTwoDir = bTwoDir;
	this->m_bGenSurf = bGenSurf;
}

ScCmdPrism::~ScCmdPrism()
{
}

//�����ͽ�������
int	ScCmdPrism::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	int nRes = GetOneSelShape(m_aSelShape);
	if((nRes == SEL_OK) && (!m_aSelShape.IsNull()) &&
		(CanPrism(m_aSelShape)))
	{
		SwitchState(S_GET_DIR_P1,"�������������");
	}
	else
	{
		SwitchState(S_GET_SHAPE,"ѡ��һ������");
		NeedSelect(TRUE);
	}

	return 0;
}

//
// ��������������ʵ���������֡�
//
BOOL	ScCmdPrism::CanPrism(const TopoDS_Shape& aShape)
{
	TopAbs_ShapeEnum eType = aShape.ShapeType();
	if(m_bGenSurf)
	{
		if(eType == TopAbs_WIRE || eType == TopAbs_EDGE)
			return TRUE;
	}
	else
	{
		if(eType == TopAbs_FACE || eType == TopAbs_SHELL)
			return TRUE;
	}

	return FALSE;
}

int	ScCmdPrism::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdPrism::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_GET_SHAPE:
		{
			int nRes = GetOneSelShape(m_aSelShape);
			if((nRes == SEL_OK) && (!m_aSelShape.IsNull()) &&
				(CanPrism(m_aSelShape)))
			{
				SwitchState(S_GET_DIR_P1,"�������������");
				NeedSelect(FALSE);
			}
		}
		break;
	case S_GET_DIR_P1:
		{
			m_pnt1 = PixelToCSPlane(point.x,point.y);

			SwitchState(S_GET_DIR_P2,"���������ĵڶ���",TRUE);
		}
		break;
	case S_GET_DIR_P2:
		{
			Done();
		}
		break;
	default:
		break;
	}
}

//����ƶ�����Ӧ����
void	ScCmdPrism::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_GET_SHAPE:
		{
			m_AISContext->MoveTo(point.x,point.y,aView, Standard_True);
		}
		break;
	case S_GET_DIR_P2:
		{
			gp_Pnt pnt = PixelToModel(point.x,point.y);
			if(pnt.Distance(m_pnt1) < Precision::Confusion())
				return;

			m_aVec.SetCoord(pnt.X() - m_pnt1.X(),pnt.Y() - m_pnt1.Y(),
				pnt.Z() - m_pnt1.Z());
			RemoveCtxObj(m_aShape);
			TopoDS_Shape aS = BuildPrism();
			Display(m_aShape,aS);
		}
		break;
	default:
		break;
	}
}

TopoDS_Shape	ScCmdPrism::BuildPrism()
{
	TopoDS_Shape aS;
	//�������췽���ƽ�淨ʸƽ��
	gp_Ax2 ax = m_pScView->CurrentAxis(FALSE);
	gp_Dir zD = ax.Direction();
	double dH = m_aVec.Dot(gp_Vec(zD));

	TopoDS_Shape aPrefile = m_aSelShape;
	if(this->m_bTwoDir)
	{
		//�ȶ�profile�����ƶ�
		gp_Trsf trsf;
		trsf.SetTranslation(gp_Vec(-dH * zD.X(),-dH * zD.Y(),-dH * zD.Z()));
		BRepBuilderAPI_Transform brepTrsf(aPrefile,trsf,TRUE);
		aPrefile = brepTrsf.Shape();

		m_aVec.SetCoord(2.0 * dH*zD.X(),2.0 * dH * zD.Y(),2.0 * dH * zD.Z());
	}
	else
	{
		m_aVec.SetCoord(dH*zD.X(),dH * zD.Y(),dH * zD.Z());
	}

	aS = BRepPrimAPI_MakePrism(aPrefile,m_aVec);
	return aS;
}

//����3ά��
int		ScCmdPrism::AcceptInput(gp_Pnt& pnt)
{
	return 0;
}

/////////////////////////////////////////////////////////////
//�����ͽ�������
ScCmdRevol::ScCmdRevol(BOOL bGenSolid)
{
	m_bGenSolid = bGenSolid;
}

int	ScCmdRevol::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	m_dAngle = 0.0;

	int nRes = GetOneSelShape(m_aProfile);
	if((nRes == SEL_OK) && (!m_aProfile.IsNull()) &&
		(CanRevol(m_aProfile)))
	{
		SwitchState(S_POINT,"��ת��������");
	}
	else
	{
		SwitchState(S_PROFILE,"ѡ��һ������");
		NeedSelect(TRUE);
	}

	return 0;
}

int	ScCmdRevol::End()
{
	RemoveCtxObj(TmpObj(0));

	return ScCommand::End();
}

BOOL	ScCmdRevol::CanRevol(const TopoDS_Shape& aShape)
{
	TopAbs_ShapeEnum eType = aShape.ShapeType();
	if(m_bGenSolid)
	{
		if(eType == TopAbs_SHELL || eType == TopAbs_FACE)
			return TRUE;
	}
	else
	{
		if(eType == TopAbs_WIRE || eType == TopAbs_EDGE)
			return TRUE;
	}

	return FALSE;
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdRevol::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_PROFILE:
		{
			int nRes = GetOneSelShape(m_aProfile);
			if((nRes == SEL_OK) && (!m_aProfile.IsNull()) &&
				(CanRevol(m_aProfile)))
			{
				SwitchState(S_POINT,"��ת��������:");
				NeedSelect(FALSE);
				NeedPoint3d("��ת�����:",FALSE);
			}
		}
		break;
	case S_POINT:
		{
			m_pnt = PixelToCSPlane(point.x,point.y);

			EndInput();
			SwitchState(S_DIR,"��ת��ڶ���:");
			NeedPoint3d("��ת��ڶ���:",FALSE);
		}
		break;
	case S_DIR:
		{
			RemoveCtxObj(TmpObj(0));

			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			double dist = pnt.Distance(m_pnt);
			if( dist < Precision::Confusion())
				return;

			m_pnt2 = pnt;
			//����һ�㣬��ȷ����ת����ʼ�㡣
			if(this->CalcRevolBeginPoint(m_pnt3) == FALSE)
			{
				End();
				return;
			}
			
			gp_Ax2 ax(pnt,gp_Dir(gp_Vec(m_pnt,m_pnt2)),gp_Dir(gp_Vec(m_pnt2,m_pnt3)));
			m_ax2 = ax;

			EndInput();
			SwitchState(S_ANGLE,"��ת�Ƕ�");
			NeedDouble("��ת�Ƕ�[0-2PI]:");
		}
		break;
	case S_ANGLE:
		{
			RemoveCtxObj(TmpObj(0));
			RemoveCtxObj(TmpObj(1));
			RemoveCtxObj(TmpObj(2));
			RemoveCtxObj(TmpObj(3));

			Done();
		}
		break;
	default:
		break;
	}
}		

BOOL	ScCmdRevol::PrjToPlane(gp_Pnt& pnt)
{
	Handle(Geom_Plane) aPln = new Geom_Plane(m_pnt2,m_ax2.Direction());
	GeomAPI_ProjectPointOnSurf pps(pnt,aPln);
	if(pps.IsDone() && pps.NbPoints() > 0)
	{
		pnt = pps.Point(1);
		return TRUE;
	}
	return FALSE;
}

BOOL	ScCmdRevol::CalcRevolBeginPoint(gp_Pnt& oPnt)
{
	gp_Pnt pnt;
	TopExp_Explorer ex;
	ex.Init(m_aProfile,TopAbs_VERTEX);
	if(!ex.More())
		return FALSE;

	Handle(Geom_Plane) aPln = new Geom_Plane(m_pnt2,gp_Dir(gp_Vec(m_pnt,m_pnt2)));
	for(;ex.More();ex.Next())
	{
		TopoDS_Vertex aV = TopoDS::Vertex(ex.Current());
		pnt = BRep_Tool::Pnt(aV);
	
		//��pntͶӰ��ƽ���ϡ�
		GeomAPI_ProjectPointOnSurf pps(pnt,aPln);
		if(pps.IsDone() && pps.NbPoints() > 0)
		{
			oPnt = pps.Point(1);
			return TRUE;
		}
	}

	return FALSE;
}

//����ƶ�����Ӧ����
void	ScCmdRevol::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_DIR:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_pnt) < Precision::Confusion())
				return;

			RemoveCtxObj(TmpObj(0));
			RemoveCtxObj(NewObj(0));
			
			m_dir.SetCoord(pnt.X() - m_pnt.X(),pnt.Y() - m_pnt.Y(),pnt.Z() - m_pnt.Z());
			TopoDS_Shape aS = BuildRevol();
			Display(NewObj(0),aS,FALSE);

			//��ʾ��ת��
			TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(pnt,m_pnt);
			Display(TmpObj(0),aE);
		}
		break;
	case S_ANGLE:
		{
			gp_Pnt pnt = PixelToModel(point.x,point.y);
			if(pnt.Distance(m_pnt2) < Precision::Confusion() ||
				pnt.Distance(m_pnt3) < Precision::Confusion())
				return;

			RemoveCtxObj(NewObj(0));
			RemoveCtxObj(TmpObj(1));
			RemoveCtxObj(TmpObj(2));
			RemoveCtxObj(TmpObj(3));
			if(PrjToPlane(pnt))
			{
				//����Ƕ�
				gp_Vec vec1(m_pnt2,m_pnt3);
				gp_Vec vec2(m_pnt2,pnt);
				m_dAngle = vec1.AngleWithRef(vec2,m_ax2.Direction());
				if(m_dAngle < 0)
					m_dAngle += 2.0 * M_PI;

				//��ʾԲ
				double dst = pnt.Distance(m_pnt2);
				gp_Circ aCirc(m_ax2,dst);
				TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(aCirc);
				Display(TmpObj(1),aE,FALSE);

				//��ʾ��ʼ��
				gp_Dir d1(gp_Vec(m_pnt2,m_pnt3)),d2(gp_Vec(m_pnt2,pnt));
				gp_Pnt tp1,tp2;
				tp1.SetX(m_pnt2.X() + dst * d1.X() * 1.2);
				tp1.SetY(m_pnt2.Y() + dst * d1.Y() * 1.2);
				tp1.SetZ(m_pnt2.Z() + dst * d1.Z() * 1.2);
				TopoDS_Edge aE1 = BRepBuilderAPI_MakeEdge(m_pnt2,tp1);
				Display(TmpObj(2),aE1,FALSE);

				//��ʾ��ֹ��
				tp2.SetX(m_pnt2.X() + dst * d2.X() * 1.2);
				tp2.SetY(m_pnt2.Y() + dst * d2.Y() * 1.2);
				tp2.SetZ(m_pnt2.Z() + dst * d2.Z() * 1.2);
				TopoDS_Edge aE2 = BRepBuilderAPI_MakeEdge(m_pnt2,tp2);
				Display(TmpObj(3),aE2,FALSE);

				//���յ�����
				TopoDS_Shape aS = BuildRevol();
				Display(NewObj(0),aS);
			}
		}
		break;
	default:
		break;
	}
}

//����3ά��
int	    ScCmdRevol::AcceptInput(gp_Pnt& pnt)
{
	return 0;
}

//���ո�����
int		ScCmdRevol::AcceptInput(double dVal)
{
	return 0;
}

TopoDS_Shape	ScCmdRevol::BuildRevol()
{
	TopoDS_Shape aShape;
	
	if(m_dAngle < Precision::Confusion() || 
		m_dAngle > 2.0 * M_PI - Precision::Confusion())
	{
		aShape = BRepPrimAPI_MakeRevol(m_aProfile,gp_Ax1(m_pnt,m_dir));
	}
	else
	{
		aShape = BRepPrimAPI_MakeRevol(m_aProfile,gp_Ax1(m_pnt,m_dir),m_dAngle);
	}

	return aShape;
}

////////////////////////////////////////////////////////////
// ����ü�ʵ��
// ʹ���������ɰ�ռ䣬����bool������вü���

//�����ͽ�������
int	ScCmdSolidCut::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SELFACE,"ѡ��һ���������ü�����");
	NeedMultiSelect(TRUE);

	return 0;
}

int	ScCmdSolidCut::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdSolidCut::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
	}
	else
	{
		switch(GetState())
		{
		case S_SELFACE:
			{
				InitSelect();
				if(MoreSelect())
				{
					TmpObj(0) = SelectedObj(FALSE);
					TopoDS_Shape aS = SelectedShape(FALSE);
					if(aS.ShapeType() == TopAbs_FACE ||
						aS.ShapeType() == TopAbs_SHELL)
					{
						m_aSurfShape = aS;

						SwitchState(S_SELOBJ,"ѡ��Ҫ�ü��Ķ���:");
					}
				}
			}
			break;
		case S_SELOBJ:
			{
				InitSelect();
				while(MoreSelect())
				{
					TmpObj(1) = SelectedObj(FALSE);
					if(TmpObj(1) != TmpObj(0))
					{
						m_aShape = SelectedShape(FALSE);
						if(!m_aShape.IsNull() && 
							((m_aShape.ShapeType() == TopAbs_SOLID) || 
							(m_aShape.ShapeType() == TopAbs_SHELL) || 
							(m_aShape.ShapeType() == TopAbs_FACE) ||
							(m_aShape.ShapeType() == TopAbs_WIRE) ||
							(m_aShape.ShapeType() == TopAbs_EDGE)))
						{
							NeedMultiSelect(FALSE);

							SwitchState(S_SELDIR,"ѡ�����Ҫ��������:");

							break;
						}
					}
					else
					{
						TmpObj(1).Nullify();
					}
					NextSelect();
				}
			}
			break;
		case S_SELDIR:
			{
				//����Ҫ��������Դ�.����m_aShape�����Ͳ�ͬ������ͬ����
				//��ȡ����Ϣ��
				gp_Pnt pickPnt,pnt;
				if(this->m_aShape.ShapeType() == TopAbs_EDGE)
				{
					//����ȡ��edge��wire������㡣
					double t;
					if(!this->m_pScView->GetEdgeNearestPoint(TopoDS::Edge(m_aShape),
						point.x,point.y,t,pickPnt))
					{
						End();
						return;
					}
				}
				else if(this->m_aShape.ShapeType() == TopAbs_WIRE)
				{
					//�ȼ򻯴���
					pickPnt = this->PixelToModel(point.x,point.y);
				}
				else 
				{
					TopoDS_Face aF;
					if(this->m_aShape.ShapeType() == TopAbs_FACE)
						aF = TopoDS::Face(m_aShape);
					else
					{
						aF = GetObjPickedFace(TmpObj(1),point.x,point.y,aView);
					}
					if(aF.IsNull())
					{
						End();
						return;
					}

					double u,v;	
					if(!this->m_pScView->GetFacePickPoint(aF,point.x,point.y,u,v,pickPnt))
					{
						End();
						return;
					}
				}
				
					
				//������ռ�
				TopoDS_Shape aToolS;
				try
				{
					if(m_aSurfShape.ShapeType() == TopAbs_FACE)
					{
						BRepPrimAPI_MakeHalfSpace mhs(TopoDS::Face(m_aSurfShape),pickPnt);
						aToolS = mhs.Solid();
					}
					else
					{
						BRepPrimAPI_MakeHalfSpace mhs(TopoDS::Shell(m_aSurfShape),pickPnt);
						aToolS = mhs.Solid();
					}
				}
				catch(Standard_Failure)
				{
					End();
					return;
				}

				if(!aToolS.IsNull())
				{
					//boolean����
					TopoDS_Shape aNewS;
					try
					{
						aNewS = BRepAlgoAPI_Cut(m_aShape,aToolS);
					}
					catch(Standard_Failure)
					{
						End();
						return;
					}
					//��ʾ�µġ�
					DelObj(0) = TmpObj(1);
					TmpObj(0).Nullify();
					TmpObj(1).Nullify();
					//ɾ���ɵ�
					m_AISContext->Remove(DelObj(0), Standard_True);
					//��ʾ�µ�
					Display(NewObj(0),aNewS);
				}

				Done();
			}
			break;
		default:
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////
// ȡ�����
// ��compound����ɵ�����shape

//�����ͽ�������
int	ScCmdDeCompound::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	if(CanDeCompound())
	{
		DeCompound();
		Done();
	}
	else
	{
		NeedSelect(TRUE);
	}

	return 0;
}

int	ScCmdDeCompound::End()
{
	return ScCommand::End();
}


//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdDeCompound::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
	}
	else
	{
		if(CanDeCompound())
		{
			DeCompound();
			Done();
		}
	}
}

BOOL	ScCmdDeCompound::CanDeCompound()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(!aS.IsNull() && aS.ShapeType() == TopAbs_COMPOUND)
		{
			DelObj(0) = SelectedObj();

			return TRUE;
		}
	}

	return FALSE;
}

void	ScCmdDeCompound::DeCompound()
{
	TopoDS_Shape aS = GetObjShape(DelObj(0));
	if(aS.IsNull())
		return;

	TopoDS_Iterator ite;
	for(ite.Initialize(aS);ite.More();ite.Next())
	{
		TopoDS_Shape aSS = ite.Value();
		if(aS.IsNull())
			continue;

		Display(NewObj(-1),aSS);
	}

	//ȥ���ɵ�
	m_AISContext->Remove(DelObj(0), Standard_True);
}