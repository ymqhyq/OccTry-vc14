#include "StdAfx.h"

#include "ScBRepLib.h"
#include "FFDObj.h"
#include "FFDAlgo.h"
#include "FFDTool.h"
#include ".\sccmdffdline.h"

ScCmdFFDLine::ScCmdFFDLine(void)
{
	m_pFFDAlgo = NULL;
	m_pFFDObj  = NULL;
	m_pFFDTool = NULL;

	m_bChanged = FALSE;
}

ScCmdFFDLine::~ScCmdFFDLine(void)
{
}

//�����ͽ�������
int		ScCmdFFDLine::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelObj())
	{
		SwitchState(S_PNT1,"���ι���ֱ�ߵ�һ��:");
	}
	else
	{
		SwitchState(S_OBJ,"ѡ��һ��Ҫ���εĶ���:");
	}

	return 0;
}

int		ScCmdFFDLine::End()
{
	//m_AISContext->CloseLocalContex();//yxk20191207
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdFFDLine::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		//m_AISContext->CloseLocalContex();//yxk20191207

		RemoveCtxObj(TmpObj(0));
		RemoveCtxObj(TmpObj(1));
		if(m_bChanged)
		{
			if(DeformObj(FALSE))
				Done();
			else
				End();
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
					SwitchState(S_PNT1,"���ι���ֱ�ߵ�һ��:");
				}
			}
			break;
		case S_PNT1:
			{
				m_startPnt = PixelToCSPlane(point.x,point.y);
				SwitchState(S_PNT2,"���ι���ֱ�ߵڶ���:");
			}
			break;
		case S_PNT2:
			{
				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_startPnt) < 1e-6)
					return;

				RemoveCtxObj(TmpObj(0));
				if(!InitDeform(pnt))
				{
					AfxMessageBox("���γ�ʼ��ʧ��.");
					End();
					return;
				}
				
				SwitchState(S_SELPNT,"ʹ�ù��߽��б���:");
				ShowToolFrame();
			}
			break;
		case S_SELPNT:
			{
				InitSelect();
				if(MoreSelect())
				{
					m_selPnt = BRep_Tool::Pnt(TopoDS::Vertex(SelectedShape(FALSE)));
				}
				//m_AISContext->CloseLocalContex();//yxk20191207
				SwitchState(S_MOVEPNT,"ѡ����Ƶ����λ��:");
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
void	ScCmdFFDLine::MoveEvent(const CPoint& point,
		const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_PNT2:
		{
			gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
			if(pnt.Distance(m_startPnt) < 1e-6)
				return;

			ShowLine(pnt);
		}
		break;
	case S_MOVEPNT:
		{
			//
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
		break;
	default:
		break;
	}
}

BOOL	ScCmdFFDLine::HasSelObj()
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

void	ScCmdFFDLine::ShowLine(const gp_Pnt& pnt)
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_startPnt,pnt);
	if(aE.IsNull())
		return;

	Display(TmpObj(0),aE);
}

//��ʽ���ߵĿ��Ƶ㣬�Ա��ڱ༭��
void	ScCmdFFDLine::ShowToolFrame()
{
	RemoveCtxObj(TmpObj(1));

	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	for(int ix = 1;ix <= m_tmpTool->NbPoles();ix ++)
	{
		gp_Pnt pnt = m_tmpTool->Pole(ix);
		TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(pnt);
		BB.Add(aC,aV);
	}

	Display(TmpObj(1),aC);

	//m_AISContext->OpenLocalContext();//yxk20191207
	m_AISContext->Activate(TmpObj(1),1);//����ѡ��
}

BOOL	ScCmdFFDLine::ChangeToolCurve(const gp_Pnt& pnt,BOOL bTmp)
{
	RemoveCtxObj(TmpObj(0));

	for(int ix = 1;ix <= m_curTool->NbPoles();ix ++)
	{
		gp_Pnt pt = m_curTool->Pole(ix);
		if(pt.Distance(m_selPnt) < 1e-10)
		{
			m_tmpTool->SetPole(ix,pnt);
			if(!bTmp)
				m_curTool->SetPole(ix,pnt);
			break;
		}
	}

	TopoDS_Edge atE = BRepBuilderAPI_MakeEdge(m_tmpTool);
	Display(TmpObj(0),atE);

	return TRUE;
}

BOOL	ScCmdFFDLine::InitDeform(const gp_Pnt& pnt)
{
	TopoDS_Shape aS = GetObjShape(SelObj(0));

	//��������
	m_pFFDObj = new FFDSurfObj;
	if(!m_pFFDObj->InitShape(aS,40,40))
	{
		AfxMessageBox("Init shape obj failed");
		return FALSE;
	}

	//��������
	m_pFFDTool = new FFDLineTool;
	if(!m_pFFDTool->InitLine(m_startPnt,pnt))
	{
		AfxMessageBox("init line failed.");
		return FALSE;
	}

	//��ʼ���㷨
	m_pFFDAlgo = new FFDLineAlgo;
	if(!m_pFFDAlgo->Init(m_pFFDObj,m_pFFDTool))
	{
		AfxMessageBox("Init algo failed.");
		return FALSE;
	}

	m_curTool = Handle(Geom_BSplineCurve)::DownCast(m_pFFDTool->m_datumCurve->Copy());
	m_tmpTool = Handle(Geom_BSplineCurve)::DownCast(m_pFFDTool->m_datumCurve->Copy());

	//��ʾ��������
	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_curTool);
	Display(TmpObj(0),aE);

	return TRUE;
}

BOOL	ScCmdFFDLine::DeformObj(BOOL bTmp)
{
	RemoveCtxObj(TmpObj(2));

	m_pFFDTool->SetLineDfmCurve(m_tmpTool);
	if(!m_pFFDAlgo->Deform())
	{
		AfxMessageBox("����ʧ��.");
		return FALSE;
	}

	TopoDS_Shape aS = m_pFFDAlgo->Shape();
	if(aS.IsNull())
	{
		AfxMessageBox("����Ϊ��.");
		return FALSE;
	}

	m_bChanged = TRUE;
	if(bTmp)
	{
		Display(TmpObj(2),aS);
	}
	else
	{
		Display(NewObj(0),aS);
		BeginUndo("FFD line");
		UndoAddNewObj(NewObj(0));
		EndUndo(TRUE);
	}

	return TRUE;
}

