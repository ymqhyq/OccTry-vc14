#include "StdAfx.h"
#include <BRepBndLib.hxx>
#include <ELSLib.hxx>
#include "ScBRepLib.h"

#include "FFDAlgo.h"
#include "FFDObj.h"
#include "FFDTool.h"

#include "ScCmdBSpline.h"
#include ".\sccmdsurfffd.h"

//////////////////////////////////////////////////////////////////////////
//
TcSurfPrjInfo::TcSurfPrjInfo()
{
	m_nus = m_nvs = 0;
	m_bInit = FALSE;
	m_pDstArray = NULL;
	m_uvArray = NULL;
}

TcSurfPrjInfo::~TcSurfPrjInfo()
{
	if(m_pDstArray)
	{
		delete[] m_pDstArray;
		m_pDstArray = NULL;
	}

	if(m_uvArray)
	{
		delete[] m_uvArray;
		m_uvArray = NULL;
	}
}

BOOL	TcSurfPrjInfo::Init(int nus,int nvs)
{
	m_nus = nus;
	m_nvs = nvs;
	int ntol = nus * nvs;
	m_pDstArray = new double[ntol];
	m_uvArray = new PNT3D[ntol];

	m_bInit = TRUE;
	return TRUE;
}

void	TcSurfPrjInfo::SetDist(int iu,int iv,double dist)
{
	int idx = IndexOf(iu,iv);
	m_pDstArray[idx] = dist;
}

double	TcSurfPrjInfo::GetDist(int iu,int iv)
{
	int idx = IndexOf(iu,iv);
	return m_pDstArray[idx];
}

void	TcSurfPrjInfo::SetPrjUV(int iu,int iv,double u,double v)
{
	int idx = IndexOf(iu,iv);
	m_uvArray[idx][0] = u;
	m_uvArray[idx][1] = v;
}

gp_Pnt2d	TcSurfPrjInfo::GetPrjUV(int iu,int iv)
{
	int idx = IndexOf(iu,iv);
	gp_Pnt2d uv(m_uvArray[idx][0],m_uvArray[idx][1]);
	return uv;
}

int		TcSurfPrjInfo::IndexOf(int iu,int iv)
{
	ASSERT((iu >= 1) && (iu <= m_nus) && (iv > 0) && (iv <= m_nvs));
	return (iu - 1) * m_nvs + iv - 1;
}

//////////////////////////////////////////////////////////////////////////
ScCmdSurfFFD::ScCmdSurfFFD(void)
{
	m_bChanged = FALSE;
	m_pSurfPrjInfo = NULL;

	m_pFFDAlgo = NULL;
	m_pFFDObj = NULL;
	m_pFFDTool = NULL;
}

ScCmdSurfFFD::~ScCmdSurfFFD(void)
{
	if(m_pFFDAlgo)
	{
		delete m_pFFDAlgo;
		m_pFFDAlgo = NULL;
	}

	if(m_pFFDObj)
	{
		delete m_pFFDObj;
		m_pFFDObj = NULL;
	}

	if(m_pFFDTool)
	{
		delete m_pFFDTool;
		m_pFFDTool = NULL;
	}
}

//�����ͽ�������
int		ScCmdSurfFFD::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelObj())
	{
		NeedSelect(FALSE);
		if(!InitDefrom())
		{
			AfxMessageBox("���γ�ʼ��ʧ��.");
			End();
			return 0;
		}
		SwitchState(S_SELPNT,"ѡ�����б���:");
	}
	else
	{
		SwitchState(S_OBJ,"ѡ��Ҫ���ε�����:");
	}

	return 0;
}

int	ScCmdSurfFFD::End()
{
	//m_AISContext->CloseLocalContex();//yxk20191207
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdSurfFFD::InputEvent(const CPoint& point,int nState,
								   const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(GetState() == S_SELPNT)
		{
			InitSelect();
			m_seqOfPnt.Clear();
			while(MoreSelect())
			{
				gp_Pnt pnt = BRep_Tool::Pnt(TopoDS::Vertex(SelectedShape()));
				m_seqOfPnt.Append(pnt);
			}
			//m_AISContext->CloseLocalContex();//yxk20191207

			if(m_seqOfPnt.Length() > 0)
			{
				NeedMultiSelect(FALSE);
				SwitchState(S_BASPNT,"ѡ�����:");
			}
			else
			{//û��ѡ��㣬������
				if(m_bChanged)
				{
					if(DoDeform(TRUE))
						Done();
					else
						End();
				}
				else
				{
					End();
				}
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
		case S_OBJ:
			{
				if(HasSelObj())
				{
					NeedSelect(FALSE);
					if(!InitDefrom())
					{
						AfxMessageBox("���γ�ʼ��ʧ��.");
						End();
						return;
					}
					SwitchState(S_SELPNT,"ѡ�����б���:");
					NeedMultiSelect(TRUE);
					ShowCtrlPnts();
				}
			}
		case S_SELPNT:
			{
				
			}
			break;
		case S_BASPNT:
			{
				m_basPnt = PixelToModel(point.x,point.y);
				SwitchState(S_CHANGING,"�����µĵ�:");
			}
			break;
		case S_CHANGING:
			{
				gp_Pnt pnt = PixelToModel(point.x,point.y);
				if(pnt.Distance(m_basPnt) < 1e-6)
					return;

				m_chgPnt = pnt;
				ChangeToolSurf(FALSE);
				if(!DoDeform(FALSE))
				{
					End();
					return;
				}
				NeedMultiSelect(TRUE);
				SwitchState(S_SELPNT,"ѡ�����б���:");
				ShowCtrlPnts();
			}
			break;
		default:
			break;
		}
	}
}

//����ƶ�����Ӧ����
void	ScCmdSurfFFD::MoveEvent(const CPoint& point,
								  const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_OBJ:
	case S_SELPNT:
	case S_BASPNT:
		break;
	case S_CHANGING:
		{
			gp_Pnt pnt = PixelToModel(point.x,point.y);
			if(pnt.Distance(m_basPnt) < 1e-6)
				return;

			m_chgPnt = pnt;
			ChangeToolSurf(TRUE);
			/*if(!DoDeform(FALSE))
			{
				End();
				return;
			}*/
			ShowCtrlPnts();
			//m_AISContext->CloseLocalContex();//yxk20191207
		}
		break;
	default:
		break;
	}
}


BOOL	ScCmdSurfFFD::HasSelObj()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aShape = SelectedShape(FALSE);
		if(ScBRepLib::IsFace(aShape) || ScBRepLib::IsShell(aShape) ||
			ScBRepLib::IsSolid(aShape))
		{
			SelObj(0) = SelectedObj();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL	ScCmdSurfFFD::ShowCtrlPnts()
{
	RemoveCtxObj(TmpObj(1));

	int iu,iv;
	gp_Pnt pnt;
	TopoDS_Vertex aV;
	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	for(iu = 1;iu <= m_toolSurf->NbUPoles();iu ++)
	{
		for(iv = 1;iv <= m_toolSurf->NbVPoles();iv ++)
		{
			pnt = m_tmpSurf->Pole(iu,iv);
			//
			aV = BRepBuilderAPI_MakeVertex(pnt);
			BB.Add(aC,aV);
		}
	}

	//
	Display(TmpObj(1),aC);

	//m_AISContext->OpenLocalContext();//yxk20191207
	m_AISContext->Activate(TmpObj(1),1);

	return TRUE;
}

BOOL	ScCmdSurfFFD::InitDefrom()
{
	TopoDS_Shape aShape = GetObjShape(SelObj(0));

	//��������ƽ��
	//�����Χ��
	double xmin,ymin,zmin,xmax,ymax,zmax;
	double zpos;
	Bnd_Box bb;
	BRepBndLib::Add(aShape,bb);
	bb.Get(xmin,ymin,zmin,xmax,ymax,zmax);
	zpos = zmin;
	bb.Enlarge(sqrt(bb.SquareExtent())/10.0);
	//���ݰ�Χ�д�������ƽ��
	
	bb.Get(xmin,ymin,zmin,xmax,ymax,zmax);
	gp_Pnt p0(xmin,ymin,zpos),p1(xmax,ymax,zpos);

	gp_Ax2 ax(gp_Pnt(0,0,zpos),gp_Dir(0,0,1));

	m_pFFDTool = new FFDSurfTool;
	if(!m_pFFDTool->InitRect(p0,p1,ax))
	{
		AfxMessageBox("��������ʧ��");
		return FALSE;
	}
	m_toolSurf = m_pFFDTool->m_datumSurf;//��ʼ�ǻ�׼��
	m_tmpSurf = Handle(Geom_BSplineSurface)::DownCast(m_toolSurf->Copy());//��ʱ��

	//�������ζ���
	m_pFFDObj = new FFDSurfObj;
	if(!m_pFFDObj->InitShape(aShape,32,32))
	{
		AfxMessageBox("����ffd objʧ��.");
		return FALSE;
	}

	//�����Ƶ�ͶӰ������������,��ʼ���㷨
	m_pFFDAlgo = new FFDSurfAlgo;
	if(!m_pFFDAlgo->Init(m_pFFDObj,m_pFFDTool))
	{
		AfxMessageBox("Init FFD algo failed.");
		return FALSE;
	}

	//��ʽ��������
	TopoDS_Face aTF = BRepBuilderAPI_MakeFace(m_tmpSurf, Precision::Confusion());//yxk20200104����, Precision::Confusion()
	Display(TmpObj(0),aTF);
	
	return TRUE;
}

BOOL	ScCmdSurfFFD::ChangeToolSurf(BOOL bTmp)
{
	int iu,iv;
	gp_Vec vchg(m_chgPnt.X() - m_basPnt.X(),m_chgPnt.Y() - m_basPnt.Y(),
		m_chgPnt.Z() - m_basPnt.Z());
	gp_Trsf tsf;
	tsf.SetTranslation(vchg);

	RemoveCtxObj(TmpObj(0));

	gp_Pnt pnt;
	for(iu = 1;iu <= m_toolSurf->NbUPoles();iu ++)
	{
		for(iv = 1;iv <= m_toolSurf->NbVPoles();iv ++)
		{
			pnt = m_toolSurf->Pole(iu,iv);
			//
			for(int idx = 1;idx <= m_seqOfPnt.Length();idx ++)
			{
				gp_Pnt tp = m_seqOfPnt.Value(idx);
				if(tp.Distance(pnt) < 1e-10)
				{
					pnt.Transform(tsf);
					m_tmpSurf->SetPole(iu,iv,pnt);
					if(!bTmp)
						m_toolSurf->SetPole(iu,iv,pnt);
				}
			}
		}
	}

	TopoDS_Face aF = BRepBuilderAPI_MakeFace(m_tmpSurf, Precision::Confusion());//yxk20200104����, Precision::Confusion()
	Display(TmpObj(0),aF);

	return TRUE;
}

//���ݼ�¼��Ϣ����������
BOOL	ScCmdSurfFFD::DoDeform(BOOL bDone)
{
	gp_Pnt pnt,newpt;
	gp_Vec d1u,d1v,N;
	gp_Pnt2d uv;
	//double dst;

		//m_AISContext->OpenLocalContext();//yxk20191207
	RemoveCtxObj(TmpObj(2));

	//����
	m_pFFDTool->SetTarSurf(m_toolSurf);
	if(!m_pFFDAlgo->Deform() || !m_pFFDAlgo->IsDone())
	{
		AfxMessageBox("Deform failed.");
		return FALSE;
	}

	TopoDS_Shape aNS = m_pFFDAlgo->Shape();


	m_bChanged = TRUE;

	if(bDone)
	{
		Display(NewObj(0),aNS);
		BeginUndo("Feng Surf FFD");
		UndoAddNewObj(NewObj(0));
		EndUndo(TRUE);
	}
	else
	{
		Display(TmpObj(2),aNS);
	}

	return TRUE;
}
