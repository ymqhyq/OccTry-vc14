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

	//��Ļ��ת��Ϊ�ռ��
	static gp_Pnt	ConvertClickToPoint(Standard_Real x, Standard_Real y, 
										Handle(V3d_View) aView);

	//��ʾ��ʾ��Ϣ
	static void		Prompt(LPCTSTR lpszText);

	//����shape�������������ٶ�shape�Ѿ���mesh�����ˡ�
	static TopoDS_Shape		BuildShapeMesh(const TopoDS_Shape& aShape,double defle);

	//��ȡface�Ĳü�����Ϣ,0:�ɹ������򷵻�ʧ�ܵı�ʶ��
	static int				FaceTrmLoopInfo(const TopoDS_Face& aFace,CStringList& lineList,
								int nLevel = 0,//���ؽ�� 0:��ϸ��Ϣ 1: ������ 2:���ش���������Ҫ��gap����
								double dTol = 0.001,double dPTol = 0.001);//���ģʽ�µ���

	static char*			ShapeTypeName(TopAbs_ShapeEnum eType);

	//��ȡbnd_box�İ˸��ǵ�
	static BOOL				GetBoxCorner(const Bnd_Box& bb,TColgp_SequenceOfPnt& pnts);

};
