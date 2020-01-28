#pragma once

#include "ScCommand.h"

class ScCmdDisplay
{
public:
	ScCmdDisplay(void);
	~ScCmdDisplay(void);
};

//////////////////////////////////////////////////////////
// 对象纹理的实现
// 使用AIS_TextureShape实现
class ScCmdTexture : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

public:
	//设置纹理
	BOOL					SetTexture(LPCTSTR lpszFile,BOOL bDone);
	
protected:
	enum{S_SHAPE,S_TEXTURE};

	BOOL					HasSelectedShape();
	int						DoTexture();

	BOOL					m_bModified;
};