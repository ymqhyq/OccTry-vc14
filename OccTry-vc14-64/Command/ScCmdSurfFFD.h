#pragma once

#include "ScCommand.h"

class FFDSurfObj;
class FFDSurfTool;
class FFDSurfAlgo;

//////////////////////////////////////////////////////////////////////////
// 记录曲面到变形曲面的投影信息
class TcSurfPrjInfo{
public:
	TcSurfPrjInfo();
	~TcSurfPrjInfo();

	BOOL					Init(int nus,int nvs);

	void					SetDist(int iu,int iv,double dist);
	double					GetDist(int iu,int iv);
	
	void					SetPrjUV(int iu,int iv,double u,double v);
	gp_Pnt2d				GetPrjUV(int iu,int iv);

	int						IndexOf(int iu,int iv);

public:
	Handle(Geom_BSplineSurface)		m_cpySurf;//拷贝的surf.

	int						m_nus,m_nvs;//控制点个数
	double					*m_pDstArray;//距离
	STR3D					m_uvArray;//参数

	BOOL					m_bInit;
};

class ScCmdSurfFFD : public ScCommand
{
public:
	ScCmdSurfFFD(void);
	~ScCmdSurfFFD(void);

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
	enum{S_OBJ,S_SELPNT,S_BASPNT,S_CHANGING};

	BOOL					HasSelObj();
	BOOL					InitDefrom();
	BOOL					ShowCtrlPnts();
	BOOL					ChangeToolSurf(BOOL bTmp);
	BOOL					DoDeform(BOOL bDone);

protected:
	Handle(Geom_BSplineSurface)		m_toolSurf,m_tmpSurf;
	TColgp_SequenceOfPnt			m_seqOfPnt;//选中点
	gp_Pnt							m_basPnt,m_chgPnt;//基点和新点
	BOOL							m_bChanged;//

	TcSurfPrjInfo					*m_pSurfPrjInfo;

	FFDSurfAlgo				*m_pFFDAlgo;
	FFDSurfObj				*m_pFFDObj;
	FFDSurfTool				*m_pFFDTool;
};
