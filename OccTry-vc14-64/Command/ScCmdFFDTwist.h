#pragma once

#include "ScCommand.h"

class ScCmdFFDTwist : public ScCommand
{
public:
	ScCmdFFDTwist(void);
	~ScCmdFFDTwist(void);

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
	enum{S_OBJ,S_AX_BEGIN,S_AX_END,S_BASEDIR,S_ANGLE};

	BOOL					HasSelObj();
	void					ShowAxis(const gp_Pnt& pnt);
	void					ShowBaseVec(const gp_Pnt& pnt);

	BOOL					DoTwist(BOOL bTmp);

	BOOL					CalcRotatePlnPoint(int nx,int ny,BOOL bBase);
	
	gp_Pnt					m_pntBegin,m_pntEnd;
	gp_Pnt					m_basePnt,m_newPnt;
	gp_Vec					m_baseVec;

protected:
	gp_Pnt					m_prePnt;//ǰһ���������
	double					m_rotrad;//��ת�Ƕ�
};
