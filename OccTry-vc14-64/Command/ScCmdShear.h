#pragma once

#include "ScCommand.h"

class ScCmdShear : public ScCommand
{
public:
	ScCmdShear(void);
	~ScCmdShear(void);

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void						InputEvent(const CPoint& point,int nState,//上述minput定义
		const Handle(V3d_View)& aView);

	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
		const Handle(V3d_View)& aView);

protected:
	enum{S_OBJ,S_BASEPNT,S_BASEDIR,S_NEWDIR};

	BOOL					HasSelObj();
    BOOL					ShowDirLine(const gp_Pnt& pnt);
	BOOL					DoShear(BOOL bTmp);

	gp_Pnt					m_basePnt;
	gp_Vec					m_baseDir;
	gp_Vec					m_newDir;
};
