//
// 使用plate相关的算法：点云生成面、过渡面、有约束的过渡面、补洞
//
#pragma once

#include "ScCommand.h"

class ScCmdPlate : public ScCommand
{
public:
	ScCmdPlate(void);
	~ScCmdPlate(void);

	//启动和结束命令
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();
};

/////////////////////////////////////////////////////////////
// 从点云生成曲面
class ScCmdPlatePointCloud : public ScCmdPlate{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

public:
	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	TopoDS_Shape			BuildSurface(const TColgp_SequenceOfXYZ& seqOfXYZ);
};

class ScCmdPlateBlend : public ScCmdPlate{
public:
	ScCmdPlateBlend(BOOL bWithCont);

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

protected:
	enum{S_SHAPE1,S_EDGE1,S_SHAPE2,S_EDGE2,S_CONT};

	TopoDS_Face				BuildBlendFace();

protected:
	TopoDS_Face			m_aFace1;
	TopoDS_Edge			m_aEdge1;
	TopoDS_Face			m_aFace2;
	TopoDS_Edge			m_aEdge2;//选择的face和edge。
	TopoDS_Edge			m_aContEdge;//限制

	BOOL				m_bWithCont; //具有约束
};

class ScCmdPlateBlendWithCont : public ScCmdPlate{
public:

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
};

////////////////////////////////////////////////////////////
// N边补洞算法。
//
class ScCmdPlateFillHole : public ScCmdPlate{
public:
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

protected:
	enum{S_SHAPE,S_EDGES};

	TopoDS_Face				FillHole();

protected:
	Handle(AIS_Shape)		m_aShapeObj;

};