//////////////////////////////////////////////////////////////////////////
// 实现Axial Deformation变形算法。
#pragma once

#include <vector>
#include "ScCommand.h"

//////////////////////////////////////////////////////////////////////////
//记录一个曲面的离散frame的消息
class TcCurveFrame{
public:
	TcCurveFrame();
	TcCurveFrame(const TcCurveFrame& other);

	TcCurveFrame&	operator=(const TcCurveFrame& other);

	void					Set(double u,const gp_Pnt& pnt,const gp_Dir& T,
								const gp_Dir& N,const gp_Dir& B);

	double					_u;
	gp_Pnt					_O;//局部原点
	gp_Dir					_dT,_dN,_dB;//tangent,Normal,biNormal
};
class TcCurveRMF{
public:
	TcCurveRMF();

	BOOL					Init(const Handle(Geom_Curve)& aCur,double df,double dl,int nSamps);
	//计算
	BOOL					CalcRotateRMF();
	//sloan方法计算
	BOOL					CalcSloanRMF();

	BOOL					CalcLcsCoord(double u,const gp_Pnt& pnt,gp_Pnt& lcpnt);

	BOOL					CalcDfmCoord(double u,const gp_Pnt& lcpnt,gp_Pnt& pnt);

protected:
	enum{M_ROTATE,M_SLOAN};

	BOOL					CalcRMF(int nMethod);//1:旋转法，2：sloan方法
	BOOL					DoCalcRotateFrame(double t,
								const gp_Dir& T0,const gp_Dir& N0,const gp_Dir& B0,
								gp_Dir& T,gp_Dir& N,gp_Dir& B);

	BOOL					DoCalcSloanFrame(double t,
								const gp_Dir& T0,const gp_Dir& N0,const gp_Dir& B0,
								gp_Dir& T,gp_Dir& N,gp_Dir& B);


public:
	Handle(Geom_Curve)			m_aCurve;//曲线
	double						m_df,m_dl;//参数范围
	double						m_nSamps;//采样点个数
	double						m_dstep;//步长
	std::vector<TcCurveFrame>	m_frmArray;
};

//////////////////////////////////////////////////////////////////////////
//记录每个变形曲面的信息
class ScAxDFSurfInfo{
public:
	ScAxDFSurfInfo();
	~ScAxDFSurfInfo();

	//初始化
	BOOL						Init(int nus,int nvs);

	void						SetUOfCurve(int iu,int iv,double u);
	double						UOfCurve(int iu,int iv);
	void						SetLcsCoord(int iu,int iv,const gp_Pnt& pnt);
	gp_Pnt						LcsCoord(int iu,int iv);

	int							IndexOf(int iu,int iv);

public:
	Handle(Geom_BSplineSurface)			m_cpySurf;//拷贝的曲面
	Handle(Geom_BSplineSurface)			m_tmpSurf;//中间曲面,临时显式使用
	int							m_nus,m_nvs;//u、v向控制点个数
	double						*m_pUOfCurve;//控制点在变形曲线上的参数值。
	STR3D						m_pArLcsCoord;//在曲线点的局部坐标系内部的局部坐标

	BOOL						m_bInit;//
};

class ScCmdAxDF : public ScCommand
{
public:
	ScCmdAxDF(void);
	~ScCmdAxDF(void);

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
	enum{S_OBJ,S_TOOL,S_SELPNT,S_MOVEPNT};

	BOOL					HasSelObj();
	BOOL					HasSelTool();
	BOOL					InitDeform();//为变形做初始化

	void					ShowToolFrame();//显式工具的控制点，以便于编辑。

	BOOL					ChangeToolCurve(const gp_Pnt& pnt,BOOL bTmp);
	BOOL					DeformObj(BOOL bTmp);

	ScAxDFSurfInfo			*m_pSurfInfo;//曲面相关信息
	Handle(Geom_BSplineCurve)		m_curTool,m_tmpTool;
	BOOL					m_bChanged;

	TcCurveRMF				*m_pCurRMF;//记录相关信息

	gp_Pnt					m_selPnt;
};
