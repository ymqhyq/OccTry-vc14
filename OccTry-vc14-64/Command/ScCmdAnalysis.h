#pragma once

#include "ScCommand.h"

class ScCmdAnalysis
{
public:
	ScCmdAnalysis(void);
	~ScCmdAnalysis(void);
};

//////////////////////////////////////////////////////////////////////////
// 曲线长度
class ScCmdCurveLength : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
								const Handle(V3d_View)& aView);

protected:
	enum{S_CURVE};

	BOOL					HasSelObj();
	BOOL					GetLength();
};