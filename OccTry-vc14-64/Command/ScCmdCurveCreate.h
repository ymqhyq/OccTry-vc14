/*
* �������ߵĴ�������.��ȡ�ṩ�Ƚ����Ƶ����ߴ���.
*
*
*
*/
#pragma once

#include "ScCommand.h"

class ScCmdCurveCreate
{
public:
	ScCmdCurveCreate(void);
	~ScCmdCurveCreate(void);
};

//ֱ��
class ScCmdLine : public ScCommand{
public:
	ScCmdLine();
	~ScCmdLine();

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
	//�ڲ�״̬
	enum{ LINE_BEGIN,LINE_POINT1,LINE_POINT2,LINE_END};

private:
	Handle(AIS_Shape)		m_aAIShape;//����Ҳ����ʱ����
	gp_Pnt					m_pnt1;//��һ����
};

//�����
class ScCmdPolyLine : public ScCommand{
public:
	ScCmdPolyLine();
	~ScCmdPolyLine();

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
	//�ڲ�״̬
	enum{ LINE_BEGIN,LINE_POINT_FIRST,LINE_POINT_NEXT,LINE_END};

	virtual  TopoDS_Shape	BuildNewShape();

protected:
	Handle(AIS_Shape)		m_aAIShape;//����Ҳ����ʱ����
	Handle(AIS_Shape)		m_aTmpObj;//��ʱ����
	gp_Pnt					m_pntPrev;//ǰһ����
	TColgp_SequenceOfPnt	m_seqOfPnt;//����
//	TopTools_ListOfShape    m_shapeList;
};

//Բ
class ScCmdCircle : public ScCommand{
public:
	ScCmdCircle();
	~ScCmdCircle();

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
	enum{ CIRCLE_CENTER,CIRCLE_RADIUS };

protected:
	Handle(AIS_Shape)		m_aShape;
	gp_Pnt					m_ptCenter;
	double					m_dRadius;
	BOOL					m_bFirst;
};

////////////////////////////////////////////////////
// ����Բ
class ScCmdCircleOf3Point : public ScCommand{
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

	//����3ά��
	virtual   int			AcceptInput(gp_Pnt& pnt) ;

protected:
	enum{S_POINT1,S_POINT2,S_POINT3};

	BOOL							MakeTmpLine();
	BOOL							MakeCirc(BOOL bDone);

	gp_Pnt					m_pnt1,m_pnt2,m_pnt3;
	
};

// ͬ��Բ/����
class ScCmdCircleByDist : public ScCommand{
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

	//���ո�����
	virtual   int			AcceptInput(double dVal) { return SC_OK; }
	
protected:
	enum{S_CIRCLE,S_DIST};

	double					m_dist;
};
// ͬ��Բ/����
class ScCmdCircleByPoint : public ScCommand{
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

	//���ո�����
	virtual   int			AcceptInput(gp_Pnt& pnt) { return SC_OK; }
	
protected:
	enum{S_CIRCLE,S_DIST};

	gp_Pnt					m_pnt;
};

//Բ����Բ��\�뾶\�Ƕ�
class ScCmdArc : public ScCommand{
public:
	ScCmdArc();
	~ScCmdArc();

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
	enum{ARC_CENTER,ARC_RADIUS,ARC_ANGLE};

	TopoDS_Edge				BuildLine();
	TopoDS_Shape			BuildArc();
	
protected:
	Handle(AIS_Shape)		m_aShape;
	gp_Pnt					m_ptC,m_pt1,m_pt2;
};

////////////////////////////////////////////////////
// Բ��/����
class ScCmdArcOfTwoPoint : public ScCommand{
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

	//���ո�����
	virtual   int			AcceptInput(gp_Pnt& pnt) ;

protected:
	enum{S_CENTER,S_POINT1,S_POINT2};

	BOOL							MakeLine();
	BOOL							MakeArc(BOOL bDone);

	gp_Pnt						m_ptC,m_pt1,m_pt2;
};

////////////////////////////////////////////////////
// ����,ǰ���������Բ���ķ�Χ.
class ScCmdArcOfThreePoint : public ScCommand{
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

	//���ո�����
	virtual   int			AcceptInput(gp_Pnt& pnt) { return SC_OK; }

protected:
	enum{S_POINT1,S_POINT2,S_POINT3};

};
////////////////////////////////////////////////////
// Բ\���\�Ƕ�
class ScCmdArcOfPointAndAngle : public ScCommand{
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

	//���ո�����
	virtual   int			AcceptInput(gp_Pnt& pnt) { return SC_OK; }

	//���ո�����
	virtual   int			AcceptInput(double dVal) { return SC_OK; }

protected:
	enum{S_CENER,S_POINT,S_ANGLE};

};

////////////////////////////////////////////////////
// ��Բ�Խ�
class ScCmdEllipseOfCorner : public ScCommand{
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

	//���ո�����
	virtual   int			AcceptInput(gp_Pnt& pnt) ;

protected:
	enum{S_POINT1,S_POINT2};

	//�������γɹ�
	BOOL					BuildShape(TopoDS_Shape& aShape);

	gp_Pnt					m_pnt1,m_pnt2;
};

////////////////////////////////////////////////////
// ��Բ����/��/����
class ScCmdEllipseOfRedius : public ScCommand{
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

	//���ո�����
	virtual   int			AcceptInput(gp_Pnt& pnt) { return SC_OK; }

	//���ո�����
	virtual   int			AcceptInput(double dVal) { return SC_OK; }
};

////////////////////////////////////////////////////
// ����/����
class ScCmdEllipseOfTwoPoint : public ScCommand{
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

	//���ո�����
	virtual   int			AcceptInput(gp_Pnt& pnt) { return SC_OK; }

	//���ո�����
	virtual   int			AcceptInput(double dVal) { return SC_OK; }
};

////////////////////////////////////////////////
// ��Բ�Ƕ�

////////////////////////////////////////////////
// ��Բ\��\�Ƕ�

////////////////////////////////////////////////
// ��Բ\����

////////////////////////////////////////////////
// ����
class ScCmdRectangle : public ScCommand{
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

	//���ո�����
	virtual   int			AcceptInput(gp_Pnt& pnt) ;

protected:
	enum{S_POINT1,S_POINT2};

	//�������γɹ�
	BOOL					BuildRectangle(TopoDS_Shape& aShape);

	gp_Pnt					m_pnt1,m_pnt2;
};

//��������
class ScCmdSpline : public ScCommand{
public:
	ScCmdSpline();
	~ScCmdSpline();

	enum{BSpline_Interp,BSpline_Approx};//��ϡ��ƽ�
	void					SetSplType(int nType);

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
	//��������
	Handle(Geom_BSplineCurve)	BuildBSpline();

protected:
	enum{SPL_FIRST_POINT,SPL_NEXT_POINT};

	int						m_nType;//����
	TColgp_SequenceOfPnt	m_seqOfPnt;//����
	AIS_SequenceOfInteractive m_seqOfObj;//����ʾ����
	Handle(AIS_Shape)		m_aShape;//��ʱ����
};


//////////////////////////////////////////////////////
// ������εĴ���

//////////////////////////////////////////////////////
// Բ�Ĵ���,����Ƚ϶�

//////////////////////////////////////////////////////
// Բ���Ĵ���

//////////////////////////////////////////////////////
// ��Բ�Ĵ���

//////////////////////////////////////////////////////
// ��Բ���Ĵ���

//////////////////////////////////////////////////////////////////////////
// Bezier���ߵĴ���
class ScCmdBezierCurve : public ScCommand{
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
	enum{S_FIRST_PNT,S_NEXT_PNT};

	BOOL					MakeCurve(BOOL bTmp);
	BOOL					ShowFrame();

	TColgp_SequenceOfPnt		m_seqOfPnt;//����
	AIS_SequenceOfInteractive	m_seqOfObj;//����ʾ����
};


