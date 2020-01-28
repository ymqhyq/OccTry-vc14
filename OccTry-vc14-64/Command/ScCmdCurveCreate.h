/*
* 基本曲线的创建命令.争取提供比较完善的曲线创建.
*
*
*
*/
#pragma once

#include "ScCommand.h"

class ScCmdCurveCreate
{
public:
	ScCmdCurveCreate(void);
	~ScCmdCurveCreate(void);
};

//直线
class ScCmdLine : public ScCommand{
public:
	ScCmdLine();
	~ScCmdLine();

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
	//内部状态
	enum{ LINE_BEGIN,LINE_POINT1,LINE_POINT2,LINE_END};

private:
	Handle(AIS_Shape)		m_aAIShape;//对象，也是临时对象
	gp_Pnt					m_pnt1;//第一个点
};

//多段线
class ScCmdPolyLine : public ScCommand{
public:
	ScCmdPolyLine();
	~ScCmdPolyLine();

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
	//内部状态
	enum{ LINE_BEGIN,LINE_POINT_FIRST,LINE_POINT_NEXT,LINE_END};

	virtual  TopoDS_Shape	BuildNewShape();

protected:
	Handle(AIS_Shape)		m_aAIShape;//对象，也是临时对象
	Handle(AIS_Shape)		m_aTmpObj;//临时对象
	gp_Pnt					m_pntPrev;//前一个点
	TColgp_SequenceOfPnt	m_seqOfPnt;//点列
//	TopTools_ListOfShape    m_shapeList;
};

//圆
class ScCmdCircle : public ScCommand{
public:
	ScCmdCircle();
	~ScCmdCircle();

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
	enum{ CIRCLE_CENTER,CIRCLE_RADIUS };

protected:
	Handle(AIS_Shape)		m_aShape;
	gp_Pnt					m_ptCenter;
	double					m_dRadius;
	BOOL					m_bFirst;
};

////////////////////////////////////////////////////
// 三点圆
class ScCmdCircleOf3Point : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//接收3维点
	virtual   int			AcceptInput(gp_Pnt& pnt) ;

protected:
	enum{S_POINT1,S_POINT2,S_POINT3};

	BOOL							MakeTmpLine();
	BOOL							MakeCirc(BOOL bDone);

	gp_Pnt					m_pnt1,m_pnt2,m_pnt3;
	
};

// 同心圆/距离
class ScCmdCircleByDist : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(double dVal) { return SC_OK; }
	
protected:
	enum{S_CIRCLE,S_DIST};

	double					m_dist;
};
// 同心圆/过点
class ScCmdCircleByPoint : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(gp_Pnt& pnt) { return SC_OK; }
	
protected:
	enum{S_CIRCLE,S_DIST};

	gp_Pnt					m_pnt;
};

//圆弧，圆心\半径\角度
class ScCmdArc : public ScCommand{
public:
	ScCmdArc();
	~ScCmdArc();

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
	enum{ARC_CENTER,ARC_RADIUS,ARC_ANGLE};

	TopoDS_Edge				BuildLine();
	TopoDS_Shape			BuildArc();
	
protected:
	Handle(AIS_Shape)		m_aShape;
	gp_Pnt					m_ptC,m_pt1,m_pt2;
};

////////////////////////////////////////////////////
// 圆心/两点
class ScCmdArcOfTwoPoint : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(gp_Pnt& pnt) ;

protected:
	enum{S_CENTER,S_POINT1,S_POINT2};

	BOOL							MakeLine();
	BOOL							MakeArc(BOOL bDone);

	gp_Pnt						m_ptC,m_pt1,m_pt2;
};

////////////////////////////////////////////////////
// 三点,前两点决定了圆弧的范围.
class ScCmdArcOfThreePoint : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(gp_Pnt& pnt) { return SC_OK; }

protected:
	enum{S_POINT1,S_POINT2,S_POINT3};

};
////////////////////////////////////////////////////
// 圆\起点\角度
class ScCmdArcOfPointAndAngle : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(gp_Pnt& pnt) { return SC_OK; }

	//接收浮点数
	virtual   int			AcceptInput(double dVal) { return SC_OK; }

protected:
	enum{S_CENER,S_POINT,S_ANGLE};

};

////////////////////////////////////////////////////
// 椭圆对角
class ScCmdEllipseOfCorner : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(gp_Pnt& pnt) ;

protected:
	enum{S_POINT1,S_POINT2};

	//创建矩形成功
	BOOL					BuildShape(TopoDS_Shape& aShape);

	gp_Pnt					m_pnt1,m_pnt2;
};

////////////////////////////////////////////////////
// 椭圆中心/长/短轴
class ScCmdEllipseOfRedius : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(gp_Pnt& pnt) { return SC_OK; }

	//接收浮点数
	virtual   int			AcceptInput(double dVal) { return SC_OK; }
};

////////////////////////////////////////////////////
// 中心/两点
class ScCmdEllipseOfTwoPoint : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(gp_Pnt& pnt) { return SC_OK; }

	//接收浮点数
	virtual   int			AcceptInput(double dVal) { return SC_OK; }
};

////////////////////////////////////////////////
// 椭圆角度

////////////////////////////////////////////////
// 椭圆\点\角度

////////////////////////////////////////////////
// 椭圆\两点

////////////////////////////////////////////////
// 矩形
class ScCmdRectangle : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(gp_Pnt& pnt) ;

protected:
	enum{S_POINT1,S_POINT2};

	//创建矩形成功
	BOOL					BuildRectangle(TopoDS_Shape& aShape);

	gp_Pnt					m_pnt1,m_pnt2;
};

//样条曲线
class ScCmdSpline : public ScCommand{
public:
	ScCmdSpline();
	~ScCmdSpline();

	enum{BSpline_Interp,BSpline_Approx};//拟合、逼近
	void					SetSplType(int nType);

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
	//生成样条
	Handle(Geom_BSplineCurve)	BuildBSpline();

protected:
	enum{SPL_FIRST_POINT,SPL_NEXT_POINT};

	int						m_nType;//类型
	TColgp_SequenceOfPnt	m_seqOfPnt;//点列
	AIS_SequenceOfInteractive m_seqOfObj;//点显示对象
	Handle(AIS_Shape)		m_aShape;//临时对象
};


//////////////////////////////////////////////////////
// 两点矩形的创建

//////////////////////////////////////////////////////
// 圆的创建,种类比较多

//////////////////////////////////////////////////////
// 圆弧的创建

//////////////////////////////////////////////////////
// 椭圆的创建

//////////////////////////////////////////////////////
// 椭圆弧的创建

//////////////////////////////////////////////////////////////////////////
// Bezier曲线的创建
class ScCmdBezierCurve : public ScCommand{
public:
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
	enum{S_FIRST_PNT,S_NEXT_PNT};

	BOOL					MakeCurve(BOOL bTmp);
	BOOL					ShowFrame();

	TColgp_SequenceOfPnt		m_seqOfPnt;//点列
	AIS_SequenceOfInteractive	m_seqOfObj;//点显示对象
};


