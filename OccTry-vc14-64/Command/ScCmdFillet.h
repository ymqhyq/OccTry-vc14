#pragma once

#include <afxtempl.h>
#include "ScCommand.h"

class ScFltEdge{
public:
	ScFltEdge() {
		_r1 = _r2 = 0.0;
	}

	TopoDS_Edge			_edge;
	double				_r1,_r2;//�����뾶
};

typedef CList<ScFltEdge *,ScFltEdge *> TFltEdgeList;

class ScCmdFillet : public ScCommand
{
public:
	ScCmdFillet(BOOL bConstRadius);
	~ScCmdFillet(void);

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
	enum{S_SHAPE,S_EDGE,S_R1,S_R2};

	void					RemoveList();
	TopoDS_Shape			BuildNewShape();

protected:
	BOOL					m_bConstRadius;
	Handle(AIS_Shape)		m_aShapeObj;
	TopoDS_Shape			m_aOldShape;//����ǰ�Ķ���
	TFltEdgeList			m_aEdgeList;
};
