#pragma once

#include <afxtempl.h>

#include <AIS_Trihedron.hxx>
#include <AIS_InteractiveContext.hxx>

///////////////////////////////////////////////////
// 表示一个局部坐标系

class ScUcs{
public:
	ScUcs();
	ScUcs(const gp_Ax2& ax);
	~ScUcs();

	//设置UCS
	void		SetUCS(const gp_Ax2& ax);
	gp_Trsf&	UtowTrsf() { return m_utowTrsf; }
	gp_Trsf&	WtouTrsf() { return m_wtouTrsf; }

public:
	gp_Ax2		m_ax2;//坐标系

protected:
	gp_Trsf		m_utowTrsf;//UCS到WCS的变换矩阵
	gp_Trsf		m_wtouTrsf;//WCS到UCS的变换矩阵

	friend class ScUcsMgr;
};

typedef CArray<ScUcs *,ScUcs *>	TUcsArray;

class ScUcsMgr
{
public:
	ScUcsMgr(void);
	~ScUcsMgr(void);

	void			SetAISCtx(const Handle(AIS_InteractiveContext)& aCtx);

	void			ShowWCSTrihedron();
	void			ShowUCSTrihedron();
	void			ShowTrihedron();

	//切换到WCS
	void			SwitchToWCS();
	void			SwitchToPrevUCS();
	void			SwitchToNextUCS();

	//是否有局部坐标系
	BOOL			HasUCS();
	BOOL			IsWCS();

	//添加一个UCS,第二个参数标识UCS表示是否使用WCS坐标。TRUE:时，否则使用当前UCS的坐标.
	void			AddUCS(ScUcs *pScUcs,BOOL bWCS = TRUE);

	//获取当前
	ScUcs*			CurrentUCS();
	ScUcs*			PrevUCS();

public:
	ScUcs			m_scWCS;//世界坐标系
	TUcsArray		m_ucsArray;
	int				m_nCurIdx;//当前ucs。-1表示世界坐标系
	ScUcs			*m_pPreUCS;//前一个UCS。如果为NULL，则为WCS

	Handle(AIS_Trihedron)	m_hWcsTrihedron;
	Handle(AIS_Trihedron)	m_hUcsTrihedron;//世界和局部坐标系的显示

	Handle(AIS_InteractiveContext)	m_AISContext;

protected:
	//从AisCtx中移出临时object，并释放内存。这里关键是释放内存。
	virtual void		RemoveCtxObj(Handle(AIS_InteractiveObject)& aObj,BOOL bUpdate = FALSE);

};

inline void		ScUcsMgr::SetAISCtx(const Handle(AIS_InteractiveContext)& aCtx)
{
	m_AISContext = aCtx;
}

inline ScUcs*	ScUcsMgr::PrevUCS()
{
	return m_pPreUCS;
}