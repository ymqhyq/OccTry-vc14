#pragma once

#include "ScCommand.h"

class ScCmdMeshGen 
{
public:
	ScCmdMeshGen(void);
	~ScCmdMeshGen(void);
};

//////////////////////////////////////////////////////////////////////////
// 将对象转换为网格
class ScCmdMeshConvertToMesh : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);


protected:
	BOOL					HasSelObj();
	BOOL					DoConvert();

};