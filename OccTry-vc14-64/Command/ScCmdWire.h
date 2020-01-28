#pragma once

#include "ScCommand.h"

class ScCmdWire
{
public:
		ScCmdWire(void);
		~ScCmdWire(void);
};

class ScCmdWireFixBase : public ScCommand{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

		//���ո�����
		virtual   int						AcceptInput(double dVal);

protected:
		enum{S_WIRE,S_TOL};

		BOOL								HasSelWire();
		virtual  BOOL					FixShape();

		double								m_dFixTol;
};

///////////////////////////////////////////////////////////////////////
// wire�ıպ�����
class ScCmdWireCloseFix : public ScCmdWireFixBase{
public:

protected:
		virtual  BOOL					FixShape();
};

/////////////////////////////////////////////////////////////////////////
// wire��������ʹ��shapeFix_wire�����Ӧ����.
class ScCmdWireFix : public ScCmdWireFixBase{
public:

protected:
		virtual  BOOL					FixShape();
};

/////////////////////////////////////////////////////////////////////////
// wire�ķ�϶������
class ScCmdWireFixGaps : public ScCmdWireFixBase{
public:

protected:
		virtual  BOOL					FixShape();
};