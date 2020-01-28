#pragma once

#include "ScCommand.h"

//���ο�����.
//Ҫ֧����UCS�еı��Σ���Ҫע��CS��ת����
class TcFFDFrame{
public:
	TcFFDFrame();

	//�Ƿ��ڱ��η�Χ��
	BOOL				InRange(const gp_Pnt& pnt);

	//1�����
	BOOL				DeformPnt(const gp_Pnt& pnt,gp_Pnt& respnt);

	//����ֲ�����
	gp_Pnt				CalcLcsCoord(const gp_Pnt& pnt);

	int					Index(int ix,int iy,int iz);

public:
	int			_l,_m,_n;//�ֲ�����ϵ��,x��y��z����ķָ�����
	int			_tcnt;//�ܵ����
	STR3D		_ctlpnts;//���ο��Ƶ�����꣬WCS��
	STR3D		_ffdpnts;//ffd�ı�������
//	gp_Ax2		_lcs;//Frame�ֲ�����ϵ
	gp_Vec		_Sx,_Ty,_Uz;//�ֲ�����ϵ����������
	PNT3D		_min;
	PNT3D		_max;//��С���������������ꡣ

	gp_Ax2		_ucs;//�û�����ϵ���ڸ�����ϵ�ڲ�����һ��box������wcs�£���δ���ˡ�
};

class ScCmdFFD : public ScCommand
{
public:
	ScCmdFFD(void);
	~ScCmdFFD(void);

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
	//�Ƚ������Ĺ��̡����ӵĴ���
	enum{S_OBJECT,S_PNT1,S_PNT2,S_PNT3,S_OPTION,S_SELPNT,S_BASPNT,S_NEWPNT};

	BOOL					HasSelObj();
	BOOL					MakeFFDFrame();
	BOOL					ShowRect(gp_Pnt pnt1,gp_Pnt pnt2);
	BOOL					ShowBox(gp_Pnt pnt1,gp_Pnt pnt2,gp_Pnt pnt3);
	BOOL					CalcBoxCorner(gp_Pnt pnt1,gp_Pnt pnt2,gp_Pnt pnt3);

	BOOL					ShowChangeFrame(gp_Pnt newpnt);

	BOOL					FindPnts(PNT3D pnt);

	BOOL					MakeFFD(BOOL bDone);
	TopoDS_Face				DeformFace(const TopoDS_Face& aFace);

	TopoDS_Shape			SewFaces(const TopoDS_Compound& aC);

	TopoDS_Shape			DeformFaceWire(const TopoDS_Face& aFace,const TopoDS_Face& aNewFace);

	TopoDS_Face				BuildDeformedFace(const Handle(Geom_Surface)& aNewSurf,const TopoDS_Face& aFace);

	TopoDS_Wire				BuildDeformedWire(const TopoDS_Wire& aWire,
									const Handle(Geom_Surface)& aNewSurf,const TopoDS_Face& aFace);

	TcFFDFrame				m_ffdFrame;//���ο�����
	gp_Pnt					m_pnt1,m_pnt2,m_pnt3;
	PNT3D					m_boxCorner[8];//

	TopTools_SequenceOfShape m_seqOfV;//ѡ�еĵ�
	gp_Pnt					m_ptBas;
};

//�����޸��ࡣ��Occ�м̳�
DEFINE_STANDARD_HANDLE(ScFFD_Modification,BRepTools_Modification)

class ScFFD_Modification : public BRepTools_Modification{
public:
	ScFFD_Modification();

	void			SetFFDFrame(TcFFDFrame *pFrm) { m_ffdFrame = pFrm; }

	virtual  Standard_Boolean NewSurface(
		const TopoDS_Face& F,
		Handle(Geom_Surface)& S,
		TopLoc_Location& L,
		Standard_Real& Tol,
		Standard_Boolean& RevWires,
		Standard_Boolean& RevFace);

	virtual  Standard_Boolean NewCurve(
		const TopoDS_Edge& E,
		Handle(Geom_Curve)& C,
		TopLoc_Location& L,
		Standard_Real& Tol);

	virtual  Standard_Boolean NewPoint(
		const TopoDS_Vertex& V,
		gp_Pnt& P,
		Standard_Real& Tol);

	virtual  Standard_Boolean NewCurve2d(
		const TopoDS_Edge& E,
		const TopoDS_Face& F,
		const TopoDS_Edge& NewE,
		const TopoDS_Face& NewF,
		Handle(Geom2d_Curve)& C,
		Standard_Real& Tol);

	virtual  Standard_Boolean NewParameter(
		const TopoDS_Vertex& V,
		const TopoDS_Edge& E,
		Standard_Real& P,
		Standard_Real& Tol) ;

	virtual  GeomAbs_Shape Continuity(
		const TopoDS_Edge& E,
		const TopoDS_Face& F1,
		const TopoDS_Face& F2,
		const TopoDS_Edge& NewE,
		const TopoDS_Face& NewF1,
		const TopoDS_Face& NewF2) ;

protected:
//yxk20200104
	//DEFINE_STANDARD_RTTI(ScFFD_Modification)
	DEFINE_STANDARD_RTTIEXT(ScFFD_Modification,BRepTools_Modification)
//endyxk
protected:
	TcFFDFrame			*m_ffdFrame;
};


class TcFFDeformShape  : public BRepBuilderAPI_ModifyShape {

public:

	void* operator new(size_t,void* anAddress) 
	{
		return anAddress;
	}
	void* operator new(size_t size) 
	{ 
		return Standard::Allocate(size); 
	}
	void  operator delete(void *anAddress) 
	{ 
		if (anAddress) Standard::Free((Standard_Address&)anAddress); 
	}

	TcFFDeformShape();

	void		Perform(const TopoDS_Shape& S,TcFFDFrame *pFrm);
};