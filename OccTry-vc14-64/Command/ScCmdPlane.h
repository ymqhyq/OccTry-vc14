#pragma once

#include "ScCommand.h"

//class ScCmdPlane
//{
//public:
//		ScCmdPlane(void);
//		~ScCmdPlane(void);
//};

//////////////////////////////////////////////////////////////////////////
// 从wire和curve生成的平面
class ScCmdPlaneFromWire : public ScCommand{
public:
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		BOOL						HasSelCurve();
		BOOL						MakePlane();
};