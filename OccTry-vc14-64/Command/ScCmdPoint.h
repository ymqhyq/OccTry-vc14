#pragma once

#include "ScCommand.h"

//class ScCmdPoint
//{
//public:
//	ScCmdPoint(void);
//	~ScCmdPoint(void);
//};

//////////////////////////////////////////////////////////////////////////
// ��ͶӰ������
class ScCmdPointPrjToCurve : public ScCommand{
public:

	//���������ʼִ�С�
	virtual	  int					Begin(Handle(AIS_InteractiveContext) aCtx);
	//�������ͨ��������ֹ���
	virtual   int					End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void					InputEvent(const CPoint& point,int nState,//����minput����
		const Handle(V3d_View)& aView);

protected:
	enum{S_CURVE,S_POINT};

	BOOL							HasSelCurve();
	BOOL							DoPrj(BOOL bTmp);

	gp_Pnt							m_selPnt;
};