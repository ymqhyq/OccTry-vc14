#pragma once

#include <AIS_Shape.hxx>
#include "ScCommand.h"

class ScCmdBRepPrim : public ScCommand
{
public:
	ScCmdBRepPrim(void);
	virtual ~ScCmdBRepPrim(void);

	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();
};

class ScCmdSphere : public ScCmdBRepPrim
{
public:
	ScCmdSphere();
	~ScCmdSphere();

	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

	//���ո�����
	virtual   int		AcceptInput(double dVal); 
	//����3ά��
	virtual   int		AcceptInput(gp_Pnt& pnt); 


public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

protected:
	//�ĸ�״̬
	enum { SPH_BEGIN,SPH_GET_CENTER,SPH_GET_RADIUS,SPH_END };

	gp_Pnt				m_ptCenter; //����
	double				m_dRadius;  //�뾶

private:
	BOOL				m_bFirst; //�Ƿ����
	Handle(AIS_Shape)	m_aShape; //��ʱ����
};

class ScCmdBox : public ScCmdBRepPrim{
public:
	ScCmdBox();
	~ScCmdBox();

	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);
	//����3ά��
	virtual   int			AcceptInput(gp_Pnt& pnt); 

protected:
	enum{BOX_POINT1,BOX_POINT2,BOX_HEIGHT};

	int						DoAcceptPoint(gp_Pnt& pnt);
	TopoDS_Face				BuildBottom();
	TopoDS_Shape			BuildBox();

protected:
	Handle(AIS_Shape)		m_aShape;//��ʱ����
	gp_Pnt					m_pnt1,m_pnt2;
	double					m_dH;//�߶�

};

class ScCmdCyl : public ScCmdBRepPrim{
public:
	ScCmdCyl(BOOL bWithAngle = FALSE);
	~ScCmdCyl();

	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);
	//����3ά��
	virtual   int			AcceptInput(gp_Pnt& pnt); 

protected:
	enum{CYL_CENTER,CYL_RADIUS,CYL_ANGLE,CYL_HEIGHT};

	TopoDS_Shape			BuildBottom();
	TopoDS_Shape			BuildCyl();
	double					ComputeAngle();

protected:
	Handle(AIS_Shape)		m_aShape,m_aShape1;
	gp_Pnt					m_ptC,m_pt1,m_pt2,m_pt3;

	BOOL					m_bWithAngle;
};

class ScCmdCone : public ScCmdBRepPrim{
public:
	ScCmdCone();
	~ScCmdCone();

	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);
	//����3ά��
	virtual   int			AcceptInput(gp_Pnt& pnt); 

protected:
	enum{CON_CENTER,CON_RADIUS,CON_ANGLE,CON_RADIUS2};

protected:
	Handle(AIS_Shape)		m_aShape;
	gp_Pnt					m_ptC,m_pt1,m_pt2,m_pt3;

};

class ScCmdTorus : public ScCmdBRepPrim{
};

/////////////////////////////////////////////////////////
// �������������������棬��������塣
// ʹ��BRepPrimAPI_MakePrism�ࡣ
//
class ScCmdPrism : public ScCommand{
public:
	ScCmdPrism(BOOL bTwoDir = FALSE,BOOL bGenSurf = FALSE);
	~ScCmdPrism();

	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);
	//����3ά��
	virtual   int			AcceptInput(gp_Pnt& pnt); 

protected:
	enum{S_GET_SHAPE,S_GET_DIR_P1,S_GET_DIR_P2};

	BOOL					CanPrism(const TopoDS_Shape& aShape);
	TopoDS_Shape			BuildPrism();

protected:
	Handle(AIS_Shape)		m_aShape;
	TopoDS_Shape			m_aSelShape;
	gp_Vec					m_aVec;//���췽��
	gp_Pnt					m_pnt1;

	BOOL					m_bTwoDir;//�Ƿ�˫������
	BOOL					m_bGenSurf;//�Ƿ��������棬TRUE���������棬FALSE�������塣
};

//////////////////////////////////////////////////////////
// ���ߡ�������ת
// ʹ����BRepPrimAPI_MakeRevol�ࡣ
class ScCmdRevol : public ScCommand{
public:
	ScCmdRevol(BOOL bGenSolid);

	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	virtual	  void			MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);
	//����3ά��
	virtual   int			AcceptInput(gp_Pnt& pnt); 
	//���ո�����
	virtual   int			AcceptInput(double dVal);

protected:
	enum{S_PROFILE,S_POINT,S_DIR,S_ANGLE};

	BOOL					CanRevol(const TopoDS_Shape& aShape);
	TopoDS_Shape			BuildRevol();

	BOOL					CalcRevolBeginPoint(gp_Pnt& oPnt);
	BOOL					PrjToPlane(gp_Pnt& pnt);

protected:
	Handle(AIS_Shape)		m_aShape;
	TopoDS_Shape			m_aProfile;
	gp_Pnt					m_pnt,m_pnt2,m_pnt3;//��
	gp_Ax2					m_ax2;
	gp_Dir					m_dir;//����
	double					m_dAngle;

	BOOL					m_bGenSolid;//�Ƿ�����Solid��
};



////////////////////////////////////////////////////////////
// ����ü�ʵ��
// ʹ���������ɰ�ռ䣬����bool������вü���
class ScCmdSolidCut : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int	Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int	End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SELFACE,S_SELOBJ,S_SELDIR};

protected:
	TopoDS_Shape		m_aSurfShape;
	TopoDS_Shape		m_aShape;
};

/////////////////////////////////////////////////////////////////
// �������
// �����shape��ϳ�compound

/////////////////////////////////////////////////////////////////
// ȡ�����
// ��compound����ɵ�����shape
class ScCmdDeCompound : public ScCommand{
public:

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	BOOL					CanDeCompound();
	void					DeCompound();
};