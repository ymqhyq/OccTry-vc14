#pragma once

#include "ScCommand.h"

class ScCmdTrsf
{
public:
	ScCmdTrsf(void);
	~ScCmdTrsf(void);
};

// 对象平移。
//
class ScCmdObjMoving : public ScCommand{
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
	enum{S_SELECT,S_MOVING};

	TopoDS_Shape			BuildCopy(const CPoint& point);

protected:
	TopoDS_Shape			m_aSelObj;
	int						m_nX,m_nY;
	gp_Pnt					m_pntStart;
};

//对象clone.
class ScCmdObjClone : public ScCommand{
public:
	ScCmdObjClone(BOOL bMulti = FALSE);

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
	enum{S_SELECT,S_MOVING};

	TopoDS_Shape			BuildClone(const CPoint& point);

protected:
	TopoDS_Shape			m_aSelObj;
	int						m_nX,m_nY;
	gp_Pnt					m_pntStart;
	BOOL					m_bMulti;//是否重复clone。
};