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

//启动和结束命令
int		ScCmdUcsPrev::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	//切换到模型空间
	this->m_pScUcsMgr->SwitchToPrevUCS();

	this->m_pScUcsMgr->ShowTrihedron();
	//view改变
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

//启动和结束命令
int		ScCmdUcsNext::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	//切换到模型空间
	this->m_pScUcsMgr->SwitchToNextUCS();

	this->m_pScUcsMgr->ShowTrihedron();
	//view改变
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

//启动和结束命令
int		ScCmdUcsModel::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	//切换到模型空间
	this->m_pScUcsMgr->SwitchToWCS();

	this->m_pScUcsMgr->ShowTrihedron();
	//view改变
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

//启动和结束命令
int	 ScCmdUcsOrg::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	Prompt("选择新的坐标系原点:");
	NeedInput("新坐标系原点:",INPUT_POINT3D);
	NeedSnap(TRUE);

	return SC_OK;
}

int	 ScCmdUcsOrg::End()
{
	return ScCommand::End();
}

//
// 点选方式下的消息响应函数.根据鼠标输入，计算新的wcs坐标。
// 
void	ScCmdUcsOrg::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState != MINPUT_LEFT)
		return;

	RemoveCtxObj(m_aShape);
	
	gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
	//生成新的坐标系
	gp_Ax2 ax = this->m_pScView->CurrentAxis(FALSE);
	ax.SetLocation(pnt);//这里是WCS坐标。

	ScUcs *pScUcs = new ScUcs(ax);//
	this->m_pScUcsMgr->AddUCS(pScUcs);
	this->m_pScUcsMgr->ShowTrihedron();
	//view改变
	this->m_pScView->UpdateUCS();

	//
	Done();
}


//鼠标移动的响应函数
void	ScCmdUcsOrg::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	RemoveCtxObj(m_aShape);

	SnapPoint(point.x,point.y,aView);
	gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
	//生成新的坐标系
	gp_Ax2 ax = this->m_pScView->CurrentAxis(FALSE);
	ax.SetLocation(pnt);//是wcs坐标

	//显示新的
	Handle(Geom_Axis2Placement) myTrihedronAxis=new Geom_Axis2Placement(ax);
	this->m_aShape = new AIS_Trihedron(myTrihedronAxis);
	m_AISContext->SetTrihedronSize(40, Standard_True);
	m_AISContext->Display(m_aShape, Standard_True);
}

//
// 接收3维点.输入的是UCS坐标。
//
int		ScCmdUcsOrg::AcceptInput(gp_Pnt& pnt)
{
	RemoveCtxObj(m_aShape);

	//生成新的坐标系
	gp_Ax2 ax = this->m_pScView->CurrentAxis(FALSE);
	ax.SetLocation(pnt);//UCS坐标

	ScUcs *pScUcs = new ScUcs(ax);
	this->m_pScUcsMgr->AddUCS(pScUcs);
	this->m_pScUcsMgr->ShowUCSTrihedron();
	this->m_pScUcsMgr->ShowWCSTrihedron();
	//view改变
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

//启动和结束命令
int		ScCmdUcsHeight::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	Prompt("选择新的高度:");
	NeedInput("新高度:",INPUT_DOUBLE);
	this->m_pScView->NeedModelPoint(TRUE);

	return 0;
}

int		ScCmdUcsHeight::End()
{
	return 0;
}

//
// 点选方式下的消息响应函数
//
void	ScCmdUcsHeight::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState != MINPUT_LEFT)
		return;

	gp_Pnt pnt = this->PixelToModel(point.x,point.y);//WCS坐标
	double dH = ComputeHeight(pnt);

	if(fabs(dH) < 1.0e-10)
		return;

	MakeNewUCS(dH);
}

//
// 鼠标移动的响应函数
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
	//显示新的
	Handle(Geom_Axis2Placement) myTrihedronAxis=new Geom_Axis2Placement(ax);
	this->m_aShape = new AIS_Trihedron(myTrihedronAxis);
	m_AISContext->SetTrihedronSize(40, Standard_True);
	m_AISContext->Display(m_aShape, Standard_True);
}

//
// 接收高度,相对于当前的UCS的偏移高度。
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
	this->m_pScUcsMgr->AddUCS(pScUcs);//WCS坐标
	this->m_pScUcsMgr->ShowTrihedron();
	//view改变
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
// 计算偏移的高度。相对于当前UCS的偏移量。
//
double	ScCmdUcsHeight::ComputeHeight(const gp_Pnt& pnt)
{
	gp_Ax2 ax = this->m_pScView->CurrentAxis();
	gp_Vec vec(ax.Location(),pnt);//WCS坐标
	//pnt和cs平面的距离。
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

//启动和结束命令
int		ScCmdUcs3Point::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	Prompt("获取新坐标系原点:");
	SetState(U3P_ORG);
	NeedInput("新原点:",INPUT_POINT3D);
	this->m_pScView->NeedModelPoint(TRUE);//需要空间点

	return SC_OK;
}

int		ScCmdUcs3Point::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdUcs3Point::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState != MINPUT_LEFT)
		return;

	gp_Pnt pnt = PixelToModel(point.x,point.y);//WCS坐标
	//处理
	DoAcceptPoint(pnt);
}
	
//鼠标移动的响应函数
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

			//显示新的
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
// 接收3维点,输入的是Ucs坐标点
//
int		ScCmdUcs3Point::AcceptInput(gp_Pnt& pnt)
{
	this->UCSToWCS(pnt);//转入世界坐标
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
			Prompt("获取Z方向的第二个点:");
			SetState(U3P_ZPOINT);
			NeedInput("第二点:",INPUT_POINT3D);
		}
		break;
	case U3P_ZPOINT:
		{
			this->m_ptZ = pnt;

			EndInput();
			Prompt("获取第三个点:");
			SetState(U3P_UPOINT);
			NeedInput("第三点:",INPUT_POINT3D);
		}
		break;
	case U3P_UPOINT:
		{
			this->m_ptU = pnt;
			//需要防止共线

			Done();
			this->m_pScView->NeedModelPoint(FALSE);//不再需要空间点

			RemoveCtxObj(m_aTriShape);

			//创建新的坐标系
			gp_Ax2 ax(m_ptOrg,
				gp_Dir(m_ptZ.X() - m_ptOrg.X(),m_ptZ.Y() - m_ptOrg.Y(),m_ptZ.Z() - m_ptOrg.Z()),
				gp_Dir(m_ptU.X() - m_ptOrg.X(),m_ptU.Y() - m_ptOrg.Y(),m_ptU.Z() - m_ptOrg.Z()));

			ScUcs *pScUcs = new ScUcs(ax);
			this->m_pScUcsMgr->AddUCS(pScUcs);
			this->m_pScUcsMgr->ShowTrihedron();
			//view改变
			this->m_pScView->UpdateUCS();
		}
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// 一个简单的基类

//启动和结束命令
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

//点选方式下的消息响应函数
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

						SwitchState(S_POINT,"选择对象上的点:");
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
	
//鼠标移动的响应函数
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


//是否需要的对象
BOOL	ScCmdUcsObjPoint::IsRequiredShape(const TopoDS_Shape& aShape)
{
	return FALSE;
}

//根据条件构造对应的坐标系
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
	//view改变
	this->m_pScView->UpdateUCS();
}

///////////////////////////////////////////////////////////////
//
	
//启动和结束命令
int	ScCmdUcsCurvePoint::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);
	
	SwitchState(S_SHAPE,"选择一条曲线:");
	NeedSelect(TRUE);

	return 0;
}

//是否需要的对象
BOOL	ScCmdUcsCurvePoint::IsRequiredShape(const TopoDS_Shape& aShape)
{
	return (aShape.ShapeType() == TopAbs_EDGE);
}

//根据条件构造对应的坐标系
BOOL	ScCmdUcsCurvePoint::BuildAxis(gp_Ax2& ax2)
{
	if(!this->m_pScView)
		return FALSE;

	TopoDS_Edge aEdge = TopoDS::Edge(m_aShape);

	double t;
	gp_Pnt pickPnt,pnt;
	if(this->m_pScView->GetEdgeNearestPoint(aEdge,m_nX,m_nY,t,pickPnt))
	{
		//计算
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

//启动和结束命令
int	ScCmdUcsFacePoint::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SHAPE,"选择一个曲面;");
	NeedSelect(TRUE);

	return 0;
}

//是否需要的对象
BOOL	ScCmdUcsFacePoint::IsRequiredShape(const TopoDS_Shape& aShape)
{
	return (aShape.ShapeType() == TopAbs_FACE);
}

//根据条件构造对应的坐标系
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

//启动和结束命令
int	ScCmdUcsShapePoint::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SHAPE,"选择一个对象:");
	NeedSelect(TRUE);

	return 0;
}

//是否需要的对象
BOOL	ScCmdUcsShapePoint::IsRequiredShape(const TopoDS_Shape& aShape)
{
	if((aShape.ShapeType() != TopAbs_VERTEX) &&
	   (aShape.ShapeType() != TopAbs_EDGE) &&
	   (aShape.ShapeType() != TopAbs_WIRE))
	   return TRUE;

	return FALSE;
}

//
// 根据条件构造对应的坐标系,根据当前的鼠标，获取选择曲面上的点，并计算坐标系。
//
BOOL	ScCmdUcsShapePoint::BuildAxis(gp_Ax2& ax2)
{
	TopoDS_Face aFace;

	//选择对象中当前选中的Face。
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

	//计算ax
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

