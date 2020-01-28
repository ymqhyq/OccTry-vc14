#pragma once

#include "ScCommand.h"

class ScCmdAnalysis
{
public:
	ScCmdAnalysis(void);
	~ScCmdAnalysis(void);
};

//////////////////////////////////////////////////////////////////////////
// ���߳���
class ScCmdCurveLength : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
								const Handle(V3d_View)& aView);

protected:
	enum{S_CURVE};

	BOOL					HasSelObj();
	BOOL					GetLength();
};