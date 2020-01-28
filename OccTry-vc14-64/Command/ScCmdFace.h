#pragma once

#include "ScCommand.h"

class ScCmdFace
{
public:
		ScCmdFace(void);
		~ScCmdFace(void);
};

/////////////////////////////////////////////////////////////////
// Face ����
class ScCmdFaceFixAll : public ScCommand{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		BOOL						HasSelFace();
		virtual  BOOL						FixShape();
};

//////////////////////////////////////////////////////////////////////////
// ����face��wire�ķ�϶
class ScCmdFaceFixGaps : public ScCmdFaceFixAll{
protected:
		virtual  BOOL						FixShape();
};