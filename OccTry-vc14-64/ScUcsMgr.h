#pragma once

#include <afxtempl.h>

#include <AIS_Trihedron.hxx>
#include <AIS_InteractiveContext.hxx>

///////////////////////////////////////////////////
// ��ʾһ���ֲ�����ϵ

class ScUcs{
public:
	ScUcs();
	ScUcs(const gp_Ax2& ax);
	~ScUcs();

	//����UCS
	void		SetUCS(const gp_Ax2& ax);
	gp_Trsf&	UtowTrsf() { return m_utowTrsf; }
	gp_Trsf&	WtouTrsf() { return m_wtouTrsf; }

public:
	gp_Ax2		m_ax2;//����ϵ

protected:
	gp_Trsf		m_utowTrsf;//UCS��WCS�ı任����
	gp_Trsf		m_wtouTrsf;//WCS��UCS�ı任����

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

	//�л���WCS
	void			SwitchToWCS();
	void			SwitchToPrevUCS();
	void			SwitchToNextUCS();

	//�Ƿ��оֲ�����ϵ
	BOOL			HasUCS();
	BOOL			IsWCS();

	//���һ��UCS,�ڶ���������ʶUCS��ʾ�Ƿ�ʹ��WCS���ꡣTRUE:ʱ������ʹ�õ�ǰUCS������.
	void			AddUCS(ScUcs *pScUcs,BOOL bWCS = TRUE);

	//��ȡ��ǰ
	ScUcs*			CurrentUCS();
	ScUcs*			PrevUCS();

public:
	ScUcs			m_scWCS;//��������ϵ
	TUcsArray		m_ucsArray;
	int				m_nCurIdx;//��ǰucs��-1��ʾ��������ϵ
	ScUcs			*m_pPreUCS;//ǰһ��UCS�����ΪNULL����ΪWCS

	Handle(AIS_Trihedron)	m_hWcsTrihedron;
	Handle(AIS_Trihedron)	m_hUcsTrihedron;//����;ֲ�����ϵ����ʾ

	Handle(AIS_InteractiveContext)	m_AISContext;

protected:
	//��AisCtx���Ƴ���ʱobject�����ͷ��ڴ档����ؼ����ͷ��ڴ档
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