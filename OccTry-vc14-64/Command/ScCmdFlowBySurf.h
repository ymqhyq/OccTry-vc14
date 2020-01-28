#pragma once

#include "ScCommand.h"

class ScCmdFlowBySurf : public ScCommand
{
public:
	ScCmdFlowBySurf(void);
	~ScCmdFlowBySurf(void);

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void						InputEvent(const CPoint& point,int nState,//上述minput定义
		const Handle(V3d_View)& aView);


protected:
	enum{S_OBJ,S_DATUM_SURF,S_TAR_SURF};

	BOOL				HasSelObj();
	BOOL				HasSelDatumSurf(int nx,int ny);
	BOOL				HasSelTarSurf(int nx,int ny);

	BOOL				DoFlow();

	gp_Pnt2d			m_datumUV;
	double				m_dU1,m_dV1,m_dU2,m_dV2;

	gp_Pnt2d			m_tarUV;
	double				m_tU1,m_tV1,m_tU2,m_tV2;
};
