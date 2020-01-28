#pragma once

#include "ScCommand.h"

class ScCmdUcs
{
public:
	ScCmdUcs(void);
	~ScCmdUcs(void);
};

class ScCmdUcsPrev : public ScCommand{
public:
	ScCmdUcsPrev();
	~ScCmdUcsPrev();

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

};

class ScCmdUcsNext : public ScCommand{
public:
	ScCmdUcsNext();
	~ScCmdUcsNext();

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();
};

class ScCmdUcsModel : public ScCommand{
public:
	ScCmdUcsModel();
	~ScCmdUcsModel();

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();
};

class ScCmdUcsOrg : public ScCommand{
public:
	ScCmdUcsOrg();
	~ScCmdUcsOrg();

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
	virtual   int			AcceptInput(gp_Pnt& pnt);

protected:
	Handle(AIS_Trihedron)		m_aShape;
};

class ScCmdUcsHeight : public ScCommand{
public:
	ScCmdUcsHeight();
	~ScCmdUcsHeight();

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
	virtual   int			AcceptInput(double dVal);

protected:
	int						MakeNewUCS(double dVal);
	gp_Ax2					BuildNewAx2(double dVal);
	double					ComputeHeight(const gp_Pnt& pnt);

protected:
	Handle(AIS_Trihedron)		m_aShape;
};

class ScCmdUcs3Point : public ScCommand{
public:
	ScCmdUcs3Point();
	~ScCmdUcs3Point();

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
	virtual   int			AcceptInput(gp_Pnt& pnt);

protected:
	void					DoAcceptPoint(const gp_Pnt& pnt);

protected:
	enum{ U3P_ORG,U3P_ZPOINT,U3P_UPOINT};

	Handle(AIS_Shape)			m_aShape;
	Handle(AIS_Trihedron)		m_aTriShape;

	gp_Pnt						m_ptOrg,m_ptZ,m_ptU;
};

//////////////////////////////////////////////////////////////
// 一个简单的基类
class ScCmdUcsObjPoint : public ScCommand{
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
	enum{S_SHAPE,S_POINT};

	//是否需要的对象
	virtual BOOL			IsRequiredShape(const TopoDS_Shape& aShape);
	//根据条件构造对应的坐标系
	virtual BOOL			BuildAxis(gp_Ax2& ax2);

	virtual void			ShowTrihedron(const gp_Ax2& ax2);
	virtual void			BuildTrihedron(const gp_Ax2& ax2);

protected:
	Handle(AIS_Trihedron)	m_aTriShape;
	Handle(AIS_InteractiveObject)	m_aSelObj;
	TopoDS_Shape			m_aShape;
	int						m_nX,m_nY;

	Handle(V3d_View)		m_aView;
};

//基于曲面上的点
class ScCmdUcsCurvePoint : public ScCmdUcsObjPoint{
public:
	
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);

protected:
	//是否需要的对象
	virtual BOOL			IsRequiredShape(const TopoDS_Shape& aShape);
	//根据条件构造对应的坐标系
	virtual BOOL			BuildAxis(gp_Ax2& ax2);
};

//基于曲面上的点
class ScCmdUcsFacePoint : public ScCmdUcsObjPoint{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);

protected:
	//是否需要的对象
	virtual BOOL			IsRequiredShape(const TopoDS_Shape& aShape);
	//根据条件构造对应的坐标系
	virtual BOOL			BuildAxis(gp_Ax2& ax2);
};

//基于shape上的点
class ScCmdUcsShapePoint : public ScCmdUcsObjPoint{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);

protected:
	//是否需要的对象
	virtual BOOL			IsRequiredShape(const TopoDS_Shape& aShape);
	//根据条件构造对应的坐标系
	virtual BOOL			BuildAxis(gp_Ax2& ax2);
};