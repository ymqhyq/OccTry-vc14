#pragma once

#include "ScCommand.h"

class FFDBendTool;
class FFDBendAlgo;
class FFDSurfObj;

class ScCmdFFDBend : public ScCommand
{
public:
	ScCmdFFDBend(void);
	~ScCmdFFDBend(void);

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
	enum{S_OBJ,S_AX_BEGIN,S_AX_END,S_PASS_PNT};

	BOOL					HasSelObj();
	void					ShowAxLine(const gp_Pnt& pnt);
	void					ShowArcCurve();

	BOOL					Deform(BOOL bTmp);

	BOOL					InitBend();

	gp_Pnt					m_axPnt1,m_axPnt2;
	gp_Pnt					m_passPnt;

	FFDSurfObj				*m_pObj;
	FFDBendAlgo				*m_pAlgo;
	FFDBendTool				*m_pTool;

	BOOL					m_bChanged;
};
