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
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

		//接收浮点数
		virtual   int						AcceptInput(double dVal);

protected:
		enum{S_WIRE,S_TOL};

		BOOL								HasSelWire();
		virtual  BOOL					FixShape();

		double								m_dFixTol;
};

///////////////////////////////////////////////////////////////////////
// wire的闭合修正
class ScCmdWireCloseFix : public ScCmdWireFixBase{
public:

protected:
		virtual  BOOL					FixShape();
};

/////////////////////////////////////////////////////////////////////////
// wire的修正，使用shapeFix_wire类的相应功能.
class ScCmdWireFix : public ScCmdWireFixBase{
public:

protected:
		virtual  BOOL					FixShape();
};

/////////////////////////////////////////////////////////////////////////
// wire的缝隙的修正
class ScCmdWireFixGaps : public ScCmdWireFixBase{
public:

protected:
		virtual  BOOL					FixShape();
};