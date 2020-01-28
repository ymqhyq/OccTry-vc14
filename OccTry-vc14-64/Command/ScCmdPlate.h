//
// ʹ��plate��ص��㷨�����������桢�����桢��Լ���Ĺ����桢����
//
#pragma once

#include "ScCommand.h"

class ScCmdPlate : public ScCommand
{
public:
	ScCmdPlate(void);
	~ScCmdPlate(void);

	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();
};

/////////////////////////////////////////////////////////////
// �ӵ�����������
class ScCmdPlatePointCloud : public ScCmdPlate{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	TopoDS_Shape			BuildSurface(const TColgp_SequenceOfXYZ& seqOfXYZ);
};

class ScCmdPlateBlend : public ScCmdPlate{
public:
	ScCmdPlateBlend(BOOL bWithCont);

	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SHAPE1,S_EDGE1,S_SHAPE2,S_EDGE2,S_CONT};

	TopoDS_Face				BuildBlendFace();

protected:
	TopoDS_Face			m_aFace1;
	TopoDS_Edge			m_aEdge1;
	TopoDS_Face			m_aFace2;
	TopoDS_Edge			m_aEdge2;//ѡ���face��edge��
	TopoDS_Edge			m_aContEdge;//����

	BOOL				m_bWithCont; //����Լ��
};

class ScCmdPlateBlendWithCont : public ScCmdPlate{
public:

	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);
};

////////////////////////////////////////////////////////////
// N�߲����㷨��
//
class ScCmdPlateFillHole : public ScCmdPlate{
public:
	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SHAPE,S_EDGES};

	TopoDS_Face				FillHole();

protected:
	Handle(AIS_Shape)		m_aShapeObj;

};