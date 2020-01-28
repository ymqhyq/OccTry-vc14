#pragma once

#include <afxtempl.h>
#include "ScCommand.h"

class ScFltEdge{
public:
	ScFltEdge() {
		_r1 = _r2 = 0.0;
	}

	TopoDS_Edge			_edge;
	double				_r1,_r2;//两个半径
};

typedef CList<ScFltEdge *,ScFltEdge *> TFltEdgeList;

class ScCmdFillet : public ScCommand
{
public:
	ScCmdFillet(BOOL bConstRadius);
	~ScCmdFillet(void);

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(double dVal);

protected:
	enum{S_SHAPE,S_EDGE,S_R1,S_R2};

	void					RemoveList();
	TopoDS_Shape			BuildNewShape();

protected:
	BOOL					m_bConstRadius;
	Handle(AIS_Shape)		m_aShapeObj;
	TopoDS_Shape			m_aOldShape;//倒角前的对象
	TFltEdgeList			m_aEdgeList;
};
