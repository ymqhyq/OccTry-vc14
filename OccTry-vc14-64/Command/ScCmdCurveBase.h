#pragma once

#include "ScCommand.h"

class ScCmdCurveBase : public ScCommand
{
public:
	ScCmdCurveBase(void);
	~ScCmdCurveBase(void);

protected:
		//�Ƿ�ѡ�ж������ѡ���˶�������󱣴���SelObj(idx)�С�
		BOOL						HasSelEdge(int idx = 0);
		BOOL						HasSelWire(int idx = 0);
		BOOL						HasSelEdgeOrWire(int idx = 0);
};
