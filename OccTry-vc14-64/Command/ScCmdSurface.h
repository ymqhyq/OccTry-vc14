#pragma once

#include "ScCmdCurve.h"
#include "ScCmdCurveCreate.h"
#include "ScCmdFaceBase.h"

class ScCmdSurface
{
public:
	ScCmdSurface(void);
	~ScCmdSurface(void);
};

//多段线
class ScCmdClosedPolyLine : public ScCmdPolyLine{
public:
	ScCmdClosedPolyLine();
	~ScCmdClosedPolyLine();


	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

protected:
	virtual  TopoDS_Shape	BuildNewShape();
};

//
// 曲面缝合的功能
//
class ScCmdSurfSewing : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SURFS};

	TopoDS_Shape			BuildNewShape();

protected:
	AIS_SequenceOfInteractive	m_seqOfObj;
	Handle(AIS_Shape)			m_aNewObj;
};

// 
// 曲面filling功能。
// 封装了MakeFilling的接口。实现多种功能。
// 注意和CmdPlate中的区别，哪里是比较底层的实现。
// 要实现四种类型的补面算法：边界约束、带点约束、带曲线约束、带面约束。
//
class ScCmdSurfFilling : public ScCommand{
public:
	ScCmdSurfFilling(int nType);

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

public:
	//补面类型
	enum {
		eBoundary = 1,
		eWithPoints,
		eWithCurves,
		eWithFace,
	};

	int						m_nFillType;//补洞类型

protected:
	enum{S_SURF,S_EDGE,S_POINTS,S_CURVES,S_FACES};    //选面，选边。

	TopoDS_Shape			BuildNewShape();

protected:
	BRepOffsetAPI_MakeFilling	*m_pApiFill;
	TopoDS_Face					m_aFace;
	TopoDS_Shape				m_aShape;
	int							m_nEdges;
};

//从闭合曲线生成面
class ScCmdSurfPlanClosedCurves : public ScCmdCurveSewing{
public:
	
protected:
	virtual TopoDS_Shape		BuildNewShape();
};

//
// 曲面裁剪算法
//
class ScCmdSurfCut : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SURF1,S_SURF2,S_CUT};

	BOOL					SplitSurf();
	void					CutSurf();

protected:
	TopoDS_Face				m_aFace1;
	TopoDS_Face				m_aFace2;
};

//
// 炸开成面。从solid和shell中提取面。
//
class ScCmdExtraFaces : public ScCommand{
public:
	
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SHAPE};

	void					ExtraFaces();

protected:
	TopoDS_Shape			m_aShape;

};

////////////////////////////////////////////////////////////////
//  平面生成
class ScCmdPlane : public ScCommand{
public:
	ScCmdPlane(int nType);

	enum{
		eTwoPoint,//两点
		eThreePoint,//三点
	};

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

protected:
	enum{S_POINT1,S_POINT2,S_POINT3};

	TopoDS_Shape			BuildPlane(const CPoint& point);

protected:
	int						m_nPlnType;
	gp_Pnt					m_pnt1,m_pnt2;
};

/////////////////////////////////////////////////////
// 管道面，使用GeomFill_Pipe类
class ScCmdSurfPipe : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//接收函数
	//接收半径
	virtual   int			AcceptInput(double dVal);

protected:
	enum{S_CURVE,S_RADIUS};
};


//////////////////////////////////////////////////////
// bsplinecurves生成的surface。
// 使用类：GeomFill_BSplineCurves
class ScCmdSurfBSplineCurves : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//接收整数
	virtual   int			AcceptInput(int nVal);
	
protected:
	enum{S_CURVES,S_TYPE};

	Handle(Geom_BSplineCurve)	m_aC1,m_aC2,m_aC3,m_aC4;
	int							m_nCnt;

};

//////////////////////////////////////////////////////
// 曲面延伸一定的长度。
// 使用：GeomLib::ExtendSurfByLength方法。
class ScCmdSurfExtent : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//接收延伸长度
	virtual   int			AcceptInput(double dVal);

protected:
	enum{S_SURF,S_UV,S_CONT,S_LEN};

	BOOL					m_bU;
	BOOL					m_bAfter;
	int						m_nCont;//连续性
	Handle(Geom_BoundedSurface) m_aSurf;
};

//////////////////////////////////////////////////////////
// 对face，获取裁剪曲面的原始曲面
class ScCmdRawSurface : public ScCmdFaceBase{
public:

protected:
	virtual BOOL			BuildShape();//生成对应shape。
};

////////////////////////////////////////////////////////////
// 获取一个曲面，该曲面是Shape、Solid或Compound中的一个Face
class ScCmdExtractFace : public ScCmdFaceBase{
public:

protected:
	virtual BOOL			BuildShape();//生成对应shape。
};

////////////////////////////////////////////////////////////////
// 由多个平面环构成的裁剪平面。简单的方法，直接使用makeface构建。
//复杂的方法，需要判断环是否相交等，情况就很复杂了。
//先简单的情况
class ScCmdMultiLoopFace : public ScCommand{
public:
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_OUTWIRE,S_INWIRES};

		BOOL						BuildShape();//
};

//////////////////////////////////////////////////////////////////////////
// 使用曲面分割平面，使用BrepFeat_splitShape
//
class ScCmdSplitPlane : public ScCommand{
public:
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_FACE,S_CURVE};
		BOOL						HasSelFace();
		BOOL						DoSplit();

		gp_Pln					m_pln;
};

//////////////////////////////////////////////////////////////////////////
// 使用曲面分割曲面，使用BRepFeat_splitShape
class ScCmdSplitSurf : public ScCommand{
public:
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_FACE,S_TOOL};
		BOOL						HasSelFace();
		BOOL						HasSelTool();
		BOOL						DoSplit();

};