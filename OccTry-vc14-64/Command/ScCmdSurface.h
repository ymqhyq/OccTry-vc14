#pragma once

#include "ScCmdCurve.h"
#include "ScCmdCurveCreate.h"
#include "ScCmdFaceBase.h"

class ScCmdSurface
{
public:
	ScCmdSurface(void);
	~ScCmdSurface(void);
};

//�����
class ScCmdClosedPolyLine : public ScCmdPolyLine{
public:
	ScCmdClosedPolyLine();
	~ScCmdClosedPolyLine();


	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

protected:
	virtual  TopoDS_Shape	BuildNewShape();
};

//
// �����ϵĹ���
//
class ScCmdSurfSewing : public ScCommand{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SURFS};

	TopoDS_Shape			BuildNewShape();

protected:
	AIS_SequenceOfInteractive	m_seqOfObj;
	Handle(AIS_Shape)			m_aNewObj;
};

// 
// ����filling���ܡ�
// ��װ��MakeFilling�Ľӿڡ�ʵ�ֶ��ֹ��ܡ�
// ע���CmdPlate�е����������ǱȽϵײ��ʵ�֡�
// Ҫʵ���������͵Ĳ����㷨���߽�Լ��������Լ����������Լ��������Լ����
//
class ScCmdSurfFilling : public ScCommand{
public:
	ScCmdSurfFilling(int nType);

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

public:
	//��������
	enum {
		eBoundary = 1,
		eWithPoints,
		eWithCurves,
		eWithFace,
	};

	int						m_nFillType;//��������

protected:
	enum{S_SURF,S_EDGE,S_POINTS,S_CURVES,S_FACES};    //ѡ�棬ѡ�ߡ�

	TopoDS_Shape			BuildNewShape();

protected:
	BRepOffsetAPI_MakeFilling	*m_pApiFill;
	TopoDS_Face					m_aFace;
	TopoDS_Shape				m_aShape;
	int							m_nEdges;
};

//�ӱպ�����������
class ScCmdSurfPlanClosedCurves : public ScCmdCurveSewing{
public:
	
protected:
	virtual TopoDS_Shape		BuildNewShape();
};

//
// ����ü��㷨
//
class ScCmdSurfCut : public ScCommand{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SURF1,S_SURF2,S_CUT};

	BOOL					SplitSurf();
	void					CutSurf();

protected:
	TopoDS_Face				m_aFace1;
	TopoDS_Face				m_aFace2;
};

//
// ը�����档��solid��shell����ȡ�档
//
class ScCmdExtraFaces : public ScCommand{
public:
	
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SHAPE};

	void					ExtraFaces();

protected:
	TopoDS_Shape			m_aShape;

};

////////////////////////////////////////////////////////////////
//  ƽ������
class ScCmdPlane : public ScCommand{
public:
	ScCmdPlane(int nType);

	enum{
		eTwoPoint,//����
		eThreePoint,//����
	};

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
	enum{S_POINT1,S_POINT2,S_POINT3};

	TopoDS_Shape			BuildPlane(const CPoint& point);

protected:
	int						m_nPlnType;
	gp_Pnt					m_pnt1,m_pnt2;
};

/////////////////////////////////////////////////////
// �ܵ��棬ʹ��GeomFill_Pipe��
class ScCmdSurfPipe : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//���պ���
	//���հ뾶
	virtual   int			AcceptInput(double dVal);

protected:
	enum{S_CURVE,S_RADIUS};
};


//////////////////////////////////////////////////////
// bsplinecurves���ɵ�surface��
// ʹ���ࣺGeomFill_BSplineCurves
class ScCmdSurfBSplineCurves : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//��������
	virtual   int			AcceptInput(int nVal);
	
protected:
	enum{S_CURVES,S_TYPE};

	Handle(Geom_BSplineCurve)	m_aC1,m_aC2,m_aC3,m_aC4;
	int							m_nCnt;

};

//////////////////////////////////////////////////////
// ��������һ���ĳ��ȡ�
// ʹ�ã�GeomLib::ExtendSurfByLength������
class ScCmdSurfExtent : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//�������쳤��
	virtual   int			AcceptInput(double dVal);

protected:
	enum{S_SURF,S_UV,S_CONT,S_LEN};

	BOOL					m_bU;
	BOOL					m_bAfter;
	int						m_nCont;//������
	Handle(Geom_BoundedSurface) m_aSurf;
};

//////////////////////////////////////////////////////////
// ��face����ȡ�ü������ԭʼ����
class ScCmdRawSurface : public ScCmdFaceBase{
public:

protected:
	virtual BOOL			BuildShape();//���ɶ�Ӧshape��
};

////////////////////////////////////////////////////////////
// ��ȡһ�����棬��������Shape��Solid��Compound�е�һ��Face
class ScCmdExtractFace : public ScCmdFaceBase{
public:

protected:
	virtual BOOL			BuildShape();//���ɶ�Ӧshape��
};

////////////////////////////////////////////////////////////////
// �ɶ��ƽ�滷���ɵĲü�ƽ�档�򵥵ķ�����ֱ��ʹ��makeface������
//���ӵķ�������Ҫ�жϻ��Ƿ��ཻ�ȣ�����ͺܸ����ˡ�
//�ȼ򵥵����
class ScCmdMultiLoopFace : public ScCommand{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_OUTWIRE,S_INWIRES};

		BOOL						BuildShape();//
};

//////////////////////////////////////////////////////////////////////////
// ʹ������ָ�ƽ�棬ʹ��BrepFeat_splitShape
//
class ScCmdSplitPlane : public ScCommand{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_FACE,S_CURVE};
		BOOL						HasSelFace();
		BOOL						DoSplit();

		gp_Pln					m_pln;
};

//////////////////////////////////////////////////////////////////////////
// ʹ������ָ����棬ʹ��BRepFeat_splitShape
class ScCmdSplitSurf : public ScCommand{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_FACE,S_TOOL};
		BOOL						HasSelFace();
		BOOL						HasSelTool();
		BOOL						DoSplit();

};