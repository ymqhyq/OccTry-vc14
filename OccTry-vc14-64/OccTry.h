// OccTry.h : OccTry Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h�� 
#endif

#include "resource.h"       // ������
//#include <Graphic3d_WNTGraphicDevice.hxx>

// COccTryApp:
// �йش����ʵ�֣������ OccTry.cpp
//

class COccTryApp : public CWinApp
{
public:
	COccTryApp();
//yxk
	//Handle_Graphic3d_WNTGraphicDevice GetGraphicDevice3D() const { return m_graphicDevice3D; } ;
	Handle(Graphic3d_GraphicDriver) GetGraphicDevice3D() const { return m_graphicDevice3D; }
//endyxk

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

protected:
//yxk20191208
	//Handle_Graphic3d_WNTGraphicDevice m_graphicDevice3D;//3d��ʾ�豸
	Handle(Graphic3d_GraphicDriver) m_graphicDevice3D;//3d��ʾ�豸
//endyxk
};

extern COccTryApp theApp;
