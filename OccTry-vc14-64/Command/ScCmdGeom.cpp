#include "StdAfx.h"
#include "ScView.h"
#include ".\sccmdgeom.h"

ScCmdGeom::ScCmdGeom(void)
{
}

ScCmdGeom::~ScCmdGeom(void)
{
}

//////////////////////////////////////////////
// ����ɨ�ӡ�
// ʹ����GeomFill_Pipe

//�����ͽ�������
int	ScCmdSweepOnePath::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	this->m_nType = -1;
	this->m_nC = 0;
	SwitchState(S_PATH,"ѡ��һ��ɨ��·��:");
	NeedSelect(TRUE);

	return 0;
}

int	ScCmdSweepOnePath::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdSweepOnePath::InputEvent(const CPoint& point,int nState,//����minput����
													const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(GetState() == S_CURVE)
		{
			TopoDS_Shape aTS = GetObjShape(TmpObj(0));
			int nCnt = 0;
			InitSelect();
			while(MoreSelect())
			{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull() && aS.ShapeType() == TopAbs_EDGE &&
					!aS.IsSame(aTS))
				{
					nCnt ++;
					TmpObj(nCnt) = SelectedObj();
				}
				else
				{
					NextSelect();
				}
			}

			this->m_nC = nCnt;
			NeedMultiSelect(FALSE);
			if(nCnt == 1)
			{
				SwitchState(S_TYPE,"����ɨ������.");
				NeedDouble("ɨ������: 0 CorrectedFrenet 1 Frenet 2 Constant");
			}
			else if(nCnt > 1)
			{
				TopoDS_Shape aS = BuildNewShape();
				if(aS.IsNull())
					End();
				else
				{
					Display(NewObj(0),aS);
					Done();
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
		case S_PATH:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape(FALSE);
					if(!aS.IsNull() && aS.ShapeType() == TopAbs_EDGE)
					{
						TmpObj(0) = SelectedObj();

						NeedSelect(FALSE);
						SwitchState(S_CURVE,"ѡ��һ����������ߣ��Ҽ�����.");
						NeedMultiSelect(TRUE);
					}
				}
			}
			break;
		case S_CURVE:
			break;
		case S_TYPE:
			break;
		default:
			break;
		}
	}
}

TopoDS_Shape	ScCmdSweepOnePath::BuildNewShape()
{
	TopoDS_Shape aNS;
	Handle(Geom_Curve) aPC;

	double dF,dL;
	TopoDS_Shape aS = GetObjShape(TmpObj(0));
	aPC = BRep_Tool::Curve(TopoDS::Edge(aS),dF,dL);
	if(aPC.IsNull())
		return aNS;

	if(this->m_nType >= 0)
	{
		GeomFill_Trihedron ft;
		if(m_nType == 0) ft = GeomFill_IsCorrectedFrenet;
		else if(m_nType == 1) ft = GeomFill_IsFrenet;
		else if(m_nType == 2) ft = GeomFill_IsFixed;
		else ft = GeomFill_IsCorrectedFrenet;

		Handle(Geom_Curve) aC = BRep_Tool::Curve(TopoDS::Edge(GetObjShape(TmpObj(1))),
			dF,dL);
		if(!aC.IsNull())
		{
			try
			{
				GeomFill_Pipe pip(aPC,aC,ft);
				pip.Perform();
				Handle(Geom_Surface) aSurf = pip.Surface();
				if(!aSurf.IsNull())
				{
					TopoDS_Face aF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());
					aNS = aF;
				}
			}
			catch(Standard_Failure)
			{
				return aNS;
			}
		}
	}
	else
	{
		if(m_nC == 2)
		{
			Handle(Geom_Curve) aC1 = BRep_Tool::Curve(TopoDS::Edge(GetObjShape(TmpObj(1))),
										dF,dL);
			Handle(Geom_Curve) aC2 = BRep_Tool::Curve(TopoDS::Edge(GetObjShape(TmpObj(2))),
										dF,dL);

			if(!aC1.IsNull() && !aC2.IsNull())
			{
				try
				{
					GeomFill_Pipe pip(aPC,aC1,aC2);
					pip.Perform();
					Handle(Geom_Surface) aSurf = pip.Surface();
					if(!aSurf.IsNull())
					{
						TopoDS_Face aF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());
						aNS = aF;
					}
				}
				catch(Standard_Failure)
				{
					return aNS;
				}
			}
		}
		else
		{
			int nC = 0;
//yxk20200104
			//GeomFill_SequenceOfCurve seqOfC;
			TColGeom_SequenceOfCurve seqOfC;
//endyxk
			for(int i = 1;i <= m_nC;i ++)
			{
				Handle(Geom_Curve) aC = BRep_Tool::Curve(TopoDS::Edge(GetObjShape(TmpObj(i))),
										dF,dL);
				if(!aC.IsNull())
				{
					seqOfC.Append(aC);
					nC ++;
				}
			}
			//
			if(nC > 0)
			{
				try
				{
					GeomFill_Pipe pip(aPC,seqOfC);
					pip.Perform();
					Handle(Geom_Surface) aSurf = pip.Surface();
					if(!aSurf.IsNull())
					{
//yxk20200104
						//TopoDS_Face aF = BRepBuilderAPI_MakeFace(aSurf);
						TopoDS_Face aF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());
//endyxk
						aNS = aF;
					}
				}
				catch(Standard_Failure)
				{
					return aNS;
				}
			}
		}
	}

	return aNS;
}

int		ScCmdSweepOnePath::AcceptInput(double dVal)
{
	if(GetState() == S_TYPE)
	{
		if(dVal != 0.0 && dVal != 1.0 && dVal != 2.0)
			return -1;

		m_nType = (int)dVal;
		TopoDS_Shape aS = BuildNewShape();
		if(aS.IsNull())
			End();
		else
		{
			Display(NewObj(0),aS);
			Done();
		}
	}

	return 0;
}

///////////////////////////////////////////////////
// ��ʾ���ߵ�����Բ
// ʹ��GeomLProp_CLProps��

//�����ͽ�������
int	ScCmdCurveCurvatureCirc::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_CURVE,"ѡ��һ��ֱ�ߣ���ʾ����Բ.");
	NeedSelect(TRUE);

	return 0;
}

int	ScCmdCurveCurvatureCirc::End()
{
	TmpObj(0) = NULL;
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void		ScCmdCurveCurvatureCirc::InputEvent(const CPoint& point,int nState,
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
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape(FALSE);
					if(!aS.IsNull() && 
						((aS.ShapeType() == TopAbs_EDGE) || 
						(aS.ShapeType() == TopAbs_WIRE)))
					{
						TmpObj(0) = SelectedObj();
						m_aShape = aS;
						
						SwitchState(S_CIRC,"��ʾ����Բ");
					}
				}
			}
			break;
		case S_CIRC:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape();
					if(!aS.IsNull() && 
						((aS.ShapeType() == TopAbs_EDGE) || 
						(aS.ShapeType() == TopAbs_WIRE)))
					{
						TmpObj(0) = SelectedObj();
						m_aShape = aS;
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
void		ScCmdCurveCurvatureCirc::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	if(GetState() == S_CIRC)
	{
		//��ʾ����Բ
		RemoveCtxObj(TmpObj(1));
		//
		TopoDS_Shape aS = MakeCurvatureCirc(point);
		Display(TmpObj(1),aS);
	}
}

TopoDS_Shape	ScCmdCurveCurvatureCirc::MakeCurvatureCirc(const CPoint& point)
{
	TopoDS_Shape aShape;
	//����
	TopoDS_Edge aEdge;
	double t;
	gp_Pnt pnt;
	
	aEdge = this->GetNearestEdge(TmpObj(0),point.x,point.y,t,pnt);
	if(aEdge.IsNull())
		return aShape;

	//��������Բ��
	double dF,dL;
	Handle(Geom_Curve) aCur = BRep_Tool::Curve(aEdge,dF,dL);
	if(aCur.IsNull())
		return aShape;

	try{
		GeomLProp_CLProps lprop(aCur,2,Precision::Confusion());
		lprop.SetParameter(t);
		if(lprop.IsTangentDefined())
		{
			gp_Dir tang,norm,zdir;
			lprop.Tangent(tang);
			lprop.Normal(norm);
			zdir = tang.Crossed(norm);
			//
			gp_Pnt pC;
			lprop.CentreOfCurvature(pC);
			double cv = lprop.Curvature();
			if(fabs(cv) > Precision::Confusion() && fabs(cv) < 10000)
			{
				double dr = 1.0/fabs(cv);
				gp_Circ circ(gp_Ax2(pC,zdir),dr);
				TopoDS_Edge aNE = BRepBuilderAPI_MakeEdge(circ);
				aShape = aNE;
			}
		}
	}catch(Standard_Failure)
	{
	}

	return aShape;
}

////////////////////////////////////////////////////////
// ��������߹���
// ʹ��Adaptor3d_IsoCurve����ع���
// ���ܻ���Ҫ��һ�����ƣ�Ŀǰ��ʾ�������ߣ����ǲü��������

//�����ͽ�������
int	ScCmdIsoCurve::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	SwitchState(S_SURF,"ѡ��һ����������");
	NeedSelect(TRUE);

	return 0;
}

int	ScCmdIsoCurve::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdIsoCurve::InputEvent(const CPoint& point,int nState,
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
		case S_SURF:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape(FALSE);
					if(!aS.IsNull())
					{
						TopExp_Explorer ex(aS,TopAbs_FACE);
						if(ex.More())
						{
							TmpObj(0) = SelectedObj();

							NeedSelect(FALSE);
							SwitchState(S_UV,"ѡ��U��V");
							NeedInteger("UVģʽ:<0 U���� 1 V���� 2 UV����>");
						}
					}
				}
			}
			break;
		case S_UV:
			{
			}
			break;
		case S_CURVE:
			{
				MakeIsoCurve(TRUE,point,aView);
				Done();
			}
			break;
		default:
			break;
		}
	}
}
	
//����ƶ�����Ӧ����
void	ScCmdIsoCurve::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	if(GetState() == S_CURVE)
	{
		MakeIsoCurve(FALSE,point,aView);
	}
}

//��������
int	ScCmdIsoCurve::AcceptInput(int nVal)
{
	if(GetState() == S_UV)
	{
		if(nVal < 0 || nVal > 2)
			return -1;

		this->m_nUV = nVal;

		SwitchState(S_CURVE,"���������������.");
	}
	return 0;
}

void	ScCmdIsoCurve::MakeIsoCurve(BOOL bDone,const CPoint& point,const Handle(V3d_View)& aView)
{
	RemoveCtxObj(TmpObj(1));
	RemoveCtxObj(TmpObj(2));

	//��������
	TopoDS_Face aFace;
	aFace = this->GetObjPickedFace(TmpObj(0),point.x,point.y,aView);
	if(aFace.IsNull())
		return;

	double u,v;
	gp_Pnt ppnt;
	this->m_pScView->GetFacePickPoint(aFace,point.x,point.y,u,v,ppnt);

	//��������
	BRepAdaptor_Surface baSurf(aFace);
	Handle(BRepAdaptor_HSurface) bhSurf = new BRepAdaptor_HSurface(baSurf);

	TopoDS_Edge aE1,aE2;
	Adaptor3d_IsoCurve uC,vC;
	if(m_nUV == 0 || m_nUV == 2)
	{
		uC.Load(bhSurf);
		uC.Load(GeomAbs_IsoU,u);
		aE1 = GetIsoCurve(uC);
	}
	if(m_nUV == 1 || m_nUV == 2)
	{
		vC.Load(bhSurf);
		vC.Load(GeomAbs_IsoV,v);
		aE2 = GetIsoCurve(vC);
	}
	if(bDone)
	{
		Display(NewObj(1),aE1);
		Display(NewObj(2),aE2);
	}
	else
	{
		Display(TmpObj(1),aE1);
		Display(TmpObj(2),aE2);
	}
}

TopoDS_Edge	 ScCmdIsoCurve::GetIsoCurve(const Adaptor3d_IsoCurve& isoC)
{
	TopoDS_Edge aE;

	switch(isoC.GetType())
	{
	case  GeomAbs_Line:
		{
			aE = BRepBuilderAPI_MakeEdge(isoC.Line());
		}
		break;
	case GeomAbs_Circle:
		{
			aE = BRepBuilderAPI_MakeEdge(isoC.Circle());
		}
		break;
	case GeomAbs_Ellipse:
		{
			aE = BRepBuilderAPI_MakeEdge(isoC.Ellipse());
		}
		break;
	case GeomAbs_Hyperbola:
		{
			aE = BRepBuilderAPI_MakeEdge(isoC.Hyperbola());
		}
		break;
	case GeomAbs_Parabola:
		{
			aE = BRepBuilderAPI_MakeEdge(isoC.Parabola());
		}
		break;
	case GeomAbs_BezierCurve:
		{
			aE = BRepBuilderAPI_MakeEdge(isoC.Bezier());
		}
		break;
	case GeomAbs_BSplineCurve:
		{
			aE = BRepBuilderAPI_MakeEdge(isoC.BSpline());
		}
		break;
	default:
		break;
	}

	return aE;
}

///////////////////////////////////////////////////////
// bspline curve�Ŀ��Ƶ�༭����
// ʹ�û�����bspline�Ĺ���
// ���Ƕ�trimmed��curve�Ĵ������ת����������curve������༭��

//�����ͽ�������
int	ScCmdBSplCurveEdit::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	this->m_bChanged = FALSE;
	SwitchState(S_CURVE,"ѡ��һ����������:");
	NeedSelect(TRUE);

	return 0;
}

int	ScCmdBSplCurveEdit::End()
{
	//m_AISContext->CloseLocalContex();//yxk20191207
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdBSplCurveEdit::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(m_bChanged)
		{
			//m_AISContext->CloseLocalContex();//yxk20191207
			Done();
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
		case S_CURVE:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Shape aS = SelectedShape(FALSE);
					if(!aS.IsNull())
					{
						if(GetBSplineCurve(aS))
						{
							DelObj(0) = SelectedObj(FALSE);//��¼

							SwitchState(S_POINT,this->m_szPrompt1);

							ShowControlPoints();
						}
					}
				}
			}
			break;
		case S_POINT:
			{
				if(SelectPoint(point))
				{
					NeedSelect(FALSE);
					SwitchState(S_MOVING,"�ı���λ��:");
				}
			}
			break;
		case S_MOVING:
			{
				OnPointChanged(point);
				//���½���ѡ��ģʽ
				NeedSelect(TRUE);
				SwitchState(S_POINT,this->m_szPrompt1);
			}
			break;
		default:
			break;
		}
	}
}
	
//����ƶ�����Ӧ����
void	ScCmdBSplCurveEdit::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_MOVING:
		{
			OnChangePoint(point);
		}
		break;
	default:
		break;
	}
}


BOOL	ScCmdBSplCurveEdit::GetBSplineCurve(const TopoDS_Shape& aS)
{
	if(aS.ShapeType() != TopAbs_EDGE)
		return FALSE;

	//
	double dF,dL;
	Handle(Geom_Curve) aCur = BRep_Tool::Curve(TopoDS::Edge(aS),dF,dL);
	if(aCur.IsNull())
		return FALSE;

	BOOL bRet = FALSE;
	if(aCur->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
	{
		m_bsplCur = Handle(Geom_BSplineCurve)::DownCast(aCur->Copy());//��¼���ݣ���ֹ�ı�ԭ�������ߡ�
		bRet = TRUE;
	}
	else if(aCur->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
	{
		Handle(Geom_TrimmedCurve) aTC = Handle(Geom_TrimmedCurve)::DownCast(aCur);
		Handle(Geom_Curve) aBC = aTC->BasisCurve();
		if(aBC->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
		{
			m_bsplCur = Handle(Geom_BSplineCurve)::DownCast(aCur->Copy());//��¼���ݣ���ֹ�ı�ԭ�������ߡ�
			m_bsplCur->Segment(dF,dL);//�жϣ�ȡһ����
			bRet = TRUE;
		}
	}

	return bRet;
}

void	ScCmdBSplCurveEdit::ShowControlPoints()
{
	TopoDS_Compound comp;
	BRep_Builder BB;
	BB.MakeCompound(comp);

	//��ȡpoles������vertex
	int nC = 0;
	for(int ix = 1;ix <= m_bsplCur->NbPoles();ix ++)
	{
		gp_Pnt pp = m_bsplCur->Pole(ix);
		TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(pp);
		if(!aV.IsNull())
		{
			BB.Add(comp,aV);
			nC ++;
		}
	}
	if(nC > 0)
	{
		Display(TmpObj(0),comp);
	}
}

//ѡ���
BOOL	ScCmdBSplCurveEdit::SelectPoint(const CPoint& point)
{
	return FALSE;
}

//�ı��
BOOL	ScCmdBSplCurveEdit::OnChangePoint(const CPoint& point)
{
	return FALSE;
}

BOOL	ScCmdBSplCurveEdit::OnPointChanged(const CPoint& point)
{
	return FALSE;
}
/////////////////////////////////////////////
//
void	ScCmdBSplCurveEditByCtrlPnt::ShowControlPoints()
{
	ScCmdBSplCurveEdit::ShowControlPoints();
	//�������ѡ����Ƶ㡣
	//m_AISContext->OpenLocalContext();//yxk20191207
	m_AISContext->Activate(TmpObj(0),1);
}

//ѡ���
BOOL	ScCmdBSplCurveEditByCtrlPnt::SelectPoint(const CPoint& point)
{
	m_nPoleIdx = 0;
	
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		gp_Pnt pnt = BRep_Tool::Pnt(TopoDS::Vertex(aS));

		TopoDS_Shape aShape = GetObjShape(TmpObj(0));
		TopExp_Explorer ex(aShape,TopAbs_VERTEX);
		int nC = 0;
		for(;ex.More();ex.Next())
		{
			TopoDS_Vertex aV = TopoDS::Vertex(ex.Current());
			gp_Pnt p = BRep_Tool::Pnt(aV);
			if(p.Distance(pnt) < Precision::Confusion())
			{
				m_nPoleIdx = nC + 1;
				break;
			}
			nC ++;
		}
	}

	if(m_nPoleIdx > 0)
	{
		//m_AISContext->CloseLocalContex();//yxk20191207
		return TRUE;
	}

	return FALSE;
}

//�ı��
BOOL	ScCmdBSplCurveEditByCtrlPnt::OnChangePoint(const CPoint& point)
{
	RemoveCtxObj(TmpObj(0));
	RemoveCtxObj(TmpObj(1));
	//�µ�
	//gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
	gp_Pnt pnt = this->PixelToModel(point.x,point.y);//����3d�ռ��޸�
	//
	this->m_bsplCur->SetPole(m_nPoleIdx,pnt);
	//
	ScCmdBSplCurveEdit::ShowControlPoints();
	//��ʾ�任�������
	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_bsplCur);
	Display(TmpObj(1),aE);

	return TRUE;
}

BOOL	ScCmdBSplCurveEditByCtrlPnt::OnPointChanged(const CPoint& point)
{
	RemoveCtxObj(DelObj(0));
	RemoveCtxObj(TmpObj(0));
	RemoveCtxObj(TmpObj(1));
	//�µ�
	//gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
	gp_Pnt pnt = this->PixelToModel(point.x,point.y);//����3d�ռ��޸�
	//
	this->m_bsplCur->SetPole(m_nPoleIdx,pnt);
	//
	//��ʾ�任�������
	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_bsplCur);
	Display(DelObj(0),aE);

	this->m_bChanged = TRUE;

	ShowControlPoints();

	return TRUE;
}

/////////////////////////////////////////

//ʲô��������
void	ScCmdBSplCurveEditByCurvPnt::ShowControlPoints()
{
	ScCmdBSplCurveEdit::ShowControlPoints();
}

//ѡ���
BOOL	ScCmdBSplCurveEditByCurvPnt::SelectPoint(const CPoint& point)
{
	double t;
	gp_Pnt nearpt;
	TopoDS_Shape aS = GetObjShape(DelObj(0));
	TopoDS_Edge aE = TopoDS::Edge(aS);
	if(this->m_pScView->GetEdgePickPoint(aE,point.x,point.y,5,
		t,nearpt))
	{
		this->m_t = t;
		this->m_pickPnt = nearpt;

		return TRUE;
	}

	return FALSE;
}

//�ı��
BOOL	ScCmdBSplCurveEditByCurvPnt::OnChangePoint(const CPoint& point)
{
	gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
	if(pnt.Distance(m_pickPnt) < Precision::Confusion())
		return FALSE;

	RemoveCtxObj(TmpObj(0));
	RemoveCtxObj(TmpObj(1));

	//�ƶ���
	int i1,i2;
	this->m_bsplCur->LocateU(m_t,Precision::Confusion(),i1,i2,TRUE);
	
	PoleRange(i1,i2);
	
	int fmi,lmi;
	this->m_bsplCur->MovePoint(m_t,pnt,i1,i2,fmi,lmi);

	//��ʾ�任�������
	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_bsplCur);
	Display(TmpObj(1),aE);

	//��ʾ���Ƶ�
	ShowControlPoints();

	return TRUE;
}

//
BOOL	ScCmdBSplCurveEditByCurvPnt::OnPointChanged(const CPoint& point)
{
	gp_Pnt pnt = this->PixelToCSPlane(point.x,point.y);
	if(pnt.Distance(m_pickPnt) < Precision::Confusion())
		return FALSE;

	RemoveCtxObj(DelObj(0));
	RemoveCtxObj(TmpObj(0));
	RemoveCtxObj(TmpObj(1));

	//�ƶ���
	int i1,i2;
	this->m_bsplCur->LocateU(m_t,Precision::Confusion(),i1,i2,TRUE);
	//
	
	int fmi,lmi;
	this->m_bsplCur->MovePoint(m_t,pnt,i1,i2,fmi,lmi);

	//��ʾ�任�������
	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_bsplCur);
	Display(DelObj(0),aE);

	return TRUE;
}

void	ScCmdBSplCurveEditByCurvPnt::PoleRange(int& i1,int& i2)
{
	//����i1��i2��knots�ķ�Χ����ҪתΪpole���Ƶ�ķ�Χ��
	//����nknots = npoles + degree + 1,ÿ��������Nk������[Uk,Uk+p+1],
	//���磺3degree��N0,����[U0,U4]���֣���u����[Uk,Uk+1],
	//����Ӱ���NkΪ��Nk-p��Nk,�Ӷ���Ӱ��Ŀ��Ƶ�[k-p,k]
	int nPoles = m_bsplCur->NbPoles();
	int nD = m_bsplCur->Degree();
	i2 = i1;
	i1 = i1 - nD/2;
	if(i1 < 1)
	{
		i1 = 1;
	}
	
	if(i2 > nPoles)
	{
		i2 = nPoles;
	}
	
}