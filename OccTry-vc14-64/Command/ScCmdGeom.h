#pragma once

#include "ScCommand.h"

class ScCmdGeom
{
public:
	ScCmdGeom(void);
	~ScCmdGeom(void);
};

//////////////////////////////////////////////
// 单轨扫掠。
// 使用类GeomFill_Pipe
class ScCmdSweepOnePath : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void						InputEvent(const CPoint& point,int nState,//上述minput定义
													const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int						AcceptInput(double dVal);
	
protected:
	enum{S_PATH,S_CURVE,S_TYPE};

	TopoDS_Shape			BuildNewShape();

	int						m_nType;
	int						m_nC;
};

///////////////////////////////////////////////////
// 显示曲线的曲率圆
// 使用GeomLProp_CLProps类
class ScCmdCurveCurvatureCirc : public ScCommand{
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
	enum{S_CURVE,S_CIRC};

	TopoDS_Shape			MakeCurvatureCirc(const CPoint& point);

	TopoDS_Shape			m_aShape;//选中的曲线，可为edge或wire。
};

////////////////////////////////////////////////////////
// 曲面的流线功能
// 使用Adaptor3d_IsoCurve和相关功能
// 可能还需要进一步完善，目前显示完整曲线，而非裁剪后的曲线
class ScCmdIsoCurve : public ScCommand{
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

	//接收整数
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_SURF,S_UV,S_CURVE};

	void					MakeIsoCurve(BOOL bDone,const CPoint& point,const Handle(V3d_View)& aView);
	TopoDS_Edge				GetIsoCurve(const Adaptor3d_IsoCurve& isoC);

	int						m_nUV;//0 :U 1: V 2:UV
};

///////////////////////////////////////////////////////
// bspline curve的控制点编辑功能
// 使用基本的bspline的功能
// 考虑对trimmed的curve的处理。最好转换成完整的curve。方便编辑。
class ScCmdBSplCurveEdit : public ScCommand{//简单的基类
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
	BOOL					GetBSplineCurve(const TopoDS_Shape& aS);
	virtual   void			ShowControlPoints();

	//选择点
	virtual   BOOL			SelectPoint(const CPoint& point);
	//改变点
	virtual   BOOL			OnChangePoint(const CPoint& point);
	//
	virtual   BOOL			OnPointChanged(const CPoint& point);

protected:
	enum{S_CURVE,S_POINT,S_MOVING};

	BOOL					m_bChanged;
	Handle(Geom_BSplineCurve)		m_bsplCur;//bspline曲线

	CString					m_szPrompt1;
};

class ScCmdBSplCurveEditByCtrlPnt : public ScCmdBSplCurveEdit{
public:

protected:
	virtual   void			ShowControlPoints();

	//选择点
	virtual   BOOL			SelectPoint(const CPoint& point);
	//改变点
	virtual   BOOL			OnChangePoint(const CPoint& point);
//
	virtual   BOOL			OnPointChanged(const CPoint& point);

protected:
	int						m_nPoleIdx;
};

///////////////////////////////////////////////////////
// bspline curve的曲线上点编辑功能
// 使用基本的bspline的MovePoint功能
class ScCmdBSplCurveEditByCurvPnt : public ScCmdBSplCurveEdit{
public:

protected:
	virtual   void			ShowControlPoints();
	//选择点
	virtual   BOOL			SelectPoint(const CPoint& point);
	//改变点
	virtual   BOOL			OnChangePoint(const CPoint& point);
//
	virtual   BOOL			OnPointChanged(const CPoint& point);

	void					PoleRange(int& i1,int& i2);

protected:
	double					m_t;
	gp_Pnt					m_pickPnt;
};