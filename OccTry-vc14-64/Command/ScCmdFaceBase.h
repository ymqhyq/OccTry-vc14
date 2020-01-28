#pragma once

#include "ScCommand.h"

///////////////////////////////////////////////////////////////////////
/*
*  ��ѡ��һ��Face�������ļ�¼���ṩ��ѡ��һ��face�Ĺ��ܣ������ѡ��
*  ��face���м򵥵Ĳ�����
*  face����ѡ�񵥶������棬Ҳ������solid��shell���е�һ��face��
*/
class ScCmdFaceBase : public ScCommand
{
public:
	ScCmdFaceBase(void);
	~ScCmdFaceBase(void);

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	//һЩ������ʵ�֡�
	virtual BOOL			HasSelFace();//�Ƿ�ѡ����face
	virtual BOOL			HasSelShape();//�Ƿ�ѡ����shape.
	virtual BOOL			BuildShape();//���ɶ�Ӧshape��
	virtual BOOL			BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace);

	virtual BOOL			OpenFaceLocalCtx();
	virtual BOOL			CloseFaceLocalCtx();

protected:
	enum{S_SHAPE,S_FACE}; //ѡ�����ѡ�����档

	TopoDS_Face				m_aFace;//ѡ�е�Face��
	BOOL					m_bLocalCtx;
};
