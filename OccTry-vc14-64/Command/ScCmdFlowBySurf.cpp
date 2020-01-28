#include "StdAfx.h"

#include <BRepTools.hxx>

#include "ScBReplib.h"
#include "ScView.h"

#include "FFDObj.h"
#include "FFDTool.h"
#include "FFDAlgo.h"

#include ".\sccmdflowbysurf.h"

ScCmdFlowBySurf::ScCmdFlowBySurf(void)
{
}

ScCmdFlowBySurf::~ScCmdFlowBySurf(void)
{
}

//�����ͽ�������
int	ScCmdFlowBySurf::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelObj())
	{
		SwitchState(S_DATUM_SURF,"ѡ���׼����,��ѡ�����߽紦:");
	}
	else
	{
		SwitchState(S_OBJ,"ѡ��Ҫ���ε�����:");
	}

	return 0;
}

int	ScCmdFlowBySurf::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdFlowBySurf::InputEvent(const CPoint& point,int nState,//����minput����
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
					SwitchState(S_DATUM_SURF,"ѡ���׼����,��ѡ�����߽紦:");
				}
			}
			break;
		case S_DATUM_SURF:
			{
				if(HasSelDatumSurf(point.x,point.y))
				{
					SwitchState(S_TAR_SURF,"ѡ��Ŀ������,��ѡ�����߽紦:");
				}
			}
			break;
		case S_TAR_SURF:
			{
				if(HasSelTarSurf(point.x,point.y))
				{
					if(DoFlow())
						Done();
					else
						End();
				}
			}
			break;
		default:
			break;
		}
	}
}


BOOL	ScCmdFlowBySurf::HasSelObj()
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

BOOL	ScCmdFlowBySurf::HasSelDatumSurf(int nx,int ny)
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aShape = SelectedShape(FALSE);
		if(ScBRepLib::IsFace(aShape))
		{
			//����������Ϣ
			double u,v;
			gp_Pnt pickPnt;
			TopoDS_Face aFace = TopoDS::Face(aShape);
			if(m_pScView->GetFacePickPoint(aFace,nx,ny,u,v,pickPnt))
			{
				SelObj(1) = SelectedObj();
				m_datumUV.SetCoord(u,v);

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL	ScCmdFlowBySurf::HasSelTarSurf(int nx,int ny)
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aShape = SelectedShape(FALSE);
		if(ScBRepLib::IsFace(aShape))
		{
			//����������Ϣ
			double u,v;
			gp_Pnt pickPnt;
			TopoDS_Face aFace = TopoDS::Face(aShape);
			if(m_pScView->GetFacePickPoint(aFace,nx,ny,u,v,pickPnt))
			{
				SelObj(2) = SelectedObj();
				m_datumUV.SetCoord(u,v);

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL	ScCmdFlowBySurf::DoFlow()
{
	TopoDS_Shape aObj = GetObjShape(SelObj(0));

	FFDSurfObj surfObj;
	if(!surfObj.InitShape(aObj,10,10))
	{
		AfxMessageBox("Init obj failed.");
		return FALSE;
	}

	TopoDS_Face aDF = TopoDS::Face(GetObjShape(SelObj(1)));
	TopoDS_Face aTF = TopoDS::Face(GetObjShape(SelObj(2)));
	Handle(Geom_Surface) aDSurf = BRep_Tool::Surface(aDF);
	Handle(Geom_Surface) aTSurf = BRep_Tool::Surface(aTF);

	if(aDSurf.IsNull() || aTSurf.IsNull())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	double u1,u2,v1,v2;
	FFDSurfFlowTool flowTool;

	BRepTools::UVBounds(aDF,u1,u2,v1,v2);
	flowTool.SetDatumSurf(aDSurf,u1,v1,u2,v2);
	BRepTools::UVBounds(aTF,u1,u2,v1,v2);
	flowTool.SetTarSurf(aTSurf,u1,v1,u2,v2);

	if(!flowTool.IsValid())
	{
		AfxMessageBox("Invalid tool.");
		return FALSE;
	}

	FFDSurfFlowAlgo algo;
	if(!algo.Init(&surfObj,&flowTool))
	{
		AfxMessageBox("Algo init failed.");
		return FALSE;
	}

	if(algo.Deform() && algo.IsDone())
	{
		TopoDS_Shape aNS = algo.Shape();
		Display(NewObj(0),aNS);

		BeginUndo("Flow on surface");
		UndoAddNewObj(NewObj(0));
		EndUndo(TRUE);

		return TRUE;
	}
	
	return FALSE;
}