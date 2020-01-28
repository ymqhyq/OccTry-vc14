#pragma once

#include "ScCommand.h"


//蒙面
class ScCmdLoft : public ScCommand{
public:
	ScCmdLoft(BOOL bSolid);

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

protected:
	enum{S_CURVES};

	TopoDS_Shape			BuildShape();

protected:
	BOOL					m_bSolid;//是否生成实体
	Handle(TopTools_HSequenceOfShape) m_aSeqOfShape;//记录edge或wire列表

};

//抽壳
class ScCmdThickSolid : public ScCommand{
public:
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
	enum{S_SHAPE,S_FACE,S_THICK};

	TopoDS_Shape			BuildNewShape();

protected:
	Handle(AIS_Shape)		m_aSelObj;
	TopoDS_Face				m_aFace;
	double					m_dThick;//抽壳厚度
};

class ScCmdOffset : public ScCommand{
};

////////////////////////////////////////////////////////////
// 偏移shape
// 使用BRepOffsetAPI_MakeOffsetShape类
//
class ScCmdOffsetShape : public ScCommand{
public:
	ScCmdOffsetShape(BOOL bOnlySurf);

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//接收浮点数
	virtual   int			AcceptInput(double dVal);

protected:
	enum{S_SHAPE,S_OFFSET};

	TopoDS_Shape			BuildNewShape();

protected:
	Handle(AIS_Shape)		m_aSelObj;
	Handle(AIS_Shape)		m_aNewObj;
	double					m_dOffset;

	BOOL					m_bOnlySurf;//是否只偏移曲面
};

class ScCmdPipe : public ScCommand{
};

class ScCmdPipeShell : public ScCmdPipe{
};

//
// 扫掠算法。有多种类型，每种的处理流程稍有区别。
// 基本上：a、选择脊线 b、选择多条曲线 
//         如果需要，可能需要选择一条导轨。
//
class ScCmdBRepSweep : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SPINE,S_PROFILES,S_GUIDE};

	TopoDS_Shape			BuildNewShape();

protected:
	Handle(AIS_Shape)			m_aNewObj;
	Handle(AIS_InteractiveObject)	m_aSpineObj;
	AIS_SequenceOfInteractive	m_seqOfObj;
	TopoDS_Wire					m_awSpine;//脊线
	TopoDS_Wire					m_awGuide;//导轨
	int							m_nType;
	gp_Ax2						m_ax2;
	gp_Vec						m_biNorm;
};
