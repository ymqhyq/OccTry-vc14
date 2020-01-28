#pragma once

class ScBRepLib
{
public:
		ScBRepLib(void);
		~ScBRepLib(void);

		//辅助函数
		static	BOOL								IsVertex(const TopoDS_Shape& aS);
		static	BOOL								IsEdge(const TopoDS_Shape& aS) ;
		static	BOOL								IsWire(const TopoDS_Shape& aS) ;
		static	BOOL								IsFace(const TopoDS_Shape& aS) ;
		static	BOOL								IsShell(const TopoDS_Shape& aS) ;
		static	BOOL								IsSolid(const TopoDS_Shape& aS) ;
		static	BOOL								IsCompound(const TopoDS_Shape& aS) ;
		static	BOOL								IsCompSolid(const TopoDS_Shape& aS) ;

		//将一条edge分割为两条
		static   BOOL								SplitEdge(const TopoDS_Edge& aEdge,double t,
																		TopoDS_Edge& aE1,TopoDS_Edge& aE2);

		//用一条edge分割另一条edge,新edge保存在list中，返回新edge的个数
		static   int								SplitEdgeByEdge(const TopoDS_Edge& aE1,const TopoDS_Edge& aE2,
																		TopTools_ListOfShape& shapeList);

		//用一条edge分割一个wire。新shape保存在list中，返回新shape的个数
		static  int									SplitWireByEdge(const TopoDS_Wire& aWire,const TopoDS_Edge& aEdge,
																		TopTools_ListOfShape& shapeList);

		//用一条wire分割一个edge。
		static  int									SplitEdgeByWire(const TopoDS_Edge& aEdge,const TopoDS_Wire& aWire,
																		TopTools_ListOfShape& shapeList);

		//用一个wire分割另一个wire
		static  int									SplitWireByWire(const TopoDS_Wire& aW1,const TopoDS_Wire& aW2,
																		TopTools_ListOfShape& shapeList);

		//edge和wire是否相连
		static   BOOL								IsConnected(const TopoDS_Wire& aW,const TopoDS_Edge& aE);

		//获取face的外环
		//使用BRepTools::OuterWire的代码，略作修改。
		static  TopoDS_Wire							OuterWire(const TopoDS_Face& aFace);

		//根据一系列wire,创建平面face.wire不相交,可能相互包含.主要用于从文字轮廓
		//生成一系列平面.
		static  int									BuildPlnFace(TopTools_ListOfShape& aWList,TopTools_ListOfShape& aFList);

		//wire 1是否包含wire2.注意:wire1和wire2不相交.当然可以判断是否相交.但这里为了效率没有处理.
		static  BOOL								IsWire2InWire1(const TopoDS_Wire& aW1,const TopoDS_Wire& aW2);

		//从一个外环和一组内环创建face
		static TopoDS_Face							BuildPlaneFace(const TopoDS_Wire& aOW,const TopTools_ListOfShape& aIWList);

		//从一系列相连的edge和wire创建一个wire.
};
