#pragma once

#include "FFDRmf.h"

class FFDTool
{
public:
	FFDTool(void);
	virtual ~FFDTool(void);

	BOOL						IsValid() const { return m_bValid; }

protected:
	BOOL						m_bValid;
};

//////////////////////////////////////////////////////////////////////////
//������ι��ߣ������׼����ͱ����������Ϣ��
// �ȿ���������Σ��ֲ���������
// ����ϵ����Ҫ��������
class FFDSurfTool : public FFDTool{
public:
	FFDSurfTool();
	virtual ~FFDSurfTool();

	//
	BOOL							InitRect(const gp_Pnt& p1,const gp_Pnt& p2,
											int nUDeg = 3,int nVDeg = 3,int nUCtrlPnts = 4,int nVCtrlPnts = 4);

	BOOL							InitRect(const gp_Pnt& p1,const gp_Pnt& p2,const gp_Ax2& ax,
											int nUDeg = 3,int nVDeg = 3,int nUCtrlPnts = 4,int nVCtrlPnts = 4);

	//��ʼ����׼����
	BOOL							InitDatumSurf(const Handle(Geom_Surface)& pSurf);

	//����Ŀ������
	BOOL							SetTarSurf(const Handle(Geom_Surface)& pSurf);

public:
	Handle(Geom_BSplineSurface)		m_datumSurf;//��׼����
	Handle(Geom_Surface)			m_tarSurf;//Ŀ������

	BOOL							m_bPlane;//��׼���Ƿ�ƽ��
	Handle(Geom_Plane)				m_aPlane;//ƽ�棬�����׼������ƽ��
	double							m_u1,m_v1,m_u2,m_v2;//plane�Ĳ�����Χ
};

//////////////////////////////////////////////////////////////////////////
//����ֱ�ߵ�FFD���ι���,ԭʼֱ��Ϊ��׼�ߣ����κ��ֱ��ΪĿ�����ߡ�
class FFDLineTool : public FFDTool{
public:
	FFDLineTool();
	virtual ~FFDLineTool();

	//�����������������ڻ���ֱ�ߵ�FFD���Σ�ԭʼֱ��Ϊ��׼�ߣ����κ��ֱ��ΪĿ�����ߡ�
	BOOL							InitLine(const gp_Pnt& p1,const gp_Pnt& p2,int nDeg = 3,int ncpts = 4);

	BOOL							SetLineDfmCurve(const Handle(Geom_BSplineCurve)& aCur);

	//����RMF
	BOOL							CalcRMF(int nSamps);

public:
	Handle(Geom_BSplineCurve)		m_datumCurve;//��׼����
	Handle(Geom_BSplineCurve)		m_tarCurve;//Ŀ������

	//��¼�������ߵ�rmf��Ϣ
	FFDCurveRMF						m_datumRmf;
	FFDCurveRMF						m_tarRmf;
	int								m_nSamps;
	BOOL							m_bChanged;//�Ƿ�ı�
};

//////////////////////////////////////////////////////////////////////////
// �������ߵı��ι��ߡ���������Ϊ��ͬ������
class FFDCurveTool : public FFDTool{
public:
	FFDCurveTool();
	virtual ~FFDCurveTool();

	BOOL							SetDatumCurve(const Handle(Geom_Curve)& aCur);
	BOOL							SetDatumCurve(const Handle(Geom_Curve)& aCur,double df,double dl);

	//����Ŀ������,���߲����ƣ����ܺ�Lineû��
	BOOL							SetTarCurve(const Handle(Geom_Curve)& aCur);
	BOOL							SetTarCurve(const Handle(Geom_Curve)& aCur,double df,double dl);

	//����RMF
	// ����������ʾ�����߼�Ķ�Ӧ��ϵ.stretch��ʾ:���������Ƿ�Ҫ����.
	BOOL							CalcRMF(int nSamps,double datumT,double tarT,BOOL bStretch = TRUE);

	double							MapParam(double datumt);

public:
	Handle(Geom_Curve)				m_datumCurve;//��׼����
	double							m_df,m_dl;//���߲�����Χ

	Handle(Geom_Curve)				m_tarCurve;//Ŀ������
	double							m_tf,m_tl;//���߲�����Χ

	//��¼�������ߵ�rmf��Ϣ
	FFDCurveRMF						m_datumRmf;
	FFDCurveRMF						m_tarRmf;

	int								m_nMapType;//ӳ������
	double							m_dMapRatio;//ӳ�����

protected:
	enum{eMap_PP,eMap_PN,eMap_NP,eMap_NN};
};

//////////////////////////////////////////////////////////////////////////
// �����������ι���
class FFDSurfFlowTool : public FFDTool{
public:
	FFDSurfFlowTool();
	virtual ~FFDSurfFlowTool();

	void							SetStretch(BOOL bStretch = TRUE);
	void							SetUseTrimSurf(BOOL bUseTrimSurf = FALSE);

	BOOL							SetDatumSurf(const Handle(Geom_Surface)& aSurf);
	BOOL							SetDatumSurf(const Handle(Geom_Surface)& aSurf,double u1,double v1,double u2,double v2);

	BOOL							SetTarSurf(const Handle(Geom_Surface)& aSurf);
	BOOL							SetTarSurf(const Handle(Geom_Surface)& aSurf,double u1,double v1,double u2,double v2);

	BOOL							MapPoint(double u,double v,gp_Pnt2d& uv);

	BOOL							CalcMapInfo();

public:
	Handle(Geom_Surface)			m_datumSurf;
	double							m_dU1,m_dV1,m_dU2,m_dV2;

	Handle(Geom_Surface)			m_tarSurf;
	double							m_tU1,m_tV1,m_tU2,m_tV2;

	double							m_dUMapRatio,m_dVMapRatio;

	BOOL							m_bStretch;
	BOOL							m_bUseTrimSurf;
};

inline void		FFDSurfFlowTool::SetStretch(BOOL bStretch)
{
	m_bStretch = bStretch;
}

inline void		FFDSurfFlowTool::SetUseTrimSurf(BOOL bUseTrimSurf)
{
	m_bUseTrimSurf = bUseTrimSurf;
}

//////////////////////////////////////////////////////////////////////////
//
//��б����
class FFDShearTool : public FFDTool{
public:
	FFDShearTool();
	virtual ~FFDShearTool();

	//��ʼ��,��׼��ͻ�׼����
	BOOL							Init(const gp_Pnt& org,const gp_Vec& baseDir);
	BOOL							Init(const gp_Pnt& org,const gp_Vec& baseDir,const gp_Ax2& ucs);

	//��б����
	BOOL							Shear(const gp_Vec& newDir);

	//��б�Ƕ�(����)
	BOOL							Shear(double drand);

	//�Ե���б任
	BOOL							TrsfPoint(const gp_Pnt& pnt,gp_Pnt& tsfpnt);

	//���ر任
	gp_GTrsf						ShearTrsf();

protected:
	gp_Ax2							m_ucsAx;//UCS����ϵ
	gp_Ax2							m_locAx;//�ֲ�����ϵ,����ucs.
	double							m_drad;//��б�Ƕ�		

	gp_Trsf							m_tsf,m_tsfr;//ת��ֲ������ת��ԭ������ϵ�ľ���,����.
	gp_GTrsf						m_shearTrsf;//�ܵı任����,gp_Trsf��֧��shear�任
};

//////////////////////////////////////////////////////////////////////////
// Ťת����
class FFDTwistTool : public FFDTool{
public:
	FFDTwistTool();
	virtual ~FFDTwistTool();

	//��ʼ��
	BOOL							Init(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Vec& baseDir);

	BOOL							Rotate(double dRad,BOOL bExtend);

	//�Ե������ת
	BOOL							RotatePoint(const gp_Pnt& pnt,gp_Pnt& newpnt);

protected:
	//��ͶӰ��ֱ��,��ȡ����
	BOOL							AxisProject(const gp_Pnt& pnt,double& t);

	//��ת�Ƕ�
	double							RotRad(double t);

protected:
	gp_Pnt							m_axPnt1,m_axPnt2;//��ת�����ʼ�����ֹ��
	gp_Vec							m_baseDir;//��׼��ʼ����(�Ƕ�)
	double							m_dRotRad;//��ת�ĽǶ�.

	BOOL							m_bExtend;//�Ƿ���������

private:
	double							m_axLen;
	gp_Vec							m_axVec;
	double							m_radSlope;//�Ƕȵ�����б��

	Handle(Geom_Line)				m_axLine;//��ת���Ӧֱ��
};

//////////////////////////////////////////////////////////////////////////
// ����bend����.
class FFDBendTool : public FFDTool{
public:
	FFDBendTool();
	virtual ~FFDBendTool();

	//��Ҫ�ĳ�ʼ��,ǰ�����ǹǸɵ�,�����������������Ʊ��η�Χ.
	BOOL							Init(const gp_Pnt& pnt1,const gp_Pnt& pnt2,
										const gp_Pnt& pntFrm,const gp_Pnt& pntTo);

	//����ͨ���㹹�����ڱ��ε���������.
	BOOL							BuildCurveTool(const gp_Pnt& pnt,BOOL bExtend,BOOL bSymmetric);

	Handle(Geom_Curve)				GetDatumCurve() const;
	Handle(Geom_Curve)				GetTarCurve() const;
	Handle(Geom_Curve)				GetArcCurve() const;

protected:
	gp_Pnt							CalcCircCenter(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Pnt& paspnt,
										const gp_Dir& cdir,const gp_Dir& N);

protected:
	gp_Pnt							m_axPnt1,m_axPnt2;

	Handle(Geom_Curve)				m_datumCurve;//��׼����
	Handle(Geom_BSplineCurve)		m_tarCurve;//Ŀ������
	Handle(Geom_BSplineCurve)		m_arcCurve;//Բ������,���ڽ�����ʾ

private:
	BOOL							m_bExtend;
	BOOL							m_bSymmetric;

	double							m_preLen,m_afterLen;//ǰ���ֱ�߳���.
};

inline Handle(Geom_Curve)	FFDBendTool::GetDatumCurve() const
{
	ASSERT(IsValid());
	return m_datumCurve;
}

inline Handle(Geom_Curve)	FFDBendTool::GetTarCurve() const
{
	ASSERT(IsValid());
	return m_tarCurve;
}

inline Handle(Geom_Curve)	FFDBendTool::GetArcCurve() const
{
	ASSERT(IsValid());
	return m_arcCurve;
}

//////////////////////////////////////////////////////////////////////////
// ׶�λ�����
class FFDTaperTool : public FFDTool{
public:
	FFDTaperTool();
	virtual	~FFDTaperTool();

	//��ʼ��.����Ǹɵ�������,������ʼ�ķ���ͳ���.bBoth��ʾ������������һ������׶�λ�.
	BOOL							Init(const gp_Pnt& pnt1,const gp_Pnt& pnt2,const gp_Vec& beginVec,
										BOOL bExtend,BOOL bBoth);

	//������һ������ĳ���,ʵ��׶�λ�.
	BOOL							SetEndLen(double dlen);

	//��һ������б任
	BOOL							TrsfPoint(const gp_Pnt& pnt,gp_Pnt& newpnt);

protected:
	gp_Pnt							m_axPnt1,m_axPnt2;//�Ǹɵ�������
	double							m_dBeginLen,m_dEndLen;//��ʼ����ֹ���˵ĳ���

	BOOL							m_bExtend;//�Ƿ�����
	BOOL							m_bBoth;//�Ƿ���������ı�

protected:
	gp_Ax2							m_locAx;//�ֲ�����ϵ
	double							m_dRatio;//����ϵ��

	gp_Trsf							m_ltowTsf,m_wtolTsf;//
};