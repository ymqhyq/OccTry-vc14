#pragma once

#include "ScCommand.h"

//class ScCmdPoint
//{
//public:
//	ScCmdPoint(void);
//	~ScCmdPoint(void);
//};

//////////////////////////////////////////////////////////////////////////
// 点投影到曲线
class ScCmdPointPrjToCurve : public ScCommand{
public:

	//启动命令，开始执行。
	virtual	  int					Begin(Handle(AIS_InteractiveContext) aCtx);
	//结束命令，通常用于终止命令。
	virtual   int					End();

	//点选方式下的消息响应函数
	virtual	  void					InputEvent(const CPoint& point,int nState,//上述minput定义
		const Handle(V3d_View)& aView);

protected:
	enum{S_CURVE,S_POINT};

	BOOL							HasSelCurve();
	BOOL							DoPrj(BOOL bTmp);

	gp_Pnt							m_selPnt;
};