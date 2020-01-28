#pragma once

#include <AIS_Shape.hxx>
#include "ScCommand.h"

class ScCmdBRepPrim : public ScCommand
{
public:
	ScCmdBRepPrim(void);
	virtual ~ScCmdBRepPrim(void);

	//启动和结束命令
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();
};

class ScCmdSphere : public ScCmdBRepPrim
{
public:
	ScCmdSphere();
	~ScCmdSphere();

	//启动和结束命令
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

	//接收浮点数
	virtual   int		AcceptInput(double dVal); 
	//接收3维点
	virtual   int		AcceptInput(gp_Pnt& pnt); 


public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

protected:
	//四个状态
	enum { SPH_BEGIN,SPH_GET_CENTER,SPH_GET_RADIUS,SPH_END };

	gp_Pnt				m_ptCenter; //球心
	double				m_dRadius;  //半径

private:
	BOOL				m_bFirst; //是否初次
	Handle(AIS_Shape)	m_aShape; //临时的球
};

class ScCmdBox : public ScCmdBRepPrim{
public:
	ScCmdBox();
	~ScCmdBox();

	//启动和结束命令
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);
	//接收3维点
	virtual   int			AcceptInput(gp_Pnt& pnt); 

protected:
	enum{BOX_POINT1,BOX_POINT2,BOX_HEIGHT};

	int						DoAcceptPoint(gp_Pnt& pnt);
	TopoDS_Face				BuildBottom();
	TopoDS_Shape			BuildBox();

protected:
	Handle(AIS_Shape)		m_aShape;//临时对象。
	gp_Pnt					m_pnt1,m_pnt2;
	double					m_dH;//高度

};

class ScCmdCyl : public ScCmdBRepPrim{
public:
	ScCmdCyl(BOOL bWithAngle = FALSE);
	~ScCmdCyl();

	//启动和结束命令
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);
	//接收3维点
	virtual   int			AcceptInput(gp_Pnt& pnt); 

protected:
	enum{CYL_CENTER,CYL_RADIUS,CYL_ANGLE,CYL_HEIGHT};

	TopoDS_Shape			BuildBottom();
	TopoDS_Shape			BuildCyl();
	double					ComputeAngle();

protected:
	Handle(AIS_Shape)		m_aShape,m_aShape1;
	gp_Pnt					m_ptC,m_pt1,m_pt2,m_pt3;

	BOOL					m_bWithAngle;
};

class ScCmdCone : public ScCmdBRepPrim{
public:
	ScCmdCone();
	~ScCmdCone();

	//启动和结束命令
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);
	//接收3维点
	virtual   int			AcceptInput(gp_Pnt& pnt); 

protected:
	enum{CON_CENTER,CON_RADIUS,CON_ANGLE,CON_RADIUS2};

protected:
	Handle(AIS_Shape)		m_aShape;
	gp_Pnt					m_ptC,m_pt1,m_pt2,m_pt3;

};

class ScCmdTorus : public ScCmdBRepPrim{
};

/////////////////////////////////////////////////////////
// 拉伸操作。曲线拉伸成面，面拉伸成体。
// 使用BRepPrimAPI_MakePrism类。
//
class ScCmdPrism : public ScCommand{
public:
	ScCmdPrism(BOOL bTwoDir = FALSE,BOOL bGenSurf = FALSE);
	~ScCmdPrism();

	//启动和结束命令
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);
	//接收3维点
	virtual   int			AcceptInput(gp_Pnt& pnt); 

protected:
	enum{S_GET_SHAPE,S_GET_DIR_P1,S_GET_DIR_P2};

	BOOL					CanPrism(const TopoDS_Shape& aShape);
	TopoDS_Shape			BuildPrism();

protected:
	Handle(AIS_Shape)		m_aShape;
	TopoDS_Shape			m_aSelShape;
	gp_Vec					m_aVec;//拉伸方向
	gp_Pnt					m_pnt1;

	BOOL					m_bTwoDir;//是否双向拉伸
	BOOL					m_bGenSurf;//是否生成曲面，TRUE：生成曲面，FALSE：生成体。
};

//////////////////////////////////////////////////////////
// 曲线、曲面旋转
// 使用了BRepPrimAPI_MakeRevol类。
class ScCmdRevol : public ScCommand{
public:
	ScCmdRevol(BOOL bGenSolid);

	//启动和结束命令
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);
	//接收3维点
	virtual   int			AcceptInput(gp_Pnt& pnt); 
	//接收浮点数
	virtual   int			AcceptInput(double dVal);

protected:
	enum{S_PROFILE,S_POINT,S_DIR,S_ANGLE};

	BOOL					CanRevol(const TopoDS_Shape& aShape);
	TopoDS_Shape			BuildRevol();

	BOOL					CalcRevolBeginPoint(gp_Pnt& oPnt);
	BOOL					PrjToPlane(gp_Pnt& pnt);

protected:
	Handle(AIS_Shape)		m_aShape;
	TopoDS_Shape			m_aProfile;
	gp_Pnt					m_pnt,m_pnt2,m_pnt3;//点
	gp_Ax2					m_ax2;
	gp_Dir					m_dir;//方向
	double					m_dAngle;

	BOOL					m_bGenSolid;//是否生成Solid。
};



////////////////////////////////////////////////////////////
// 曲面裁剪实体
// 使用曲面生成半空间，运用bool运算进行裁剪。
class ScCmdSolidCut : public ScCommand{
public:

	//启动和结束命令
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SELFACE,S_SELOBJ,S_SELDIR};

protected:
	TopoDS_Shape		m_aSurfShape;
	TopoDS_Shape		m_aShape;
};

/////////////////////////////////////////////////////////////////
// 组合命令
// 将多个shape组合成compound

/////////////////////////////////////////////////////////////////
// 取消组合
// 将compound分离成单个的shape
class ScCmdDeCompound : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	BOOL					CanDeCompound();
	void					DeCompound();
};