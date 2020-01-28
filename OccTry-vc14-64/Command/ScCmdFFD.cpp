#include "StdAfx.h"
#include <ScBRepLib.h>
#include <BRepAlgo.hxx>
#include <BRepLib.hxx>
#include <ShapeCustom.hxx>

#include "ScCmdBSpline.h"
#include ".\sccmdffd.h"

//////////////////////////////////////////////////////////////////////////
//
TcFFDFrame::TcFFDFrame()
{
	this->_l = 2;
	this->_m = 3;
	this->_n = 2;
	this->_tcnt = (_l + 1) * (_m + 1) * (_n + 1);

	this->_ctlpnts = NULL;
}

//是否在变形范围内
BOOL	TcFFDFrame::InRange(const gp_Pnt& pnt)
{
	if(pnt.X() < _min[0] || pnt.X() > _max[0])
		return FALSE;

	if(pnt.Y() < _min[1] || pnt.Y() > _max[1])
		return FALSE;

	if(pnt.Z() < _min[2] || pnt.Z() > _max[2])
		return FALSE;

	return TRUE;
}

double  BinCoef(int n, int j)
{
	int	 i,	a = 1, b = 1;

	for (i = j+1; i <= n; i++) a *= i;
	for (i = 1; i <= n-j; i++) b *= i;
	return ((double)a/(double)b);
}

//计算c(n,j)*(1-t)^(n - j) * t^j
double  BernFunc(double t,int n,int j)
{
	double dval = BinCoef(n,j);
	int ix;
	for(ix = 0;ix < n - j;ix ++)
		dval *= (1.0 - t);
	for(ix = 0;ix < j;ix ++)
		dval *= t;

	return dval;
}

//1点变形
BOOL	TcFFDFrame::DeformPnt(const gp_Pnt& pnt,gp_Pnt& respnt)
{
	//先计算旧的lcs内的坐标
	gp_Pnt lp = CalcLcsCoord(pnt);

	//计算新的控制点
	int ix,iy,iz;
	double xv = 0.0,yv = 0.0,zv = 0.0;
	double x1,y1,z1,x2,y2,z2;
	double dcoef;
	int idx = 0;
	for(ix = 0;ix <= _l;ix ++)
	{
		x1 = y1 = z1 = 0.0;
		for(iy = 0;iy <= _m;iy ++)
		{
			x2 = y2 = z2 = 0.0;
			for(iz = 0;iz <= _n;iz ++)
			{
				dcoef = BernFunc(lp.Z(),_n,iz);
				x2 += dcoef * _ffdpnts[idx][0];
				y2 += dcoef * _ffdpnts[idx][1];
				z2 += dcoef * _ffdpnts[idx][2];
				idx ++;
			}
			dcoef = BernFunc(lp.Y(),_m,iy);
			x1 += dcoef * x2;
			y1 += dcoef * y2;
			z1 += dcoef * z2;
		}
		dcoef = BernFunc(lp.X(),_l,ix);
		xv += dcoef * x1;
		yv += dcoef * y1;
		zv += dcoef * z1;
	}

	respnt.SetCoord(xv,yv,zv);

//	DTRACE("\n ffd before[%f,%f,%f],after[%f,%f,%f]",pnt.X(),pnt.Y(),pnt.Z(),xv,yv,zv);

	return TRUE;
}

//计算局部坐标
gp_Pnt	TcFFDFrame::CalcLcsCoord(const gp_Pnt& pnt)
{
	gp_Pnt lp;
	gp_Vec xd = _Sx;
	gp_Vec yd = _Ty;
	gp_Vec zd = _Uz;

	gp_Vec dv(pnt.X() - _min[0],pnt.Y() - _min[1],pnt.Z() - _min[2]);
	double val = dv.Dot(gp_Vec(yd.Crossed(zd)))/(yd.Crossed(zd).Dot(xd));
	lp.SetX(val);
	val = dv.Dot(gp_Vec(xd.Crossed(zd)))/(xd.Crossed(zd).Dot(yd));
	lp.SetY(val);
	val = dv.Dot(gp_Vec(xd.Crossed(yd)))/(xd.Crossed(yd).Dot(zd));
	lp.SetZ(val);

//	DTRACE("\n s %f, t %f, u %f",lp.X(),lp.Y(),lp.Z());

	return lp;
}

int		TcFFDFrame::Index(int ix,int iy,int iz)
{
	int idx = 0;
	idx = ix * (_m + 1) * (_n + 1);
	idx += iy * (_n + 1);
	idx += iz;

	return idx;
}
//////////////////////////////////////////////////////////////////////////
//
ScCmdFFD::ScCmdFFD(void)
{
}

ScCmdFFD::~ScCmdFFD(void)
{
}

//启动和结束命令
int	ScCmdFFD::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedMultiSelect(TRUE);
	if(HasSelObj())
	{
		SwitchState(S_PNT1,"创建变形控制器，选择第一点:");
	}
	else
	{
		SwitchState(S_OBJECT,"选择一个要变形的对象:");
	}

	return 0;
}

int	ScCmdFFD::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdFFD::InputEvent(const CPoint& point,int nState,
								   const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(GetState() == S_SELPNT)
		{
			InitSelect();
			if(MoreSelect())
			{
				while(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape();
					m_seqOfV.Append(aS);
				}

				//m_AISContext->CloseLocalContex();//yxk20191207
				SwitchState(S_BASPNT,"选择改变的基点:");
			}
			else
			{
				//没有选择，结束
				//m_AISContext->CloseLocalContex();//yxk20191207
				if(MakeFFD(TRUE))
					Done();
				else
					End();
			}
		}
		else
		{
			End();
		}
	}
	else
	{
		switch(GetState())
		{
		case S_OBJECT:
			{
				if(HasSelObj())
				{
					SwitchState(S_PNT1,"创建变形控制器，选择第一点:");
				}
			}
			break;
		case S_PNT1:
			{
				m_pnt1 = PixelToCSPlane(point.x,point.y);
				SwitchState(S_PNT2,"创建变形控制器，选择第二点:");
			}
			break;
		case S_PNT2:
			{
				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_pnt1) < 1e-6)
					return;

				m_pnt2 = pnt;
				SwitchState(S_PNT3,"创建变形控制器，选择第三点:");
			}
			break;
		case S_PNT3:
			{
				gp_Pnt pnt = PixelToModel(point.x,point.y);
				if(pnt.Distance(m_pnt1) < 1e-6 ||
					pnt.Distance(m_pnt2) < 1e-6)
					return;

				m_pnt3 = pnt;
				if(!MakeFFDFrame())
				{
					AfxMessageBox("创建变形器失败。");
					return;
				}
				SwitchState(S_SELPNT,"选择变形点,右键结束选择.");
				//
				//m_AISContext->OpenLocalContext();//yxk20191207
				m_AISContext->Activate(TmpObj(0),1);//vertex
			}
			break;
		case S_OPTION:
			break;
		case S_SELPNT:
			break;
		case S_BASPNT:
			{
				m_ptBas = PixelToModel(point.x,point.y);
				SwitchState(S_NEWPNT,"选择改变后的点:");
			}
			break;
		case S_NEWPNT:
			{
				MakeFFD(FALSE);
				//
				SwitchState(S_SELPNT,"选择变形点,右键结束选择.");
				//改动后控制点保存到原来的，准备新的修改
				for(int ix = 0;ix < m_ffdFrame._tcnt;ix ++)
				{
					m_ffdFrame._ctlpnts[ix][0] = m_ffdFrame._ffdpnts[ix][0];
					m_ffdFrame._ctlpnts[ix][1] = m_ffdFrame._ffdpnts[ix][1];
					m_ffdFrame._ctlpnts[ix][2] = m_ffdFrame._ffdpnts[ix][2];
				}
				m_seqOfV.Clear();
				//
				//m_AISContext->OpenLocalContext();//yxk20191207
				m_AISContext->Activate(TmpObj(0),1);//vertex
				/*if(MakeFFD(TRUE))
					Done();
				else
					End();*/
			}
			break;
		default:
			break;
		}
	}
}

//鼠标移动的响应函数
void	ScCmdFFD::MoveEvent(const CPoint& point,
								  const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_OBJECT:
		break;
	case S_PNT1:
		break;
	case S_PNT2:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_pnt1) < 1e-6)
				return;

			m_pnt2 = pnt;
			ShowRect(m_pnt1,pnt);
		}
		break;
	case S_PNT3:
		{
			gp_Pnt pnt = PixelToModel(point.x,point.y);
			if(pnt.Distance(m_pnt1) < 1e-6 ||
				pnt.Distance(m_pnt2) < 1e-6)
				return;

			m_pnt3 = pnt;
			ShowBox(m_pnt1,m_pnt2,pnt);
		}
		break;
	case S_OPTION:
		break;
	case S_SELPNT:
		break;
	case S_BASPNT:
		{
						
		}
		break;
	case S_NEWPNT:
		{
			gp_Pnt pnt = PixelToModel(point.x,point.y);
			if(pnt.Distance(m_ptBas) < 1e-6)
				return;

			ShowChangeFrame(pnt);
//			MakeFFD(FALSE);
		}
		break;
	default:
		break;
	}
}

BOOL	ScCmdFFD::HasSelObj()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(aS.IsNull())
			return FALSE;

		if(!ScBRepLib::IsVertex(aS) && !ScBRepLib::IsEdge(aS) && !ScBRepLib::IsWire(aS))
		{
			SelObj(0) = SelectedObj(FALSE);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL	ScCmdFFD::ShowRect(gp_Pnt pnt1,gp_Pnt pnt2)
{
	RemoveCtxObj(TmpObj(0));

	if(pnt1.Distance(pnt2) < 1e-6)
		return FALSE;

	gp_Pnt pnt3,pnt4;
	pnt3 = pnt1;
	pnt3.SetX(pnt2.X());
	pnt4 = pnt2;
	pnt4.SetX(pnt1.X());

	TopoDS_Edge aE1 = BRepBuilderAPI_MakeEdge(pnt1,pnt3);
	TopoDS_Edge aE2 = BRepBuilderAPI_MakeEdge(pnt3,pnt2);
	TopoDS_Edge aE3 = BRepBuilderAPI_MakeEdge(pnt2,pnt4);
	TopoDS_Edge aE4 = BRepBuilderAPI_MakeEdge(pnt4,pnt1);
	TopoDS_Wire aW = BRepBuilderAPI_MakeWire(aE1,aE2,aE3,aE4);

	if(aW.IsNull())
		return FALSE;

	Display(TmpObj(0),aW);

	return TRUE;
}

BOOL	ScCmdFFD::ShowBox(gp_Pnt pnt1,gp_Pnt pnt2,gp_Pnt pnt3)
{
	RemoveCtxObj(TmpObj(0));

	if(pnt1.Distance(pnt2) < 1e-6 || pnt1.Distance(pnt3) < 1e-6 ||
		pnt2.Distance(pnt3) < 1e-6)
		return FALSE;

	if(!CalcBoxCorner(pnt1,pnt2,pnt3))
		return FALSE;

	//生成对象
	TopoDS_Compound comp;
	BRep_Builder BB;
	BB.MakeCompound(comp);

	int b[] = {0,1,2,3,4,5,6,7,0,1,2,3};
	int e[] = {1,2,3,0,5,6,7,4,4,5,6,7};

	for(int ix = 0;ix < 12;ix ++)
	{
		gp_Pnt pb,pe;
		pb.SetCoord(m_boxCorner[b[ix]][0],m_boxCorner[b[ix]][1],m_boxCorner[b[ix]][2]);
		pe.SetCoord(m_boxCorner[e[ix]][0],m_boxCorner[e[ix]][1],m_boxCorner[e[ix]][2]);
		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(pb,pe);
		BB.Add(comp,aE);
	}

	Display(TmpObj(0),comp);


	return TRUE;
}

BOOL	ScCmdFFD::CalcBoxCorner(gp_Pnt pnt1,gp_Pnt pnt2,gp_Pnt pnt3)
{
	if(pnt1.Distance(pnt2) < 1e-6 || pnt1.Distance(pnt3) < 1e-6 ||
		pnt2.Distance(pnt3) < 1e-6)
		return FALSE;

	gp_Pnt minpt,maxpt;
	if(pnt1.X() < pnt2.X())
	{
		minpt.SetX(pnt1.X());
		maxpt.SetX(pnt2.X());
	}
	else
	{
		minpt.SetX(pnt2.X());
		maxpt.SetX(pnt1.X());
	}
	
	if(pnt1.Y() < pnt2.Y())
	{
		minpt.SetY(pnt1.Y());
		maxpt.SetY(pnt2.Y());
	}
	else
	{
		minpt.SetY(pnt2.Y());
		maxpt.SetY(pnt1.Y());
	}

	if(pnt1.Z() < pnt3.Z())
	{
		minpt.SetZ(pnt1.Z());
		maxpt.SetZ(pnt3.Z());
	}
	else
	{
		minpt.SetZ(pnt3.Z());
		maxpt.SetZ(pnt1.Z());
	}

	if(fabs(maxpt.Z() - minpt.Z()) < 1e-6)
		return FALSE;

	m_boxCorner[0][0] = minpt.X();
	m_boxCorner[0][1] = minpt.Y();
	m_boxCorner[0][2] = minpt.Z();

	m_boxCorner[1][0] = maxpt.X();
	m_boxCorner[1][1] = minpt.Y();
	m_boxCorner[1][2] = minpt.Z();

	m_boxCorner[2][0] = maxpt.X();
	m_boxCorner[2][1] = maxpt.Y();
	m_boxCorner[2][2] = minpt.Z();

	m_boxCorner[3][0] = minpt.X();
	m_boxCorner[3][1] = maxpt.Y();
	m_boxCorner[3][2] = minpt.Z();

	m_boxCorner[4][0] = minpt.X();
	m_boxCorner[4][1] = minpt.Y();
	m_boxCorner[4][2] = maxpt.Z();

	m_boxCorner[5][0] = maxpt.X();
	m_boxCorner[5][1] = minpt.Y();
	m_boxCorner[5][2] = maxpt.Z();

	m_boxCorner[6][0] = maxpt.X();
	m_boxCorner[6][1] = maxpt.Y();
	m_boxCorner[6][2] = maxpt.Z();

	m_boxCorner[7][0] = minpt.X();
	m_boxCorner[7][1] = maxpt.Y();
	m_boxCorner[7][2] = maxpt.Z();

	return TRUE;
}

BOOL	ScCmdFFD::MakeFFDFrame()
{
	//先不考虑ucs的情况。
	if(!CalcBoxCorner(m_pnt1,m_pnt2,m_pnt3))
		return FALSE;

	//记录范围
	m_ffdFrame._min[0] = m_boxCorner[0][0];
	m_ffdFrame._min[1] = m_boxCorner[0][1];
	m_ffdFrame._min[2] = m_boxCorner[0][2];
	m_ffdFrame._max[0] = m_boxCorner[6][0];
	m_ffdFrame._max[1] = m_boxCorner[6][1];
	m_ffdFrame._max[2] = m_boxCorner[6][2];

	//设置局部坐标系
	m_ffdFrame._Sx.SetCoord(m_boxCorner[1][0] - m_boxCorner[0][0],m_boxCorner[1][1] - m_boxCorner[0][1],
		m_boxCorner[1][2] - m_boxCorner[0][2]);
	m_ffdFrame._Ty.SetCoord(m_boxCorner[3][0] - m_boxCorner[0][0],m_boxCorner[3][1] - m_boxCorner[0][1],
		m_boxCorner[3][2] - m_boxCorner[0][2]);
	m_ffdFrame._Uz.SetCoord(m_boxCorner[4][0] - m_boxCorner[0][0],m_boxCorner[4][1] - m_boxCorner[0][1],
		m_boxCorner[4][2] - m_boxCorner[0][2]);
	
	//计算控制点
	m_ffdFrame._ctlpnts = new PNT3D[m_ffdFrame._tcnt];
	m_ffdFrame._ffdpnts = new PNT3D[m_ffdFrame._tcnt];
	//计算每个点的坐标
	double mx = m_boxCorner[0][0],my = m_boxCorner[0][1],mz = m_boxCorner[0][2];
	double dx = (m_boxCorner[6][0] - m_boxCorner[0][0])/m_ffdFrame._l,
		dy = (m_boxCorner[6][1] - m_boxCorner[0][1])/m_ffdFrame._m,
		dz = (m_boxCorner[6][2] - m_boxCorner[0][2])/m_ffdFrame._n;
	double x,y,z;
	int ix,iy,iz,idx;
	for(ix = 0;ix <= m_ffdFrame._l;ix ++)
	{
		x = mx + ix * dx;
		for(iy = 0;iy <= m_ffdFrame._m;iy ++)
		{
			y = my + iy * dy;
			for(iz = 0;iz <= m_ffdFrame._n;iz ++)
			{
				idx = ix * (m_ffdFrame._m + 1) * (m_ffdFrame._n + 1) + iy * (m_ffdFrame._n + 1) + iz; 
				z = mz + iz * dz;
				m_ffdFrame._ctlpnts[idx][0] = x;
				m_ffdFrame._ctlpnts[idx][1] = y;
				m_ffdFrame._ctlpnts[idx][2] = z;

				m_ffdFrame._ffdpnts[idx][0] = x;
				m_ffdFrame._ffdpnts[idx][1] = y;
				m_ffdFrame._ffdpnts[idx][2] = z;
			}
		}
	}

	//创建显示对象,显示box和各个点，每个点如何表示?先简单使用点表示
	//生成对象
	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	int b[] = {0,1,2,3,4,5,6,7,0,1,2,3};
	int e[] = {1,2,3,0,5,6,7,4,4,5,6,7};

	for(ix = 0;ix < 12;ix ++)
	{
		gp_Pnt pb,pe;
		pb.SetCoord(m_boxCorner[b[ix]][0],m_boxCorner[b[ix]][1],m_boxCorner[b[ix]][2]);
		pe.SetCoord(m_boxCorner[e[ix]][0],m_boxCorner[e[ix]][1],m_boxCorner[e[ix]][2]);
		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(pb,pe);
		BB.Add(aC,aE);
	}
	//依次创建每个点
	for(ix = 0;ix < m_ffdFrame._tcnt;ix ++)
	{
		gp_Pnt pnt(m_ffdFrame._ctlpnts[ix][0],m_ffdFrame._ctlpnts[ix][1],m_ffdFrame._ctlpnts[ix][2]);
		TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(pnt);
		BB.Add(aC,aV);
	}

	RemoveCtxObj(TmpObj(0));
	Display(TmpObj(0),aC);

	return TRUE;
}

BOOL	ScCmdFFD::FindPnts(PNT3D pnt)
{
	for(int ix = 1;ix <= m_seqOfV.Length();ix ++)
	{
		TopoDS_Vertex aV = TopoDS::Vertex(m_seqOfV.Value(ix));
		gp_Pnt tp = BRep_Tool::Pnt(aV);

		if(fabs(tp.X() - pnt[0]) < 1e-10 && fabs(tp.Y() - pnt[1]) < 1e-10 &&
			fabs(tp.Z() - pnt[2]) < 1e-10)
			return TRUE;
	}

	return FALSE;
}

BOOL	ScCmdFFD::ShowChangeFrame(gp_Pnt newpnt)
{
	gp_Vec vec(newpnt.X() - m_ptBas.X(),newpnt.Y() - m_ptBas.Y(),newpnt.Z() - m_ptBas.Z());

	//
	for(int ix = 0;ix < m_ffdFrame._tcnt;ix ++)
	{
		if(FindPnts(m_ffdFrame._ctlpnts[ix]))
		{
			m_ffdFrame._ffdpnts[ix][0] = m_ffdFrame._ctlpnts[ix][0] + vec.X();
			m_ffdFrame._ffdpnts[ix][1] = m_ffdFrame._ctlpnts[ix][1] + vec.Y();
			m_ffdFrame._ffdpnts[ix][2] = m_ffdFrame._ctlpnts[ix][2] + vec.Z();
		}
	}

	//显示临时对象
	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	int b[] = {0,1,2,3,4,5,6,7,0,1,2,3};
	int e[] = {1,2,3,0,5,6,7,4,4,5,6,7};

	for(int ix = 0;ix < 12;ix ++)
	{
		gp_Pnt pb,pe;
		pb.SetCoord(m_boxCorner[b[ix]][0],m_boxCorner[b[ix]][1],m_boxCorner[b[ix]][2]);
		pe.SetCoord(m_boxCorner[e[ix]][0],m_boxCorner[e[ix]][1],m_boxCorner[e[ix]][2]);
		TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(pb,pe);
		BB.Add(aC,aE);
	}
	//依次创建每个点
	for(int ix = 0;ix < m_ffdFrame._tcnt;ix ++)
	{
		gp_Pnt pnt(m_ffdFrame._ffdpnts[ix][0],m_ffdFrame._ffdpnts[ix][1],
			m_ffdFrame._ffdpnts[ix][2]);
		TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(pnt);
		BB.Add(aC,aV);
	}

	RemoveCtxObj(TmpObj(0));
	Display(TmpObj(0),aC);

	return FALSE;
}

BOOL	ScCmdFFD::MakeFFD(BOOL bDone)
{
	//第一步，将shape转换为nurbs。
	TopoDS_Shape aS = GetObjShape(SelObj(0));
	TopoDS_Shape aDS = ShapeCustom::DirectFaces(aS);
	BRepBuilderAPI_NurbsConvert nc;
	nc.Perform(aDS);
	if(!nc.IsDone())
	{
		AfxMessageBox("转换失败.");
		return FALSE;
	}

	TopoDS_Shape aNS = nc.Shape();
	TopoDS_Shape affdS;

	/*TcFFDeformShape ds;
	ds.Perform(aNS,&m_ffdFrame);
	if(!ds.IsDone())
		return FALSE;
	affdS = ds.Shape();*/
	//第二步，遍历每个shape，改变每个shape。
	//先分离为单个的曲面。
	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);
	
	TopExp_Explorer ex;
	for(ex.Init(aNS,TopAbs_FACE);ex.More();ex.Next())
	{
		TopoDS_Face aF = TopoDS::Face(ex.Current());
		//
		TopoDS_Face aNF = DeformFace(aF);
		if(aNF.IsNull())
			return FALSE;

		BB.Add(aC,aNF);

	/*	TopoDS_Shape aS = DeformFaceWire(aF,aNF);
		BB.Add(aC,aS);*/
	}
	////第三步，sewing重新缝合.
	affdS = SewFaces(aC);
	
	if(affdS.IsNull())
		return FALSE;

	//if(!BRepAlgo::IsValid(affdS))
	//{
	//	AfxMessageBox("无效对象。");
	//	return FALSE;
	//}

	//删除旧的显示新的。
	if(bDone)
	{
		if(!TmpObj(1).IsNull())
			m_AISContext->Remove(TmpObj(1), Standard_True);
		Display(NewObj(0),affdS);
	}
	else
	{
		if(!TmpObj(1).IsNull())
			m_AISContext->Remove(TmpObj(1), Standard_True);
		Display(TmpObj(1),affdS);
	}

	return TRUE;
}

TopoDS_Shape	ScCmdFFD::SewFaces(const TopoDS_Compound& aC)
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

TopoDS_Face		ScCmdFFD::DeformFace(const TopoDS_Face& aFace)
{
	TopoDS_Face aNF;
	Handle(Geom_Surface) aSurf = Handle(Geom_Surface)::DownCast(BRep_Tool::Surface(aFace)->Copy());
	Handle(Geom_BSplineSurface) aSplSurf = Handle(Geom_BSplineSurface)::DownCast(aSurf);

	//如果必要，提升阶数
	int ud = aSplSurf->UDegree(),vd = aSplSurf->VDegree();
	if(ud < 3 || vd < 3)
	{
		if(ud < 3) ud = 3;
		if(vd < 3) vd = 3;
		aSplSurf->IncreaseDegree(ud,vd);
		DTRACE("\n Increase Degree.");
	}

	//增加控制点
	TcBSplLib::IncSurfCtrlPnts(aSplSurf,100,100);

	//修改控制点
	int iu,iv;
	for(iu = 1;iu <= aSplSurf->NbUPoles();iu ++)
	{
		for(iv = 1;iv <= aSplSurf->NbVPoles();iv ++)
		{
			gp_Pnt pole = aSplSurf->Pole(iu,iv);
			//首先判断再不在范围内
			if(!m_ffdFrame.InRange(pole))
				continue;

			gp_Pnt pn;//
			m_ffdFrame.DeformPnt(pole,pn);
			aSplSurf->SetPole(iu,iv,pn);
		}
	}

	//根据新曲面和原来的face，生成新的face。
	aNF = BuildDeformedFace(aSplSurf,aFace);
	return aNF;
	//aNF = BRepBuilderAPI_MakeFace(aSurf);
/*	BRepBuilderAPI_MakeFace mf;
	mf.Init(aSurf,Standard_False);

	//根据参数曲线生成空间线
	TopoDS_Wire aW,aNW;
	double df,dl;
	TopExp_Explorer exw;
	for(exw.Init(aFace,TopAbs_WIRE);exw.More();exw.Next())
	{
		aW = TopoDS::Wire(exw.Current());
		if(aW.IsNull())
			continue;

		BRepBuilderAPI_MakeWire mw;

		BRepTools_WireExplorer eex;
		for(eex.Init(aW,aFace);eex.More();eex.Next())
		{
			TopoDS_Edge aE = eex.Current();

			Handle(Geom2d_Curve) aCur2d = BRep_Tool::CurveOnSurface(aE,aFace,df,dl);
			if(aCur2d.IsNull())
				continue;

			TopoDS_Edge aNE = BRepBuilderAPI_MakeEdge(aCur2d,aSurf,df,dl);
			if(aNE.IsNull())
				continue;

			BRepLib::BuildCurve3d(aNE);
			mw.Add(aNE);
			if(!mw.IsDone())
			{
				int i = 0;
			}
		}

		if(mw.IsDone())
		{
			aNW = mw.Wire();
		//	aNW.Orientation(aW.Orientation());
			mf.Add(aNW);
		}
	}

	if(mf.IsDone())
	{
		aNF = mf.Face();
	}

	return aNF;*/
}

//根据新曲面和原来的face，生成新的face。
TopoDS_Face		ScCmdFFD::BuildDeformedFace(const Handle(Geom_Surface)& aNewSurf,const TopoDS_Face& aFace)
{
	TopoDS_Face aNF;

	TopoDS_Wire aOutW = ScBRepLib::OuterWire(aFace);
	if(aOutW.IsNull())
	{
		ASSERT(FALSE);
		return aNF;
	}

	//生成新的aWire
	TopoDS_Wire aNW = BuildDeformedWire(aOutW,aNewSurf,aFace);
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
		aNW = BuildDeformedWire(aW,aNewSurf,aFace);
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

TopoDS_Wire		ScCmdFFD::BuildDeformedWire(const TopoDS_Wire& aWire,
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
		aNE.Orientation(aE.Orientation());
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


TopoDS_Shape	ScCmdFFD::DeformFaceWire(const TopoDS_Face& aFace,const TopoDS_Face& aNewFace)
{
	//曲面被修改了，生成新的face。
	//由于边界3d曲线可能也被修改了，因此，需要对每个edge也进行处理
	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aNewFace);

	TopoDS_Wire aW,aNW;
	double df,dl;
	TopExp_Explorer exw;
	for(exw.Init(aFace,TopAbs_WIRE);exw.More();exw.Next())
	{
		aW = TopoDS::Wire(exw.Current());
		if(aW.IsNull())
			continue;

		BRepTools_WireExplorer eex;
		for(eex.Init(aW,aFace);eex.More();eex.Next())
		{
			TopoDS_Edge aE = eex.Current();

			Handle(Geom2d_Curve) aCur2d = BRep_Tool::CurveOnSurface(aE,aFace,df,dl);
			if(aCur2d.IsNull())
				continue;

			TopoDS_Edge aNE = BRepBuilderAPI_MakeEdge(aCur2d,aSurf,df,dl);
			if(aNE.IsNull())
				continue;

			BRepLib::BuildCurve3d(aNE);
			//aNE.Orientation(aE.Orientation());
			BB.Add(aC,aNE);
		}
		
	}

	return aC;
}

//两个修改类。从Occ中继承
//yxk20191208
//IMPLEMENT_STANDARD_HANDLE(ScFFD_Modification,BRepTools_Modification)
IMPLEMENT_STANDARD_RTTIEXT(ScFFD_Modification,BRepTools_Modification)
//
//IMPLEMENT_STANDARD_TYPE(ScFFD_Modification)
//IMPLEMENT_STANDARD_SUPERTYPE(BRepTools_Modification) 
//IMPLEMENT_STANDARD_SUPERTYPE(MMgt_TShared)
//IMPLEMENT_STANDARD_SUPERTYPE(Standard_Transient)
//IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
//IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(BRepTools_Modification) 
//IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(MMgt_TShared)
//IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(Standard_Transient)
//IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
//IMPLEMENT_STANDARD_TYPE_END(ScFFD_Modification)
//endyxk

ScFFD_Modification::ScFFD_Modification()
{

}


Standard_Boolean ScFFD_Modification::NewSurface(
		const TopoDS_Face& F,
		Handle(Geom_Surface)& S,
		TopLoc_Location& L,
		Standard_Real& Tol,
		Standard_Boolean& RevWires,
		Standard_Boolean& RevFace)
{
	Tol = BRep_Tool::Tolerance(F);
	RevWires = Standard_False;
	RevFace = Standard_False;
	S = Handle(Geom_Surface)::DownCast(BRep_Tool::Surface(F,L)->Copy());
	if(!L.IsIdentity())
		S = Handle(Geom_Surface)::DownCast(S->Transformed(L.Transformation()));

	Handle(Standard_Type) TheTypeS = S->DynamicType();
	if (TheTypeS == STANDARD_TYPE(Geom_BSplineSurface)) 
	{
		Handle(Geom_BSplineSurface) S2 = Handle(Geom_BSplineSurface)::DownCast(S);
		for(Standard_Integer i = 1; i <= S2->NbUPoles(); i++) 
			for(Standard_Integer j = 1; j <= S2->NbVPoles(); j++) 
			{
				gp_Pnt pole = S2->Pole(i,j);
				//首先判断再不在范围内
				if(!m_ffdFrame->InRange(pole))
					continue;

				gp_Pnt pn;//
				m_ffdFrame->DeformPnt(pole,pn);
				S2->SetPole(i,j,pn);
			}
	}
	else
	{
		if (TheTypeS == STANDARD_TYPE(Geom_BezierSurface)) 
		{
			Handle(Geom_BezierSurface) S2 = Handle(Geom_BezierSurface)::DownCast(S);
			for(Standard_Integer i = 1; i <= S2->NbUPoles(); i++) 
				for(Standard_Integer j = 1; j <= S2->NbVPoles(); j++) 
				{
					gp_Pnt pole = S2->Pole(i,j);
					//首先判断再不在范围内
					if(!m_ffdFrame->InRange(pole))
						continue;

					gp_Pnt pn;//
					m_ffdFrame->DeformPnt(pole,pn);
					S2->SetPole(i,j,pn);
				}
		}
		else
			Standard_NoSuchObject_Raise_if(1,"BRepTools_GTrsfModification : Pb no BSpline/Bezier Type Surface");
	}

	L.Identity();
	return Standard_True;
}

Standard_Boolean ScFFD_Modification::NewCurve(
		const TopoDS_Edge& E,
		Handle(Geom_Curve)& C,
		TopLoc_Location& L,
		Standard_Real& Tol)
{
	Standard_Real f,l;
	Tol = BRep_Tool::Tolerance(E);
	C = BRep_Tool::Curve(E, L, f, l);

	if (!C.IsNull()) 
	{
		C = Handle(Geom_Curve)::DownCast(C->Copy()->Transformed(L.Transformation()));
		Handle(Standard_Type) TheTypeC = C->DynamicType();
		if (TheTypeC == STANDARD_TYPE(Geom_BSplineCurve)) 
		{
			Handle(Geom_BSplineCurve) C2 = Handle(Geom_BSplineCurve)::DownCast(C);
			for(Standard_Integer i = 1; i <= C2->NbPoles(); i++) 
			{
				gp_Pnt pole = C2->Pole(i);
				//首先判断再不在范围内
				if(!m_ffdFrame->InRange(pole))
					continue;

				gp_Pnt pn;//
				m_ffdFrame->DeformPnt(pole,pn);
				C2->SetPole(i,pn);
			}
		}
		else
			if(TheTypeC == STANDARD_TYPE(Geom_BezierCurve)) 
			{
				Handle(Geom_BezierCurve) C2 = Handle(Geom_BezierCurve)::DownCast(C);
				for(Standard_Integer i = 1; i <= C2->NbPoles(); i++) 
				{
					gp_Pnt pole = C2->Pole(i);
					//首先判断再不在范围内
					if(!m_ffdFrame->InRange(pole))
						continue;

					gp_Pnt pn;//
					m_ffdFrame->DeformPnt(pole,pn);
					C2->SetPole(i,pn);
				}
			}
			else
				Standard_NoSuchObject_Raise_if(1,"BRepTools_GTrsfModification : Pb no BSpline/Bezier Type Curve");
		C = new Geom_TrimmedCurve(C, f, l);
	}
	L.Identity() ;  
	return Standard_True;
}

Standard_Boolean ScFFD_Modification::NewPoint(
		const TopoDS_Vertex& V,
		gp_Pnt& P,
		Standard_Real& Tol)
{
	gp_Pnt pnt = BRep_Tool::Pnt(V);
	Tol = BRep_Tool::Tolerance(V);

	//首先判断再不在范围内
	if(!m_ffdFrame->InRange(pnt))
		return Standard_False;

	m_ffdFrame->DeformPnt(pnt,P);

	return Standard_True;
}

Standard_Boolean ScFFD_Modification::NewCurve2d(
		const TopoDS_Edge& E,
		const TopoDS_Face& F,
		const TopoDS_Edge& NewE,
		const TopoDS_Face& NewF,
		Handle(Geom2d_Curve)& C,
		Standard_Real& Tol)
{
	return Standard_False;
}

Standard_Boolean ScFFD_Modification::NewParameter(
		const TopoDS_Vertex& V,
		const TopoDS_Edge& E,
		Standard_Real& P,
		Standard_Real& Tol) 
{
	return Standard_False;
}

GeomAbs_Shape ScFFD_Modification::Continuity(
		const TopoDS_Edge& E,
		const TopoDS_Face& F1,
		const TopoDS_Face& F2,
		const TopoDS_Edge& NewE,
		const TopoDS_Face& NewF1,
		const TopoDS_Face& NewF2) 
{
	return BRep_Tool::Continuity(E,F1,F2);
}


//////////////////////////////////////////////////////////////////////////
//

TcFFDeformShape::TcFFDeformShape()
{
	myModification = new ScFFD_Modification();
}

void	TcFFDeformShape::Perform(const TopoDS_Shape& S,TcFFDFrame *pFrm)
{
	Handle(ScFFD_Modification) aM = Handle(ScFFD_Modification)::DownCast(myModification);
	aM->SetFFDFrame(pFrm);
	DoModif(S,myModification);
}