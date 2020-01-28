#pragma once

#include "ScCommand.h"

class ScCmdBoolean
{
public:
		ScCmdBoolean(void);
		~ScCmdBoolean(void);
};

//boolean运算
class ScCmdBool : public ScCommand{
public:
		ScCmdBool();

		//启动和结束命令
		virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int	End();

public:
		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_SHAPE1,S_SHAPE2};

		BOOL						HasSelObj1();
		BOOL						HasSelObj2();

		virtual   void			MakeBoolean();
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

///////////////////////////////////////////////////////////////////////////////////////
// 新的bool算法,BRepAlgoAPI的算法
class ScCmdBoolCommon : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

class ScCmdBoolFuse : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

class ScCmdBoolCut : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

///////////////////////////////////////////////////////////////////////////////////////
// 旧的bool算法,BRepAlgo的算法
class ScCmdBoolCommonOld : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

class ScCmdBoolFuseOld : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

class ScCmdBoolCutOld : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

//////////////////////////////////////////////////////////////////////////////////////
// 交运算的中间结果
class ScCmdBoolCommonSections : public ScCmdBool{
public:
protected:
		virtual   void			MakeBoolean();
};

class ScCmdBoolCommonGenObjs : public ScCmdBool{
public:
		ScCmdBoolCommonGenObjs(BOOL bFirst);

protected:
		virtual   void			MakeBoolean();

		BOOL						m_bFirst;
};
