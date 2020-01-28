#pragma once

#include <vector>

class FFDSurfObj;
class FFDSurfTool;
class FFDLineTool;
class FFDCurveTool;
class FFDSurfFlowTool;
class FFDShearTool;
class FFDTwistTool;
class FFDBendTool;

class FFDAlgo
{
public:
	FFDAlgo(void);
	virtual ~FFDAlgo(void);

	BOOL					IsValid() const { return m_bValid; }
	BOOL					IsDone() const { return m_bDone; }

	TopoDS_Shape			Shape() const { return m_aNewShape; };

protected:
	BOOL					m_bValid;
	BOOL					m_bDone;
	TopoDS_Shape			m_aNewShape;
};

//////////////////////////////////////////////////////////////////////////
//

class FFDSurfToolMapItem{
public:
	FFDSurfToolMapItem(){
		m_u = m_v = 0.0;
		m_dist = 0.0;
	}

	double					m_u,m_v;//变形点在基准曲面上的参数坐标
	double					m_dist;//变形点到基准曲面的最近距离。
};

//曲面和变形工具间的映射关系
class FFDSurfToolMap{
public:
	FFDSurfToolMap();
	~FFDSurfToolMap();

	BOOL					Init(int nus,int nvs);

	//变形曲面投影到工具曲面上
	BOOL					MapToDatumSurf(FFDSurfTool *pTool);

	void					SetDist(int iu,int iv,double dist);
	double					GetDist(int iu,int iv);

	void					SetPrjUV(int iu,int iv,double u,double v);
	gp_Pnt2d				GetPrjUV(int iu,int iv);

	int						IndexOf(int iu,int iv);

public:
	Handle(Geom_BSplineSurface)			m_aSurf;//要变形的曲面

	int									m_nus,m_nvs;//控制点个数
	FFDSurfToolMapItem					*m_pSurfToolMap;//曲面上控制点和基准曲面的关联映射。

	BOOL								m_bInit;//是否初始化
};

class FFDSurfData{
public:
	TopoDS_Face				m_aFace;//关联face
	FFDSurfToolMap			m_aMap;//关联信息
};

//基于曲面的变形
class FFDSurfAlgo : public FFDAlgo{
public:
	FFDSurfAlgo();
	virtual ~FFDSurfAlgo();

	//算法初始化
	BOOL					Init(FFDSurfObj *pObj,FFDSurfTool *pTool);

	//进行变形
	BOOL					Deform();


protected:
	FFDSurfObj				*m_pObj;
	FFDSurfTool				*m_pTool;

	//需要建立surf和其map间的关系
	std::vector<FFDSurfData *>			m_surfDatas;//face的信息
};

//////////////////////////////////////////////////////////////////////////
//

//记录一个控制点在frame内的数据
class FFDCurveToolMapItem{
public:
	FFDCurveToolMapItem();

	double					m_t;//控制点对应的曲线上的参数值。
};

//记录了曲面和tool的对应关系
class FFDCurveToolMap{
public:
	FFDCurveToolMap();
	~FFDCurveToolMap();

	//初始化
	BOOL							Init(const Handle(Geom_BSplineSurface)& aSurf);

	//设置和查询数据
	void							SetParam(int iu,int iv,double t);
	double							GetParam(int iu,int iv);

	int								IndexOf(int iu,int iv);

public:
	Handle(Geom_BSplineSurface)		m_aSurf;//要变形的曲面

	int								m_nus,m_nvs;//u、v向控制点个数
	FFDCurveToolMapItem				*m_pMapData;//映射数据，为每个控制点在对应的局部坐标系内的信息。

	BOOL							m_bInit;//
};

//保存曲面和相关数据
class FFDCurveSurfData{
public:
	TopoDS_Face						m_aFace;
	FFDCurveToolMap					m_aMap;
};

//基于直线的变形
class FFDLineAlgo : public FFDAlgo{
public:
	FFDLineAlgo();
	virtual ~FFDLineAlgo();

	//算法初始化
	BOOL					Init(FFDSurfObj *pObj,FFDLineTool *pTool);

	void					Clear();

	//进行变形
	BOOL					Deform();


protected:
	FFDCurveSurfData*		GetSurfData(const TopoDS_Face& aFace);

protected:
	FFDSurfObj				*m_pObj;
	FFDLineTool				*m_pTool;

	std::vector<FFDCurveSurfData *>		m_surfDatas;//曲面的数据

};

//基于曲线的变形
class FFDCurveAlgo : public FFDAlgo{
public:
	FFDCurveAlgo();
	virtual ~FFDCurveAlgo();

	//算法初始化
	BOOL					Init(FFDSurfObj *pObj,FFDCurveTool *pTool);

	//进行变形
	BOOL					Deform();


protected:
	FFDSurfObj				*m_pObj;
	FFDCurveTool			*m_pTool;


};

//////////////////////////////////////////////////////////////////////////
// 沿曲面流动
class FFDSurfFlowAlgo : public FFDAlgo{
public:
	FFDSurfFlowAlgo();
	virtual ~FFDSurfFlowAlgo();

	//算法初始化
	BOOL					Init(FFDSurfObj *pObj,FFDSurfFlowTool *pTool);

	//进行变形
	BOOL					Deform();

protected:
	FFDSurfObj				*m_pObj;
	FFDSurfFlowTool			*m_pTool;
};

//////////////////////////////////////////////////////////////////////////
// shear倾斜变形
class FFDShearAlgo : public FFDAlgo{
public:
	FFDShearAlgo();
	virtual ~FFDShearAlgo();

	//算法初始化
	BOOL					Init(FFDSurfObj *pObj,FFDShearTool *pTool);

	//进行变形
	BOOL					Deform();

protected:
	FFDSurfObj				*m_pObj;
	FFDShearTool			*m_pTool;
};

//////////////////////////////////////////////////////////////////////////
// 扭转变形算法
class FFDTwistAlgo : public FFDAlgo{
public:
	FFDTwistAlgo();
	virtual ~FFDTwistAlgo();

	//算法初始化
	BOOL					Init(FFDSurfObj *pObj,FFDTwistTool *pTool);

	//进行变形
	BOOL					Deform();

protected:
	FFDSurfObj				*m_pObj;
	FFDTwistTool			*m_pTool;
};

//////////////////////////////////////////////////////////////////////////
// 弯曲变形算法,用到了在曲线上流动功能
class FFDBendAlgo : public FFDAlgo{
public:
	FFDBendAlgo();
	virtual	~FFDBendAlgo();

	//初始化
	BOOL					Init(FFDSurfObj *pObj,FFDBendTool *pTool);

	//进行变形
	BOOL					Deform();

protected:
	FFDSurfObj				*m_pObj;
	FFDBendTool				*m_pTool;
};