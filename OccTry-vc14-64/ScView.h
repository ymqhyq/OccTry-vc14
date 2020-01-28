#pragma once

//简单的view封装
#include <V3d_View.hxx>

enum{
	ScView_Front,
	ScView_Back,
	ScView_Top,
	ScView_Bottom,
	ScView_Left,
	ScView_Right,
};

class ScUcs;
class ScUcsMgr;
class CView;

class ScView
{
public:
	ScView(const Handle(V3d_View)& aView);
	~ScView(void);

	//设置视图类型
	int			SetProj(int nType);

	//屏幕点转换为空间(WCS or UCS)中的点。
	gp_Pnt	    PixelToModel(Standard_Real x, Standard_Real y,BOOL bWCS = TRUE);

	//屏幕点转换为cs的平面上的点
	gp_Pnt	    PixelToCSPlane(Standard_Real x, Standard_Real y,BOOL bWCS = TRUE);

	//屏幕点转换为当前坐标系中的点，根据情况显示不同的点。
	gp_Pnt		PixelToObjCS(int x,int y);

	//坐标转换函数
	int			WCSToUCS(gp_Pnt& pnt);
	int			UCSToWCS(gp_Pnt& pnt);
	int			WCSToUCS(gp_Vec& vec);
	int			UCSToWCS(gp_Vec& vec);

	//根据屏幕坐标获取从改点发出的眼坐标。
	//line在wcs中。
	gp_Lin		GetEyeLine(int nX,int nY);

	//获取edge上点击的点。用于选中edge后，获取edge上的点击点。
	//即当点击选中curve时，获取此时点击的点。dTol为屏幕坐标系下的范围容差。
	//结果WCS点。
	BOOL		GetEdgePickPoint(const TopoDS_Edge& aEdge,
								int nX,int nY,double dTol,
								double& t,gp_Pnt& pickPnt);

	//获取edge上离nx，ny最近的投影点。如果超出范围，返回端点。
	BOOL		GetEdgeNearestPoint(const TopoDS_Edge& aEdge,
								int nX,int nY,
								double& t,gp_Pnt& pnt);
	BOOL		GetEdgeNearestPoint(const TopoDS_Edge& aEdge,
								int nX,int nY,
								double& t,gp_Pnt& pnt,double& dist);

	//获取face上点击的点。用于选中shape或face后，获取shape或face上的点击点。
	//即当点击选中face时，获取此时点击的点。
	//结果WCS点。
	BOOL		GetFacePickPoint(const TopoDS_Face& aFace,
								int nx,int ny,
								double& u,double& v,gp_Pnt& pickPnt);

	//获取shape上点击的点。
	//即当点击选中shape时，获取此时点击的点。
	//结果WCS点。
	gp_Pnt		GetShapePickPoint(const TopoDS_Shape& aShape,int nx,int ny);

	//计算点pnt到当前工作平面的高度.Pnt为WCS点。
	double		ComputeHeight(const gp_Pnt& pnt);

	//是否需要空间中的点。
	void		NeedModelPoint(BOOL bNeed);

	//当前的ax坐标系，根据平面不同而不同
	gp_Ax2		CurrentAxis(BOOL bUCS = FALSE);

	//设置当前局部坐标
	void		UpdateUCS();
	void		SetUcsMgr(ScUcsMgr *pUcsMgr);

public:
	Handle(V3d_View)		m_aView;
	int						m_nViewType;//默认front。
	ScUcsMgr				*m_pScUcsMgr;//
	CView					*m_pViewWnd;

protected:
	//计算直线和平面的交点
	BOOL		ComputeLinPlanIntersection(const gp_Lin& aL,
							const gp_Pln& aPlan,gp_Pnt& aInterPnt);

private:
	BOOL		m_bNeedModelPoint;//是否需要模型空间中的点。通常，鼠标点转换为工作平面上的
	                              //点，有时，也需要空间的点。
};

inline void		ScView::SetUcsMgr(ScUcsMgr *pUcsMgr)
{
	m_pScUcsMgr = pUcsMgr;
}

//是否需要空间中的点。
inline	void	ScView::NeedModelPoint(BOOL bNeed)
{
	m_bNeedModelPoint = bNeed;
}