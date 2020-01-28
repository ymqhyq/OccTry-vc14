#pragma once

#include "ScCommand.h"

class ScCmdGeom
{
public:
	ScCmdGeom(void);
	~ScCmdGeom(void);
};

//////////////////////////////////////////////
// ����ɨ�ӡ�
// ʹ����GeomFill_Pipe
class ScCmdSweepOnePath : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void						InputEvent(const CPoint& point,int nState,//����minput����
													const Handle(V3d_View)& aView);

	//���ո�����
	virtual   int						AcceptInput(double dVal);
	
protected:
	enum{S_PATH,S_CURVE,S_TYPE};

	TopoDS_Shape			BuildNewShape();

	int						m_nType;
	int						m_nC;
};

///////////////////////////////////////////////////
// ��ʾ���ߵ�����Բ
// ʹ��GeomLProp_CLProps��
class ScCmdCurveCurvatureCirc : public ScCommand{
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
	enum{S_CURVE,S_CIRC};

	TopoDS_Shape			MakeCurvatureCirc(const CPoint& point);

	TopoDS_Shape			m_aShape;//ѡ�е����ߣ���Ϊedge��wire��
};

////////////////////////////////////////////////////////
// ��������߹���
// ʹ��Adaptor3d_IsoCurve����ع���
// ���ܻ���Ҫ��һ�����ƣ�Ŀǰ��ʾ�������ߣ����ǲü��������
class ScCmdIsoCurve : public ScCommand{
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

	//��������
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_SURF,S_UV,S_CURVE};

	void					MakeIsoCurve(BOOL bDone,const CPoint& point,const Handle(V3d_View)& aView);
	TopoDS_Edge				GetIsoCurve(const Adaptor3d_IsoCurve& isoC);

	int						m_nUV;//0 :U 1: V 2:UV
};

///////////////////////////////////////////////////////
// bspline curve�Ŀ��Ƶ�༭����
// ʹ�û�����bspline�Ĺ���
// ���Ƕ�trimmed��curve�Ĵ������ת����������curve������༭��
class ScCmdBSplCurveEdit : public ScCommand{//�򵥵Ļ���
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
	BOOL					GetBSplineCurve(const TopoDS_Shape& aS);
	virtual   void			ShowControlPoints();

	//ѡ���
	virtual   BOOL			SelectPoint(const CPoint& point);
	//�ı��
	virtual   BOOL			OnChangePoint(const CPoint& point);
	//
	virtual   BOOL			OnPointChanged(const CPoint& point);

protected:
	enum{S_CURVE,S_POINT,S_MOVING};

	BOOL					m_bChanged;
	Handle(Geom_BSplineCurve)		m_bsplCur;//bspline����

	CString					m_szPrompt1;
};

class ScCmdBSplCurveEditByCtrlPnt : public ScCmdBSplCurveEdit{
public:

protected:
	virtual   void			ShowControlPoints();

	//ѡ���
	virtual   BOOL			SelectPoint(const CPoint& point);
	//�ı��
	virtual   BOOL			OnChangePoint(const CPoint& point);
//
	virtual   BOOL			OnPointChanged(const CPoint& point);

protected:
	int						m_nPoleIdx;
};

///////////////////////////////////////////////////////
// bspline curve�������ϵ�༭����
// ʹ�û�����bspline��MovePoint����
class ScCmdBSplCurveEditByCurvPnt : public ScCmdBSplCurveEdit{
public:

protected:
	virtual   void			ShowControlPoints();
	//ѡ���
	virtual   BOOL			SelectPoint(const CPoint& point);
	//�ı��
	virtual   BOOL			OnChangePoint(const CPoint& point);
//
	virtual   BOOL			OnPointChanged(const CPoint& point);

	void					PoleRange(int& i1,int& i2);

protected:
	double					m_t;
	gp_Pnt					m_pickPnt;
};