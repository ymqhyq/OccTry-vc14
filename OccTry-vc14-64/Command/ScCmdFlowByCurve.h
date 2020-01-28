#pragma once

#include "ScCommand.h"

class ScCmdFlowByCurve : public ScCommand
{
public:
	ScCmdFlowByCurve(void);
	~ScCmdFlowByCurve(void);

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void						InputEvent(const CPoint& point,int nState,//����minput����
		const Handle(V3d_View)& aView);


protected:
	enum{S_OBJ,S_DATUM_CURVE,S_TAR_CURVE};

	BOOL				HasSelObj();
	BOOL				HasSelDatumCurve(int nx,int ny);
	BOOL				HasSelTarCurve(int nx,int ny);

	BOOL				DoFlow();

	gp_Pnt				m_datumPnt;
	double				m_datumT;
	gp_Pnt				m_tarPnt;
	double				m_tarT;
};
