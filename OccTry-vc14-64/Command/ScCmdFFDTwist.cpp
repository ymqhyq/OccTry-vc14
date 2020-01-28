#include "StdAfx.h"

#include "ScBRepLib.h"

#include "FFDObj.h"
#include "FFDTool.h"
#include "FFDAlgo.h"

#include ".\sccmdffdtwist.h"

ScCmdFFDTwist::ScCmdFFDTwist(void)
{
	m_rotrad = 0.0;
}

ScCmdFFDTwist::~ScCmdFFDTwist(void)
{
}

//�����ͽ�������
int	  ScCmdFFDTwist::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	if(HasSelObj())
	{
		NeedSnap(TRUE);
		SwitchState(S_AX_BEGIN,"��ת����ʼ��:");
	}
	else
	{
		NeedSelect(TRUE);
		SwitchState(S_OBJ,"ѡ��һ������:");
	}

	return 0;
}

int	  ScCmdFFDTwist::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdFFDTwist::InputEvent(const CPoint& point,int nState,//����minput����
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
		case S_OBJ:
			{
				if(HasSelObj())
				{
					NeedSnap(TRUE);
					SwitchState(S_AX_BEGIN,"��ת����ʼ��:");
				}
			}
			break;
		case S_AX_BEGIN:
			{
				m_pntBegin = PixelToCSPlane(point.x,point.y);
				SwitchState(S_AX_END,"��ת���յ�:");
			}
			break;
		case S_AX_END:
			{
				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_pntBegin) < 1e-6)
					return;

				m_pntEnd = pnt;
				SwitchState(S_BASEDIR,"��ת��ʼλ��;");
			}
			break;
		case S_BASEDIR:
			{
				if(!CalcRotatePlnPoint(point.x,point.y,TRUE))
					return;

				m_prePnt = m_basePnt;
				SwitchState(S_ANGLE,"��ת�Ƕ�");
			}
			break;
		case S_ANGLE:
			{
				if(DoTwist(FALSE))
				{
					Done();
				}
				else
				{
					End();
				}
			}
			break;
		default:
			break;
		}
	}
}

//����ƶ�����Ӧ����
void	ScCmdFFDTwist::MoveEvent(const CPoint& point,
								  const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
		case S_AX_END:
			{
				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_pntBegin) < 1e-6)
					return;

				ShowAxis(pnt);
			}
			break;
		case S_BASEDIR:
			{
				if(!CalcRotatePlnPoint(point.x,point.y,TRUE))
					return;

				ShowBaseVec(m_basePnt);
			}
			break;
		case S_ANGLE:
			{
				if(!CalcRotatePlnPoint(point.x,point.y,FALSE))
					return;

				//������ת�Ƕ�
				gp_Vec nv(m_pntEnd,m_newPnt),nv2(m_pntEnd,m_prePnt);
				double dAng = nv2.AngleWithRef(nv,gp_Vec(m_pntBegin,m_pntEnd));//�������Ҫ
				m_rotrad += dAng;

				ShowBaseVec(m_newPnt);
				DoTwist(TRUE);

				m_prePnt = m_newPnt;
			}
			break;
		default:
			break;
	}
}

//���ݿռ�������תƽ���ϵĵ�.
BOOL	ScCmdFFDTwist::CalcRotatePlnPoint(int nx,int ny,BOOL bBase)
{
	gp_Pnt pnt = PixelToModel(nx,ny);
	if(pnt.Distance(m_pntEnd) < 1e-6)
		return FALSE;

	//��ת����vec��Ҫ����ת�ᴹֱ,������תƽ����
	gp_Vec basVec(m_pntEnd,pnt),axVec(m_pntBegin,m_pntEnd);
	if(basVec.IsParallel(axVec,0.001))//����ƽ��,�޷�����
		return FALSE;

	double dVal = basVec.Dot(axVec);//��֤�Ƿ�ֱ
	if(fabs(dVal) > 1e-10)
	{
		gp_Vec nVec = basVec.Crossed(axVec);
		basVec = axVec.Crossed(nVec);
	}
	
	//ʵ��baseVec���ܱȽϳ�,��Ҫ�������pnt�ȽϽ��ĵ�
	gp_Pnt npnt;
	Handle(Geom_Line) aLin = new Geom_Line(m_pntEnd,gp_Dir(basVec));
	GeomAPI_ProjectPointOnCurve prj(pnt,aLin);
	if(prj.NbPoints() > 0)
	{
		npnt = prj.NearestPoint();
	}
	else
	{
		npnt.SetCoord(m_pntEnd.X() + basVec.X(),m_pntEnd.Y() + basVec.Y(),m_pntEnd.Z() + basVec.Z());
	}


	if(bBase)
	{
		m_basePnt = npnt;
		m_baseVec = basVec;
	}
	else
	{
		m_newPnt = npnt;
	}

	return TRUE;
}


BOOL	ScCmdFFDTwist::HasSelObj()
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

void	ScCmdFFDTwist::ShowAxis(const gp_Pnt& pnt)
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_pntBegin,pnt);
	Display(TmpObj(0),aE);
}

void	ScCmdFFDTwist::ShowBaseVec(const gp_Pnt& pnt)
{
	RemoveCtxObj(TmpObj(1));

	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_pntEnd,pnt);
	Display(TmpObj(1),aE);
}

BOOL	ScCmdFFDTwist::DoTwist(BOOL bTmp)
{
	TopoDS_Shape aS = GetObjShape(SelObj(0));

	RemoveCtxObj(TmpObj(2));

	FFDSurfObj obj;
	if(!obj.InitShape(aS,40,40))
	{
		AfxMessageBox("Init obj failed.");
		return FALSE;
	}

	FFDTwistTool tool;
	if(!tool.Init(m_pntBegin,m_pntEnd,m_baseVec))
	{
		return FALSE;
	}

	tool.Rotate(m_rotrad,FALSE);

	FFDTwistAlgo algo;
	if(!algo.Init(&obj,&tool))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if(algo.Deform() && algo.IsDone())
	{
		TopoDS_Shape aNS = algo.Shape();

		if(bTmp)
		{
			Display(TmpObj(2),aNS);
		}
		else
		{
			Display(NewObj(0),aNS);
			BeginUndo("twist");
			UndoAddNewObj(NewObj(0));
			EndUndo(TRUE);
		}

		return TRUE;
	}

	return FALSE;
}