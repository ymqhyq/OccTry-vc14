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

//辅助函数
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

//将一条edge分割为两条
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

		//创建两个新的edge
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

//用一条edge分割另一条edge,新edge保存在list中，返回新edge的个数
int		ScBRepLib::SplitEdgeByEdge(const TopoDS_Edge& aE1,const TopoDS_Edge& aE2,
																	TopTools_ListOfShape& shapeList)
{
		int nC = 0;
		std::vector<double>  arpars;//交点参数列表

		//为每段生成一个新的edge。
		double df,dl;
		Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE1,df,dl);
		if(aCur.IsNull())
				return FALSE;

		if(df > dl)
		{
				double tmp = df;dl = df;df = tmp;
		}

		//求交
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
		//依次获取参数
		for(int ix = 1;ix <= nC;ix ++)
		{
				double t ;
				if(dss.SupportTypeShape1(ix) == BRepExtrema_IsOnEdge)
				{
						//要判断一下距离
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

		//排序
		std::sort(arpars.begin(),arpars.end());

		//为没对参数间生成一个edge
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
						//注意方向，保证新分割edge的顺序和edge的方向一致
						if(aE1.Orientation() == TopAbs_FORWARD)
								shapeList.Append(aNE);
						else
								shapeList.Prepend(aNE);
						nC ++;
				}
		}
		
		return nC;
}

//用一条edge分割一个wire。新shape保存在list中，返回新shape的个数
int			ScBRepLib::SplitWireByEdge(const TopoDS_Wire& aWire,const TopoDS_Edge& aEdge,
																TopTools_ListOfShape& shapeList)
{
		if(aWire.IsNull() || aEdge.IsNull())
				return 0;

		int  nC = 0,iC = 0,nNewC = 0;
		TopTools_ListOfShape tmplist,wlist;
		TopoDS_Wire aW;
		TopExp_Explorer  ex;
		
		//依次剖分wire中的edge，并进行必要的连接。
		for(ex.Init(aWire,TopAbs_EDGE);ex.More();ex.Next())
		{
				TopoDS_Edge aE = TopoDS::Edge(ex.Current());

				tmplist.Clear();
				nC = ScBRepLib::SplitEdgeByEdge(aE,aEdge,tmplist);
				if(nC == 0)//没有新的edge
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

						//从新分割edge中找到和aW相邻的edge，并记录另一端的edge，该edge
						//和后续的edge相邻。
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
										shapeList.Append(ite.Value());//新的edge
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

//用一条wire分割一个edge。
int			ScBRepLib::SplitEdgeByWire(const TopoDS_Edge& aEdge,const TopoDS_Wire& aWire,
																TopTools_ListOfShape& shapeList)
{
		int nC = 0,iC;
		std::vector<double>  arpars;//交点参数列表

		//为每段生成一个新的edge。
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

				//求交
				BRepExtrema_DistShapeShape dss(aEdge,aE);
				try{
						dss.Perform();
				}catch(Standard_Failure){
						continue;
				}

				//获取交点
				for(int ix = 1;ix <= dss.NbSolution();ix ++)
				{
						if(dss.SupportTypeShape1(ix) == BRepExtrema_IsOnEdge)
						{
								//要判断一下距离
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
				return 0;//无交点

		arpars.push_back(dl);
		std::sort(arpars.begin(),arpars.end());

		//生成新的edge
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
						//注意方向，保证新分割edge的顺序和edge的方向一致
						if(aEdge.Orientation() == TopAbs_FORWARD)
								shapeList.Append(aNE);
						else
								shapeList.Prepend(aNE);
						nC ++;
				}
		}

		return nC;
}

//用一个wire分割另一个wire
int			ScBRepLib::SplitWireByWire(const TopoDS_Wire& aW1,const TopoDS_Wire& aW2,
																TopTools_ListOfShape& shapeList)
{
		if(aW1.IsNull() || aW2.IsNull())
				return 0;

		int  nC = 0,iC = 0,nNewC = 0;
		TopTools_ListOfShape tmplist,wlist;
		TopoDS_Wire aW;
		TopExp_Explorer  ex;

		//依次剖分wire中的edge，并进行必要的连接。
		for(ex.Init(aW1,TopAbs_EDGE);ex.More();ex.Next())
		{
				TopoDS_Edge aE = TopoDS::Edge(ex.Current());

				tmplist.Clear();
				nC = ScBRepLib::SplitEdgeByWire(aE,aW2,tmplist);
				if(nC == 0)//没有新的edge
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

						//从新分割edge中找到和aW相邻的edge，并记录另一端的edge，该edge
						//和后续的edge相邻。
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
										shapeList.Append(ite.Value());//新的edge
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


//edge和wire是否相连
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

//使用BRepTools::OuterWire的代码，略作修改。
TopoDS_Wire		ScBRepLib::OuterWire(const TopoDS_Face& aFace)
{
	TopoDS_Wire Wres;
	TopExp_Explorer expw (aFace,TopAbs_WIRE);
	BOOL bOverlap = FALSE;//是否包围盒相互覆盖

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
				//如下简单判断范围可能不行，例如：一个大的外wire和一个小的wire，当小环很
				//贴近大环时，由于包围盒算法，可能导致两种包围盒重叠，导致错误的选择。
				//下面算法适用于好的情况。
				if ((umin <= UMin) &&
					(umax >= UMax) &&
					(vmin <= VMin) &&
					(vmax >= VMax)) 
				{//w包围盒包围了记录的包围盒。
					Wres = W;
					UMin = umin;
					UMax = umax;
					VMin = vmin;
					VMax = vmax;
				}
				else
				{
					//判断是否重叠
					if(umin > UMax || umax < UMin || vmin > VMax || vmax < VMin)
					{//不可能重叠
					}
					else
					{
						//如果一个区间在另一个区间内，则d1,d2应当异号。
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

	//重叠使用另一个算法
	TopoDS_Wire aOWire;
	try
	{
		aOWire = ShapeAlgo::AlgoContainer()->OuterWire(aFace);//Note:aFace 出现异常???
	}
	catch(Standard_Failure)
	{
		aOWire = Wres;//返回先前的。
	}

	return aOWire;
}


//根据一系列wire,创建平面face.wire不相交,可能相互包含.主要用于从文字轮廓
//生成一系列平面.
int		ScBRepLib::BuildPlnFace(TopTools_ListOfShape& aWList,TopTools_ListOfShape& aFList)
{
	int nFs = 0;

	//先根据包围盒,判断是否相互包含.
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
	
	TopTools_DataMapOfShapeListOfShape mSS;//记录了包含关系,key的shape包含了list中的shape.
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
				//相交,判断两者的关系.先1后2,和先2后1.注意:包围盒相交甚至包含,但wire本身可能不相交
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
					continue;//包围盒相交,实际不相交
				}
			}
		}

		if(!bInter)//什么都不相交,独立的wire
		{
			seqAlone.Append(aW1);
		}
	}
	//对相互包含的wire,判断相应关系,创建平面
	TopTools_DataMapIteratorOfDataMapOfShapeListOfShape mapit;
	for(mapit.Initialize(mSS);mapit.More();mapit.Next())
	{
		TopoDS_Wire aOW = TopoDS::Wire(mapit.Key());
		const TopTools_ListOfShape& aList = mapit.Value();

		//创建face
		TopoDS_Face aF = ScBRepLib::BuildPlaneFace(aOW,aList);
		if(!aF.IsNull())
		{
			aFList.Append(aF);
			nFs ++;
		}
	}

	//对独立的wire创建平面.
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

//wire 1是否包含wire2.
BOOL	ScBRepLib::IsWire2InWire1(const TopoDS_Wire& aW1,const TopoDS_Wire& aW2)
{
	BRepBuilderAPI_FindPlane fpln(aW1);
	if(!fpln.Found())
		return FALSE;

	gp_Pln aPln = fpln.Plane()->Pln();
	//构造face
	BRepBuilderAPI_MakeFace mf(aPln,aW1);
	if(!mf.IsDone())
		return FALSE;

	TopoDS_Face aNF = mf.Face();

	//判断w2的一个点是否在mf上.
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
			//验证是否在aW1内部
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

//从一个外环和一组内环创建face
TopoDS_Face		ScBRepLib::BuildPlaneFace(const TopoDS_Wire& aOW,const TopTools_ListOfShape& aIWList)
{

	TopoDS_Face aNF;

	try{
		//先使用外环创建一个平面
		BRepBuilderAPI_MakeFace MF(aOW,Standard_True);
		if(MF.IsDone())
		{
			aNF = MF.Face();
			if(aNF.IsNull())
				return aNF;
		}

		//下面依次对内环进行判断,主要是要保证环闭合,并且环的方向正确
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

			//判断内环的方向
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

		//获取最终的face
		aNF = MF.Face();

	}catch(Standard_Failure){
		return aNF;
	}
	
	return aNF;
}