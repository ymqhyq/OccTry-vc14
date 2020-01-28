#pragma once

#include "ScCommand.h"
#include "ScCmdCurveBase.h"

class ScCmdBSpline
{
public:
	ScCmdBSpline(void);
	~ScCmdBSpline(void);
};

//////////////////////////////////////////////////////////////////////////
// ������,�ṩһЩ�����Ĺ���
class TcBSplLib{
public:

	//�������ߵĿ��Ƶ㵽һ����Ŀ
	static	BOOL			IncCurveCtrlPnts(Handle(Geom_BSplineCurve) &aCur,int ncpts,
								double df,double dl);

	//��������Ŀ��Ƶ㵽һ����Ŀ
	static  BOOL			IncSurfCtrlPnts(Handle(Geom_BSplineSurface) &aSurf,int nucpts,int nvcpts);
};

///////////////////////////////////////////////////////////////////////////
// ����תΪBSpline
class ScCmdCurveToBSpline : public ScCmdCurveBase{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

protected:
	BOOL					DoConvert();
};

//////////////////////////////////////////////////////////////////////////
// ����תΪBSpline
class ScCmdSurfToBSpline : public ScCommand{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

protected:
	BOOL					HasSelSurf();
	BOOL					DoConvert();
};

//////////////////////////////////////////////////////////////////////////
// ʵ��תΪBSpline
class ScCmdSolidToBSpline : public ScCommand{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);


protected:
	BOOL					HasSelSolid();
	BOOL					DoConvert();
};

//////////////////////////////////////////////////////////////////////////
// ������Ƶ�༭

//bezier���ߵĿ��Ƶ�༭
class ScCmdBezierCurveEditByCP : public ScCommand{
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
	enum{S_CURVE,S_SELPNT,S_CHGPNT};

	BOOL					HasSelCurve();
	BOOL					ShowCtrlPnts();
	BOOL					DoEdit(BOOL bTmp,BOOL bDone = FALSE);

	gp_Pnt					m_selPnt;//ѡ�е�
	gp_Pnt					m_chgPnt;//�޸ĺ�ĵ�
	Handle(Geom_BezierCurve)	m_midCur,m_tmpCur;
};

class ScCmdBSplSurfEditByCP : public ScCommand{
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
	enum{S_SURF,S_SELPNT,S_BASPNT,S_CHGPNT};

	BOOL					HasSelSurf();
	BOOL					ShowControlPnts(BOOL bSel);
	BOOL					ChangeSurf(BOOL bTmp);

	BOOL					InChange(const gp_Pnt& pnt);

	TopTools_SequenceOfShape		m_seqOfV;	//ѡ�еĵ�
	Handle(Geom_BSplineSurface)		m_splSurf,m_midSurf;//��ʱ��������м������
	
	gp_Pnt					m_basPnt,m_chgPnt;
	BOOL					m_bChged;
};

//////////////////////////////////////////////////////////////////////////
// ���״���

//////////////////////////////////////////////////////////////////////////
// bezier��������

//////////////////////////////////////////////////////////////////////////
// BSpline��������
class ScCmdBSplCurveIncDegree : public ScCmdCurveBase{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//���ո�����
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_OBJ,S_DEGREE};

	BOOL					HasSelObj();
	BOOL					DoIncDegree(int nDegree);

	int						m_nOldDegree;
};

//////////////////////////////////////////////////////////////////////////
// bspline��������
class ScCmdBSplSurfIncDegree : public ScCommand{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//���ո�����
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_OBJ,S_UDEGREE,S_VDEGREE};

	BOOL					HasSelObj();
	BOOL					DoIncDegree();

	int						m_nOldUDegree,m_nOldVDegree;
	int						m_nUDegree,m_nVDegree;
};

//////////////////////////////////////////////////////////////////////////
// ������
class ScCmdBSplCurveInsertKnots : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//���ո�����
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_CURVE,S_INSERT};

	BOOL					HasSelCurve();
	BOOL					ShowCtrlPnts();
	BOOL					DoInsert(BOOL bDone);

	Handle(Geom_BSplineCurve)	m_cpyCur;//�����Ĺ�������
	double					m_df,m_dl;
	int						m_nFlag;
	gp_Pnt					m_newPt;
};

class ScCmdBSplSurfInsertKnots : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//���ո�����
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_SURF,S_FLAG,S_INSERT};

	BOOL					HasSelSurf();
	BOOL					ShowCtrlPnts();
	BOOL					DoInsert(BOOL bDone);

	Handle(Geom_BSplineSurface)		m_cpySurf;
	
	int						m_nFlag;
	gp_Pnt					m_clkPnt;

};

//////////////////////////////////////////////////////////////////////////
// ���ӿ��Ƶ�
class ScCmdBSplCurveAddCtrlPnts : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//���ո�����
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_CURVE,S_INPUT};

	BOOL					HasSelCurve();
	void					ShowCtrlPnts();
	BOOL					DoAddCtrlPnts(BOOL bDone);

	Handle(Geom_BSplineCurve)	m_cpyCur,m_tmpCur;//�����Ĺ�������
	double					m_df,m_dl;
	int						m_ncpts;//�¿��Ƶ�ĸ���
};

class ScCmdBSplSurfAddCtrlPnts : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//���ո�����
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_SURF,S_UINPUT,S_VINPUT};

	BOOL					HasSelSurf();
	void					ShowCtrlPnts();
	BOOL					DoAddCtrlPnts(BOOL bDone);

	Handle(Geom_BSplineSurface)		m_cpySurf,m_tmpSurf;
	int						m_nucpts,m_nvcpts;

};

//////////////////////////////////////////////////////////////////////////
// ����һ���������������Ƶ������Σ��ص�۲�߽紦�������
class ScCmdBSplCtrlPntsTest : public ScCommand{
public:

	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//���ո�����
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_OBJ,S_UPNTS,S_VPNTS};

	BOOL					HasSelObj();
	void					ShowCtrlPnts(const TopoDS_Shape& aShape);
	BOOL					DoInsert();

	int						m_nUPnts,m_nVPnts;
};

//////////////////////////////////////////////////////////////////////////
// �������ת��Ϊһ��BSpline����
// ���ԴӼ���edgeת,��edge��wire���ת,��wireת��.֧�ֶ��ֹ���.
class ScCmdCurveConcatToSpline : public ScCommand{
public:
	
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

protected:
	enum{S_CURVE,S_CONCAT};

	BOOL					DoConcat();
};