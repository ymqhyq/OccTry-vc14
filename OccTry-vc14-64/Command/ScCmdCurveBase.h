#pragma once

#include "ScCommand.h"

class ScCmdCurveBase : public ScCommand
{
public:
	ScCmdCurveBase(void);
	~ScCmdCurveBase(void);

protected:
		//是否选中对象，如果选中了对象，则对象保存在SelObj(idx)中。
		BOOL						HasSelEdge(int idx = 0);
		BOOL						HasSelWire(int idx = 0);
		BOOL						HasSelEdgeOrWire(int idx = 0);
};
