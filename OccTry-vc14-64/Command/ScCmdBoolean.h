#pragma once

#include "ScCommand.h"

class ScCmdBoolean
{
public:
		ScCmdBoolean(void);
		~ScCmdBoolean(void);
};

//boolean����
class ScCmdBool : public ScCommand{
public:
		ScCmdBool();

		//�����ͽ�������
		virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int	End();

public:
		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_SHAPE1,S_SHAPE2};

		BOOL						HasSelObj1();
		BOOL						HasSelObj2();

		virtual   void			MakeBoolean();
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

///////////////////////////////////////////////////////////////////////////////////////
// �µ�bool�㷨,BRepAlgoAPI���㷨
class ScCmdBoolCommon : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

class ScCmdBoolFuse : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

class ScCmdBoolCut : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

///////////////////////////////////////////////////////////////////////////////////////
// �ɵ�bool�㷨,BRepAlgo���㷨
class ScCmdBoolCommonOld : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

class ScCmdBoolFuseOld : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

class ScCmdBoolCutOld : public ScCmdBool{
public:
		virtual   TopoDS_Shape  BuildNewShape(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2);
};

//////////////////////////////////////////////////////////////////////////////////////
// ��������м���
class ScCmdBoolCommonSections : public ScCmdBool{
public:
protected:
		virtual   void			MakeBoolean();
};

class ScCmdBoolCommonGenObjs : public ScCmdBool{
public:
		ScCmdBoolCommonGenObjs(BOOL bFirst);

protected:
		virtual   void			MakeBoolean();

		BOOL						m_bFirst;
};
