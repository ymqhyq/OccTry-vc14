#pragma once

#include <afxtempl.h>

#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Point.hxx>

#include <vector>

class ScView;
////////////////////////////////////////////////////////////////
// �Զ���׽�Ĺ���
// Ŀǰ��ʵ�ֶ˵�Ĳ�׽�����������ٴ���
//

//��׽�����Ϣ
class ScSnapPoint{
public:
	ScSnapPoint(){
		m_nX = m_nY = 0;
		m_bSnaped = FALSE;
	}
	ScSnapPoint(const ScSnapPoint& other)
	{
		*this = other;
	}

	ScSnapPoint&	operator=(const ScSnapPoint& other)
	{
		m_pnt = other.m_pnt;
		m_nX  = other.m_nX;
		m_nY  = other.m_nY;
		m_bSnaped = other.m_bSnaped;

		return *this;
	}

	void			Dump();

public:
	gp_Pnt			m_pnt;//3ά��
	int				m_nX,m_nY;//��Ļ��
	BOOL			m_bSnaped;//�Ƿ񱻲�׽
};

typedef std::vector<ScSnapPoint> TSnapPointSet;

class ScSnapMgr
{
public:
	ScSnapMgr(const Handle(AIS_InteractiveContext)& aCtx);
	~ScSnapMgr(void);

	void				SetScView(ScView *pView) { m_pScView = pView; }

	//ʹ�ܺͽ�ֹ��׽
	void				Enable(BOOL bEnable);
	BOOL				IsEnable() const { return m_bEnable; }

	//���µ㣬���»�ȡ����Ϣ��
	BOOL				UpdateSnapPoints();

	//�Զ���׽
	void				Snap(int nX,int nY,const Handle(V3d_View)& aView);

	//��ȡ����pnt����Ĳ�׽��
	BOOL				GetSnapPoint(const gp_Pnt& pnt,gp_Pnt& snapPnt,BOOL bCompare = FALSE);

protected:
	void				Clear();

	//��ȡcs��صĵ�
	void				GetCSSnapPoint();

	//��ȡ����ĵ�
	BOOL				GetShapeSnapPoints(const TopoDS_Shape& aShape,CRect& rc);

	BOOL				WCSToPixel(const gp_Pnt& pnt,int& nx,int& ny);

	//���ǰ���Զ���׽���
	void				ClearSnapMarker();
	//�����Զ���׽���
	void				DrawSnapMarker();

	int					PixelDist(const ScSnapPoint& p1,const ScSnapPoint& p2);

	BOOL				IsNearest(int x1,int y1,int x2,int y2);

protected:
	Handle(AIS_InteractiveContext)		m_AISContext;//
	ScView								*m_pScView;
	BOOL								m_bEnable;
	int									m_nSnapSize;//��׽��Χ
	int									m_nMarkerSize;//��Ǵ�С

	TSnapPointSet						m_csSet;//����ϵ��صĵ�
	TSnapPointSet						m_temSet;//�˵�
	TSnapPointSet						m_cenSet;//Բ�ĵ�
	TSnapPointSet						m_midSet;//�е�

	ScSnapPoint							m_snapPoint;//�����
	BOOL								m_bHasPoint;//�Ƿ��в�׽��

	CPen								*m_pMarkerPen;

	Handle(AIS_Point)					m_aPntMarker;
};
