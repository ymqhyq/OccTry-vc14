#pragma once

class ScBRepLib
{
public:
		ScBRepLib(void);
		~ScBRepLib(void);

		//��������
		static	BOOL								IsVertex(const TopoDS_Shape& aS);
		static	BOOL								IsEdge(const TopoDS_Shape& aS) ;
		static	BOOL								IsWire(const TopoDS_Shape& aS) ;
		static	BOOL								IsFace(const TopoDS_Shape& aS) ;
		static	BOOL								IsShell(const TopoDS_Shape& aS) ;
		static	BOOL								IsSolid(const TopoDS_Shape& aS) ;
		static	BOOL								IsCompound(const TopoDS_Shape& aS) ;
		static	BOOL								IsCompSolid(const TopoDS_Shape& aS) ;

		//��һ��edge�ָ�Ϊ����
		static   BOOL								SplitEdge(const TopoDS_Edge& aEdge,double t,
																		TopoDS_Edge& aE1,TopoDS_Edge& aE2);

		//��һ��edge�ָ���һ��edge,��edge������list�У�������edge�ĸ���
		static   int								SplitEdgeByEdge(const TopoDS_Edge& aE1,const TopoDS_Edge& aE2,
																		TopTools_ListOfShape& shapeList);

		//��һ��edge�ָ�һ��wire����shape������list�У�������shape�ĸ���
		static  int									SplitWireByEdge(const TopoDS_Wire& aWire,const TopoDS_Edge& aEdge,
																		TopTools_ListOfShape& shapeList);

		//��һ��wire�ָ�һ��edge��
		static  int									SplitEdgeByWire(const TopoDS_Edge& aEdge,const TopoDS_Wire& aWire,
																		TopTools_ListOfShape& shapeList);

		//��һ��wire�ָ���һ��wire
		static  int									SplitWireByWire(const TopoDS_Wire& aW1,const TopoDS_Wire& aW2,
																		TopTools_ListOfShape& shapeList);

		//edge��wire�Ƿ�����
		static   BOOL								IsConnected(const TopoDS_Wire& aW,const TopoDS_Edge& aE);

		//��ȡface���⻷
		//ʹ��BRepTools::OuterWire�Ĵ��룬�����޸ġ�
		static  TopoDS_Wire							OuterWire(const TopoDS_Face& aFace);

		//����һϵ��wire,����ƽ��face.wire���ཻ,�����໥����.��Ҫ���ڴ���������
		//����һϵ��ƽ��.
		static  int									BuildPlnFace(TopTools_ListOfShape& aWList,TopTools_ListOfShape& aFList);

		//wire 1�Ƿ����wire2.ע��:wire1��wire2���ཻ.��Ȼ�����ж��Ƿ��ཻ.������Ϊ��Ч��û�д���.
		static  BOOL								IsWire2InWire1(const TopoDS_Wire& aW1,const TopoDS_Wire& aW2);

		//��һ���⻷��һ���ڻ�����face
		static TopoDS_Face							BuildPlaneFace(const TopoDS_Wire& aOW,const TopTools_ListOfShape& aIWList);

		//��һϵ��������edge��wire����һ��wire.
};
