#pragma once

#include "ScCommand.h"
#include <AIS_SequenceOfInteractive.hxx>

class ScCmdEdit
{
public:
	ScCmdEdit(void);
	~ScCmdEdit(void);
};

//////////////////////////////////////////////////////////////////////
//删除命令。删除一个或多个对象。
class ScCmdDelete : public ScCommand{
public:
	
	//启动和结束命令
	virtual	  int		Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int		End();
};