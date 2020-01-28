#pragma once

#include <V3d_View.hxx>

class ScView;

#define FACE_TRMLOOP_CHECK_ERR  0x01
#define FACE_TRMLOOP_GAP_ERR    0x02
#define FACE_TRMLOOP_FATAL_ERR  0x04

class ScTools
{
public:
	ScTools(void);
	~ScTools(void);

	//屏幕点转换为空间点
	static gp_Pnt	ConvertClickToPoint(Standard_Real x, Standard_Real y, 
										Handle(V3d_View) aView);

	//显示提示信息
	static void		Prompt(LPCTSTR lpszText);

	//构建shape的网格对象。这里假定shape已经有mesh存在了。
	static TopoDS_Shape		BuildShapeMesh(const TopoDS_Shape& aShape,double defle);

	//获取face的裁剪环信息,0:成功，否则返回失败的标识。
	static int				FaceTrmLoopInfo(const TopoDS_Face& aFace,CStringList& lineList,
								int nLevel = 0,//返回结果 0:详细信息 1: 错误结果 2:严重错误结果，主要是gap过大。
								double dTol = 0.001,double dPTol = 0.001);//简洁模式下的误差。

	static char*			ShapeTypeName(TopAbs_ShapeEnum eType);

	//获取bnd_box的八个角点
	static BOOL				GetBoxCorner(const Bnd_Box& bb,TColgp_SequenceOfPnt& pnts);

};
