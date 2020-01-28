#pragma once

#include <TColgp_SequenceOfPnt.hxx>
#include <AIS_SequenceOfInteractive.hxx>
#include <AIS_InteractiveObject.hxx>
#include "ScCommand.h"
#include "ScCmdFaceBase.h"
#include "ScCmdCurveBase.h"

class ScCmdCurve 
{
public:
	ScCmdCurve(void);
	~ScCmdCurve(void);
};



///////////////////////////////////////////////////////
// �������
class ScCmdCurveSewing : public ScCommand{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_CURVES};

	virtual TopoDS_Shape		BuildNewShape();

protected:
	AIS_SequenceOfInteractive	m_seqOfObj;
	Handle(AIS_Shape)			m_aNewObj;
};

//////////////////////////////////////////////////////////////////////////
// ����ȡ�����
class ScCmdCurveWireToEdge : public ScCmdCurveBase{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

protected:
	BOOL					DoWireToEdge();
};

/////////////////////////////////////////////////////////////
// ���ߵ�ʵ���ͶӰ
class ScCmdBrepProjection : public ScCommand{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_CURVE,S_SHAPE};

	TopoDS_Shape			BuildNewShape();

protected:
	Handle(AIS_InteractiveObject) m_aCurveObj;
	TopoDS_Shape				m_aShape;
	TopoDS_Shape				m_aProjShape;
	Handle(AIS_Shape)			m_aNewObj;
};

////
// ����
class ScCmdInterCurve : public ScCommand{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SHAPE1,S_SHAPE2};

	void					BuildInterCurve();

protected:
	TopoDS_Shape			m_aShape1,
							m_aShape2;

};

////////////////////////////////////////////////
// ������
class ScCmdPoint :public ScCommand{
public:
	ScCmdPoint(BOOL bPlane = FALSE);

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	BOOL					m_bPlanePoint;//�Ƿ�ƽ���
	int						m_nIdx;
};

/////////////////////////////////////////////////////
// ���쵽��
class ScCmdExtendToPoint : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//���պ���
	//��������
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_CURVE,S_POINT,S_DIR,S_CONT};

	gp_Pnt					m_extPnt;//Ҫ���쵽�ĵ�
	BOOL					m_bAfter;//����ǰ�˻��Ǻ��
};

//////////////////////////////////////////////////////////
// ƫ������,ʹ��BRepOffsetAPI_MakeOffset�ࡣ
class ScCmdCurveOffset : public ScCommand{
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
	enum{S_CURVE,S_OFFVAL};
};

/////////////////////////////////////////////////////////////
// �򵥻���,��Ҫ�ṩ���÷�����
class ScCmdOneFace : public ScCommand{
public:

protected:
	//�Ƿ�ѡ����face�����������tmpObj(0)�С�
	BOOL				HasSelFace();
};

/////////////////////////////////////////////////////////////
// face�ı߽�
class ScCmdCurveFaceBoundary : public ScCmdOneFace{
public:
	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_FACE,S_CURVE};

	int						m_nNewCurves;

};

//Face��ȫ��3d�߽�
class ScCmdFaceBoundary3d : public ScCmdFaceBase{
public:

protected:
	virtual BOOL			BuildShape();//���ɶ�Ӧshape��
	virtual BOOL			BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace);
};

//Face��ȫ�������������ɱ߽�
class ScCmdFaceBoundary2d : public ScCmdFaceBase{
public:

protected:
	virtual BOOL			BuildShape();//���ɶ�Ӧshape��
	virtual BOOL			BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace);
};

//Face��ȫ���ü����Ĳ�������
class ScCmdFaceParamLoop : public ScCmdFaceBase{

protected:
	virtual BOOL			BuildShape();//���ɶ�Ӧshape��
	virtual BOOL			BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace);
};

//////////////////////////////////////////////////////////////////////////
// ���߼�Ľ��� BRepExtram_DistShapeShape��
class ScCmdCurveInterPoint : public ScCommand{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_CURVE1,S_CURVE2};

		BOOL						HasSelCurve(BOOL bFirst);
		BOOL						DoInter();
};

//////////////////////////////////////////////////////////////////////////
// ���߻��ܵ���ʽ
class ScCmdCurveFrameBase : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

protected:
	enum{S_CURVE,S_FRAME};

	BOOL					HasSelCurve();
	virtual BOOL			BuildFrame();

	virtual	BOOL			CalcFrame(double t,gp_Dir& T,gp_Dir& N,gp_Dir& B) { return FALSE; }

	Handle(Geom_Curve)		m_aSelCurve;
	TopoDS_Shape			m_aC;//��ʽframe�Ķ���
	double					m_df,m_dl;
};

//Frenet���
class ScCmdCurveFrenetFrame : public ScCmdCurveFrameBase{
public:

protected:
	virtual	BOOL			CalcFrame(double t,gp_Dir& T,gp_Dir& N,gp_Dir& B);
};

//��ת���1
class ScCmdCurveRotateFrame1 : public ScCmdCurveFrameBase{

protected:
	virtual BOOL			BuildFrame();
};

//��ת���2
class ScCmdCurveRotateFrame2 : public ScCmdCurveFrameBase{

protected:
	virtual BOOL			BuildFrame();
};

//sloan���
class ScCmdCurveSloanFrame : public ScCmdCurveFrameBase{

protected:
	virtual BOOL			BuildFrame();
};


//˫������
class ScCmdCurveDblRefFrame : public ScCmdCurveFrameBase{

protected:
	virtual BOOL			BuildFrame();
};
