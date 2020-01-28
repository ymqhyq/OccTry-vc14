#include "StdAfx.h"
#include <vector>
#include <algorithm>
#include <TopExp.hxx>
#include <Precision.hxx>
#include <ShapeAlgo.hxx>
#include <BRepTools.hxx>
#include <Bnd_SeqOfBox.hxx>
#include <BRepBndLib.hxx>

#include ".\scbreplib.h"

ScBRepLib::ScBRepLib(void)
{
}

ScBRepLib::~ScBRepLib(void)
{
}

//��������
BOOL			ScBRepLib::IsVertex(const TopoDS_Shape& aS) 
{
		return (!aS.IsNull() && (aS.ShapeType() == TopAbs_VERTEX));
}

BOOL			ScBRepLib::IsEdge(const TopoDS_Shape& aS) 
{
		return (!aS.IsNull() && (aS.ShapeType() == TopAbs_EDGE));
}

BOOL			ScBRepLib::IsWire(const TopoDS_Shape& aS) 
{
		return (!aS.IsNull() && (aS.ShapeType() == TopAbs_WIRE));
}

BOOL			ScBRepLib::IsFace(const TopoDS_Shape& aS) 
{
		return (!aS.IsNull() && (aS.ShapeType() == TopAbs_FACE));
}

BOOL			ScBRepLib::IsShell(const TopoDS_Shape& aS) 
{
		return (!aS.IsNull() && (aS.ShapeType() == TopAbs_SHELL));
}

BOOL			ScBRepLib::IsSolid(const TopoDS_Shape& aS) 
{
		return (!aS.IsNull() && (aS.ShapeType() == TopAbs_SOLID));
}

BOOL			ScBRepLib::IsCompound(const TopoDS_Shape& aS) 
{
		return (!aS.IsNull() && (aS.ShapeType() == TopAbs_COMPOUND));
}

BOOL			ScBRepLib::IsCompSolid(const TopoDS_Shape& aS) 
{
		return (!aS.IsNull() && (aS.ShapeType() == TopAbs_COMPSOLID));
}

//��һ��edge�ָ�Ϊ����
BOOL			ScBRepLib::SplitEdge(const TopoDS_Edge& aEdge,double t,
													  TopoDS_Edge& aE1,TopoDS_Edge& aE2)
{
		if(aEdge.IsNull())
				return FALSE;

		double df,dl;
		Handle(Geom_Curve) aCur = BRep_Tool::Curve(aEdge,df,dl);
		if(aCur.IsNull())
				return FALSE;

		if(df > dl)
		{
				double tmp = df;dl = df;df = tmp;
		}
		double dtol = 1e-6;
		if(t - dtol < df || t + dtol > dl)
		{
				return FALSE;
		}

		//���������µ�edge
		try
		{
				aE1 = BRepBuilderAPI_MakeEdge(aCur,df,t);
				aE2 = BRepBuilderAPI_MakeEdge(aCur,t,dl);
				if(aE1.IsNull() || aE2.IsNull())
						return FALSE;
		}
		catch (Standard_Failure)
		{
				return FALSE;
		}

		return TRUE;
}

//��һ��edge�ָ���һ��edge,��edge������list�У�������edge�ĸ���
int		ScBRepLib::SplitEdgeByEdge(const TopoDS_Edge& aE1,const TopoDS_Edge& aE2,
																	TopTools_ListOfShape& shapeList)
{
		int nC = 0;
		std::vector<double>  arpars;//��������б�

		//Ϊÿ������һ���µ�edge��
		double df,dl;
		Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE1,df,dl);
		if(aCur.IsNull())
				return FALSE;

		if(df > dl)
		{
				double tmp = df;dl = df;df = tmp;
		}

		//��
		BRepExtrema_DistShapeShape  dss(aE1,aE2);
		try{
				dss.Perform();
		}catch(Standard_Failure){
				return 0;
		}

		nC = dss.NbSolution();
		if(nC == 0)
				return 0;

		arpars.push_back(df);
		int iC = 0;
		gp_Pnt p1,p2;
		//���λ�ȡ����
		for(int ix = 1;ix <= nC;ix ++)
		{
				double t ;
				if(dss.SupportTypeShape1(ix) == BRepExtrema_IsOnEdge)
				{
						//Ҫ�ж�һ�¾���
						p1 = dss.PointOnShape1(ix);
						p2 = dss.PointOnShape2(ix);
						if(p1.Distance(p2) < Precision::Confusion())
						{
								dss.ParOnEdgeS1(ix,t);
								arpars.push_back(t);
								iC ++;
						}
						else
						{
								DTRACE("\n extrema point,not interpnt");
						}
				}
		}
		arpars.push_back(dl);

		if(iC == 0)
				return 0;

		//����
		std::sort(arpars.begin(),arpars.end());

		//Ϊû�Բ���������һ��edge
		double t1,t2;
		TopoDS_Edge aNE;
		nC = 0;
		for(int ix = 0;ix < arpars.size() - 1;ix ++)
		{
				t1 = arpars.at(ix);
				t2 = arpars.at(ix + 1);
				if(fabs(t1 - t2) < 1e-6)
						continue;
				aNE = BRepBuilderAPI_MakeEdge(aCur,t1,t2);
				if(!aNE.IsNull())
				{
						//ע�ⷽ�򣬱�֤�·ָ�edge��˳���edge�ķ���һ��
						if(aE1.Orientation() == TopAbs_FORWARD)
								shapeList.Append(aNE);
						else
								shapeList.Prepend(aNE);
						nC ++;
				}
		}
		
		return nC;
}

//��һ��edge�ָ�һ��wire����shape������list�У�������shape�ĸ���
int			ScBRepLib::SplitWireByEdge(const TopoDS_Wire& aWire,const TopoDS_Edge& aEdge,
																TopTools_ListOfShape& shapeList)
{
		if(aWire.IsNull() || aEdge.IsNull())
				return 0;

		int  nC = 0,iC = 0,nNewC = 0;
		TopTools_ListOfShape tmplist,wlist;
		TopoDS_Wire aW;
		TopExp_Explorer  ex;
		
		//�����ʷ�wire�е�edge�������б�Ҫ�����ӡ�
		for(ex.Init(aWire,TopAbs_EDGE);ex.More();ex.Next())
		{
				TopoDS_Edge aE = TopoDS::Edge(ex.Current());

				tmplist.Clear();
				nC = ScBRepLib::SplitEdgeByEdge(aE,aEdge,tmplist);
				if(nC == 0)//û���µ�edge
				{
						wlist.Append(aE);
				}
				else
				{
						TopTools_ListIteratorOfListOfShape  ll(wlist);
						BRepBuilderAPI_MakeWire  mw;
						for(;ll.More();ll.Next())
								mw.Add(TopoDS::Edge(ll.Value()));
						wlist.Clear();

						//���·ָ�edge���ҵ���aW���ڵ�edge������¼��һ�˵�edge����edge
						//�ͺ�����edge���ڡ�
						 iC = 0;
						TopTools_ListIteratorOfListOfShape  ite(tmplist);
						for(;ite.More();ite.Next())
						{
								if(iC == 0)
								{
										mw.Add(TopoDS::Edge(ite.Value()));
										aW = mw.Wire();
										if(!aW.IsNull())
										{
												nNewC ++;
												shapeList.Append(aW);
										}
								}
								else 	if(iC == nC - 1)
								{
										wlist.Append(ite.Value());
								}
								else
								{
										nNewC ++;
										shapeList.Append(ite.Value());//�µ�edge
								}

								iC ++;
						}//end for
				}//end else
		}//end for
		if(wlist.Extent() > 0)
		{
				TopTools_ListIteratorOfListOfShape  ll(wlist);
				BRepBuilderAPI_MakeWire  mw;
				for(;ll.More();ll.Next())
						mw.Add(TopoDS::Edge(ll.Value()));
				TopoDS_Wire aW = mw.Wire();
				if(!aW.IsNull())
				{
						nNewC ++;
						shapeList.Append(aW);
				}
		}

		return nNewC;
}

//��һ��wire�ָ�һ��edge��
int			ScBRepLib::SplitEdgeByWire(const TopoDS_Edge& aEdge,const TopoDS_Wire& aWire,
																TopTools_ListOfShape& shapeList)
{
		int nC = 0,iC;
		std::vector<double>  arpars;//��������б�

		//Ϊÿ������һ���µ�edge��
		double df,dl;
		Handle(Geom_Curve) aCur = BRep_Tool::Curve(aEdge,df,dl);
		if(aCur.IsNull())
				return FALSE;

		if(df > dl)
		{
				double tmp = df;dl = df;df = tmp;
		}

		arpars.push_back(df);

		double t;
		gp_Pnt p1,p2;
		TopExp_Explorer ex;

		iC = 0;
		for(ex.Init(aWire,TopAbs_EDGE);ex.More();ex.Next())
		{
				TopoDS_Edge aE = TopoDS::Edge(ex.Current());

				//��
				BRepExtrema_DistShapeShape dss(aEdge,aE);
				try{
						dss.Perform();
				}catch(Standard_Failure){
						continue;
				}

				//��ȡ����
				for(int ix = 1;ix <= dss.NbSolution();ix ++)
				{
						if(dss.SupportTypeShape1(ix) == BRepExtrema_IsOnEdge)
						{
								//Ҫ�ж�һ�¾���
								p1 = dss.PointOnShape1(ix);
								p2 = dss.PointOnShape2(ix);
								if(p1.Distance(p2) < Precision::Confusion())
								{
										dss.ParOnEdgeS1(ix,t);
										arpars.push_back(t);
										iC ++;
								}
								else
								{
										DTRACE("\n extrema point,not interpnt");
								}
						}
				}
		}

		if(iC == 0)
				return 0;//�޽���

		arpars.push_back(dl);
		std::sort(arpars.begin(),arpars.end());

		//�����µ�edge
		double t1,t2;
		TopoDS_Edge aNE;
		nC = 0;
		for(int ix = 0;ix < arpars.size() - 1;ix ++)
		{
				t1 = arpars.at(ix);
				t2 = arpars.at(ix + 1);
				if(fabs(t1 - t2) < 1e-6)
						continue;
				aNE = BRepBuilderAPI_MakeEdge(aCur,t1,t2);
				if(!aNE.IsNull())
				{
						//ע�ⷽ�򣬱�֤�·ָ�edge��˳���edge�ķ���һ��
						if(aEdge.Orientation() == TopAbs_FORWARD)
								shapeList.Append(aNE);
						else
								shapeList.Prepend(aNE);
						nC ++;
				}
		}

		return nC;
}

//��һ��wire�ָ���һ��wire
int			ScBRepLib::SplitWireByWire(const TopoDS_Wire& aW1,const TopoDS_Wire& aW2,
																TopTools_ListOfShape& shapeList)
{
		if(aW1.IsNull() || aW2.IsNull())
				return 0;

		int  nC = 0,iC = 0,nNewC = 0;
		TopTools_ListOfShape tmplist,wlist;
		TopoDS_Wire aW;
		TopExp_Explorer  ex;

		//�����ʷ�wire�е�edge�������б�Ҫ�����ӡ�
		for(ex.Init(aW1,TopAbs_EDGE);ex.More();ex.Next())
		{
				TopoDS_Edge aE = TopoDS::Edge(ex.Current());

				tmplist.Clear();
				nC = ScBRepLib::SplitEdgeByWire(aE,aW2,tmplist);
				if(nC == 0)//û���µ�edge
				{
						wlist.Append(aE);
				}
				else
				{
						TopTools_ListIteratorOfListOfShape  ll(wlist);
						BRepBuilderAPI_MakeWire  mw;
						for(;ll.More();ll.Next())
								mw.Add(TopoDS::Edge(ll.Value()));
						wlist.Clear();

						//���·ָ�edge���ҵ���aW���ڵ�edge������¼��һ�˵�edge����edge
						//�ͺ�����edge���ڡ�
						iC = 0;
						TopTools_ListIteratorOfListOfShape  ite(tmplist);
						for(;ite.More();ite.Next())
						{
								if(iC == 0)
								{
										mw.Add(TopoDS::Edge(ite.Value()));
										aW = mw.Wire();
										if(!aW.IsNull())
										{
												nNewC ++;
												shapeList.Append(aW);
										}
								}
								else 	if(iC == nC - 1)
								{
										wlist.Append(ite.Value());
								}
								else
								{
										nNewC ++;
										shapeList.Append(ite.Value());//�µ�edge
								}

								iC ++;
						}//end for
				}//end else
		}//end for
		if(wlist.Extent() > 0)
		{
				TopTools_ListIteratorOfListOfShape  ll(wlist);
				BRepBuilderAPI_MakeWire  mw;
				for(;ll.More();ll.Next())
						mw.Add(TopoDS::Edge(ll.Value()));
				TopoDS_Wire aW = mw.Wire();
				if(!aW.IsNull())
				{
						nNewC ++;
						shapeList.Append(aW);
				}
		}

		return nNewC;
}


//edge��wire�Ƿ�����
BOOL			ScBRepLib::IsConnected(const TopoDS_Wire& aW,const TopoDS_Edge& aE)
{
		if(aW.IsNull() || aE.IsNull())
				return FALSE;

		TopoDS_Vertex  v1,v2,v3,v4;
		TopExp::Vertices(aE,v1,v2);
		TopExp::Vertices(aW,v3,v4);
		gp_Pnt p1,p2,p3,p4;
		p1 = BRep_Tool::Pnt(v1);
		p2 = BRep_Tool::Pnt(v2);
		p3 = BRep_Tool::Pnt(v3);
		p4 = BRep_Tool::Pnt(v4);
		double dTol = Precision::Confusion();
		if(p1.Distance(p3) < dTol || p1.Distance(p4) < dTol ||
				p2.Distance(p3) < dTol || p2.Distance(p4) < dTol)
				return TRUE;

		return FALSE;
}

//ʹ��BRepTools::OuterWire�Ĵ��룬�����޸ġ�
TopoDS_Wire		ScBRepLib::OuterWire(const TopoDS_Face& aFace)
{
	TopoDS_Wire Wres;
	TopExp_Explorer expw (aFace,TopAbs_WIRE);
	BOOL bOverlap = FALSE;//�Ƿ��Χ���໥����

	if (expw.More()) 
	{
		Wres = TopoDS::Wire(expw.Current());
		expw.Next();
		if (expw.More()) 
		{
			Standard_Real UMin, UMax, VMin, VMax;
			Standard_Real umin, umax, vmin, vmax;
			BRepTools::UVBounds(aFace,Wres,UMin,UMax,VMin,VMax);
			while (expw.More()) 
			{
				const TopoDS_Wire& W = TopoDS::Wire(expw.Current());
				BRepTools::UVBounds(aFace,W,umin, umax, vmin, vmax);
				//���¼��жϷ�Χ���ܲ��У����磺һ�������wire��һ��С��wire����С����
				//������ʱ�����ڰ�Χ���㷨�����ܵ������ְ�Χ���ص������´����ѡ��
				//�����㷨�����ںõ������
				if ((umin <= UMin) &&
					(umax >= UMax) &&
					(vmin <= VMin) &&
					(vmax >= VMax)) 
				{//w��Χ�а�Χ�˼�¼�İ�Χ�С�
					Wres = W;
					UMin = umin;
					UMax = umax;
					VMin = vmin;
					VMax = vmax;
				}
				else
				{
					//�ж��Ƿ��ص�
					if(umin > UMax || umax < UMin || vmin > VMax || vmax < VMin)
					{//�������ص�
					}
					else
					{
						//���һ����������һ�������ڣ���d1,d2Ӧ����š�
						double Ud1 = UMax - umax,Ud2 = UMin - umin;
						double Vd1 = VMax - vmax,Vd2 = VMin - vmin;
						if((Ud1 * Ud2 >= 0.0) && (Vd1 * Vd2 > 0.0) ||
							(Ud1 * Ud2 > 0.0) && (Vd1 * Vd2 >= 0.0))
						{
							DTRACE("\n wire overlap [%f,%f]*[%f,%f] ps [%f,%f]*[%f,%f]",
								umin,umax,vmin,vmax,UMin,UMax,VMin,VMax);
							bOverlap = TRUE;
							break;
						}
					}
				}
				expw.Next();
			}
		}
	}
	if(!bOverlap)
	{
		return Wres;
	}

	//�ص�ʹ����һ���㷨
	TopoDS_Wire aOWire;
	try
	{
		aOWire = ShapeAlgo::AlgoContainer()->OuterWire(aFace);//Note:aFace �����쳣???
	}
	catch(Standard_Failure)
	{
		aOWire = Wres;//������ǰ�ġ�
	}

	return aOWire;
}


//����һϵ��wire,����ƽ��face.wire���ཻ,�����໥����.��Ҫ���ڴ���������
//����һϵ��ƽ��.
int		ScBRepLib::BuildPlnFace(TopTools_ListOfShape& aWList,TopTools_ListOfShape& aFList)
{
	int nFs = 0;

	//�ȸ��ݰ�Χ��,�ж��Ƿ��໥����.
	Bnd_SeqOfBox seqBox;
	Bnd_Box bb;
	TopTools_SequenceOfShape seqW,seqAlone;
	TopTools_ListIteratorOfListOfShape its(aWList);
	for(;its.More();its.Next())
	{
		TopoDS_Wire aW = TopoDS::Wire(its.Value());
		BRepBndLib::Add(aW,bb);
		seqBox.Append(bb);
		seqW.Append(aW);
	}
	
	TopTools_DataMapOfShapeListOfShape mSS;//��¼�˰�����ϵ,key��shape������list�е�shape.
	for(int ix = 1;ix <= seqW.Length();ix ++)
	{
		TopoDS_Wire aW1 = TopoDS::Wire(seqW.Value(ix));
		const Bnd_Box& bb1 = seqBox.Value(ix);

		BOOL bInter = FALSE;
		for(int jx = 1;jx <= seqW.Length();jx ++)
		{
			if(ix == jx)
				continue;

			TopoDS_Wire aW2 = TopoDS::Wire(seqW.Value(jx));
			const Bnd_Box& bb2 = seqBox.Value(jx);

			if(bb1.IsOut(bb2) && bb2.IsOut(bb1))
			{
				continue;
			}
			else
			{
				//�ཻ,�ж����ߵĹ�ϵ.��1��2,����2��1.ע��:��Χ���ཻ��������,��wire������ܲ��ཻ
				if(ScBRepLib::IsWire2InWire1(aW1,aW2))
				{
					if(!mSS.IsBound(aW1))
					{
						TopTools_ListOfShape aList;
						mSS.Bind(aW1,aList);
					}
					BOOL bF = FALSE;
					TopTools_ListIteratorOfListOfShape lit;
					for(lit.Initialize(mSS.Find(aW1));lit.More();lit.Next())
					{
						if(aW2.IsEqual(lit.Value()))
						{
							bF = TRUE;
							break;
						}
					}
					if(!bF)
						mSS.ChangeFind(aW1).Append(aW2);
					bInter = TRUE;
				}
				else if(ScBRepLib::IsWire2InWire1(aW2,aW1))
				{
					if(!mSS.IsBound(aW2))
					{
						TopTools_ListOfShape aList;
						mSS.Bind(aW2,aList);
					}
					BOOL bF = FALSE;
					TopTools_ListIteratorOfListOfShape lit;
					for(lit.Initialize(mSS.Find(aW2));lit.More();lit.Next())
					{
						if(aW1.IsEqual(lit.Value()))
						{
							bF = TRUE;
							break;
						}
					}
					if(!bF)
						mSS.ChangeFind(aW2).Append(aW1);
					bInter = TRUE;
				}
				else
				{
					continue;//��Χ���ཻ,ʵ�ʲ��ཻ
				}
			}
		}

		if(!bInter)//ʲô�����ཻ,������wire
		{
			seqAlone.Append(aW1);
		}
	}
	//���໥������wire,�ж���Ӧ��ϵ,����ƽ��
	TopTools_DataMapIteratorOfDataMapOfShapeListOfShape mapit;
	for(mapit.Initialize(mSS);mapit.More();mapit.Next())
	{
		TopoDS_Wire aOW = TopoDS::Wire(mapit.Key());
		const TopTools_ListOfShape& aList = mapit.Value();

		//����face
		TopoDS_Face aF = ScBRepLib::BuildPlaneFace(aOW,aList);
		if(!aF.IsNull())
		{
			aFList.Append(aF);
			nFs ++;
		}
	}

	//�Զ�����wire����ƽ��.
	for(int ix = 1;ix <= seqAlone.Length();ix ++)
	{
		TopoDS_Wire aW = TopoDS::Wire(seqAlone.Value(ix));
		BRepBuilderAPI_MakeFace MF(aW,Standard_True);
		if(MF.IsDone())
		{
			TopoDS_Face aNF = MF.Face();
			if(!aNF.IsNull())
			{
				aFList.Append(aNF);
				nFs ++;
			}
		}
	}

	return nFs;
}

//wire 1�Ƿ����wire2.
BOOL	ScBRepLib::IsWire2InWire1(const TopoDS_Wire& aW1,const TopoDS_Wire& aW2)
{
	BRepBuilderAPI_FindPlane fpln(aW1);
	if(!fpln.Found())
		return FALSE;

	gp_Pln aPln = fpln.Plane()->Pln();
	//����face
	BRepBuilderAPI_MakeFace mf(aPln,aW1);
	if(!mf.IsDone())
		return FALSE;

	TopoDS_Face aNF = mf.Face();

	//�ж�w2��һ�����Ƿ���mf��.
	BOOL bIN = FALSE;
	int nC = 0;
	BRepTopAdaptor_FClass2d fcls(aNF,Precision::PConfusion());

	TopExp_Explorer ex;
	for(ex.Init(aW2,TopAbs_VERTEX);ex.More();ex.Next())
	{
		TopoDS_Vertex aV = TopoDS::Vertex(ex.Current());
		gp_Pnt pnt = BRep_Tool::Pnt(aV);

		GeomAPI_ProjectPointOnSurf pp(pnt,fpln.Plane());
		if(pp.IsDone() && pp.NbPoints() > 0)
		{
			gp_Pnt2d uv;
			double u,v;
			pp.LowerDistanceParameters(u,v);
			uv.SetCoord(u,v);
			//��֤�Ƿ���aW1�ڲ�
			TopAbs_State sta = fcls.Perform(uv);
			if(sta == TopAbs_IN)
			{
				bIN = TRUE;
				break;
			}
		}
		else
		{
			nC ++;
			if(nC > 3)
				break;
		}
	}
	
	return bIN;
}

//��һ���⻷��һ���ڻ�����face
TopoDS_Face		ScBRepLib::BuildPlaneFace(const TopoDS_Wire& aOW,const TopTools_ListOfShape& aIWList)
{

	TopoDS_Face aNF;

	try{
		//��ʹ���⻷����һ��ƽ��
		BRepBuilderAPI_MakeFace MF(aOW,Standard_True);
		if(MF.IsDone())
		{
			aNF = MF.Face();
			if(aNF.IsNull())
				return aNF;
		}

		//�������ζ��ڻ������ж�,��Ҫ��Ҫ��֤���պ�,���һ��ķ�����ȷ
		BRep_Builder B;
		TopTools_ListIteratorOfListOfShape lit;
		for(lit.Initialize(aIWList);lit.More();lit.Next())
		{
			TopoDS_Wire aW = TopoDS::Wire(lit.Value());
			
			if(aW.IsNull() || !BRep_Tool::IsClosed(aW))
			{
				DTRACE("\n inner wire make null or not closed.");
				continue;
			}

			//�ж��ڻ��ķ���
			TopoDS_Shape aTS = aNF.EmptyCopied();
			TopoDS_Face aTF = TopoDS::Face(aTS);
			aTF.Orientation(TopAbs_FORWARD);
			B.Add(aTF,aW);
			BRepTopAdaptor_FClass2d fcls(aTF,Precision::PConfusion());
			TopAbs_State sta = fcls.PerformInfinitePoint();
			if(sta == TopAbs_OUT)
				aW.Reverse();

			MF.Add(aW);
		}

		//��ȡ���յ�face
		aNF = MF.Face();

	}catch(Standard_Failure){
		return aNF;
	}
	
	return aNF;
}