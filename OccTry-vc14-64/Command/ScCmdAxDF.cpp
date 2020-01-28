#include "StdAfx.h"
#include "ScBRepLib.h"
#include "ScCmdBSpline.h"
#include ".\sccmdaxdf.h"

//////////////////////////////////////////////////////////////////////////
//
TcCurveFrame::TcCurveFrame()
{
	_u = 0.0;
}

TcCurveFrame::TcCurveFrame(const TcCurveFrame& other)
{
	*this = other;
}

TcCurveFrame&	TcCurveFrame::operator=(const TcCurveFrame& other)
{
	_u = other._u;
	_O = other._O;
	_dT = other._dT;
	_dN = other._dN;
	_dB = other._dB;

	return *this;
}

void	TcCurveFrame::Set(double u,const gp_Pnt& pnt,const gp_Dir& T,
							const gp_Dir& N,const gp_Dir& B)
{
	_u = u;
	_O = pnt;
	_dT = T;
	_dN = N;
	_dB = B;
}

//////////////////////////////////////////////////////////////////////////
//
TcCurveRMF::TcCurveRMF()
{
	this->m_nSamps = 64;//Ĭ�ϸ���
}

BOOL	TcCurveRMF::Init(const Handle(Geom_Curve)& aCur,double df,double dl,int nSamps)
{
	this->m_aCurve = aCur;
	m_nSamps = nSamps;
	m_df = df;
	m_dl = dl;
	m_dstep = (dl - df)/nSamps;//ע������

	return TRUE;
}

BOOL	TcCurveRMF::CalcLcsCoord(double u,const gp_Pnt& pnt,gp_Pnt& lcpnt)
{
	//���ҵ����ڵ�frm
	UINT idx = (UINT)((u - m_df)/m_dstep);
	if(idx >= m_frmArray.size())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	TcCurveFrame& frm = m_frmArray.at(idx);

	gp_Trsf wtol;
	gp_Ax3 ax(frm._O,frm._dN,frm._dT);
	wtol.SetTransformation(ax);//��wcs��lcs��ת��
	lcpnt = pnt;
	lcpnt.Transform(wtol);//ת��lcs��

	return TRUE;
}

BOOL	TcCurveRMF::CalcDfmCoord(double u,const gp_Pnt& lcpnt,gp_Pnt& pnt)
{
	//���ҵ����ڵ�frm
	UINT idx = (UINT)((u - m_df)/m_dstep);
	if(idx >= m_frmArray.size())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	TcCurveFrame& frm = m_frmArray.at(idx);

	gp_Trsf ltow;
	gp_Ax3 ax(frm._O,frm._dN,frm._dT);
	ltow.SetTransformation(ax);//��wcs��lcs��ת��
	ltow.Invert();

	pnt = lcpnt;
	pnt.Transform(ltow);//ת��wcs��

	return TRUE;
}

//����
BOOL	TcCurveRMF::CalcRotateRMF()
{
	return CalcRMF(M_ROTATE);
}

//sloan��������
BOOL	TcCurveRMF::CalcSloanRMF()
{
	return CalcRMF(M_SLOAN);
}

//1:��ת����2��sloan����
BOOL	TcCurveRMF::CalcRMF(int nMethod)
{
	m_frmArray.clear();//�����

	double t;
	gp_Dir T0,N0,B0,T,N,B;
	gp_Pnt pnt;
	TcCurveFrame frm;

	int ix;
	for(ix = 0;ix <= m_nSamps;ix ++)
	{
		t = m_df + ix * m_dstep;
		if(ix == 0)
		{//��ʼ����
			gp_Vec vt,vtt;
			BOOL bOk = TRUE;
			try{
				m_aCurve->D2(t,pnt,vt,vtt);
				if(vt.SquareMagnitude() < 1e-10)//û����
					return FALSE;
				T = gp_Dir(vt);
				if(vtt.SquareMagnitude() < 1e-10)
					bOk = FALSE;
				else
				{
					B0 = gp_Dir(vtt);
					B = T.Crossed(B0);
					if(B.XYZ().Modulus() < 1e-6)
					{
						gp_Ax2 axe(gp_Pnt(0,0,0),T);
						B.SetXYZ(axe.YDirection().XYZ());
					}
					N = B.Crossed(T);
				}
			}catch(Standard_Failure){//1������
				bOk = FALSE;
			}

			if(!bOk)
			{
				try{
					m_aCurve->D1(t,pnt,vt);
				}catch(Standard_Failure){
					return FALSE;
				}
				if(vt.SquareMagnitude() < 1e-10)//û����
					return FALSE;
				T = gp_Dir(vt);
				//�������ʼ��
				gp_Ax2 axe(gp_Pnt(0,0,0),T);
				B.SetXYZ(axe.YDirection().XYZ());
				N = B0.Crossed(T);
			}

			frm.Set(t,pnt,T,N,B);
			m_frmArray.push_back(frm);
		}
		else
		{//ʹ����ת������
			if(nMethod == M_ROTATE)
			{
				if(!DoCalcRotateFrame(t,T0,N0,B0,T,N,B))
					return FALSE;
			}
			else if(nMethod == M_SLOAN)
			{
				if(!DoCalcSloanFrame(t,T0,N0,B0,T,N,B))
					return FALSE;
			}
		}

		T0 = T;N0 = N;B0 = B;
	}

	return TRUE;
}

BOOL	TcCurveRMF::DoCalcRotateFrame(double t,
						  const gp_Dir& T0,const gp_Dir& N0,const gp_Dir& B0,
						  gp_Dir& T,gp_Dir& N,gp_Dir& B)
{
	gp_Pnt pnt;
	TcCurveFrame frm;
	gp_Vec vt;

	try{
		m_aCurve->D1(t,pnt,vt);
	}catch(Standard_Failure){
		return FALSE;
	}

	if(vt.SquareMagnitude() < 1e-10)
		return FALSE;

	T = gp_Dir(vt);
	//��T0��T1=A
	gp_Dir A;
	A = T0.Crossed(T);
	if(A.XYZ().Modulus() < 1e-10)
	{
		T = T0;
		N = N0;
		B = B0;
	}
	else
	{
		//������ת����
		double sqx = A.X() * A.X(),sqy = A.Y() * A.Y(),sqz = A.Z() * A.Z();
		double cos = T0.Dot(T),cosl = 1 - cos;
		double xycosl = A.X() * A.Y() * cosl;
		double yzcosl = A.Y() * A.Z() * cosl;
		double zxcosl = A.X() * A.Z() * cosl;
		double sin = sqrt(1 - cos * cos),xsin = A.X() * sin,
			ysin = A.Y() * sin,zsin = A.Z() * sin;

		gp_Mat m0(sqx + (1 - sqx) * cos,xycosl + zsin,zxcosl - ysin,
			xycosl - zsin,sqy + (1 - sqy) * cos, yzcosl + xsin,
			zxcosl + ysin,yzcosl - xsin,sqz + (1 - sqz) * cos);

		//�����µ�N
		N.SetX(m0.Row(1).Dot(N0.XYZ()));
		N.SetY(m0.Row(2).Dot(N0.XYZ()));
		N.SetZ(m0.Row(3).Dot(N0.XYZ()));

		B = T.Crossed(N);
	}

	frm.Set(t,pnt,T,N,B);
	m_frmArray.push_back(frm);

	return TRUE;
}

BOOL	TcCurveRMF::DoCalcSloanFrame(double t,
						 const gp_Dir& T0,const gp_Dir& N0,const gp_Dir& B0,
						 gp_Dir& T,gp_Dir& N,gp_Dir& B)
{
	gp_Pnt pnt;
	TcCurveFrame frm;
	gp_Vec vt;

	try{
		m_aCurve->D1(t,pnt,vt);
	}catch(Standard_Failure){
		return FALSE;
	}
	
	if(vt.SquareMagnitude() < 1e-10)
		return FALSE;
	T = gp_Dir(vt);
	N = B0.Crossed(T);
	if(N.XYZ().Modulus() < 1e-10)
		return FALSE;//�˻����
	B = T.Crossed(N);

	frm.Set(t,pnt,T,N,B);
	m_frmArray.push_back(frm);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//��¼ÿ�������������Ϣ
ScAxDFSurfInfo::ScAxDFSurfInfo()
{
	m_nus = m_nvs = 0;
	m_bInit = FALSE;
	m_pArLcsCoord = NULL;
	m_pUOfCurve = NULL;
}

ScAxDFSurfInfo::~ScAxDFSurfInfo()
{
	if(m_pUOfCurve)
	{
		delete [] m_pUOfCurve;
		m_pUOfCurve = NULL;
	}

	if(m_pArLcsCoord)
	{
		delete[] m_pArLcsCoord;
		m_pArLcsCoord = NULL;
	}
}

//��ʼ��
BOOL	ScAxDFSurfInfo::Init(int nus,int nvs)
{
	m_nus = nus;
	m_nvs = nvs;

	m_pUOfCurve = new double[nus * nvs];
	m_pArLcsCoord = new PNT3D[nus * nvs];

	m_bInit = TRUE;

	return TRUE;
}

void	ScAxDFSurfInfo::SetUOfCurve(int iu,int iv,double u)
{
	ASSERT(m_bInit);
	m_pUOfCurve[IndexOf(iu,iv)] = u;
}

double	ScAxDFSurfInfo::UOfCurve(int iu,int iv)
{
	ASSERT(m_bInit);
	return m_pUOfCurve[IndexOf(iu,iv)];
}

void	ScAxDFSurfInfo::SetLcsCoord(int iu,int iv,const gp_Pnt& pnt)
{
	ASSERT(m_bInit);
	int idx  = IndexOf(iu,iv);
	m_pArLcsCoord[idx][0] = pnt.X();
	m_pArLcsCoord[idx][1] = pnt.Y();
	m_pArLcsCoord[idx][2] = pnt.Z();
}

gp_Pnt	ScAxDFSurfInfo::LcsCoord(int iu,int iv)
{
	ASSERT(m_bInit);
	int idx  = IndexOf(iu,iv);
	gp_Pnt pnt;
	pnt.SetCoord(m_pArLcsCoord[idx][0],m_pArLcsCoord[idx][1],m_pArLcsCoord[idx][2]);

	return pnt;
}

//ע��:iu��iv��1��ʼ
int		ScAxDFSurfInfo::IndexOf(int iu,int iv)
{
	ASSERT((iu >= 1) && (iu <= m_nus) && (iv >= 1) && (iv <= m_nvs));
	
	int idx = (iu - 1) * m_nvs + iv - 1;
	return idx;
}


ScCmdAxDF::ScCmdAxDF(void)
{
	m_pSurfInfo = NULL;
	m_bChanged = FALSE;
	m_pCurRMF = NULL;
}

ScCmdAxDF::~ScCmdAxDF(void)
{
	if(m_pSurfInfo)
	{
		delete m_pSurfInfo;
	}
	if(m_pCurRMF)
	{
		delete m_pCurRMF;
	}
}

//�����ͽ�������
int		ScCmdAxDF::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);

	if(HasSelObj())
	{
		SwitchState(S_TOOL,"ѡ��һ��������Ϊ���ι���:");
	}
	else
	{
		SwitchState(S_OBJ,"ѡ��һ��Ҫ��������:");
	}

	return 0;
}

int		ScCmdAxDF::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdAxDF::InputEvent(const CPoint& point,int nState,
								   const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		//m_AISContext->CloseLocalContex();//yxk20191207

		if(m_bChanged)
		{	
			//
			RemoveCtxObj(TmpObj(0));
			RemoveCtxObj(TmpObj(1));

			m_AISContext->Display(SelObj(1),Standard_False);
			m_AISContext->Redisplay(SelObj(1), Standard_True);

			if(DeformObj(FALSE))
				Done();
			else
				End();
		}
		else
		{
			if(!SelObj(1).IsNull())
			{
				m_AISContext->Display(SelObj(1),Standard_False);
				m_AISContext->Redisplay(SelObj(1), Standard_True);
			}
			//
			End();
		}
	}
	else
	{
		switch(GetState())
		{
		case S_OBJ:
			{
				if(HasSelObj())
				{
					SwitchState(S_TOOL,"ѡ��һ��������Ϊ���ι���:");
				}
			}
			break;
		case S_TOOL:
			{
				if(HasSelTool())
				{
					if(!InitDeform())
					{
						if(!SelObj(1).IsNull())
						{
							m_AISContext->Display(SelObj(1),Standard_False);
							m_AISContext->Redisplay(SelObj(1), Standard_True);
						}

						End();
						return;
					}

					ShowToolFrame();
					SwitchState(S_SELPNT,"ʹ�ù��߽��б���:");
//					m_AISContext->Remove(SelObj(0));
				}
			}
			break;
		case S_SELPNT:
			{
				InitSelect();
				if(MoreSelect())
				{
					m_selPnt = BRep_Tool::Pnt(TopoDS::Vertex(SelectedShape(FALSE)));
					//m_AISContext->CloseLocalContex();//yxk20191207
					NeedSelect(FALSE);
					SwitchState(S_MOVEPNT,"�ı���λ�õ�������:");
		//			m_AISContext->Display(SelObj(0));
		//			m_AISContext->Redisplay(SelObj(0));
				}
			}
			break;
		case S_MOVEPNT:
			{
				//m_AISContext->CloseLocalContex();//yxk20191207
				//
				gp_Pnt pnt = PixelToModel(point.x,point.y);
				if(pnt.Distance(m_selPnt) < 1e-6)
					return;

				//�ı�����
				ChangeToolCurve(pnt,FALSE);
				DeformObj(TRUE);

				ShowToolFrame();
				SwitchState(S_SELPNT,"ʹ�ù��߽��б���:");
				NeedSelect(TRUE);
			}
			break;
		default:
			break;
		}
	}
}

//����ƶ�����Ӧ����
void	ScCmdAxDF::MoveEvent(const CPoint& point,
								  const Handle(V3d_View)& aView)
{
	if(GetState() == S_MOVEPNT)
	{
		//m_AISContext->CloseLocalContex();//yxk20191207

		gp_Pnt pnt = PixelToModel(point.x,point.y);
		if(pnt.Distance(m_selPnt) < 1e-6)
			return;

		//�ı�����
		ChangeToolCurve(pnt,TRUE);
//		DeformObj(TRUE);

		ShowToolFrame();
		//m_AISContext->CloseLocalContex();//yxk20191207
	}
}

BOOL	ScCmdAxDF::HasSelObj()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsFace(aS))
		{
			SelObj(0) = SelectedObj(FALSE);

			TopoDS_Face aF = TopoDS::Face(aS);
			Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aF);
			if(!aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
			{
				BRepBuilderAPI_NurbsConvert ns;
				ns.Perform(aS);
				if(!ns.IsDone())
					return FALSE;

				aF = TopoDS::Face(ns.Shape());
				if(aF.IsNull())
					return FALSE;

				aSurf = BRep_Tool::Surface(aF);
			}

			m_pSurfInfo = new ScAxDFSurfInfo;
			m_pSurfInfo->m_cpySurf = Handle(Geom_BSplineSurface)::DownCast(aSurf->Copy());
			if(m_pSurfInfo->m_cpySurf.IsNull())
			{
				delete m_pSurfInfo;
				return FALSE;
			}
			//��Ҫ�������������
			int ud = m_pSurfInfo->m_cpySurf->UDegree(),vd = m_pSurfInfo->m_cpySurf->VDegree();
			if(ud < 3 || vd < 3)
			{
				if(ud < 3) ud = 3;
				if(vd < 3) vd = 3;
				m_pSurfInfo->m_cpySurf->IncreaseDegree(ud,vd);
			}
			//���ӿ��Ƶ���
			TcBSplLib::IncSurfCtrlPnts(m_pSurfInfo->m_cpySurf,16,16);

			m_pSurfInfo->m_tmpSurf = Handle(Geom_BSplineSurface)::DownCast(m_pSurfInfo->m_cpySurf->Copy());
			m_pSurfInfo->Init(m_pSurfInfo->m_cpySurf->NbUPoles(),m_pSurfInfo->m_cpySurf->NbVPoles());

			return TRUE;
		}
	}

	return FALSE;
}

BOOL	ScCmdAxDF::HasSelTool()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsEdge(aS))
		{
			SelObj(1) = SelectedObj();
			TopoDS_Edge aE = TopoDS::Edge(aS);

			double df,dl;
			Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE,df,dl);
			if(aCur.IsNull())
				return FALSE;

			if(!aCur->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
			{
				BRepBuilderAPI_NurbsConvert ns;
				ns.Perform(aS);
				if(!ns.IsDone())
					return FALSE;

				aE = TopoDS::Edge(ns.Shape());
				if(aE.IsNull())
					return FALSE;

				aCur = BRep_Tool::Curve(aE,df,dl);
				if(aCur.IsNull())
					return FALSE;

				Handle(Geom_BSplineCurve) abCur = Handle(Geom_BSplineCurve)::DownCast(aCur);
				if(abCur->Degree() < 3)
					abCur->IncreaseDegree(3);
			}

			m_curTool = Handle(Geom_BSplineCurve)::DownCast(aCur->Copy());
			m_tmpTool = Handle(Geom_BSplineCurve)::DownCast(aCur->Copy());

			m_pCurRMF = new TcCurveRMF();
			m_pCurRMF->Init(m_tmpTool,df,dl,64);

			//��ʽ��ʱ�ɱ༭����
			TopoDS_Edge atE = BRepBuilderAPI_MakeEdge(m_tmpTool);
			Display(TmpObj(1),atE);
			m_AISContext->Remove(SelObj(1), Standard_True);

			return TRUE;
		}
	}
	return FALSE;
}

//ҪΪÿ�����Ƶ�����������ϵ�����㣬��¼����������ֲ�����ϵ��������ֲ����ꡣ
BOOL	ScCmdAxDF::InitDeform()//Ϊ��������ʼ��
{
	int iu,iv;

	if(!m_pCurRMF->CalcSloanRMF() && !m_pCurRMF->CalcRotateRMF())
	{
		DTRACE("\n cal RMF failed.");
		return FALSE;
	}

	gp_Pnt pnt,lcpnt,ptNear;
	for(iu = 1;iu <= m_pSurfInfo->m_cpySurf->NbUPoles();iu ++)
	{
		for(iv = 1;iv <= m_pSurfInfo->m_cpySurf->NbVPoles();iv ++)
		{
			gp_Pnt pnt = m_pSurfInfo->m_cpySurf->Pole(iu,iv);

			//���������Ĳ���
			GeomAPI_ProjectPointOnCurve pp(pnt,m_curTool);
			if(pp.NbPoints() == 0)
				return FALSE;

			double u = pp.LowerDistanceParameter();
			m_pSurfInfo->SetUOfCurve(iu,iv,u);

			//����ֲ�����ϵ�������ڲ��ľֲ�����
			if(!m_pCurRMF->CalcLcsCoord(u,pnt,lcpnt))
				return FALSE;
			////��¼
			m_pSurfInfo->SetLcsCoord(iu,iv,lcpnt);
		}
	}

	return TRUE;
}

void	ScCmdAxDF::ShowToolFrame()//��ʽ���ߵĿ��Ƶ㣬�Ա��ڱ༭��
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	for(int ix = 1;ix <= m_tmpTool->NbPoles();ix ++)
	{
		gp_Pnt pnt = m_tmpTool->Pole(ix);
		TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(pnt);
		BB.Add(aC,aV);
	}

	Display(TmpObj(0),aC);

	//m_AISContext->OpenLocalContext();//yxk20191207
	m_AISContext->Activate(TmpObj(0),1);//����ѡ��
}

BOOL	ScCmdAxDF::ChangeToolCurve(const gp_Pnt& pnt,BOOL bTmp)
{
	RemoveCtxObj(TmpObj(1));

	for(int ix = 1;ix <= m_curTool->NbPoles();ix ++)
	{
		gp_Pnt pt = m_curTool->Pole(ix);
		if(pt.Distance(m_selPnt) < 1e-10)
		{
			m_tmpTool->SetPole(ix,pnt);
			break;
		}
	}

	if(!bTmp)
	{
		for(int ix = 1;ix <= m_tmpTool->NbPoles();ix ++)
		{
			m_curTool->SetPole(ix,m_tmpTool->Pole(ix));
		}
	}

	TopoDS_Edge atE = BRepBuilderAPI_MakeEdge(m_tmpTool);
	Display(TmpObj(1),atE);

	return TRUE;
}

BOOL	ScCmdAxDF::DeformObj(BOOL bTmp)
{
	//����u���¼���ֲ�����ϵ�������¿��Ƶ�
	int iu,iv;

	RemoveCtxObj(TmpObj(2));

	//���¼���RMF
	if(!m_pCurRMF->CalcSloanRMF() && !m_pCurRMF->CalcRotateRMF())
	{
		DTRACE("\n re calc RMF error.");
		return FALSE;
	}

	gp_Pnt pnt,lcpnt;	
	for(iu = 1;iu <= m_pSurfInfo->m_tmpSurf->NbUPoles();iu ++)
	{
		for(iv = 1;iv <= m_pSurfInfo->m_tmpSurf->NbVPoles();iv ++)
		{
			double u = m_pSurfInfo->UOfCurve(iu,iv);
			lcpnt = m_pSurfInfo->LcsCoord(iu,iv);//��ȡ�ֲ�����

			//����ֲ�����ϵ�������ڲ��ľֲ�����
			if(!m_pCurRMF->CalcDfmCoord(u,lcpnt,pnt))
				return FALSE;
			//���µ�
			m_pSurfInfo->m_tmpSurf->SetPole(iu,iv,pnt);
		}
	}
	TopoDS_Face aF = BRepBuilderAPI_MakeFace(m_pSurfInfo->m_tmpSurf, Precision::Confusion());//yxk20200104
	if(aF.IsNull())
		return FALSE;

	m_bChanged = TRUE;

	if(bTmp)
	{
		Display(TmpObj(2),aF);
	}
	else
	{
		Display(NewObj(0),aF);
		BeginUndo("AxDF deform");
		UndoAddNewObj(NewObj(0));
		EndUndo(TRUE);
	}

	return TRUE;
}
