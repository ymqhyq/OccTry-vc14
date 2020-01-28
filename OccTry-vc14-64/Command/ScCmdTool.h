#pragma once

#include "ScCommand.h"
#include "ScCmdCurve.h"
#include "ScCmdFaceBase.h"

class ScCmdTool
{
public:
	ScCmdTool(void);
	~ScCmdTool(void);
};

/////////////////////////////////////////////////////////
// 获取Face的裁剪环信息
//
class ScCmdFaceTrmLoopInfo : public ScCommand{
public:
	//启动命令，开始执行。
	virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
	//结束命令，通常用于终止命令。
	virtual   int						End();

	//点选方式下的消息响应函数
	virtual	  void						InputEvent(const CPoint& point,int nState,//上述minput定义
													const Handle(V3d_View)& aView);

protected:
	enum{S_SHAPE,S_INFO};

	BOOL						HasSelShape();

	TopoDS_Face					m_aFace;

};

//////////////////////////////////////////////////////////
// 对face，由3d曲线生成新的参数曲线。
// 再3d曲线和参数曲线不匹配的时候。
class ScCmdToolCurve2d : public ScCommand{
public:
	//启动命令，开始执行。
	virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
	//结束命令，通常用于终止命令。
	virtual   int						End();

	//点选方式下的消息响应函数
	virtual	  void						InputEvent(const CPoint& point,int nState,//上述minput定义
													const Handle(V3d_View)& aView);

protected:
	enum{S_SHAPE};

	BOOL						HasSelShape();
	void						BuildCurve2d();
};



///////////////////////////////////////////////////////////
// 曲面边界的切矢显示。
class ScCmdFaceBndTang : public ScCmdFaceBase{
public:
	//启动命令，开始执行。
	virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
	//结束命令，通常用于终止命令。
	virtual   int						End();

protected:
	virtual	  BOOL						BuildShape();//生成对应shape。

	BOOL								m_bBuilded;//是否创建了
};

//////////////////////////////////////////////////////////
// 曲面的法矢
class ScCmdFaceNormal : public ScCmdFaceBase{
public:
	//启动命令，开始执行。
	virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
	//结束命令，通常用于终止命令。
	virtual   int						End();

protected:
	virtual	  BOOL						BuildShape();//生成对应shape。

	BOOL								m_bBuilded;//是否创建了
};

//////////////////////////////////////////////////////////
// 曲线的法矢的显示
class ScCmdCurveTang : public ScCommand{
public:
		//启动命令，开始执行。
		virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
		//结束命令，通常用于终止命令。
		virtual   int						End();

		//点选方式下的消息响应函数
		virtual	  void						InputEvent(const CPoint& point,int nState,//上述minput定义
				const Handle(V3d_View)& aView);

protected:
		enum{S_SELCURVE};
		BOOL										HasSelCurve();
		virtual	  BOOL						BuildShape();//生成对应shape。

		BOOL								m_bBuilded;//是否创建了
};

/////////////////////////////////////////////////////////////////
// 对象检查的基类
class ScCmdCheckBase : public ScCommand{
public:
		//启动命令，开始执行。
		virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
		//结束命令，通常用于终止命令。
		virtual   int						End();

		//点选方式下的消息响应函数
		virtual	  void						InputEvent(const CPoint& point,int nState,//上述minput定义
				const Handle(V3d_View)& aView);

protected:
		BOOL										HasSelObject();
		virtual     BOOL						Check();
};

class ScCmdCheckValid : public ScCmdCheckBase{
public:
protected:
		virtual     BOOL						Check();
};

class ScCmdCheckTopoValid : public ScCmdCheckBase{
public:
protected:
		virtual     BOOL						Check();
};

class ScCmdCheckClosed : public ScCmdCheckBase{
public:
protected:
		virtual     BOOL						Check();
};