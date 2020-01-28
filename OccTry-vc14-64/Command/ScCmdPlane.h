#pragma once

#include "ScCommand.h"

//class ScCmdPlane
//{
//public:
//		ScCmdPlane(void);
//		~ScCmdPlane(void);
//};

//////////////////////////////////////////////////////////////////////////
// ��wire��curve���ɵ�ƽ��
class ScCmdPlaneFromWire : public ScCommand{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		BOOL						HasSelCurve();
		BOOL						MakePlane();
};