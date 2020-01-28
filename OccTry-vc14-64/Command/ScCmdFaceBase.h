#pragma once

#include "ScCommand.h"

///////////////////////////////////////////////////////////////////////
/*
*  和选择一个Face相关命令的记录。提供了选择一个face的功能，并针对选择
*  的face进行简单的操作。
*  face可以选择单独的曲面，也可以是solid、shell等中的一个face。
*/
class ScCmdFaceBase : public ScCommand
{
public:
	ScCmdFaceBase(void);
	~ScCmdFaceBase(void);

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	//一些公共的实现。
	virtual BOOL			HasSelFace();//是否选择了face
	virtual BOOL			HasSelShape();//是否选择了shape.
	virtual BOOL			BuildShape();//生成对应shape。
	virtual BOOL			BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace);

	virtual BOOL			OpenFaceLocalCtx();
	virtual BOOL			CloseFaceLocalCtx();

protected:
	enum{S_SHAPE,S_FACE}; //选择对象，选择曲面。

	TopoDS_Face				m_aFace;//选中的Face。
	BOOL					m_bLocalCtx;
};
