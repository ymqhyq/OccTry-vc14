#include "StdAfx.h"
#include ".\scucsmgr.h"

ScUcs::ScUcs()
{
}

ScUcs::ScUcs(const gp_Ax2& ax)
{
	SetUCS(ax);
}

ScUcs::~ScUcs()
{
}

//设置UCS
void	ScUcs::SetUCS(const gp_Ax2& ax)
{
	this->m_ax2 = ax;
	
	//
	gp_Ax2 wax(gp_Pnt(0.0,0.0,0.0),gp_Dir(0.0,0.0,1.0),gp_Dir(1.0,0.0,0.0));
	this->m_utowTrsf.SetTransformation(gp_Ax3(ax),gp_Ax3(wax));
	this->m_wtouTrsf.SetTransformation(gp_Ax3(wax),gp_Ax3(ax));
}

////////////////////////////////////////////////////////////////////
//
ScUcsMgr::ScUcsMgr(void)
{
	this->m_nCurIdx = -1;
	this->m_pPreUCS = NULL;
	
	gp_Ax2 wax(gp_Pnt(0.0,0.0,0.0),gp_Dir(0.0,0.0,1.0),gp_Dir(1.0,0.0,0.0));
	this->m_scWCS.SetUCS(wax);
}

ScUcsMgr::~ScUcsMgr(void)
{
}

//
// 显示世界坐标系
//
void	ScUcsMgr::ShowWCSTrihedron()
{ 
	RemoveCtxObj(m_hWcsTrihedron);
	
	gp_Ax2 wax = gp::XOY();
	
	Handle(Geom_Axis2Placement) myTrihedronAxis=new Geom_Axis2Placement(wax);
	this->m_hWcsTrihedron = new AIS_Trihedron(myTrihedronAxis);
	m_AISContext->SetTrihedronSize(50, Standard_True);
	m_AISContext->Display(m_hWcsTrihedron, Standard_True);

}

//
// 当当前是局部坐标系时，显示。否则不显示。
//
void	ScUcsMgr::ShowUCSTrihedron()
{
	if(!HasUCS() || IsWCS())
		return;

	RemoveCtxObj(m_hUcsTrihedron);
	
	ScUcs *pScUcs = this->CurrentUCS();
	ASSERT(pScUcs);
	Handle(Geom_Axis2Placement) myTrihedronAxis=new Geom_Axis2Placement(pScUcs->m_ax2);
	this->m_hUcsTrihedron = new AIS_Trihedron(myTrihedronAxis);
	m_AISContext->SetColor(this->m_hUcsTrihedron,Quantity_NOC_GOLD,Standard_False);
	m_AISContext->SetTrihedronSize(40, Standard_True);
	m_AISContext->Display(m_hUcsTrihedron, Standard_True);
}

//
// 根据情况，只显示一个坐标系
//
void	ScUcsMgr::ShowTrihedron()
{
	RemoveCtxObj(m_hWcsTrihedron);
	RemoveCtxObj(m_hUcsTrihedron);
	
	if(IsWCS())
	{
		this->ShowWCSTrihedron();
	}
	else
	{
		this->ShowUCSTrihedron();
	}
}

//切换到WCS
void	ScUcsMgr::SwitchToWCS()
{
	//记录前一个
	this->m_pPreUCS = this->CurrentUCS();
	this->m_nCurIdx = -1;
}

void	ScUcsMgr::SwitchToPrevUCS()
{
	//WCS是第一个。
	if(IsWCS())
		return;

	//记录前一个
	this->m_pPreUCS = this->CurrentUCS();
	ASSERT(this->m_nCurIdx >= 0);
	this->m_nCurIdx --;
}

void	ScUcsMgr::SwitchToNextUCS()
{
	if(this->m_nCurIdx >= this->m_ucsArray.GetCount() - 1)
		return; //最后一个

	//记录前一个
	this->m_pPreUCS = this->CurrentUCS();
	//
	this->m_nCurIdx ++;
}

//是否有局部坐标系
BOOL	ScUcsMgr::HasUCS()
{
	return (m_ucsArray.GetCount() > 0);
}

BOOL	ScUcsMgr::IsWCS()
{
	return (this->m_nCurIdx < 0);
}

//
// 添加一个,并设为当前。
// 如果当前ucs实在前一个ucs的基础上定义的，则需要将该ucs从前一个ucs中
// 转入WCS，再记录下来。这样保证每个ucs中记录的都是wcs中的坐标，方便转换。
//
void	ScUcsMgr::AddUCS(ScUcs *pScUcs,BOOL bWCS)
{
	//记录前一个
	this->m_pPreUCS = this->CurrentUCS();

	if(!IsWCS() && !bWCS)//在当前UCS中定义
	{
		//获取前一个UCS
		ScUcs *pPreUcs = CurrentUCS();
		//从ucs转换到世界
		gp_Ax2 uax = pScUcs->m_ax2;
		uax.Transform(pPreUcs->m_utowTrsf);
		pScUcs->SetUCS(uax);
	}

	m_ucsArray.Add(pScUcs);
	this->m_nCurIdx = m_ucsArray.GetCount() - 1;
}

	//获取当前
ScUcs*	ScUcsMgr::CurrentUCS()
{
	if(IsWCS())
		return NULL;

	return m_ucsArray.GetAt(m_nCurIdx);
}

//从AisCtx中移出临时object，并释放内存。这里关键是释放内存。
void	ScUcsMgr::RemoveCtxObj(Handle(AIS_InteractiveObject)& aObj,BOOL bUpdate)
{
	if(!aObj.IsNull())
	{
		m_AISContext->Remove(aObj,bUpdate);
		//释放内存，很重要.因为如果aObj是类的属性时，引用将一直存在，不释放，
		//频繁创建临时对象，会导致内存泄漏。
		aObj.Nullify();
	}
}