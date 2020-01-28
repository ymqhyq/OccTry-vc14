#pragma once

#include "ScCommand.h"

class ScCmdTrsf
{
public:
	ScCmdTrsf(void);
	~ScCmdTrsf(void);
};

// ����ƽ�ơ�
//
class ScCmdObjMoving : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SELECT,S_MOVING};

	TopoDS_Shape			BuildCopy(const CPoint& point);

protected:
	TopoDS_Shape			m_aSelObj;
	int						m_nX,m_nY;
	gp_Pnt					m_pntStart;
};

//����clone.
class ScCmdObjClone : public ScCommand{
public:
	ScCmdObjClone(BOOL bMulti = FALSE);

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SELECT,S_MOVING};

	TopoDS_Shape			BuildClone(const CPoint& point);

protected:
	TopoDS_Shape			m_aSelObj;
	int						m_nX,m_nY;
	gp_Pnt					m_pntStart;
	BOOL					m_bMulti;//�Ƿ��ظ�clone��
};