#pragma once

#include "ScCommand.h"

class ScCmdFace
{
public:
		ScCmdFace(void);
		~ScCmdFace(void);
};

/////////////////////////////////////////////////////////////////
// Face 修正
class ScCmdFaceFixAll : public ScCommand{
public:
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		BOOL						HasSelFace();
		virtual  BOOL						FixShape();
};

//////////////////////////////////////////////////////////////////////////
// 修正face的wire的缝隙
class ScCmdFaceFixGaps : public ScCmdFaceFixAll{
protected:
		virtual  BOOL						FixShape();
};