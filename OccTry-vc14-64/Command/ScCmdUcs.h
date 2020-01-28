#pragma once

#include "ScCommand.h"

class ScCmdUcs
{
public:
	ScCmdUcs(void);
	~ScCmdUcs(void);
};

class ScCmdUcsPrev : public ScCommand{
public:
	ScCmdUcsPrev();
	~ScCmdUcsPrev();

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

};

class ScCmdUcsNext : public ScCommand{
public:
	ScCmdUcsNext();
	~ScCmdUcsNext();

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();
};

class ScCmdUcsModel : public ScCommand{
public:
	ScCmdUcsModel();
	~ScCmdUcsModel();

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();
};

class ScCmdUcsOrg : public ScCommand{
public:
	ScCmdUcsOrg();
	~ScCmdUcsOrg();

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//����3ά��
	virtual   int			AcceptInput(gp_Pnt& pnt);

protected:
	Handle(AIS_Trihedron)		m_aShape;
};

class ScCmdUcsHeight : public ScCommand{
public:
	ScCmdUcsHeight();
	~ScCmdUcsHeight();

		//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//���ո�����
	virtual   int			AcceptInput(double dVal);

protected:
	int						MakeNewUCS(double dVal);
	gp_Ax2					BuildNewAx2(double dVal);
	double					ComputeHeight(const gp_Pnt& pnt);

protected:
	Handle(AIS_Trihedron)		m_aShape;
};

class ScCmdUcs3Point : public ScCommand{
public:
	ScCmdUcs3Point();
	~ScCmdUcs3Point();

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//����3ά��
	virtual   int			AcceptInput(gp_Pnt& pnt);

protected:
	void					DoAcceptPoint(const gp_Pnt& pnt);

protected:
	enum{ U3P_ORG,U3P_ZPOINT,U3P_UPOINT};

	Handle(AIS_Shape)			m_aShape;
	Handle(AIS_Trihedron)		m_aTriShape;

	gp_Pnt						m_ptOrg,m_ptZ,m_ptU;
};

//////////////////////////////////////////////////////////////
// һ���򵥵Ļ���
class ScCmdUcsObjPoint : public ScCommand{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);


protected:
	enum{S_SHAPE,S_POINT};

	//�Ƿ���Ҫ�Ķ���
	virtual BOOL			IsRequiredShape(const TopoDS_Shape& aShape);
	//�������������Ӧ������ϵ
	virtual BOOL			BuildAxis(gp_Ax2& ax2);

	virtual void			ShowTrihedron(const gp_Ax2& ax2);
	virtual void			BuildTrihedron(const gp_Ax2& ax2);

protected:
	Handle(AIS_Trihedron)	m_aTriShape;
	Handle(AIS_InteractiveObject)	m_aSelObj;
	TopoDS_Shape			m_aShape;
	int						m_nX,m_nY;

	Handle(V3d_View)		m_aView;
};

//���������ϵĵ�
class ScCmdUcsCurvePoint : public ScCmdUcsObjPoint{
public:
	
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);

protected:
	//�Ƿ���Ҫ�Ķ���
	virtual BOOL			IsRequiredShape(const TopoDS_Shape& aShape);
	//�������������Ӧ������ϵ
	virtual BOOL			BuildAxis(gp_Ax2& ax2);
};

//���������ϵĵ�
class ScCmdUcsFacePoint : public ScCmdUcsObjPoint{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);

protected:
	//�Ƿ���Ҫ�Ķ���
	virtual BOOL			IsRequiredShape(const TopoDS_Shape& aShape);
	//�������������Ӧ������ϵ
	virtual BOOL			BuildAxis(gp_Ax2& ax2);
};

//����shape�ϵĵ�
class ScCmdUcsShapePoint : public ScCmdUcsObjPoint{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);

protected:
	//�Ƿ���Ҫ�Ķ���
	virtual BOOL			IsRequiredShape(const TopoDS_Shape& aShape);
	//�������������Ӧ������ϵ
	virtual BOOL			BuildAxis(gp_Ax2& ax2);
};