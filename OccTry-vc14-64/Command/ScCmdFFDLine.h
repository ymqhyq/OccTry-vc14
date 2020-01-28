#pragma once

#include "ScCommand.h"

class FFDSurfObj;
class FFDLineAlgo;
class FFDLineTool;

class ScCmdFFDLine : public ScCommand
{
public:
	ScCmdFFDLine(void);
	~ScCmdFFDLine(void);

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//鼠标移动的响应函数
	virtual	  void			MoveEvent(const CPoint& point,
		const Handle(V3d_View)& aView);

protected:
	enum{S_OBJ,S_PNT1,S_PNT2,S_SELPNT,S_MOVEPNT};

	BOOL					HasSelObj();
	void					ShowLine(const gp_Pnt& pnt);
	void					ShowToolFrame();//显式工具的控制点，以便于编辑。

	BOOL					ChangeToolCurve(const gp_Pnt& pnt,BOOL bTmp);

	BOOL					InitDeform(const gp_Pnt& pnt);
	BOOL					DeformObj(BOOL bTmp);

	FFDSurfObj				*m_pFFDObj;
	FFDLineTool				*m_pFFDTool;
	FFDLineAlgo				*m_pFFDAlgo;

	Handle(Geom_BSplineCurve)	m_curTool,m_tmpTool;

	BOOL					m_bChanged;
	gp_Pnt					m_startPnt,m_selPnt;
};
