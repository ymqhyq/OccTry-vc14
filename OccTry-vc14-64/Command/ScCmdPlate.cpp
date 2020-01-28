#include "StdAfx.h"

#include <BRepAlgo.hxx>
#include <TopExp.hxx>
#include <BRepLib.hxx>
#include <GeomProjLib.hxx>

#include ".\sccmdplate.h"
#include "ScCmdMgr.h"

ScCmdPlate::ScCmdPlate(void)
{
}

ScCmdPlate::~ScCmdPlate(void)
{
}

int	ScCmdPlate::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);
	return 0;
}

int	ScCmdPlate::End()
{
	return ScCommand::End();
}


////////////////////////////////////////////////////////////////
//
//�����ͽ�������
int	ScCmdPlatePointCloud::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCmdPlate::Begin(aCtx);

	Prompt("ѡ�����㣬�Ҽ�����");
	NeedMultiSelect(TRUE);

	return 0;
}

int	ScCmdPlatePointCloud::End()
{
	return ScCmdPlate::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdPlatePointCloud::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		BOOL bDone = FALSE;
		TColgp_SequenceOfXYZ seqOfXYZ;
		InitSelect();
		while(MoreSelect())
		{
			TopoDS_Shape aS = SelectedShape();
			if(aS.IsNull() || aS.ShapeType() != TopAbs_VERTEX)
				continue;

			gp_Pnt pnt = BRep_Tool::Pnt(TopoDS::Vertex(aS));
			seqOfXYZ.Append(pnt.XYZ());
		}

		if(seqOfXYZ.Length() > 0)
		{
			TopoDS_Shape aS = BuildSurface(seqOfXYZ);
			if(!aS.IsNull())
			{
				bDone = TRUE;
				Display(NewObj(0),aS);
			}
		}

		if(bDone) Done();
		else	  End();
	}
}

//
// ���ﴦ��Ƚϼ򵥣�û�м����ʼƽ�棬Ҳ�����ֱ��ʹ��API������ƽ�档
//
TopoDS_Shape	ScCmdPlatePointCloud::BuildSurface(const TColgp_SequenceOfXYZ& seqOfXYZ)
{
	TopoDS_Shape aShape;
	int nbPnt = seqOfXYZ.Length();
	int i;

	Plate_Plate plate;
	for(i = 1;i <= nbPnt;i ++)
	{
		gp_Pnt ptProj(seqOfXYZ.Value(i).X(), seqOfXYZ.Value(i).Y(), 0. );
		gp_Vec aVec( ptProj, seqOfXYZ.Value(i));
		gp_XY  pntXY(seqOfXYZ.Value(i).X(),seqOfXYZ.Value(i).Y());
		Plate_PinpointConstraint PCst( pntXY,aVec.XYZ()  );
		plate.Load(PCst);// Load plate
	}

	plate.SolveTI(2,1);
	if(!plate.IsDone())
		return aShape;

	// Computation of plate surface
	gp_Pnt Or(0,0,0.);
	gp_Dir Norm(0., 0., 1.);
	Handle(Geom_Plane) myPlane = 
		new Geom_Plane(Or, Norm);// Plane of normal Oz
	Handle(GeomPlate_Surface) myPlateSurf = 
		new GeomPlate_Surface( myPlane, plate);//plate surface

	GeomPlate_MakeApprox aMKS(myPlateSurf, Precision::Approximation(), 4, 7, 0.001, 0);//bspline surface
	Handle(Geom_Surface) aSurf = aMKS.Surface();
	if(aSurf.IsNull())
		return aShape;

	TopoDS_Face aF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());//yxk20200104
	aShape = aF;

	return aShape;
}

////////////////////////////////////////////////////////////////
//
ScCmdPlateBlend::ScCmdPlateBlend(BOOL bWithCont)
{
	this->m_bWithCont = bWithCont;
}

//�����ͽ�������
int	ScCmdPlateBlend::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCmdPlate::Begin(aCtx);

	SwitchState(S_SHAPE1,"ѡ���һ������:");
	return 0;
}

int	ScCmdPlateBlend::End()
{
	return ScCmdPlate::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdPlateBlend::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SHAPE1:
		{
			m_AISContext->InitCurrent();
			if(m_AISContext->MoreCurrent())
			{
				Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(
					m_AISContext->Current());
				TopoDS_Shape aShape = aObj->Shape();
				if(aShape.ShapeType() == TopAbs_FACE)
				{
					m_aFace1 = TopoDS::Face(aShape);
					//
					SwitchState(S_EDGE1,"ѡ�������ϵ�һ����:");
					//m_AISContext->OpenLocalContext();//yxk20191207
					m_AISContext->Activate(aObj,2);
				}
			}
		}
		break;
	case S_EDGE1:
		{
			m_AISContext->InitSelected();
			if(m_AISContext->MoreSelected())
			{
				m_aEdge1 = TopoDS::Edge(m_AISContext->SelectedShape());
				//m_AISContext->CloseLocalContex();//yxk20191207

				SwitchState(S_SHAPE2,"ѡ��ڶ�������:");
			}
		}
		break;
	case S_SHAPE2:
		{
			//���ԵĴ����ظ�
			m_AISContext->InitCurrent();
			if(m_AISContext->MoreCurrent())
			{
				Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(
					m_AISContext->Current());
				TopoDS_Shape aShape = aObj->Shape();
				if(aShape.ShapeType() == TopAbs_FACE)
				{
					m_aFace2 = TopoDS::Face(aShape);
					//
					SwitchState(S_EDGE2,"ѡ�������ϵ�һ����:");
					//m_AISContext->OpenLocalContext();//yxk20191207
					m_AISContext->Activate(aObj,2);
				}
			}
		}
		break;
	case S_EDGE2:
		{
			m_AISContext->InitSelected();
			if(m_AISContext->MoreSelected())
			{
				m_aEdge2 = TopoDS::Edge(m_AISContext->SelectedShape());
				//m_AISContext->CloseLocalContex();//yxk20191207

				//��������
				if(!this->m_bWithCont)
				{
					TopoDS_Face aF = BuildBlendFace();
					Handle(AIS_Shape) aObj;
					Display(aObj,aF);

					Done();
				}
				else
				{
					SwitchState(S_CONT,"ѡ��һ��Լ������:");
				}
			}
		}
		break;
	case S_CONT:
		{
			m_AISContext->InitCurrent();
			if(m_AISContext->MoreCurrent())
			{
				Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(
					m_AISContext->Current());
				TopoDS_Shape aShape = aObj->Shape();
				if(aShape.ShapeType() == TopAbs_EDGE)
				{
					m_aContEdge = TopoDS::Edge(aShape);
		
					TopoDS_Face aF = BuildBlendFace();
					Handle(AIS_Shape) aObj;
					Display(aObj,aF);

					Done();
				}
			}
		}
		break;
	default:
		break;
	}
}

//����ƶ�����Ӧ����
void	ScCmdPlateBlend::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	m_AISContext->MoveTo(point.x,point.y,aView, Standard_True);
}

TopoDS_Face		ScCmdPlateBlend::BuildBlendFace()
{
	TopoDS_Face aFace;

	BRepAdaptor_Curve aCur3d1(m_aEdge1),aCur3d2(m_aEdge2);
	BRepAdaptor_Curve2d aCur2d1(m_aEdge1,m_aFace1),aCur2d2(m_aEdge2,m_aFace2);
	BRepAdaptor_Surface aSurf1(m_aFace1),aSurf2(m_aFace2);

	//�����ʼ������
	int ix,nbPntsOnFace = 10;//���������
	double u,Tol = 0.001;
	Handle(TColgp_HArray1OfPnt) theTanPoints = new
		TColgp_HArray1OfPnt (1,2*nbPntsOnFace );

	double Delta = (aCur3d1.LastParameter() - aCur3d1.FirstParameter())/(nbPntsOnFace - 1);
	for (u = aCur3d1.FirstParameter(),ix = 1;ix <= nbPntsOnFace; ix++,u += Delta)
		theTanPoints->SetValue(ix,aCur3d1.Value(u));

	Delta = (aCur3d2.LastParameter() - aCur3d2.FirstParameter())/(nbPntsOnFace - 1);
	for (u = aCur3d2.FirstParameter(),ix = 1;ix <= nbPntsOnFace; ix++,u += Delta)
		theTanPoints->SetValue(nbPntsOnFace + ix,aCur3d2.Value(u));

	GeomPlate_BuildAveragePlane aMkPlane (theTanPoints,int(Tol),1,1,1);
	Handle(Geom_Plane) aPlane = aMkPlane.Plane();

	gp_Pln aPln = aPlane->Pln();
	gp_XYZ aNormale = aPln.Axis().Direction().XYZ();
	gp_Trsf aTrsf; // to compute the U and V of the points
	aTrsf.SetTransformation(aPln.Position());//ת����ƽ������ϵ�еı任

	Plate_Plate aPlate;
	gp_Vec V1,V2,W1,W2;
	gp_Pnt2d P2d;
	gp_Pnt P, PP;

	aPlane->D1(0,0,P,W1,W2);//��ȡƽ���du��dv

	//��һ�������tangenciesԼ��
	Delta = (aCur3d1.LastParameter() - aCur3d1.FirstParameter())/(nbPntsOnFace - 1);
	for (u = aCur3d1.FirstParameter(),ix=1; ix <= nbPntsOnFace; ix++,u += Delta)
	{
		P = aCur3d1.Value(u).Transformed(aTrsf);//ת����ƽ������ϵ��
		gp_XY UV(P.X(),P.Y());
		aPlate.Load(Plate_PinpointConstraint(UV,aNormale*P.Z()));//uvΪ����,������
		aCur2d1.D0(u,P2d);
		aSurf1.D1(P2d.X(),P2d.Y(),P,V1,V2);  // extract surface UV of the point
		aPlate.Load(Plate_GtoCConstraint(UV,
			Plate_D1(W1.XYZ(),W2.XYZ()),//��ʼ�����
			Plate_D1(V1.XYZ(),V2.XYZ())));//���ߵ�
	}
	// 2nd surface 
	Delta = (aCur3d2.LastParameter() - aCur3d2.FirstParameter())/(nbPntsOnFace - 1);
	for (u = aCur3d2.FirstParameter(),ix = 1; ix <= nbPntsOnFace; ix++,u += Delta) 
	{
		P = aCur3d2.Value(u).Transformed(aTrsf);
		gp_XY UV(P.X(),P.Y());
		aPlate.Load(Plate_PinpointConstraint(UV,aNormale*P.Z()));//��ͶӰ��ƽ���ϡ�����ƽ������ϵ�ĵ㡣

		aCur2d2.D0(u,P2d);
		aSurf2.D1(P2d.X(),P2d.Y(),P,V1,V2); 
		aPlate.Load(Plate_GtoCConstraint(UV,
			Plate_D1(W1.XYZ(),W2.XYZ()),
			//Plate_D1(V1.XYZ()*-1,V2.XYZ()*-1)));//Ϊ�η���?
		    Plate_D1(V1.XYZ(),V2.XYZ())));//Ϊ�η���?
	}

	if(this->m_bWithCont)
	{//���߲��������Լ��
		double t1,t2,t;
		Handle(Geom_Curve) aCurve = BRep_Tool::Curve(m_aContEdge,t1,t2);
		double st = (t2 - t1)/10;

		t = 0;
		for(int ix = 0;ix <= 10;ix ++)
		{
			gp_Pnt pnt = aCurve->Value(t);
			pnt.Transform(aTrsf);
			aPlate.Load(Plate_PinpointConstraint(gp_XY(pnt.X(),pnt.Y()),
				aNormale * pnt.Z()));

			t += st;
		}
	}

	//��������
	Standard_Integer Order = 3; // constraints continuity + 2
	aPlate.SolveTI(Order,1.);
	if (!aPlate.IsDone()){
		MessageBox(0,"Error : Build plate not valid!","CasCade Error",MB_ICONERROR);
		return aFace;
	}

	//�������
	//Plate Surface creation 
	Handle(GeomPlate_Surface) aPlateSurface = new GeomPlate_Surface(aPlane,aPlate);
	//BSplineSurface approximation 
	GeomPlate_MakeApprox aMkSurf(aPlateSurface,Tol,1,8,0.1,1);
	Handle(Geom_Surface) theSurface =aMkSurf.Surface();

	BOOL bWithWire = TRUE;
	if(bWithWire)
	{
		//����Face
		double First,Last;
		Handle(Geom2d_Curve)C1,C2,C3,C4;//�ĸ��ü���.
		Handle(Geom_Curve)C;
		//����ԭ����edge����ͶӰ����surface�ϣ���ȡ�ü����ߡ�
		C = BRep_Tool::Curve(m_aEdge1,First,Last);
		double TolProj = 0.01;
		C1 = GeomProjLib::Curve2d(C,First,Last,theSurface,TolProj);//ͶӰ��������
		if(C1.IsNull())
		{
			MessageBox(0,"Error : Build edge failed!","CasCade Error",MB_ICONERROR);
			return aFace;
		}
		TopoDS_Edge Ed1 = BRepBuilderAPI_MakeEdge(C1,theSurface).Edge();

		C = BRep_Tool::Curve(m_aEdge2,First,Last);
		TolProj = 0.01;
		C3 = GeomProjLib::Curve2d(C,First,Last,theSurface,TolProj);
		if(C3.IsNull())
		{
			MessageBox(0,"Error : Build edge failed!","CasCade Error",MB_ICONERROR);
			return aFace;
		}
		TopoDS_Edge Ed3 = BRepBuilderAPI_MakeEdge(C3,theSurface).Edge();

		//������ʹ�ò���ֱ�ߣ������ɲü�edge��
		C2 = GCE2d_MakeSegment(C1->Value(C1->FirstParameter()),
			C3->Value(C3->FirstParameter())).Value();//�����ռ��ֱ��,��C1��C3 //yxk20200104����.Value()
		TopoDS_Edge Ed2 = BRepBuilderAPI_MakeEdge(C2,theSurface).Edge();
		C4 = GCE2d_MakeSegment(C1->Value(C1->LastParameter()),
			C3->Value(C3->LastParameter())).Value();//��C1��C3. //yxk20200104����.Value()
		TopoDS_Edge Ed4 = BRepBuilderAPI_MakeEdge(C4,theSurface).Edge();
		Ed2.Reverse();
		Ed3.Reverse();//Ҫ��֤���������ȷ��.
		TopoDS_Wire theWire = BRepBuilderAPI_MakeWire(Ed1,Ed2,Ed3,Ed4);
		TopoDS_Face theFace = BRepBuilderAPI_MakeFace(theWire);
		BRepLib::BuildCurves3d(theFace);//���ɶ�Ӧ�ռ�ü�����
		if (!BRepAlgo::IsValid(theFace))
		{
			//�ٳ����µĲü��������ɡ�
			C2 = GCE2d_MakeSegment(C1->Value(C1->LastParameter()),
				C3->Value(C3->FirstParameter())).Value();//C1β��C3���ס�//yxk20200104����.Value()
			TopoDS_Edge Ed2 = BRepBuilderAPI_MakeEdge(C2,theSurface).Edge();
			C4 = GCE2d_MakeSegment(C3->Value(C3->LastParameter()),
				C1->Value(C1->FirstParameter())).Value();//yxk20200104����.Value()
			TopoDS_Edge Ed4 = BRepBuilderAPI_MakeEdge(C4,theSurface).Edge();
			Ed3.Reverse();
			theWire = BRepBuilderAPI_MakeWire(Ed1,Ed2,Ed3,Ed4);
			theFace = BRepBuilderAPI_MakeFace(theWire);
			BRepLib::BuildCurves3d(theFace);
			if (!BRepAlgo::IsValid(theFace))
			{
				MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd,"Error : The plate surface is not valid!!!","CasCade Error",MB_ICONERROR);
				return aFace;
			}
		}

		aFace = theFace;
	}
	else
	{
		aFace = BRepBuilderAPI_MakeFace(theSurface, Precision::Confusion());//yxk20200104
	}

	return aFace;
}

////////////////////////////////////////////////////////////////
//
//�����ͽ�������
int	ScCmdPlateBlendWithCont::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCmdPlate::Begin(aCtx);
	return 0;
}

int	ScCmdPlateBlendWithCont::End()
{
	return ScCmdPlate::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdPlateBlendWithCont::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
}

//����ƶ�����Ӧ����
void	ScCmdPlateBlendWithCont::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}

////////////////////////////////////////////////////////////////
//
//�����ͽ�������
int	ScCmdPlateFillHole::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCmdPlate::Begin(aCtx);

	SwitchState(S_SHAPE,"ѡ�����ڶ���");
	//��һ��LocalContext


	return 0;
}

int	ScCmdPlateFillHole::End()
{
	return ScCmdPlate::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdPlateFillHole::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SHAPE:
		{
			m_AISContext->InitCurrent();
			if(m_AISContext->MoreCurrent())
			{
				m_aShapeObj = Handle(AIS_Shape)::DownCast(m_AISContext->Current());
				//m_AISContext->OpenLocalContext();//yxk20191207
				m_AISContext->Unhilight(m_aShapeObj, Standard_True);
				m_AISContext->Activate(m_aShapeObj,2);

				SwitchState(S_EDGES,"ѡ�񶴵ı߽磬�Ҽ�������");
				this->NeedMultiSelect(TRUE);//��ѡ
			}
		}
		break;
	case S_EDGES:
		{
			if(nState == MINPUT_LEFT)
				m_AISContext->ShiftSelect(Standard_True);
			else
			{
				TopoDS_Shape aS = FillHole();
				Handle(AIS_Shape) aObj;
				Display(aObj,aS);
				Done();
			}
		}
		break;
	default:
		break;
	}
}

//����ƶ�����Ӧ����
void	ScCmdPlateFillHole::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SHAPE:
	case S_EDGES:
		{
			m_AISContext->MoveTo(point.x,point.y,aView, Standard_True);
		}
		break;
	default:
		break;
	}
}

TopoDS_Face		ScCmdPlateFillHole::FillHole()
{
	TopoDS_Face aFace;
	Standard_Integer nbedges = 0;
	for (m_AISContext->InitSelected(); m_AISContext->MoreSelected();
		m_AISContext->NextSelected()) 
	{
			nbedges++;
	}
//yxk20191208
	//Handle(GeomPlate_HArray1OfHCurveOnSurface) Fronts =
	//		new GeomPlate_HArray1OfHCurveOnSurface(1,nbedges);
	Handle(GeomPlate_HArray1OfHCurve) Fronts =
			new GeomPlate_HArray1OfHCurve(1,nbedges);
//endyxk
	Handle(TColStd_HArray1OfInteger) Tang = 
			new TColStd_HArray1OfInteger(1,nbedges);//����������Ҫ��
	Handle(TColStd_HArray1OfInteger) NbPtsCur = 
			new TColStd_HArray1OfInteger(1,nbedges);//����ȡ�������??

	Standard_Integer i = 0;
	TopoDS_Shape S1 = m_aShapeObj->Shape();
	TopTools_IndexedDataMapOfShapeListOfShape M;
	TopExp::MapShapesAndAncestors(S1, TopAbs_EDGE, TopAbs_FACE, M);//�ݴ��ҳ�edge��������face.

	//���ι���Լ��������
	for (m_AISContext->InitSelected(); m_AISContext->MoreSelected();m_AISContext->NextSelected()) 
	{
		i++;
		Tang->SetValue(i,1);//G1����
		NbPtsCur->SetValue(i,10);

		//����curveԼ��,�ܷ���
		TopoDS_Edge E = TopoDS::Edge(m_AISContext->SelectedShape());
		TopoDS_Face F = TopoDS::Face(M.FindFromKey(E).First());

		BRepAdaptor_Surface S(F);
		GeomAdaptor_Surface aGAS = S.Surface();
		Handle(GeomAdaptor_HSurface) aHGAS = new GeomAdaptor_HSurface(aGAS);

		Handle(BRepAdaptor_HCurve2d) C = new BRepAdaptor_HCurve2d();
		C->ChangeCurve2d().Initialize(E,F);

		Adaptor3d_CurveOnSurface ConS(C,aHGAS);

		Handle (Adaptor3d_HCurveOnSurface) HConS = new Adaptor3d_HCurveOnSurface(ConS);
		Fronts->SetValue(i,HConS);
	}

	//����blend�档ע�⣺��ʹedge���պ�Ҳ���Թ��죬�����յ������޷���ʾ����Ϊ�ü������պ�
	//���Լ򵥵Ĺ��죬�������������һ��ֱ�ߵȡ�
	GeomPlate_BuildPlateSurface abuildplate(NbPtsCur,Fronts,Tang,3);
	abuildplate.Perform();
	if (!abuildplate.IsDone())
	{ // New in 2.0
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd,"Error : Build plate not valid!","CasCade Error",MB_ICONERROR);
		return aFace;
	}

	//������棬����surface
	Handle(GeomPlate_Surface) aplate = abuildplate.Surface();

	GeomPlate_MakeApprox aMKS(aplate, Precision::Approximation(), 4, 7, 0.001, 1);
	Handle(Geom_BSplineSurface) support = aMKS.Surface();

	//����ü���������պϡ�
	BRepBuilderAPI_MakeWire MW;
	TopTools_Array1OfShape tab(1,nbedges);
	for (int i=1 ; i<=nbedges ; i++) 
	{
		//���������Ĵ������λ�ȡ���ߵķ���sense���ط���order����������˳��
		//1��ʾҪ��������
		if (abuildplate.Sense()->Value(abuildplate.Order()->Value(i))==1) 
		{
			//2d��������edge���ټ���3d���ߡ�
			BRepBuilderAPI_MakeEdge ME(abuildplate.Curves2d()->Value(abuildplate.Order()->Value(i)),
				support,
				Fronts->Value(abuildplate.Order()->Value(i))->LastParameter(),
				Fronts->Value(abuildplate.Order()->Value(i))->FirstParameter());
			TopoDS_Edge E = ME.Edge();
			BRepLib::BuildCurves3d(E);
			tab(abuildplate.Order()->Value(i)) = E;
			//MW.Add(E);
		}
		else 
		{
			BRepBuilderAPI_MakeEdge ME(abuildplate.Curves2d()->Value(abuildplate.Order()->Value(i)),
				support,
				Fronts->Value(abuildplate.Order()->Value(i))->FirstParameter(),
				Fronts->Value(abuildplate.Order()->Value(i))->LastParameter());
			TopoDS_Edge E = ME.Edge();
			BRepLib::BuildCurves3d(E);
			tab(abuildplate.Order()->Value(i)) = E;
		}	
	}

	//����ü���
	for (i=1 ; i<=nbedges ; i++) 
		MW.Add(TopoDS::Edge(tab(i)));
	TopoDS_Wire W;
	try{
		W=MW.Wire();
	}
	catch(StdFail_NotDone)
	{
		AfxMessageBox("Can't build wire!");
		return aFace;
	}

	if (!(W.Closed())){
		AfxMessageBox("Wire is not closed!");
		return aFace;
		//Standard_Failure::Raise("Wire is not closed");

	}

	//�������Face��
	BRepBuilderAPI_MakeFace MF(support,W,Standard_True);
	TopoDS_Face aface;
	aface = MF.Face();

	//�жϻ��ķ����Ƿ���
	BRepTopAdaptor_FClass2d clas2d(aface,Precision::Confusion());
	if (clas2d.PerformInfinitePoint() == TopAbs_IN) {
		W.Reverse();
		BRepBuilderAPI_MakeFace MF1(support,W,Standard_True);
		aface = MF1.Face();
	}

	//������֤һ��
	if (!BRepAlgo::IsValid(aface))
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd,"Error : The plate face is not valid!","CasCade Error",MB_ICONERROR);

	//m_AISContext->CloseLocalContex();//yxk20191207

	aFace = aface;
	return aFace;
}
