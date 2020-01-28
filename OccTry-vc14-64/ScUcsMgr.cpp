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

//����UCS
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
// ��ʾ��������ϵ
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
// ����ǰ�Ǿֲ�����ϵʱ����ʾ��������ʾ��
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
// ���������ֻ��ʾһ������ϵ
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

//�л���WCS
void	ScUcsMgr::SwitchToWCS()
{
	//��¼ǰһ��
	this->m_pPreUCS = this->CurrentUCS();
	this->m_nCurIdx = -1;
}

void	ScUcsMgr::SwitchToPrevUCS()
{
	//WCS�ǵ�һ����
	if(IsWCS())
		return;

	//��¼ǰһ��
	this->m_pPreUCS = this->CurrentUCS();
	ASSERT(this->m_nCurIdx >= 0);
	this->m_nCurIdx --;
}

void	ScUcsMgr::SwitchToNextUCS()
{
	if(this->m_nCurIdx >= this->m_ucsArray.GetCount() - 1)
		return; //���һ��

	//��¼ǰһ��
	this->m_pPreUCS = this->CurrentUCS();
	//
	this->m_nCurIdx ++;
}

//�Ƿ��оֲ�����ϵ
BOOL	ScUcsMgr::HasUCS()
{
	return (m_ucsArray.GetCount() > 0);
}

BOOL	ScUcsMgr::IsWCS()
{
	return (this->m_nCurIdx < 0);
}

//
// ���һ��,����Ϊ��ǰ��
// �����ǰucsʵ��ǰһ��ucs�Ļ����϶���ģ�����Ҫ����ucs��ǰһ��ucs��
// ת��WCS���ټ�¼������������֤ÿ��ucs�м�¼�Ķ���wcs�е����꣬����ת����
//
void	ScUcsMgr::AddUCS(ScUcs *pScUcs,BOOL bWCS)
{
	//��¼ǰһ��
	this->m_pPreUCS = this->CurrentUCS();

	if(!IsWCS() && !bWCS)//�ڵ�ǰUCS�ж���
	{
		//��ȡǰһ��UCS
		ScUcs *pPreUcs = CurrentUCS();
		//��ucsת��������
		gp_Ax2 uax = pScUcs->m_ax2;
		uax.Transform(pPreUcs->m_utowTrsf);
		pScUcs->SetUCS(uax);
	}

	m_ucsArray.Add(pScUcs);
	this->m_nCurIdx = m_ucsArray.GetCount() - 1;
}

	//��ȡ��ǰ
ScUcs*	ScUcsMgr::CurrentUCS()
{
	if(IsWCS())
		return NULL;

	return m_ucsArray.GetAt(m_nCurIdx);
}

//��AisCtx���Ƴ���ʱobject�����ͷ��ڴ档����ؼ����ͷ��ڴ档
void	ScUcsMgr::RemoveCtxObj(Handle(AIS_InteractiveObject)& aObj,BOOL bUpdate)
{
	if(!aObj.IsNull())
	{
		m_AISContext->Remove(aObj,bUpdate);
		//�ͷ��ڴ棬����Ҫ.��Ϊ���aObj���������ʱ�����ý�һֱ���ڣ����ͷţ�
		//Ƶ��������ʱ���󣬻ᵼ���ڴ�й©��
		aObj.Nullify();
	}
}