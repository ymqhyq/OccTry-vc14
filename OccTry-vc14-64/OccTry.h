// OccTry.h : OccTry 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号
//#include <Graphic3d_WNTGraphicDevice.hxx>

// COccTryApp:
// 有关此类的实现，请参阅 OccTry.cpp
//

class COccTryApp : public CWinApp
{
public:
	COccTryApp();
//yxk
	//Handle_Graphic3d_WNTGraphicDevice GetGraphicDevice3D() const { return m_graphicDevice3D; } ;
	Handle(Graphic3d_GraphicDriver) GetGraphicDevice3D() const { return m_graphicDevice3D; }
//endyxk

// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

protected:
//yxk20191208
	//Handle_Graphic3d_WNTGraphicDevice m_graphicDevice3D;//3d显示设备
	Handle(Graphic3d_GraphicDriver) m_graphicDevice3D;//3d显示设备
//endyxk
};

extern COccTryApp theApp;
