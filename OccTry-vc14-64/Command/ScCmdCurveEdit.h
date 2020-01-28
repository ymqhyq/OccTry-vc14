#pragma once

#include <vector>
#include "ScCommand.h"
#include "ScCmdCurveBase.h"

class ScCmdCurveEdit
{
public:
		ScCmdCurveEdit(void);
		~ScCmdCurveEdit(void);
};

//////////////////////////////////////////////////////////////////////////
// 曲线被点分割
class ScCmdCurveSplitByPoint : public ScCmdCurveBase{
public:
		//启动和结束命令
		virtual	  int				Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int				End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

		//鼠标移动的响应函数
		virtual	  void			MoveEvent(const CPoint& point,
												const Handle(V3d_View)& aView);

protected:
		enum{S_CURVE,S_POINT};

		BOOL						DoSplit();

		double						m_t;
		gp_Pnt					m_ptClk;//点击点距离曲线上最近的点。
		BOOL						m_bSelPnt;
};

//////////////////////////////////////////////////////////////////////////
// 曲线被曲线分割
class ScCmdCurveSplitByCurve : public ScCmdCurveBase{
public:
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);


protected:
		enum{S_CURVE,S_TOOL};

		BOOL						DoSplit();
};

////////////////////////////////////////////////////////////////////////////
// 曲线裁剪，交点分割曲线
class ScCmdCurveTrimSplit : public ScCmdCurveBase{
public:
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_CURVE1,S_CURVE2};

		BOOL						DoSplit();
};

////////////////////////////////////////////////////////////////////////////
// 曲线裁剪，快速裁剪
class ScCmdCurveQuickTrim : public ScCmdCurveBase{
public:
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_CURVE1,S_CURVE2,S_TRIM};

		BOOL						DoSplit();
		BOOL						DoTrim(int nx,int ny);
		BOOL						DoTrimOver();

		BOOL						HasDel(std::vector<int>&  arx,int ix);//是否ix已经删除

		TopTools_ListOfShape							m_shapeList1,m_shapeList2;
		TopTools_DataMapOfShapeInteger	m_shapeMap;
		std::vector<int>										m_arIdx1,m_arIdx2;//记录删除对象的下标.
		int																m_nNewC;//新创建临时对象数。
};

////////////////////////////////////////////////////////////////////////////
// 
class ScCmdCurveFillet : public ScCmdCurveBase{
public:
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

		//接收浮点数
		virtual   int			AcceptInput(double dVal); 
protected:
		enum{S_WIRE,S_VERTEX,S_RADIUS};

		BOOL					HasClosedWire();
		BOOL					MakeFillet();

		TopoDS_Face				m_aFace;
		TopoDS_Vertex			m_aVertex;//要倒角的点
		double					m_dRadius;//倒角半径
};

////////////////////////////////////////////////////////////////////////////
//
class ScCmdCurveChamfer : public ScCommand{
public:
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);
};