#pragma once

#include "ScCommand.h"
#include "ScCmdCurveBase.h"

class ScCmdBSpline
{
public:
	ScCmdBSpline(void);
	~ScCmdBSpline(void);
};

//////////////////////////////////////////////////////////////////////////
// 工具类,提供一些基本的功能
class TcBSplLib{
public:

	//增加曲线的控制点到一定数目
	static	BOOL			IncCurveCtrlPnts(Handle(Geom_BSplineCurve) &aCur,int ncpts,
								double df,double dl);

	//增加曲面的控制点到一定数目
	static  BOOL			IncSurfCtrlPnts(Handle(Geom_BSplineSurface) &aSurf,int nucpts,int nvcpts);
};

///////////////////////////////////////////////////////////////////////////
// 曲线转为BSpline
class ScCmdCurveToBSpline : public ScCmdCurveBase{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

protected:
	BOOL					DoConvert();
};

//////////////////////////////////////////////////////////////////////////
// 曲面转为BSpline
class ScCmdSurfToBSpline : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

protected:
	BOOL					HasSelSurf();
	BOOL					DoConvert();
};

//////////////////////////////////////////////////////////////////////////
// 实体转为BSpline
class ScCmdSolidToBSpline : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);


protected:
	BOOL					HasSelSolid();
	BOOL					DoConvert();
};

//////////////////////////////////////////////////////////////////////////
// 曲面控制点编辑

//bezier曲线的控制点编辑
class ScCmdBezierCurveEditByCP : public ScCommand{
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
	enum{S_CURVE,S_SELPNT,S_CHGPNT};

	BOOL					HasSelCurve();
	BOOL					ShowCtrlPnts();
	BOOL					DoEdit(BOOL bTmp,BOOL bDone = FALSE);

	gp_Pnt					m_selPnt;//选中点
	gp_Pnt					m_chgPnt;//修改后的点
	Handle(Geom_BezierCurve)	m_midCur,m_tmpCur;
};

class ScCmdBSplSurfEditByCP : public ScCommand{
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
	enum{S_SURF,S_SELPNT,S_BASPNT,S_CHGPNT};

	BOOL					HasSelSurf();
	BOOL					ShowControlPnts(BOOL bSel);
	BOOL					ChangeSurf(BOOL bTmp);

	BOOL					InChange(const gp_Pnt& pnt);

	TopTools_SequenceOfShape		m_seqOfV;	//选中的点
	Handle(Geom_BSplineSurface)		m_splSurf,m_midSurf;//临时的曲面和中间的曲面
	
	gp_Pnt					m_basPnt,m_chgPnt;
	BOOL					m_bChged;
};

//////////////////////////////////////////////////////////////////////////
// 升阶处理

//////////////////////////////////////////////////////////////////////////
// bezier曲线升阶

//////////////////////////////////////////////////////////////////////////
// BSpline曲线升阶
class ScCmdBSplCurveIncDegree : public ScCmdCurveBase{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_OBJ,S_DEGREE};

	BOOL					HasSelObj();
	BOOL					DoIncDegree(int nDegree);

	int						m_nOldDegree;
};

//////////////////////////////////////////////////////////////////////////
// bspline曲面升阶
class ScCmdBSplSurfIncDegree : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_OBJ,S_UDEGREE,S_VDEGREE};

	BOOL					HasSelObj();
	BOOL					DoIncDegree();

	int						m_nOldUDegree,m_nOldVDegree;
	int						m_nUDegree,m_nVDegree;
};

//////////////////////////////////////////////////////////////////////////
// 插入结点
class ScCmdBSplCurveInsertKnots : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_CURVE,S_INSERT};

	BOOL					HasSelCurve();
	BOOL					ShowCtrlPnts();
	BOOL					DoInsert(BOOL bDone);

	Handle(Geom_BSplineCurve)	m_cpyCur;//拷贝的工作曲线
	double					m_df,m_dl;
	int						m_nFlag;
	gp_Pnt					m_newPt;
};

class ScCmdBSplSurfInsertKnots : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_SURF,S_FLAG,S_INSERT};

	BOOL					HasSelSurf();
	BOOL					ShowCtrlPnts();
	BOOL					DoInsert(BOOL bDone);

	Handle(Geom_BSplineSurface)		m_cpySurf;
	
	int						m_nFlag;
	gp_Pnt					m_clkPnt;

};

//////////////////////////////////////////////////////////////////////////
// 增加控制点
class ScCmdBSplCurveAddCtrlPnts : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_CURVE,S_INPUT};

	BOOL					HasSelCurve();
	void					ShowCtrlPnts();
	BOOL					DoAddCtrlPnts(BOOL bDone);

	Handle(Geom_BSplineCurve)	m_cpyCur,m_tmpCur;//拷贝的工作曲线
	double					m_df,m_dl;
	int						m_ncpts;//新控制点的个数
};

class ScCmdBSplSurfAddCtrlPnts : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_SURF,S_UINPUT,S_VINPUT};

	BOOL					HasSelSurf();
	void					ShowCtrlPnts();
	BOOL					DoAddCtrlPnts(BOOL bDone);

	Handle(Geom_BSplineSurface)		m_cpySurf,m_tmpSurf;
	int						m_nucpts,m_nvcpts;

};

//////////////////////////////////////////////////////////////////////////
// 测试一个或多个曲面插入控制点后的情形，重点观察边界处的情况。
class ScCmdBSplCtrlPntsTest : public ScCommand{
public:

	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_OBJ,S_UPNTS,S_VPNTS};

	BOOL					HasSelObj();
	void					ShowCtrlPnts(const TopoDS_Shape& aShape);
	BOOL					DoInsert();

	int						m_nUPnts,m_nVPnts;
};

//////////////////////////////////////////////////////////////////////////
// 组合曲线转换为一条BSpline曲线
// 可以从几个edge转,从edge和wire混合转,从wire转等.支持多种功能.
class ScCmdCurveConcatToSpline : public ScCommand{
public:
	
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

protected:
	enum{S_CURVE,S_CONCAT};

	BOOL					DoConcat();
};