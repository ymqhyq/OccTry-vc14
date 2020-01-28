#pragma once

#include "FFDRmf.h"

class FFDTool
{
public:
	FFDTool(void);
	virtual ~FFDTool(void);

	BOOL						IsValid() const { return m_bValid; }

protected:
	BOOL						m_bValid;
};

//////////////////////////////////////////////////////////////////////////
//曲面变形工具，保存基准曲面和变形曲面的信息。
// 先考虑整体变形，局部后续考虑
// 坐标系还需要考虑在内
class FFDSurfTool : public FFDTool{
public:
	FFDSurfTool();
	virtual ~FFDSurfTool();

	//
	BOOL							InitRect(const gp_Pnt& p1,const gp_Pnt& p2,
											int nUDeg = 3,int nVDeg = 3,int nUCtrlPnts = 4,int nVCtrlPnts = 4);

	BOOL							InitRect(const gp_Pnt& p1,const gp_Pnt& p2,const gp_Ax2& ax,
											int nUDeg = 3,int nVDeg = 3,int nUCtrlPnts = 4,int nVCtrlPnts = 4);

	//初始化基准曲面
	BOOL							InitDatumSurf(const Handle(Geom_Surface)& pSurf);

	//设置目标曲面
	BOOL							SetTarSurf(const Handle(Geom_Surface)& pSurf);

public:
	Handle(Geom_BSplineSurface)		m_datumSurf;//基准曲面
	Handle(Geom_Surface)			m_tarSurf;//目标曲面

	BOOL							m_bPlane;//基准面是否平面
	Handle(Geom_Plane)				m_aPlane;//平面，如果基准曲面是平面
	double							m_u1,m_v1,m_u2,m_v2;//plane的参数范围
};

//////////////////////////////////////////////////////////////////////////
//基于直线的FFD变形工具,原始直线为基准线，变形后的直线为目标曲线。
class FFDLineTool : public FFDTool{
public:
	FFDLineTool();
	virtual ~FFDLineTool();

	//下面两个方法适用于基于直线的FFD变形，原始直线为基准线，变形后的直线为目标曲线。
	BOOL							InitLine(const gp_Pnt& p1,const gp_Pnt& p2,int nDeg = 3,int ncpts = 4);

	BOOL							SetLineDfmCurve(const Handle(Geom_BSplineCurve)& aCur);

	//计算RMF
	BOOL							CalcRMF(int nSamps);

public:
	Handle(Geom_BSplineCurve)		m_datumCurve;//基准曲线
	Handle(Geom_BSplineCurve)		m_tarCurve;//目标曲线

	//记录两个曲线的rmf信息
	FFDCurveRMF						m_datumRmf;
	FFDCurveRMF						m_tarRmf;
	int								m_nSamps;
	BOOL							m_bChanged;//是否改变
};

//////////////////////////////////////////////////////////////////////////
// 基于曲线的变形工具。两条曲线为不同的曲线
class FFDCurveTool : public FFDTool{
public:
	FFDCurveTool();
	virtual ~FFDCurveTool();

	BOOL							SetDatumCurve(const Handle(Geom_Curve)& aCur);
	BOOL							SetDatumCurve(const Handle(Geom_Curve)& aCur,double df,double dl);

	//设置目标曲线,曲线不限制，可能和Line没有
	BOOL							SetTarCurve(const Handle(Geom_Curve)& aCur);
	BOOL							SetTarCurve(const Handle(Geom_Curve)& aCur,double df,double dl);

	//计算RMF
	// 两个参数表示了曲线间的对应关系.stretch表示:变形物体是否要拉伸.
	BOOL							CalcRMF(int nSamps,double datumT,double tarT,BOOL bStretch = TRUE);

	double							MapParam(double datumt);

public:
	Handle(Geom_Curve)				m_datumCurve;//基准曲线
	double							m_df,m_dl;//曲线参数范围

	Handle(Geom_Curve)				m_tarCurve;//目标曲线
	double							m_tf,m_tl;//曲线参数范围

	//记录两个曲线的rmf信息
	FFDCurveRMF						m_datumRmf;
	FFDCurveRMF						m_tarRmf;

	int								m_nMapType;//映射类型
	double							m_dMapRatio;//映射比例

protected:
	enum{eMap_PP,eMap_PN,eMap_NP,eMap_NN};
};

//////////////////////////////////////////////////////////////////////////
// 曲面流动变形工具
class FFDSurfFlowTool : public FFDTool{
public:
	FFDSurfFlowTool();
	virtual ~FFDSurfFlowTool();

	void							SetStretch(BOOL bStretch = TRUE);
	void							SetUseTrimSurf(BOOL bUseTrimSurf = FALSE);

	BOOL							SetDatumSurf(const Handle(Geom_Surface)& aSurf);
	BOOL							SetDatumSurf(const Handle(Geom_Surface)& aSurf,double u1,double v1,double u2,double v2);

	BOOL							SetTarSurf(const Handle(Geom_Surface)& aSurf);
	BOOL							SetTarSurf(const Handle(Geom_Surface)& aSurf,double u1,double v1,double u2,double v2);

	BOOL							MapPoint(double u,double v,gp_Pnt2d& uv);

	BOOL							CalcMapInfo();

public:
	Handle(Geom_Surface)			m_datumSurf;
	double							m_dU1,m_dV1,m_dU2,m_dV2;

	Handle(Geom_Surface)			m_tarSurf;
	double							m_tU1,m_tV1,m_tU2,m_tV2;

	double							m_dUMapRatio,m_dVMapRatio;

	BOOL							m_bStretch;
	BOOL							m_bUseTrimSurf;
};

inline void		FFDSurfFlowTool::SetStretch(BOOL bStretch)
{
	m_bStretch = bStretch;
}

inline void		FFDSurfFlowTool::SetUseTrimSurf(BOOL bUseTrimSurf)
{
	m_bUseTrimSurf = bUseTrimSurf;
}

//////////////////////////////////////////////////////////////////////////
//
//倾斜工具
class FFDShearTool : public FFDTool{
public:
	FFDShearTool();
	virtual ~FFDShearTool();

	//初始化,基准点和基准方向
	BOOL							Init(const gp_Pnt& org,const gp_Vec& baseDir);
	BOOL							Init(const gp_Pnt& org,const gp_Vec& baseDir,const gp_Ax2& ucs);

	//倾斜方向
	BOOL							Shear(const gp_Vec& newDir);

	//倾斜角度(弧度)
	BOOL							Shear(double drand);

	//对点进行变换
	BOOL							TrsfPoint(const gp_Pnt& pnt,gp_Pnt& tsfpnt);

	//返回变换
	gp_GTrsf						ShearTrsf();

protected:
	gp_Ax2							m_ucsAx;//UCS坐标系
	gp_Ax2							m_locAx;//局部坐标系,基于ucs.
	double							m_drad;//倾斜角度		

	gp_Trsf							m_tsf,m_tsfr;//转入局部坐标和转回原来坐标系的矩阵,互逆.
	gp_GTrsf						m_shearTrsf;//总的变换矩阵,gp_Trsf不支持shear变换
};

//////////////////////////////////////////////////////////////////////////
// 扭转工具
class FFDTwistTool : public FFDTool{
public:
	FFDTwistTool();
	virtual ~FFDTwistTool();

	//初始化
	BOOL							Init(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Vec& baseDir);

	BOOL							Rotate(double dRad,BOOL bExtend);

	//对点进行旋转
	BOOL							RotatePoint(const gp_Pnt& pnt,gp_Pnt& newpnt);

protected:
	//点投影到直线,获取参数
	BOOL							AxisProject(const gp_Pnt& pnt,double& t);

	//旋转角度
	double							RotRad(double t);

protected:
	gp_Pnt							m_axPnt1,m_axPnt2;//旋转轴的起始点和终止点
	gp_Vec							m_baseDir;//基准起始方向(角度)
	double							m_dRotRad;//旋转的角度.

	BOOL							m_bExtend;//是否无限延伸

private:
	double							m_axLen;
	gp_Vec							m_axVec;
	double							m_radSlope;//角度的线性斜率

	Handle(Geom_Line)				m_axLine;//旋转轴对应直线
};

//////////////////////////////////////////////////////////////////////////
// 弯曲bend工具.
class FFDBendTool : public FFDTool{
public:
	FFDBendTool();
	virtual ~FFDBendTool();

	//必要的初始化,前两点是骨干点,后面两个点用于限制变形范围.
	BOOL							Init(const gp_Pnt& pnt1,const gp_Pnt& pnt2,
										const gp_Pnt& pntFrm,const gp_Pnt& pntTo);

	//根据通过点构造用于变形的两条曲线.
	BOOL							BuildCurveTool(const gp_Pnt& pnt,BOOL bExtend,BOOL bSymmetric);

	Handle(Geom_Curve)				GetDatumCurve() const;
	Handle(Geom_Curve)				GetTarCurve() const;
	Handle(Geom_Curve)				GetArcCurve() const;

protected:
	gp_Pnt							CalcCircCenter(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Pnt& paspnt,
										const gp_Dir& cdir,const gp_Dir& N);

protected:
	gp_Pnt							m_axPnt1,m_axPnt2;

	Handle(Geom_Curve)				m_datumCurve;//基准曲线
	Handle(Geom_BSplineCurve)		m_tarCurve;//目标曲线
	Handle(Geom_BSplineCurve)		m_arcCurve;//圆弧曲线,用于交互显示

private:
	BOOL							m_bExtend;
	BOOL							m_bSymmetric;

	double							m_preLen,m_afterLen;//前后的直线长度.
};

inline Handle(Geom_Curve)	FFDBendTool::GetDatumCurve() const
{
	ASSERT(IsValid());
	return m_datumCurve;
}

inline Handle(Geom_Curve)	FFDBendTool::GetTarCurve() const
{
	ASSERT(IsValid());
	return m_tarCurve;
}

inline Handle(Geom_Curve)	FFDBendTool::GetArcCurve() const
{
	ASSERT(IsValid());
	return m_arcCurve;
}

//////////////////////////////////////////////////////////////////////////
// 锥形化工具
class FFDTaperTool : public FFDTool{
public:
	FFDTaperTool();
	virtual	~FFDTaperTool();

	//初始化.传入骨干的两个点,还有起始的方向和长度.bBoth表示是两个方向还是一个方向锥形化.
	BOOL							Init(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Vec& beginVec,
										BOOL bExtend,BOOL bBoth);

	//更新另一个方向的长度,实现锥形化.
	BOOL							SetEndLen(double dlen);

	//对一个点进行变换
	BOOL							TrsfPoint(const gp_Pnt& pnt,gp_Pnt& newpnt);

protected:
	gp_Pnt							m_axPnt1,m_axPnt2;//骨干的两个点
	double							m_dBeginLen,m_dEndLen;//起始和终止两端的长度

	BOOL							m_bExtend;//是否延伸
	BOOL							m_bBoth;//是否两个方向改变

protected:
	gp_Ax2							m_locAx;//局部坐标系
	double							m_dRatio;//变形系数

	gp_Trsf							m_ltowTsf,m_wtolTsf;//
};