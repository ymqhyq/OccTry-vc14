#include "StdAfx.h"
#include "..\ScView.h"
#include "..\ScUcsMgr.h"
#include "..\ScInputMgr.h"
#include ".\sccmducs.h"
#include "ScCmdMgr.h"

ScCmdUcs::ScCmdUcs(void)
{
}

ScCmdUcs::~ScCmdUcs(void)
{
}

ScCmdUcsPrev::ScCmdUcsPrev()
{
}

ScCmdUcsPrev::~ScCmdUcsPrev()
{
}

//�����ͽ�������
int		ScCmdUcsPrev::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	//�л���ģ�Ϳռ�
	this->m_pScUcsMgr->SwitchToPrevUCS();

	this->m_pScUcsMgr->ShowTrihedron();
	//view�ı�
	this->m_pScView->UpdateUCS();

	Done();

	return 0;
}

int		ScCmdUcsPrev::End()
{
	return 0;
}

ScCmdUcsNext::ScCmdUcsNext()
{
}

ScCmdUcsNext::~ScCmdUcsNext()
{
}

//�����ͽ�������
int		ScCmdUcsNext::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	//�л���ģ�Ϳռ�
	this->m_pScUcsMgr->SwitchToNextUCS();

	this->m_pScUcsMgr->ShowTrihedron();
	//view�ı�
	this->m_pScView->UpdateUCS();

	Done();

	return 0;
}

int		ScCmdUcsNext::End()
{
	return 0;
}

////////////////////////////////////////////////////////////
//
ScCmdUcsModel::ScCmdUcsModel()
{
}

ScCmdUcsModel::~ScCmdUcsModel()
{
}

//�����ͽ�������
int		ScCmdUcsModel::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	//�л���ģ�Ϳռ�
	this->m_pScUcsMgr->SwitchToWCS();

	this->m_pScUcsMgr->ShowTrihedron();
	//view�ı�
	this->m_pScView->UpdateUCS();

	Done();

	return 0;
}

int		ScCmdUcsModel::End()
{
	return 0;
}

//////////////////////////////////////////////////////////////////
//
ScCmdUcsOrg::ScCmdUcsOrg()
{
}

ScCmdUcsOrg::~ScCmdUcsOrg()
{
}

//�����ͽ�������
int	 ScCmdUcsOrg::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	Prompt("ѡ���µ�����ϵԭ��:");
	NeedInput("������ϵԭ��:",INPUT_POINT3D);
	NeedSnap(TRUE);

	return SC_OK;
}

int	 ScCmdUcsOrg::End()
{
	return ScCommand::End();
}

//
// ��ѡ��ʽ�µ���Ϣ��Ӧ����.����������룬�����µ�wcs���ꡣ
// 
void	ScCmdUcsOrg::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState != MINPUT_LEFT)
		return;

	RemoveCtxObj(m_aShape);
	
	gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
	//�����µ�����ϵ
	gp_Ax2 ax = this->m_pScView->CurrentAxis(FALSE);
	ax.SetLocation(pnt);//������WCS���ꡣ

	ScUcs *pScUcs = new ScUcs(ax);//
	this->m_pScUcsMgr->AddUCS(pScUcs);
	this->m_pScUcsMgr->ShowTrihedron();
	//view�ı�
	this->m_pScView->UpdateUCS();

	//
	Done();
}


//����ƶ�����Ӧ����
void	ScCmdUcsOrg::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	RemoveCtxObj(m_aShape);

	SnapPoint(point.x,point.y,aView);
	gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
	//�����µ�����ϵ
	gp_Ax2 ax = this->m_pScView->CurrentAxis(FALSE);
	ax.SetLocation(pnt);//��wcs����

	//��ʾ�µ�
	Handle(Geom_Axis2Placement) myTrihedronAxis=new Geom_Axis2Placement(ax);
	this->m_aShape = new AIS_Trihedron(myTrihedronAxis);
	m_AISContext->SetTrihedronSize(40, Standard_True);
	m_AISContext->Display(m_aShape, Standard_True);
}

//
// ����3ά��.�������UCS���ꡣ
//
int		ScCmdUcsOrg::AcceptInput(gp_Pnt& pnt)
{
	RemoveCtxObj(m_aShape);

	//�����µ�����ϵ
	gp_Ax2 ax = this->m_pScView->CurrentAxis(FALSE);
	ax.SetLocation(pnt);//UCS����

	ScUcs *pScUcs = new ScUcs(ax);
	this->m_pScUcsMgr->AddUCS(pScUcs);
	this->m_pScUcsMgr->ShowUCSTrihedron();
	this->m_pScUcsMgr->ShowWCSTrihedron();
	//view�ı�
	this->m_pScView->UpdateUCS();

	//
	Done();
	return SC_OK;
}

/////////////////////////////////////////////////////////////////////
//
ScCmdUcsHeight::ScCmdUcsHeight()
{
}

ScCmdUcsHeight::~ScCmdUcsHeight()
{
}

//�����ͽ�������
int		ScCmdUcsHeight::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	Prompt("ѡ���µĸ߶�:");
	NeedInput("�¸߶�:",INPUT_DOUBLE);
	this->m_pScView->NeedModelPoint(TRUE);

	return 0;
}

int		ScCmdUcsHeight::End()
{
	return 0;
}

//
// ��ѡ��ʽ�µ���Ϣ��Ӧ����
//
void	ScCmdUcsHeight::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState != MINPUT_LEFT)
		return;

	gp_Pnt pnt = this->PixelToModel(point.x,point.y);//WCS����
	double dH = ComputeHeight(pnt);

	if(fabs(dH) < 1.0e-10)
		return;

	MakeNewUCS(dH);
}

//
// ����ƶ�����Ӧ����
//
void	ScCmdUcsHeight::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	gp_Pnt pnt = this->PixelToModel(point.x,point.y);

	double dH = ComputeHeight(pnt);
	if(fabs(dH) < 1.0e-10)
		return;

	RemoveCtxObj(m_aShape);

	gp_Ax2 ax = BuildNewAx2(dH);
	//��ʾ�µ�
	Handle(Geom_Axis2Placement) myTrihedronAxis=new Geom_Axis2Placement(ax);
	this->m_aShape = new AIS_Trihedron(myTrihedronAxis);
	m_AISContext->SetTrihedronSize(40, Standard_True);
	m_AISContext->Display(m_aShape, Standard_True);
}

//
// ���ո߶�,����ڵ�ǰ��UCS��ƫ�Ƹ߶ȡ�
//
int		ScCmdUcsHeight::AcceptInput(double dVal)
{
	if(fabs(dVal) < 1.0e-10)
		return -1;

	MakeNewUCS(dVal);
	return 0;
}

int		ScCmdUcsHeight::MakeNewUCS(double dVal)
{
	RemoveCtxObj(m_aShape);

	gp_Ax2 ax = BuildNewAx2(dVal);

	ScUcs *pScUcs = new ScUcs(ax);
	this->m_pScUcsMgr->AddUCS(pScUcs);//WCS����
	this->m_pScUcsMgr->ShowTrihedron();
	//view�ı�
	this->m_pScView->UpdateUCS();

	this->m_pScView->NeedModelPoint(FALSE);

	Done();

	return 0;
}

gp_Ax2	ScCmdUcsHeight::BuildNewAx2(double dVal)
{
	gp_Ax2 ax = this->m_pScView->CurrentAxis();
	gp_Pnt loc = ax.Location();
	gp_XYZ n = ax.Direction().XYZ();
	n *= dVal;
	gp_Pnt np(loc.XYZ() + n);

	ax.SetLocation(np);

	return ax;
}

//
// ����ƫ�Ƶĸ߶ȡ�����ڵ�ǰUCS��ƫ������
//
double	ScCmdUcsHeight::ComputeHeight(const gp_Pnt& pnt)
{
	gp_Ax2 ax = this->m_pScView->CurrentAxis();
	gp_Vec vec(ax.Location(),pnt);//WCS����
	//pnt��csƽ��ľ��롣
	double dH = vec.XYZ().Dot(ax.Direction().XYZ());
	if(fabs(dH) < 1.0e-10)
		return 0.0;

	return dH;
}
////////////////////////////////////////////////////////////////////
//
ScCmdUcs3Point::ScCmdUcs3Point()
{
}

ScCmdUcs3Point::~ScCmdUcs3Point()
{
}

//�����ͽ�������
int		ScCmdUcs3Point::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	Prompt("��ȡ������ϵԭ��:");
	SetState(U3P_ORG);
	NeedInput("��ԭ��:",INPUT_POINT3D);
	this->m_pScView->NeedModelPoint(TRUE);//��Ҫ�ռ��

	return SC_OK;
}

int		ScCmdUcs3Point::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdUcs3Point::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState != MINPUT_LEFT)
		return;

	gp_Pnt pnt = PixelToModel(point.x,point.y);//WCS����
	//����
	DoAcceptPoint(pnt);
}
	
//����ƶ�����Ӧ����
void	ScCmdUcs3Point::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case U3P_ZPOINT:
		{
			gp_Pnt pnt = PixelToModel(point.x,point.y);

			RemoveCtxObj(m_aShape);

			BRepBuilderAPI_MakeEdge me(m_ptOrg,pnt);
			if(me.IsDone())
			{
				m_aShape = new AIS_Shape(me);
				m_AISContext->Display(m_aShape, Standard_True);
			}
		}
		break;
	case U3P_UPOINT:
		{
			gp_Pnt pnt = PixelToModel(point.x,point.y);

			if(pnt.Distance(m_ptOrg) < Precision::Confusion() * 10.0)
				return;

			RemoveCtxObj(m_aShape);
			RemoveCtxObj(m_aTriShape);

			gp_Ax2 ax(m_ptOrg,
				gp_Dir(m_ptZ.X() - m_ptOrg.X(),m_ptZ.Y() - m_ptOrg.Y(),m_ptZ.Z() - m_ptOrg.Z()),
				gp_Dir(pnt.X() - m_ptOrg.X(),pnt.Y() - m_ptOrg.Y(),pnt.Z() - m_ptOrg.Z()));

			//��ʾ�µ�
			Handle(Geom_Axis2Placement) myTrihedronAxis=new Geom_Axis2Placement(ax);
			this->m_aTriShape = new AIS_Trihedron(myTrihedronAxis);
			m_AISContext->SetTrihedronSize(40, Standard_True);
			m_AISContext->Display(m_aTriShape, Standard_True);
		}
		break;
	default:
		break;
	}
}

//
// ����3ά��,�������Ucs�����
//
int		ScCmdUcs3Point::AcceptInput(gp_Pnt& pnt)
{
	this->UCSToWCS(pnt);//ת����������
	DoAcceptPoint(pnt);
	return SC_OK;
}

void	ScCmdUcs3Point::DoAcceptPoint(const gp_Pnt& pnt)
{
	switch(GetState())
	{
	case U3P_ORG:
		{
			this->m_ptOrg = pnt;

			EndInput();
			Prompt("��ȡZ����ĵڶ�����:");
			SetState(U3P_ZPOINT);
			NeedInput("�ڶ���:",INPUT_POINT3D);
		}
		break;
	case U3P_ZPOINT:
		{
			this->m_ptZ = pnt;

			EndInput();
			Prompt("��ȡ��������:");
			SetState(U3P_UPOINT);
			NeedInput("������:",INPUT_POINT3D);
		}
		break;
	case U3P_UPOINT:
		{
			this->m_ptU = pnt;
			//��Ҫ��ֹ����

			Done();
			this->m_pScView->NeedModelPoint(FALSE);//������Ҫ�ռ��

			RemoveCtxObj(m_aTriShape);

			//�����µ�����ϵ
			gp_Ax2 ax(m_ptOrg,
				gp_Dir(m_ptZ.X() - m_ptOrg.X(),m_ptZ.Y() - m_ptOrg.Y(),m_ptZ.Z() - m_ptOrg.Z()),
				gp_Dir(m_ptU.X() - m_ptOrg.X(),m_ptU.Y() - m_ptOrg.Y(),m_ptU.Z() - m_ptOrg.Z()));

			ScUcs *pScUcs = new ScUcs(ax);
			this->m_pScUcsMgr->AddUCS(pScUcs);
			this->m_pScUcsMgr->ShowTrihedron();
			//view�ı�
			this->m_pScView->UpdateUCS();
		}
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// һ���򵥵Ļ���

//�����ͽ�������
int	ScCmdUcsObjPoint::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);
	return 0;
}

int	ScCmdUcsObjPoint::End()
{
	RemoveCtxObj(m_aTriShape);

	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdUcsObjPoint::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	m_aView = aView;
	if(nState == MINPUT_RIGHT)
	{
		this->m_pScCmdMgr->StopCommand();
	}
	else
	{
		switch(GetState())
		{
		case S_SHAPE:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape(FALSE);
					if(IsRequiredShape(aS))
					{
						m_aSelObj = SelectedObj(FALSE);
						m_aShape = aS;
						m_nX = point.x;
						m_nY = point.y;

						SwitchState(S_POINT,"ѡ������ϵĵ�:");
						NeedSelect(FALSE);
					}
				}
			}
			break;
		case S_POINT:
			{
				gp_Ax2 ax2;
				if(BuildAxis(ax2))
					BuildTrihedron(ax2);
				Done();
			}
			break;
		default:
			break;
		}
	}
}
	
//����ƶ�����Ӧ����
void	ScCmdUcsObjPoint::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	m_aView = aView;
	switch(GetState())
	{
	case S_POINT:
		{
			m_nX = point.x;
			m_nY = point.y;

			gp_Ax2 ax2;
			if(BuildAxis(ax2))
				ShowTrihedron(ax2);
		}
		break;
	default:
		break;
	}
}


//�Ƿ���Ҫ�Ķ���
BOOL	ScCmdUcsObjPoint::IsRequiredShape(const TopoDS_Shape& aShape)
{
	return FALSE;
}

//�������������Ӧ������ϵ
BOOL	ScCmdUcsObjPoint::BuildAxis(gp_Ax2& ax2)
{
	return FALSE;
}

void	ScCmdUcsObjPoint::ShowTrihedron(const gp_Ax2& ax2)
{
	RemoveCtxObj(m_aTriShape);

	Handle(Geom_Axis2Placement) myTrihedronAxis=new Geom_Axis2Placement(ax2);
	this->m_aTriShape = new AIS_Trihedron(myTrihedronAxis);
	m_AISContext->SetTrihedronSize(40, Standard_True);
	m_AISContext->Display(m_aTriShape, Standard_True);
}

void	ScCmdUcsObjPoint::BuildTrihedron(const gp_Ax2& ax2)
{
	RemoveCtxObj(m_aTriShape);

	ScUcs *pScUcs = new ScUcs(ax2);
	this->m_pScUcsMgr->AddUCS(pScUcs);
	this->m_pScUcsMgr->ShowTrihedron();
	//view�ı�
	this->m_pScView->UpdateUCS();
}

///////////////////////////////////////////////////////////////
//
	
//�����ͽ�������
int	ScCmdUcsCurvePoint::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);
	
	SwitchState(S_SHAPE,"ѡ��һ������:");
	NeedSelect(TRUE);

	return 0;
}

//�Ƿ���Ҫ�Ķ���
BOOL	ScCmdUcsCurvePoint::IsRequiredShape(const TopoDS_Shape& aShape)
{
	return (aShape.ShapeType() == TopAbs_EDGE);
}

//�������������Ӧ������ϵ
BOOL	ScCmdUcsCurvePoint::BuildAxis(gp_Ax2& ax2)
{
	if(!this->m_pScView)
		return FALSE;

	TopoDS_Edge aEdge = TopoDS::Edge(m_aShape);

	double t;
	gp_Pnt pickPnt,pnt;
	if(this->m_pScView->GetEdgeNearestPoint(aEdge,m_nX,m_nY,t,pickPnt))
	{
		//����
		gp_Vec rt,rtt;
		BRepAdaptor_Curve baCur(aEdge);
		baCur.D2(t,pnt,rt,rtt);
		if(rt.Magnitude() < Precision::Confusion() ||
			rtt.Magnitude() < Precision::Confusion())
			return FALSE;

		gp_Ax2 ax(pnt,gp_Dir(rt),gp_Dir(rtt));
		ax2 = ax;

		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////
//

//�����ͽ�������
int	ScCmdUcsFacePoint::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SHAPE,"ѡ��һ������;");
	NeedSelect(TRUE);

	return 0;
}

//�Ƿ���Ҫ�Ķ���
BOOL	ScCmdUcsFacePoint::IsRequiredShape(const TopoDS_Shape& aShape)
{
	return (aShape.ShapeType() == TopAbs_FACE);
}

//�������������Ӧ������ϵ
BOOL	ScCmdUcsFacePoint::BuildAxis(gp_Ax2& ax2)
{
	if(!this->m_pScView)
		return FALSE;

	TopoDS_Face aFace = TopoDS::Face(m_aShape);

	double u,v;
	gp_Pnt pickPnt,pnt;
	if(this->m_pScView->GetFacePickPoint(aFace,m_nX,m_nY,u,v,pickPnt))
	{
		double Tol = BRep_Tool::Tolerance(aFace);
		gp_Vec du,dv;
		BRepAdaptor_Surface baSurf(aFace);
		baSurf.D1(u,v,pnt,du,dv);
		if(du.Magnitude() < Tol || dv.Magnitude() < Tol)
			return FALSE;

		du.Normalize();
		dv.Normalize();
		gp_Vec N = du.Crossed(dv);
		if(N.Magnitude() < Tol)
			return FALSE;

		gp_Ax2 ax(pnt,gp_Dir(N),gp_Dir(du));
		ax2 = ax;

		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////
//

//�����ͽ�������
int	ScCmdUcsShapePoint::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SHAPE,"ѡ��һ������:");
	NeedSelect(TRUE);

	return 0;
}

//�Ƿ���Ҫ�Ķ���
BOOL	ScCmdUcsShapePoint::IsRequiredShape(const TopoDS_Shape& aShape)
{
	if((aShape.ShapeType() != TopAbs_VERTEX) &&
	   (aShape.ShapeType() != TopAbs_EDGE) &&
	   (aShape.ShapeType() != TopAbs_WIRE))
	   return TRUE;

	return FALSE;
}

//
// �������������Ӧ������ϵ,���ݵ�ǰ����꣬��ȡѡ�������ϵĵ㣬����������ϵ��
//
BOOL	ScCmdUcsShapePoint::BuildAxis(gp_Ax2& ax2)
{
	TopoDS_Face aFace;

	//ѡ������е�ǰѡ�е�Face��
	//m_AISContext->OpenLocalContext();//yxk20191207
	m_AISContext->Activate(m_aSelObj,4);//face

	m_AISContext->MoveTo(m_nX,m_nY,m_aView,Standard_True);
	m_AISContext->Select(Standard_True);

	InitSelect();
	if(MoreSelect())
	{
		aFace = TopoDS::Face(SelectedShape(FALSE));
	}

	//m_AISContext->CloseLocalContex();//yxk20191207

	if(aFace.IsNull())
		return FALSE;

	//����ax
	double u,v;
	gp_Pnt pickPnt,pnt;
	if(this->m_pScView->GetFacePickPoint(aFace,m_nX,m_nY,u,v,pickPnt))
	{
		double Tol = BRep_Tool::Tolerance(aFace);
		gp_Vec du,dv;
		BRepAdaptor_Surface baSurf(aFace);
		baSurf.D1(u,v,pnt,du,dv);
		if(du.Magnitude() < Tol || dv.Magnitude() < Tol)
			return FALSE;

		du.Normalize();
		dv.Normalize();
		gp_Vec N = du.Crossed(dv);
		if(N.Magnitude() < Tol)
			return FALSE;

		gp_Ax2 ax(pnt,gp_Dir(N),gp_Dir(du));
		ax2 = ax;

		return TRUE;
	}

	return FALSE;
}

