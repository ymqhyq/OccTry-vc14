#pragma once

#include <vector>

//涉及到曲线的frame的类
//////////////////////////////////////////////////////////////////////////
//记录一个曲面的离散frame的消息

//曲线上一个点的Frame信息。
class FFDCurvePointFrame{
public:
	FFDCurvePointFrame();
	FFDCurvePointFrame(const FFDCurvePointFrame& other);

	FFDCurvePointFrame&	operator=(const FFDCurvePointFrame& other);

	void					Set(double u,const gp_Pnt& pnt,const gp_Dir& T,
		const gp_Dir& N,const gp_Dir& B);

	double					_u;
	gp_Pnt					_O;//局部原点
	gp_Dir					_dT,_dN,_dB;//tangent,Normal,biNormal
};

//记录一个曲线上的一系列离散点上的frame信息
// RMF:rotate minium frame
class FFDCurveRMF{
public:
	FFDCurveRMF();

	BOOL					Init(const Handle(Geom_Curve)& aCur,double df,double dl,int nSamps);

	//清除计算结果
	void					Clear();

	//设置初始法矢，主要应用于直线等初始法矢无法确定的情况
	void					SetStartNormal(const gp_Dir& startN);

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
	std::vector<FFDCurvePointFrame>	m_frmArray;

protected:
	BOOL						m_bSetStartNormal;
	gp_Dir						m_startNormal;
};

