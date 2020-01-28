#include "StdAfx.h"
#include <algorithm>
#include "..\ScView.h"
#include "ScBRepLib.h"
#include ".\sccmdcurveedit.h"

ScCmdCurveEdit::ScCmdCurveEdit(void)
{
		//m_bSelPnt = FALSE;
}

ScCmdCurveEdit::~ScCmdCurveEdit(void)
{
}

//////////////////////////////////////////////////////////////////////////
// ���߱���ָ�

//�����ͽ�������
 int		ScCmdCurveSplitByPoint::Begin(Handle(AIS_InteractiveContext) aCtx)
 {
		 ScCmdCurveBase::Begin(aCtx);

		 if(HasSelEdgeOrWire())
		 {
				 SwitchState(S_POINT,"ѡ��������һ��:");
		 }
		 else
		 {
				 NeedSelect(TRUE);
				 SwitchState(S_CURVE,"ѡ��Ҫ�ָ������:");
		 }

		 return 0;
 }

 int		ScCmdCurveSplitByPoint::End()
 {
		 return ScCmdCurveBase::End();
 }

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void		ScCmdCurveSplitByPoint::InputEvent(const CPoint& point,int nState,
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
				case S_CURVE:
						{
								if(HasSelEdgeOrWire())
								{
										SwitchState(S_POINT,"ѡ��������һ��:");
								}
						}
						break;
				case S_POINT:
						{
								RemoveCtxObj(TmpObj(0));

								if(m_bSelPnt)
								{
										if(DoSplit())
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

//����ƶ�����Ӧ����
void			ScCmdCurveSplitByPoint::MoveEvent(const CPoint& point,
								  const Handle(V3d_View)& aView)
{
		switch(GetState())
		{
		case S_POINT:
				{
						double dist,t;
						gp_Pnt  ptClk;
						TopoDS_Shape aS = GetObjShape(SelObj(0));
						if(ScBRepLib::IsEdge(aS))
						{
								if(m_pScView->GetEdgeNearestPoint(TopoDS::Edge(aS),point.x,point.y,t,
										ptClk,dist))
								{
										RemoveCtxObj(TmpObj(0));

										TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(ptClk);
										Display(TmpObj(0),aV);

										m_t = t;
										m_ptClk = ptClk;
										m_bSelPnt = TRUE;
								}
						}
						else if(ScBRepLib::IsWire(aS))
						{

						}
				}
				break;
		default:
				break;
		}
}


BOOL			ScCmdCurveSplitByPoint::DoSplit()
{
		TopoDS_Shape aS = GetObjShape(SelObj(0));
		if(ScBRepLib::IsEdge(aS))
		{
				//���������µ�edge
				TopoDS_Edge aNE1,aNE2;
				if(!ScBRepLib::SplitEdge(TopoDS::Edge(aS),m_t,aNE1,aNE2))
						return FALSE;
				
				//ɾ���ɵģ���ʾ�µ�
				m_AISContext->Remove(SelObj(0),Standard_False);
				Display(NewObj(0),aNE1,FALSE);
				Display(NewObj(1),aNE2);
				//
				BeginUndo("split curve by point");
				UndoAddDelObj(SelObj(0));
				UndoAddNewObj(NewObj(0));
				UndoAddNewObj(NewObj(1));
				EndUndo(TRUE);

				return TRUE;
		}
		else
		{

		}

		return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// ���߱����߷ָ�

//�����ͽ�������
 int			ScCmdCurveSplitByCurve::Begin(Handle(AIS_InteractiveContext) aCtx)
 {
		 ScCmdCurveBase::Begin(aCtx);

		 NeedSelect(TRUE);
		 if(HasSelEdgeOrWire())
		 {
				 SwitchState(S_TOOL,"ѡ��ü���������:");
		 }
		 else
		 {
				 SwitchState(S_CURVE,"ѡ��һ�����ָ�����:");
		 }

		return 0;
 }

 int			ScCmdCurveSplitByCurve::End()
 {
		 return ScCmdCurveBase::End();
 }

//��ѡ��ʽ�µ���Ϣ��Ӧ����
 void		ScCmdCurveSplitByCurve::InputEvent(const CPoint& point,int nState,
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
				case S_CURVE:
						{
								if(HasSelEdgeOrWire())
								{
										SwitchState(S_TOOL,"ѡ��ü���������:");
								}
						}
						break;
				case S_TOOL:
						{
								if(HasSelEdge(1))
								{
										if(DoSplit())
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

 //���зָ
 //�����wire������edge���κ͹����󽻣����зָ
 //����ת��Ϊedge��edge���ཻ��
BOOL				ScCmdCurveSplitByCurve::DoSplit()
{
		TopoDS_Shape   aS1 = GetObjShape(SelObj(0));
		TopoDS_Shape   aS2 = GetObjShape(SelObj(1));
		TopTools_ListOfShape  shapeList;

		TopoDS_Edge aE2 = TopoDS::Edge(aS2);
		if(ScBRepLib::IsEdge(aS1))
		{
				ScBRepLib::SplitEdgeByEdge(TopoDS::Edge(aS1),aE2,shapeList);
		}
		else
		{
				ScBRepLib::SplitWireByEdge(TopoDS::Wire(aS1),aE2,shapeList);
		}

		if(shapeList.Extent() == 0)
				return FALSE;

		BeginUndo("curve split");
		//ɾ���ɵģ���ʾ�µ�
		m_AISContext->Remove(SelObj(0),Standard_False);
		UndoAddDelObj(SelObj(0));

		int iC = 0;
		TopTools_ListIteratorOfListOfShape ite(shapeList);
		for(;ite.More();ite.Next())
		{
				Display(NewObj(iC),ite.Value(),FALSE);
				if(!NewObj(iC).IsNull())
						UndoAddNewObj(NewObj(iC));
				iC ++;
		}
		m_AISContext->UpdateCurrentViewer();

		EndUndo(TRUE);

		return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// ���߲ü�������ָ�����

//�����ͽ�������
int		ScCmdCurveTrimSplit::Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		NeedSelect(TRUE);
		if(HasSelEdgeOrWire(0))
		{
				SwitchState(S_CURVE2,"ѡ��ڶ�������:");
		}
		else
		{
				SwitchState(S_CURVE1,"ѡ���һ������:");
		}

		return 0;
}

int		ScCmdCurveTrimSplit::	End()
{
		return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void		ScCmdCurveTrimSplit::	InputEvent(const CPoint& point,int nState,
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
				case S_CURVE1:
						{
								if(HasSelEdgeOrWire(0))
								{
										SwitchState(S_CURVE2,"ѡ��ڶ�������:");
								}
						}
						break;
				case S_CURVE2:
						{
								if(HasSelEdgeOrWire(1))
								{
										if(DoSplit())
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


BOOL		ScCmdCurveTrimSplit::	DoSplit()
{
		TopoDS_Shape aS1 = GetObjShape(SelObj(0));
		TopoDS_Shape aS2 = GetObjShape(SelObj(1));
		TopTools_ListOfShape  shapeList1,shapeList2;
		int  nC1 = 0,nC2 = 0;

		if(ScBRepLib::IsEdge(aS1))
		{
				if(ScBRepLib::IsEdge(aS2))
				{
						nC1 = ScBRepLib::SplitEdgeByEdge(TopoDS::Edge(aS1),TopoDS::Edge(aS2),shapeList1);
						nC2 = ScBRepLib::SplitEdgeByEdge(TopoDS::Edge(aS2),TopoDS::Edge(aS1),shapeList2);
				}
				else
				{
						nC1 = ScBRepLib::SplitEdgeByWire(TopoDS::Edge(aS1),TopoDS::Wire(aS2),shapeList1);
						nC2 = ScBRepLib::SplitWireByEdge(TopoDS::Wire(aS2),TopoDS::Edge(aS1),shapeList2);
				}
		}
		else
		{
				if(ScBRepLib::IsEdge(aS2))
				{
						nC1 = ScBRepLib::SplitWireByEdge(TopoDS::Wire(aS1),TopoDS::Edge(aS2),shapeList1);
						nC2 = ScBRepLib::SplitEdgeByWire(TopoDS::Edge(aS2),TopoDS::Wire(aS1),shapeList2);
				}
				else
				{
						nC1 = ScBRepLib::SplitWireByWire(TopoDS::Wire(aS1),TopoDS::Wire(aS2),shapeList1);
						nC2 = ScBRepLib::SplitWireByWire(TopoDS::Wire(aS2),TopoDS::Wire(aS1),shapeList2);
				}
		}

		if(nC1 == 0 && nC2 == 0)
				return FALSE;//û�зָ�

		int  iC = 0;
		BeginUndo("Curve Trim split");
		if(nC1 > 0)
		{
				m_AISContext->Remove(SelObj(0),FALSE);
				UndoAddDelObj(SelObj(0));
				//��ʾ�µ�
				TopTools_ListIteratorOfListOfShape ite(shapeList1);
				for(;ite.More();ite.Next())
				{
						Display(NewObj(iC),ite.Value(),FALSE);
						if(!NewObj(iC).IsNull())
								UndoAddNewObj(NewObj(iC));
						iC ++;
				}
				m_AISContext->UpdateCurrentViewer();
		}
		if(nC2 > 0)
		{
				m_AISContext->Remove(SelObj(1),FALSE);
				UndoAddDelObj(SelObj(1));
				//��ʾ�µ�
				TopTools_ListIteratorOfListOfShape ite(shapeList2);
				for(;ite.More();ite.Next())
				{
						Display(NewObj(iC),ite.Value(),FALSE);
						if(!NewObj(iC).IsNull())
								UndoAddNewObj(NewObj(iC));
						iC ++;
				}
				m_AISContext->UpdateCurrentViewer();
		}
		EndUndo(TRUE);

		return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// ���߲ü������ٲü�

//�����ͽ�������
int		ScCmdCurveQuickTrim::Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		NeedSelect(TRUE);
		if(HasSelEdgeOrWire(0))
		{
				SwitchState(S_CURVE2,"ѡ��ڶ�������:");
		}
		else
		{
				SwitchState(S_CURVE1,"ѡ���һ������:");
		}

		return 0;
}

int		ScCmdCurveQuickTrim::End()
{
		return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void		ScCmdCurveQuickTrim::InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView)
{
		if(nState == MINPUT_RIGHT)
		{
				if(DoTrimOver())
						Done();
				else
						End();
		}
		else
		{
				switch(GetState())
				{
				case S_CURVE1:
						{
								if(HasSelEdgeOrWire(0))
								{
										SwitchState(S_CURVE2,"ѡ��ڶ�������:");
								}
						}
						break;
				case S_CURVE2:
						{
								if(HasSelEdgeOrWire(1))
								{
										if(DoSplit())
										{
												SwitchState(S_TRIM,"ѡ��Ҫȥ�������߶�:");
										}
										else
												End();
								}
						}
						break;
				case S_TRIM:
						{
								DoTrim(point.x,point.y);
						}
						break;
				default:
						break;
				}
		}
}


BOOL				ScCmdCurveQuickTrim::DoSplit()
{
		TopoDS_Shape aS1 = GetObjShape(SelObj(0));
		TopoDS_Shape aS2 = GetObjShape(SelObj(1));
		int  nC1 = 0,nC2 = 0;

		if(ScBRepLib::IsEdge(aS1))
		{
				if(ScBRepLib::IsEdge(aS2))
				{
						nC1 = ScBRepLib::SplitEdgeByEdge(TopoDS::Edge(aS1),TopoDS::Edge(aS2),m_shapeList1);
						nC2 = ScBRepLib::SplitEdgeByEdge(TopoDS::Edge(aS2),TopoDS::Edge(aS1),m_shapeList2);
				}
				else
				{
						nC1 = ScBRepLib::SplitEdgeByWire(TopoDS::Edge(aS1),TopoDS::Wire(aS2),m_shapeList1);
						nC2 = ScBRepLib::SplitWireByEdge(TopoDS::Wire(aS2),TopoDS::Edge(aS1),m_shapeList2);
				}
		}
		else
		{
				if(ScBRepLib::IsEdge(aS2))
				{
						nC1 = ScBRepLib::SplitWireByEdge(TopoDS::Wire(aS1),TopoDS::Edge(aS2),m_shapeList1);
						nC2 = ScBRepLib::SplitEdgeByWire(TopoDS::Edge(aS2),TopoDS::Wire(aS1),m_shapeList2);
				}
				else
				{
						nC1 = ScBRepLib::SplitWireByWire(TopoDS::Wire(aS1),TopoDS::Wire(aS2),m_shapeList1);
						nC2 = ScBRepLib::SplitWireByWire(TopoDS::Wire(aS2),TopoDS::Wire(aS1),m_shapeList2);
				}
		}

		if(nC1 == 0 && nC2 == 0)
				return FALSE;//û�зָ�

		int  iC = 0;
		if(nC1 > 0)
		{
				m_AISContext->Remove(SelObj(0),FALSE);
				//��ʾ�µ�
				TopTools_ListIteratorOfListOfShape ite(m_shapeList1);
				for(;ite.More();ite.Next())
				{
						Display(TmpObj(iC),ite.Value(),FALSE);
						m_shapeMap.Bind(ite.Value(),iC);//��¼����˳��
						iC ++;
				}
				m_AISContext->UpdateCurrentViewer();
		}
		if(nC2 > 0)
		{
				m_AISContext->Remove(SelObj(1),FALSE);
				//��ʾ�µ�
				TopTools_ListIteratorOfListOfShape ite(m_shapeList2);
				for(;ite.More();ite.Next())
				{
						Display(TmpObj(iC),ite.Value(),FALSE);
						m_shapeMap.Bind(ite.Value(),iC);//��¼����˳��
						iC ++;
				}
				m_AISContext->UpdateCurrentViewer();
		}

		m_nNewC = iC;
		
		return TRUE;
}

//���ݵ�����ҵ���Ӧ�Ķ��󣬲������ҵ�Ҫɾ���Ķ��󣬼�¼ɾ��index.
BOOL				ScCmdCurveQuickTrim::DoTrim(int nx,int ny)
{
		//���shape1���¶���
		int idx = 0;
		double t;
		gp_Pnt pnt;
		BOOL  bTrm = FALSE;
		TopTools_ListIteratorOfListOfShape ite(m_shapeList1);
		for(;ite.More();ite.Next())
		{
				TopoDS_Shape aS = ite.Value();
				//�Ѿ�ɾ�������ߣ�������
				idx = m_shapeMap.Find(aS);
				if(idx < 0 || idx >= m_nNewC)
						continue;
				if(HasDel(m_arIdx1,idx))
						continue;

				TopExp_Explorer ex(aS,TopAbs_EDGE);
				for(;ex.More();ex.Next())
				{
						TopoDS_Edge  aE = TopoDS::Edge(ex.Current());
						if(m_pScView->GetEdgePickPoint(aE,nx,ny,5,t,pnt))
						{
								//ɾ��aS��Ӧ��obj
								m_AISContext->Remove(TmpObj(idx), Standard_True);//ɾ������
								m_arIdx1.push_back(idx);
								bTrm = TRUE;
								break;
						}
				}

				if(bTrm)
						break;
		}

		if(bTrm)
				return TRUE;

		ite.Initialize(m_shapeList2);
		for(;ite.More();ite.Next())
		{
				TopoDS_Shape aS = ite.Value();
				//�Ѿ�ɾ�������ߣ�������
				idx = m_shapeMap.Find(aS);
				if(idx < 0 || idx >= m_nNewC)
						continue;
				if(HasDel(m_arIdx2,idx))
						continue;

				TopExp_Explorer ex(aS,TopAbs_EDGE);
				for(;ex.More();ex.Next())
				{
						//�Ѿ�ɾ����edge��������
						TopoDS_Edge  aE = TopoDS::Edge(ex.Current());
						if(m_pScView->GetEdgePickPoint(aE,nx,ny,5,t,pnt))
						{
								//ɾ��aS��Ӧ��obj
								m_AISContext->Remove(TmpObj(idx), Standard_True);//ɾ������
								m_arIdx2.push_back(idx);
								bTrm = TRUE;
								break;
						}
				}

				if(bTrm)
						break;
		}

		return bTrm;
}

BOOL		ScCmdCurveQuickTrim::HasDel(std::vector<int>&  arx,int ix)//�Ƿ�ix�Ѿ�ɾ��
{
		for(int idx = 0;idx < arx.size();idx ++)
		{
				if(arx.at(idx) == ix)
						return TRUE;
		}
		return FALSE;
}

BOOL		ScCmdCurveQuickTrim::DoTrimOver()
{
		if(m_arIdx1.size() == 0 && m_arIdx2.size() == 0)
		{
				ClearTmpObjs();
				//�ָ��ɵ���ʾ
				TopoDS_Shape aS1 = GetObjShape(SelObj(0));
				TopoDS_Shape aS2 = GetObjShape(SelObj(1));
				Display(NewObj(0),aS1,FALSE);
				Display(NewObj(1),aS2);

				return FALSE;
		}

		TopTools_ListOfShape  nsList,tmList;
		int  idx = 0;

		std::sort(m_arIdx1.begin(),m_arIdx1.end());//��С��������
		TopTools_ListIteratorOfListOfShape ite(m_shapeList1);
		for(;ite.More();ite.Next())
		{
				idx = m_shapeMap.Find(ite.Value());
				if(HasDel(m_arIdx1,idx))
				{
							if(tmList.Extent() == 1)
							{
									TopTools_ListIteratorOfListOfShape ils(tmList);
									for(;ils.More();ils.Next())
									{
											nsList.Append(ils.Value());
									}
							}
							else if(tmList.Extent() > 1)
							{
									//tmList���컷
									BRepBuilderAPI_MakeWire  mw;
									TopTools_ListIteratorOfListOfShape ils(tmList);
									for(;ils.More();ils.Next())
									{
											TopExp_Explorer ex;
											for(ex.Init(ils.Value(),TopAbs_EDGE);ex.More();ex.Next())
											{
													mw.Add(TopoDS::Edge(ex.Current()));
											}
									}
									if(mw.IsDone())
									{
												nsList.Append(mw.Wire());	
									}
							}
							tmList.Clear();
				}
				else
				{
						tmList.Append(ite.Value());
				}
		}
		if(tmList.Extent() == 1)
		{
				TopTools_ListIteratorOfListOfShape ils(tmList);
				for(;ils.More();ils.Next())
				{
						nsList.Append(ils.Value());
				}
		}
		else if(tmList.Extent() > 1)
		{
				//tmList���컷
				BRepBuilderAPI_MakeWire  mw;
				TopTools_ListIteratorOfListOfShape ils(tmList);
				for(;ils.More();ils.Next())
				{
						TopExp_Explorer ex;
						for(ex.Init(ils.Value(),TopAbs_EDGE);ex.More();ex.Next())
						{
								mw.Add(TopoDS::Edge(ex.Current()));
						}
				}
				if(mw.IsDone())
				{
						nsList.Append(mw.Wire());	
				}
		}
		tmList.Clear();

		ite.Initialize(m_shapeList2);
		for(;ite.More();ite.Next())
		{
				idx = m_shapeMap.Find(ite.Value());
				if(HasDel(m_arIdx2,idx))
				{
						if(tmList.Extent() == 1)
						{
								TopTools_ListIteratorOfListOfShape ils(tmList);
								for(;ils.More();ils.Next())
								{
										nsList.Append(ils.Value());
								}
						}
						else if(tmList.Extent() > 1)//ƴ����wire
						{
								//tmList���컷
								BRepBuilderAPI_MakeWire  mw;
								TopTools_ListIteratorOfListOfShape ils(tmList);
								for(;ils.More();ils.Next())
								{
										TopExp_Explorer ex;
										for(ex.Init(ils.Value(),TopAbs_EDGE);ex.More();ex.Next())
										{
												mw.Add(TopoDS::Edge(ex.Current()));
										}
								}
								if(mw.IsDone())
								{
										nsList.Append(mw.Wire());	
								}
						}
						tmList.Clear();
				}
				else
				{
						tmList.Append(ite.Value());
				}
		}
		if(tmList.Extent() == 1)
		{
				TopTools_ListIteratorOfListOfShape ils(tmList);
				for(;ils.More();ils.Next())
				{
						nsList.Append(ils.Value());
				}
		}
		else if(tmList.Extent() > 1)//ƴ����wire
		{
				//tmList���컷
				BRepBuilderAPI_MakeWire  mw;
				TopTools_ListIteratorOfListOfShape ils(tmList);
				for(;ils.More();ils.Next())
				{
						TopExp_Explorer ex;
						for(ex.Init(ils.Value(),TopAbs_EDGE);ex.More();ex.Next())
						{
								mw.Add(TopoDS::Edge(ex.Current()));
						}
				}
				if(mw.IsDone())
				{
						nsList.Append(mw.Wire());	
				}
		}

		//��ʾ�µĶ���ɾ���ɵĶ���
		BeginUndo("Curve quick trim.");
		UndoAddDelObj(SelObj(0));
		UndoAddDelObj(SelObj(1));
		//��ʾ�¶���
		idx = 0;
		TopTools_ListIteratorOfListOfShape itels(nsList);
		for(;itels.More();itels.Next())
		{
				Display(NewObj(idx),itels.Value(),FALSE);
				UndoAddNewObj(NewObj(idx));
		}
		m_AISContext->UpdateCurrentViewer();

		EndUndo(TRUE);

		return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// 

//�����ͽ�������
int	ScCmdCurveFillet::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);

	if(HasClosedWire())
	{
		SwitchState(S_VERTEX,"ѡ��Ҫ��Բ�ǵĵ�,���ӵ������߱�����ֱ��:");
		//m_AISContext->OpenLocalContext();//yxk20191207
		m_AISContext->Activate(SelObj(0),1);//vertex
	}
	else
	{
		SwitchState(S_WIRE,"ѡ��պϵ��������:");
	}

	return 0;
}

int	ScCmdCurveFillet::End()
{
	RemoveCtxObj(TmpObj(0));
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdCurveFillet::InputEvent(const CPoint& point,int nState,
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
		case S_WIRE:
			{
				if(HasClosedWire())
				{
					SwitchState(S_VERTEX,"ѡ��Ҫ��Բ�ǵĵ�,���ӵ������߱�����ֱ��:");
					//m_AISContext->OpenLocalContext();//yxk20191207
					m_AISContext->Activate(SelObj(0),1);//vertex
				}
			}
			break;
		case S_VERTEX:
			{
				InitSelect();
				if(MoreSelect())
				{
					m_aVertex = TopoDS::Vertex(SelectedShape(FALSE));
					//m_AISContext->CloseLocalContex();//yxk20191207
					SwitchState(S_RADIUS,"���뵹�ǰ뾶:");
					NeedDouble("���ǰ뾶:");
				}
			}
			break;
		default:
			break;
		}
	}
}

//���ո�����
int	ScCmdCurveFillet::AcceptInput(double dVal)
{
	if(dVal <= 0.0)
		return -1;

	if(GetState() == S_RADIUS)
	{
		m_dRadius = dVal;
		if(MakeFillet())
			Done();
		else
			End();
	}

	return 0;
}


BOOL		ScCmdCurveFillet::HasClosedWire()
{
	if(HasSelWire(0))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL		ScCmdCurveFillet::MakeFillet()
{
	TopoDS_Wire aW = TopoDS::Wire(GetObjShape(SelObj(0)));

	BRepBuilderAPI_MakeFace mf(aW);
	mf.Build();
	if(!mf.IsDone())
		return FALSE;
	
	BRepFilletAPI_MakeFillet2d mfl(mf.Face());
	mfl.AddFillet(m_aVertex,m_dRadius);
	mfl.Build();
	if(mfl.IsDone())
	{
		BRepBuilderAPI_MakeWire MW;
		TopExp_Explorer exp (mfl.Shape(), TopAbs_EDGE);
		for (; exp.More(); exp.Next())
			MW.Add(TopoDS::Edge(exp.Current()));
		MW.Build();
		if (!MW.IsDone())
			return FALSE;

		TopoDS_Wire aNW = MW.Wire();
		m_AISContext->Remove(SelObj(0),FALSE);
		Display(NewObj(0),aNW);

		BeginUndo("Fillet 2d");
		UndoAddDelObj(SelObj(0));
		UndoAddNewObj(NewObj(0));
		EndUndo(TRUE);

		return TRUE;
	}

	return FALSE;
}

