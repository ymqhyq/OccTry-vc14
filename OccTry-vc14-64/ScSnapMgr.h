#pragma once

#include <afxtempl.h>

#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Point.hxx>

#include <vector>

class ScView;
////////////////////////////////////////////////////////////////
// 自动捕捉的管理。
// 目前先实现端点的捕捉。其他后续再处理。
//

//捕捉点的信息
class ScSnapPoint{
public:
	ScSnapPoint(){
		m_nX = m_nY = 0;
		m_bSnaped = FALSE;
	}
	ScSnapPoint(const ScSnapPoint& other)
	{
		*this = other;
	}

	ScSnapPoint&	operator=(const ScSnapPoint& other)
	{
		m_pnt = other.m_pnt;
		m_nX  = other.m_nX;
		m_nY  = other.m_nY;
		m_bSnaped = other.m_bSnaped;

		return *this;
	}

	void			Dump();

public:
	gp_Pnt			m_pnt;//3维点
	int				m_nX,m_nY;//屏幕点
	BOOL			m_bSnaped;//是否被捕捉
};

typedef std::vector<ScSnapPoint> TSnapPointSet;

class ScSnapMgr
{
public:
	ScSnapMgr(const Handle(AIS_InteractiveContext)& aCtx);
	~ScSnapMgr(void);

	void				SetScView(ScView *pView) { m_pScView = pView; }

	//使能和禁止捕捉
	void				Enable(BOOL bEnable);
	BOOL				IsEnable() const { return m_bEnable; }

	//更新点，重新获取点信息。
	BOOL				UpdateSnapPoints();

	//自动捕捉
	void				Snap(int nX,int nY,const Handle(V3d_View)& aView);

	//获取距离pnt最近的捕捉点
	BOOL				GetSnapPoint(const gp_Pnt& pnt,gp_Pnt& snapPnt,BOOL bCompare = FALSE);

protected:
	void				Clear();

	//获取cs相关的点
	void				GetCSSnapPoint();

	//获取对象的点
	BOOL				GetShapeSnapPoints(const TopoDS_Shape& aShape,CRect& rc);

	BOOL				WCSToPixel(const gp_Pnt& pnt,int& nx,int& ny);

	//清除前面自动捕捉标记
	void				ClearSnapMarker();
	//绘制自动捕捉标记
	void				DrawSnapMarker();

	int					PixelDist(const ScSnapPoint& p1,const ScSnapPoint& p2);

	BOOL				IsNearest(int x1,int y1,int x2,int y2);

protected:
	Handle(AIS_InteractiveContext)		m_AISContext;//
	ScView								*m_pScView;
	BOOL								m_bEnable;
	int									m_nSnapSize;//捕捉范围
	int									m_nMarkerSize;//标记大小

	TSnapPointSet						m_csSet;//坐标系相关的点
	TSnapPointSet						m_temSet;//端点
	TSnapPointSet						m_cenSet;//圆心点
	TSnapPointSet						m_midSet;//中点

	ScSnapPoint							m_snapPoint;//捕获点
	BOOL								m_bHasPoint;//是否有捕捉点

	CPen								*m_pMarkerPen;

	Handle(AIS_Point)					m_aPntMarker;
};
