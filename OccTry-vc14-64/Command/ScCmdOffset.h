#pragma once

#include "ScCommand.h"


//����
class ScCmdLoft : public ScCommand{
public:
	ScCmdLoft(BOOL bSolid);

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

protected:
	enum{S_CURVES};

	TopoDS_Shape			BuildShape();

protected:
	BOOL					m_bSolid;//�Ƿ�����ʵ��
	Handle(TopTools_HSequenceOfShape) m_aSeqOfShape;//��¼edge��wire�б�

};

//���
class ScCmdThickSolid : public ScCommand{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//���ո�����
	virtual   int			AcceptInput(double dVal);

protected:
	enum{S_SHAPE,S_FACE,S_THICK};

	TopoDS_Shape			BuildNewShape();

protected:
	Handle(AIS_Shape)		m_aSelObj;
	TopoDS_Face				m_aFace;
	double					m_dThick;//��Ǻ��
};

class ScCmdOffset : public ScCommand{
};

////////////////////////////////////////////////////////////
// ƫ��shape
// ʹ��BRepOffsetAPI_MakeOffsetShape��
//
class ScCmdOffsetShape : public ScCommand{
public:
	ScCmdOffsetShape(BOOL bOnlySurf);

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//���ո�����
	virtual   int			AcceptInput(double dVal);

protected:
	enum{S_SHAPE,S_OFFSET};

	TopoDS_Shape			BuildNewShape();

protected:
	Handle(AIS_Shape)		m_aSelObj;
	Handle(AIS_Shape)		m_aNewObj;
	double					m_dOffset;

	BOOL					m_bOnlySurf;//�Ƿ�ֻƫ������
};

class ScCmdPipe : public ScCommand{
};

class ScCmdPipeShell : public ScCmdPipe{
};

//
// ɨ���㷨���ж������ͣ�ÿ�ֵĴ���������������
// �����ϣ�a��ѡ���� b��ѡ��������� 
//         �����Ҫ��������Ҫѡ��һ�����졣
//
class ScCmdBRepSweep : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SPINE,S_PROFILES,S_GUIDE};

	TopoDS_Shape			BuildNewShape();

protected:
	Handle(AIS_Shape)			m_aNewObj;
	Handle(AIS_InteractiveObject)	m_aSpineObj;
	AIS_SequenceOfInteractive	m_seqOfObj;
	TopoDS_Wire					m_awSpine;//����
	TopoDS_Wire					m_awGuide;//����
	int							m_nType;
	gp_Ax2						m_ax2;
	gp_Vec						m_biNorm;
};
