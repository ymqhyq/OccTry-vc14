#include "StdAfx.h"
#include "ScInput.h"
#include ".\sccmdoffset.h"
#include "ScOffsetShapeDlg.h"
#include "ScDlgSweepOption.h"

ScCmdLoft::ScCmdLoft(BOOL bSolid)
{
	this->m_bSolid = bSolid;
}

//�����ͽ�������
int		ScCmdLoft::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_CURVES,"ѡ�����߶����Ҽ���������.");
	m_aSeqOfShape.Nullify();
	m_aSeqOfShape = new TopTools_HSequenceOfShape();

	return 0;
}

int		ScCmdLoft::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdLoft::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		TopoDS_Shape aS = BuildShape();
		Handle(AIS_Shape) aObj;
		Display(aObj,aS);

		Done();
	}
	else
	{
		switch(GetState())
		{
		case S_CURVES:
			{//��Ҫ��֤�Ƿ�ͬһ�����ߡ�TODO:
				m_AISContext->InitCurrent();
				if(m_AISContext->MoreCurrent())
				{
					Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(
						m_AISContext->Current());
					if(!aObj.IsNull())
					{
						TopoDS_Shape aShape = aObj->Shape();
						if(aShape.ShapeType() == TopAbs_EDGE ||
							aShape.ShapeType() == TopAbs_WIRE)
						{
							m_aSeqOfShape->Append(aShape);
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

//����ƶ�����Ӧ����
void	ScCmdLoft::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	m_AISContext->MoveTo(point.x,point.y,aView, Standard_True);
}

TopoDS_Shape	ScCmdLoft::BuildShape()
{
	TopoDS_Shape aS;
	int nLen = m_aSeqOfShape->Length();
	if(nLen < 2)
		return aS;

	BRepOffsetAPI_ThruSections BRepLoft;
	if(this->m_bSolid)
		BRepLoft.Init(Standard_True);
	else
		BRepLoft.Init(Standard_False);

	for(int ix = 1;ix <= nLen;ix++)
	{
		TopoDS_Shape atS = m_aSeqOfShape->Value(ix);
		if(atS.ShapeType() == TopAbs_WIRE)
		{
			BRepLoft.AddWire(TopoDS::Wire(atS));
		}
		else if(atS.ShapeType() == TopAbs_EDGE)
		{
			TopoDS_Wire aW = BRepBuilderAPI_MakeWire(TopoDS::Edge(atS));
			if(!aW.IsNull())
				BRepLoft.AddWire(aW);
		}
	}

	BRepLoft.Build();

	aS = BRepLoft.Shape();

	return aS;
}

///////////////////////////////////////////////////////////////////////////////
//
//�����ͽ�������
int	ScCmdThickSolid::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);
	SwitchState(S_SHAPE,"ѡ��Ҫ��ǵĶ���:");
	return 0;
}

int	ScCmdThickSolid::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdThickSolid::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SHAPE:
		{
			m_AISContext->InitCurrent();
			if(m_AISContext->MoreCurrent())
			{
				Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(
					m_AISContext->Current());
				if(!aObj.IsNull())
				{
					m_aSelObj = aObj;
					
					SwitchState(S_FACE,"ѡ��Ҫȥ������:");
					//m_AISContext->OpenLocalContext();//yxk20191207
					m_AISContext->Activate(aObj,4);//��
				}
			}
		}
		break;
	case S_FACE:
		{
			m_AISContext->InitSelected();
			if(m_AISContext->MoreSelected())
			{
				m_aFace = TopoDS::Face(m_AISContext->SelectedShape());
				//m_AISContext->CloseLocalContex();//yxk20191207

				SwitchState(S_THICK,"�����Ǻ��:");
				NeedInput("��Ǻ��:",INPUT_DOUBLE,100);
			}
		}
		break;
	case S_THICK:
		break;
	default:
		break;
	}
}

//����ƶ�����Ӧ����
void	ScCmdThickSolid::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SHAPE:
	case S_FACE:
		m_AISContext->MoveTo(point.x,point.y,aView, Standard_True);
		break;
	default:
		break;
	}
}

//���ո�����
int		ScCmdThickSolid::AcceptInput(double dVal)
{
	switch(GetState())
	{
	case S_THICK:
		{
			if(fabs(dVal) < 0.001)
				return SC_INPUT_ERR;

			EndInput();
			this->m_dThick = dVal;
			//
			TopoDS_Shape aShape = BuildNewShape();
			if(!aShape.IsNull())
			{
				this->m_aSelObj->Set(aShape);
				m_AISContext->Redisplay(m_aSelObj, Standard_True);
			}

			Done();
		}
		break;
	}

	return 0;
}

TopoDS_Shape	ScCmdThickSolid::BuildNewShape()
{
	TopoDS_Shape aS;
	TopTools_ListOfShape aList;
	aList.Append(m_aFace);

	try{
		BRepOffsetAPI_MakeThickSolid mts(m_aSelObj->Shape(),aList,m_dThick,0.01);
		mts.Build();
		aS = mts.Shape();
	}
	catch(Standard_Failure)
	{
		AfxMessageBox("���ʧ��");
	}
	return aS;
}

///////////////////////////////////////////////////////////
//
ScCmdOffsetShape::ScCmdOffsetShape(BOOL bOnlySurf)
{
	m_bOnlySurf = bOnlySurf;
}

//�����ͽ�������
int	ScCmdOffsetShape::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);
	SwitchState(S_SHAPE,"ѡ��Ҫƫ�ƵĶ���:");
	NeedSelect(TRUE);
	return 0;
}

int	ScCmdOffsetShape::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdOffsetShape::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SHAPE:
		{
			InitSelect();
			if(MoreSelect())
			{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(aS.IsNull())
					return;

				BOOL bSel = TRUE;
				if(m_bOnlySurf)
				{
					if((aS.ShapeType() != TopAbs_FACE) &&
						(aS.ShapeType() != TopAbs_SHELL))
					{
						bSel = FALSE;
					}
				}
				if(bSel)
				{
					TmpObj(0) = SelectedObj();

					SwitchState(S_OFFSET,"����ƫ��ֵ:");
					NeedDouble("ƫ��ֵ:");
					NeedSelect(FALSE);
				}
			}
		}
		break;
	default:
		break;
	}
}

//���ո�����
int	 ScCmdOffsetShape::AcceptInput(double dVal)
{
	switch(GetState())
	{
	case S_OFFSET:
		{
			if(fabs(dVal) < 0.001)
				return SC_INPUT_ERR;

			this->m_dOffset = dVal;

			TopoDS_Shape aS = BuildNewShape();
			if(!aS.IsNull())
			{
				Display(NewObj(0),aS);
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
	return 0;
}

TopoDS_Shape	ScCmdOffsetShape::BuildNewShape()
{
	TopoDS_Shape aShape;
	
	ScOffsetShapeDlg dlg;
	dlg.DoModal();

	TopoDS_Shape aFS = GetObjShape(TmpObj(0));
	TmpObj(0) = NULL;

	BRepOffsetAPI_MakeOffsetShape moff(aFS,m_dOffset,
		0.001,(BRepOffset_Mode)dlg.m_nOffMode,Standard_False,Standard_False,
		(GeomAbs_JoinType)dlg.m_nJoinType);

	try{
		moff.Build();
		aShape = moff.Shape();
	}
	catch(Standard_Failure)
	{
		AfxMessageBox("ƫ��ʧ��.");
	}

	return aShape;
}

//////////////////////////////////////////////////////////////////
//

//�����ͽ�������
int	ScCmdBRepSweep::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	ScDlgSweepOption dlg;
	dlg.DoModal();

	m_nType = dlg.m_nSweepType;
	if(m_nType == Sweep_ConstantNormal)
	{
		m_biNorm = dlg.m_vBiNorm;
	}
	else if(m_nType == Sweep_Fixed)
	{
		m_ax2 = dlg.m_ax2;
	}

	SwitchState(S_SPINE,"ѡ��һ������:");
	NeedSelect(TRUE);

	return 0;
}

int	ScCmdBRepSweep::End()
{
	return ScCommand::End();
}


//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdBRepSweep::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		switch(GetState())
		{
		case S_PROFILES:
			{
				m_AISContext->InitCurrent();
				while(m_AISContext->MoreCurrent())
				{
					Handle(AIS_InteractiveObject) aObj = m_AISContext->Current();
					if(aObj == m_aSpineObj)
					{
						m_AISContext->NextCurrent();
						continue;//����ѡ�м���.
					}
					Handle(AIS_Shape) asObj = Handle(AIS_Shape)::DownCast(aObj);
					if(asObj.IsNull())
						continue;

					this->m_seqOfObj.Append(aObj);//��¼

					m_AISContext->NextCurrent();
				}

				if(m_seqOfObj.Length() > 0)
				{
					NeedMultiSelect(FALSE);
					//���Ƿ���Ҫ����
					if(m_nType == Sweep_GuideAC ||
						m_nType == Sweep_GuidePlan ||
						m_nType == Sweep_GuideACWithContact ||
						m_nType == Sweep_GuidePlanWithContact)
					{
						SwitchState(S_GUIDE,"ѡ��һ����������.");
						NeedSelect(TRUE);
						m_AISContext->ClearCurrents(Standard_True);
					}
					else
					{
						TopoDS_Shape aS = BuildNewShape();
						Display(m_aNewObj,aS);
						Done();
					}
				}
			}
			break;
		default:
			break;
		}
	}
	else
	{
		switch(GetState())
		{
		case S_SPINE:
			{
				m_AISContext->InitCurrent();
				if(m_AISContext->MoreCurrent())
				{
					Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(
						m_AISContext->Current());
					if(!aObj.IsNull())
					{
						BOOL bOk = FALSE;
						TopoDS_Shape aS = aObj->Shape();
						if(aS.ShapeType() == TopAbs_WIRE)
						{
							m_awSpine = TopoDS::Wire(aS);
							bOk = TRUE;
						}
						else if(aS.ShapeType() == TopAbs_EDGE)
						{
							m_awSpine = BRepBuilderAPI_MakeWire(TopoDS::Edge(aS));
							bOk = TRUE;
						}

						if(bOk)
						{
							m_aSpineObj = m_AISContext->Current();
							//ȥѡ��ǰ�����ѡ�˵�������
							//m_AISContext->ClearCurrents();
							//m_AISContext->HilightWithColor(m_aSpineObj,Quantity_NOC_BLUE1);//yxk20191208
							//
							SwitchState(S_PROFILES,"ѡ��һ�������������(��Ϊ�ߡ���)���Ҽ�����:");
							NeedMultiSelect(TRUE);
						}
					}
				}
			}
			break;
		case S_PROFILES:
			{
			}
			break;
		case S_GUIDE:
			{
				m_AISContext->InitCurrent();
				if(m_AISContext->MoreCurrent())
				{
					Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(
						m_AISContext->Current());
					if(!aObj.IsNull())
					{
						BOOL bOk = FALSE;
						TopoDS_Shape aS = aObj->Shape();
						if(aS.ShapeType() == TopAbs_WIRE)
						{
							m_awGuide = TopoDS::Wire(aS);
							bOk = TRUE;
						}
						else if(aS.ShapeType() == TopAbs_EDGE)
						{
							m_awGuide = BRepBuilderAPI_MakeWire(TopoDS::Edge(aS));
							bOk = TRUE;
						}

						if(bOk)
						{
							TopoDS_Shape aS = BuildNewShape();
							Display(m_aNewObj,aS);
							Done();
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

TopoDS_Shape	ScCmdBRepSweep::BuildNewShape()
{
	TopoDS_Shape aNewShape;

	BRepOffsetAPI_MakePipeShell mSweep(m_awSpine);

	//���profile,ֻ����vertex��wire��������ˡ�
	int nC = m_seqOfObj.Length();
	int nS = 0;
	for(int ic = 1;ic <= nC;ic++)
	{
		Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(
						m_seqOfObj.Value(ic));
		if(!aObj.IsNull())
		{
			TopoDS_Shape aS = aObj->Shape();
			if(aS.ShapeType() == TopAbs_VERTEX || 
				aS.ShapeType() == TopAbs_WIRE)
			{
				mSweep.Add(aS);
				nS ++;
			}
			else if(aS.ShapeType() == TopAbs_EDGE)
			{
				TopoDS_Wire aW = BRepBuilderAPI_MakeWire(TopoDS::Edge(aS));
				mSweep.Add(aW);
				nS ++;
			}
		}
	}

	if(nS == 0)
		return aNewShape;

	//������ز���
	switch(m_nType)
	{
	case Sweep_CorrectedFrenet:
		{
			mSweep.SetMode(Standard_False);
		}
		break;
	case Sweep_Frenet:
		{
			mSweep.SetMode(Standard_True);
		}
		break;
	case Sweep_Fixed:
		{
			mSweep.SetMode(m_ax2);
		}
		break;
	case Sweep_ConstantNormal:
		{
			mSweep.SetMode(gp_Dir(m_biNorm));
		}
		break;
	case Sweep_GuideAC:
		{
			//mSweep.SetMode(m_awGuide,Standard_True,Standard_False);
			mSweep.SetMode(m_awGuide,Standard_True/*,Standard_False*/);//yxk20191208
		}
		break;
	case Sweep_GuidePlan:
		{
			//mSweep.SetMode(m_awGuide,Standard_False,Standard_False);
			mSweep.SetMode(m_awGuide, Standard_True/*,Standard_False*/);//yxk20191208
	}
		break;
	case Sweep_GuideACWithContact:
		{
			//mSweep.SetMode(m_awGuide,Standard_True,Standard_True);
			mSweep.SetMode(m_awGuide, Standard_True/*,Standard_False*/);//yxk20191208
	}
		break;
	case Sweep_GuidePlanWithContact:
		{
			//mSweep.SetMode(m_awGuide,Standard_False,Standard_True);
			mSweep.SetMode(m_awGuide, Standard_True/*,Standard_False*/);//yxk20191208
	}
		break;
	default:
		break;
	}

	try{
		mSweep.Build();
		if(mSweep.IsDone())
			mSweep.MakeSolid();

		aNewShape = mSweep.Shape();
	}
	catch(Standard_Failure)
	{
		AfxMessageBox("ɨ��ʧ��");
		return aNewShape;
	}

	return aNewShape;
}
