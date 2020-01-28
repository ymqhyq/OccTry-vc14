#pragma once

#include "ScCommand.h"

class ScCmdShear : public ScCommand
{
public:
	ScCmdShear(void);
	~ScCmdShear(void);

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void						InputEvent(const CPoint& point,int nState,//����minput����
		const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
		const Handle(V3d_View)& aView);

protected:
	enum{S_OBJ,S_BASEPNT,S_BASEDIR,S_NEWDIR};

	BOOL					HasSelObj();
    BOOL					ShowDirLine(const gp_Pnt& pnt);
	BOOL					DoShear(BOOL bTmp);

	gp_Pnt					m_basePnt;
	gp_Vec					m_baseDir;
	gp_Vec					m_newDir;
};
