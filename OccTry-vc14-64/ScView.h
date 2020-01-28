#pragma once

//�򵥵�view��װ
#include <V3d_View.hxx>

enum{
	ScView_Front,
	ScView_Back,
	ScView_Top,
	ScView_Bottom,
	ScView_Left,
	ScView_Right,
};

class ScUcs;
class ScUcsMgr;
class CView;

class ScView
{
public:
	ScView(const Handle(V3d_View)& aView);
	~ScView(void);

	//������ͼ����
	int			SetProj(int nType);

	//��Ļ��ת��Ϊ�ռ�(WCS or UCS)�еĵ㡣
	gp_Pnt	    PixelToModel(Standard_Real x, Standard_Real y,BOOL bWCS = TRUE);

	//��Ļ��ת��Ϊcs��ƽ���ϵĵ�
	gp_Pnt	    PixelToCSPlane(Standard_Real x, Standard_Real y,BOOL bWCS = TRUE);

	//��Ļ��ת��Ϊ��ǰ����ϵ�еĵ㣬���������ʾ��ͬ�ĵ㡣
	gp_Pnt		PixelToObjCS(int x,int y);

	//����ת������
	int			WCSToUCS(gp_Pnt& pnt);
	int			UCSToWCS(gp_Pnt& pnt);
	int			WCSToUCS(gp_Vec& vec);
	int			UCSToWCS(gp_Vec& vec);

	//������Ļ�����ȡ�Ӹĵ㷢���������ꡣ
	//line��wcs�С�
	gp_Lin		GetEyeLine(int nX,int nY);

	//��ȡedge�ϵ���ĵ㡣����ѡ��edge�󣬻�ȡedge�ϵĵ���㡣
	//�������ѡ��curveʱ����ȡ��ʱ����ĵ㡣dTolΪ��Ļ����ϵ�µķ�Χ�ݲ
	//���WCS�㡣
	BOOL		GetEdgePickPoint(const TopoDS_Edge& aEdge,
								int nX,int nY,double dTol,
								double& t,gp_Pnt& pickPnt);

	//��ȡedge����nx��ny�����ͶӰ�㡣���������Χ�����ض˵㡣
	BOOL		GetEdgeNearestPoint(const TopoDS_Edge& aEdge,
								int nX,int nY,
								double& t,gp_Pnt& pnt);
	BOOL		GetEdgeNearestPoint(const TopoDS_Edge& aEdge,
								int nX,int nY,
								double& t,gp_Pnt& pnt,double& dist);

	//��ȡface�ϵ���ĵ㡣����ѡ��shape��face�󣬻�ȡshape��face�ϵĵ���㡣
	//�������ѡ��faceʱ����ȡ��ʱ����ĵ㡣
	//���WCS�㡣
	BOOL		GetFacePickPoint(const TopoDS_Face& aFace,
								int nx,int ny,
								double& u,double& v,gp_Pnt& pickPnt);

	//��ȡshape�ϵ���ĵ㡣
	//�������ѡ��shapeʱ����ȡ��ʱ����ĵ㡣
	//���WCS�㡣
	gp_Pnt		GetShapePickPoint(const TopoDS_Shape& aShape,int nx,int ny);

	//�����pnt����ǰ����ƽ��ĸ߶�.PntΪWCS�㡣
	double		ComputeHeight(const gp_Pnt& pnt);

	//�Ƿ���Ҫ�ռ��еĵ㡣
	void		NeedModelPoint(BOOL bNeed);

	//��ǰ��ax����ϵ������ƽ�治ͬ����ͬ
	gp_Ax2		CurrentAxis(BOOL bUCS = FALSE);

	//���õ�ǰ�ֲ�����
	void		UpdateUCS();
	void		SetUcsMgr(ScUcsMgr *pUcsMgr);

public:
	Handle(V3d_View)		m_aView;
	int						m_nViewType;//Ĭ��front��
	ScUcsMgr				*m_pScUcsMgr;//
	CView					*m_pViewWnd;

protected:
	//����ֱ�ߺ�ƽ��Ľ���
	BOOL		ComputeLinPlanIntersection(const gp_Lin& aL,
							const gp_Pln& aPlan,gp_Pnt& aInterPnt);

private:
	BOOL		m_bNeedModelPoint;//�Ƿ���Ҫģ�Ϳռ��еĵ㡣ͨ��������ת��Ϊ����ƽ���ϵ�
	                              //�㣬��ʱ��Ҳ��Ҫ�ռ�ĵ㡣
};

inline void		ScView::SetUcsMgr(ScUcsMgr *pUcsMgr)
{
	m_pScUcsMgr = pUcsMgr;
}

//�Ƿ���Ҫ�ռ��еĵ㡣
inline	void	ScView::NeedModelPoint(BOOL bNeed)
{
	m_bNeedModelPoint = bNeed;
}