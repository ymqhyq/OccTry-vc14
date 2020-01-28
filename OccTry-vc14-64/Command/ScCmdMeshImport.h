#pragma once

#include "ScCommand.h"

class ScCmdMeshImport
{
public:
	ScCmdMeshImport(void);
	~ScCmdMeshImport(void);
};

class ScCmdMeshImportMs3d : public ScCommand{
public:
	ScCmdMeshImportMs3d();

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

};