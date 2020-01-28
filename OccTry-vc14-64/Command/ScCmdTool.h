#pragma once

#include "ScCommand.h"
#include "ScCmdCurve.h"
#include "ScCmdFaceBase.h"

class ScCmdTool
{
public:
	ScCmdTool(void);
	~ScCmdTool(void);
};

/////////////////////////////////////////////////////////
// ��ȡFace�Ĳü�����Ϣ
//
class ScCmdFaceTrmLoopInfo : public ScCommand{
public:
	//���������ʼִ�С�
	virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
	//�������ͨ��������ֹ���
	virtual   int						End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void						InputEvent(const CPoint& point,int nState,//����minput����
													const Handle(V3d_View)& aView);

protected:
	enum{S_SHAPE,S_INFO};

	BOOL						HasSelShape();

	TopoDS_Face					m_aFace;

};

//////////////////////////////////////////////////////////
// ��face����3d���������µĲ������ߡ�
// ��3d���ߺͲ������߲�ƥ���ʱ��
class ScCmdToolCurve2d : public ScCommand{
public:
	//���������ʼִ�С�
	virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
	//�������ͨ��������ֹ���
	virtual   int						End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void						InputEvent(const CPoint& point,int nState,//����minput����
													const Handle(V3d_View)& aView);

protected:
	enum{S_SHAPE};

	BOOL						HasSelShape();
	void						BuildCurve2d();
};



///////////////////////////////////////////////////////////
// ����߽����ʸ��ʾ��
class ScCmdFaceBndTang : public ScCmdFaceBase{
public:
	//���������ʼִ�С�
	virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
	//�������ͨ��������ֹ���
	virtual   int						End();

protected:
	virtual	  BOOL						BuildShape();//���ɶ�Ӧshape��

	BOOL								m_bBuilded;//�Ƿ񴴽���
};

//////////////////////////////////////////////////////////
// ����ķ�ʸ
class ScCmdFaceNormal : public ScCmdFaceBase{
public:
	//���������ʼִ�С�
	virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
	//�������ͨ��������ֹ���
	virtual   int						End();

protected:
	virtual	  BOOL						BuildShape();//���ɶ�Ӧshape��

	BOOL								m_bBuilded;//�Ƿ񴴽���
};

//////////////////////////////////////////////////////////
// ���ߵķ�ʸ����ʾ
class ScCmdCurveTang : public ScCommand{
public:
		//���������ʼִ�С�
		virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
		//�������ͨ��������ֹ���
		virtual   int						End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void						InputEvent(const CPoint& point,int nState,//����minput����
				const Handle(V3d_View)& aView);

protected:
		enum{S_SELCURVE};
		BOOL										HasSelCurve();
		virtual	  BOOL						BuildShape();//���ɶ�Ӧshape��

		BOOL								m_bBuilded;//�Ƿ񴴽���
};

/////////////////////////////////////////////////////////////////
// ������Ļ���
class ScCmdCheckBase : public ScCommand{
public:
		//���������ʼִ�С�
		virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
		//�������ͨ��������ֹ���
		virtual   int						End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void						InputEvent(const CPoint& point,int nState,//����minput����
				const Handle(V3d_View)& aView);

protected:
		BOOL										HasSelObject();
		virtual     BOOL						Check();
};

class ScCmdCheckValid : public ScCmdCheckBase{
public:
protected:
		virtual     BOOL						Check();
};

class ScCmdCheckTopoValid : public ScCmdCheckBase{
public:
protected:
		virtual     BOOL						Check();
};

class ScCmdCheckClosed : public ScCmdCheckBase{
public:
protected:
		virtual     BOOL						Check();
};