#pragma once

#include <vector>

//�漰�����ߵ�frame����
//////////////////////////////////////////////////////////////////////////
//��¼һ���������ɢframe����Ϣ

//������һ�����Frame��Ϣ��
class FFDCurvePointFrame{
public:
	FFDCurvePointFrame();
	FFDCurvePointFrame(const FFDCurvePointFrame& other);

	FFDCurvePointFrame&	operator=(const FFDCurvePointFrame& other);

	void					Set(double u,const gp_Pnt& pnt,const gp_Dir& T,
		const gp_Dir& N,const gp_Dir& B);

	double					_u;
	gp_Pnt					_O;//�ֲ�ԭ��
	gp_Dir					_dT,_dN,_dB;//tangent,Normal,biNormal
};

//��¼һ�������ϵ�һϵ����ɢ���ϵ�frame��Ϣ
// RMF:rotate minium frame
class FFDCurveRMF{
public:
	FFDCurveRMF();

	BOOL					Init(const Handle(Geom_Curve)& aCur,double df,double dl,int nSamps);

	//���������
	void					Clear();

	//���ó�ʼ��ʸ����ҪӦ����ֱ�ߵȳ�ʼ��ʸ�޷�ȷ�������
	void					SetStartNormal(const gp_Dir& startN);

	//����
	BOOL					CalcRotateRMF();
	//sloan��������
	BOOL					CalcSloanRMF();

	BOOL					CalcLcsCoord(double u,const gp_Pnt& pnt,gp_Pnt& lcpnt);

	BOOL					CalcDfmCoord(double u,const gp_Pnt& lcpnt,gp_Pnt& pnt);

protected:
	enum{M_ROTATE,M_SLOAN};

	BOOL					CalcRMF(int nMethod);//1:��ת����2��sloan����

	BOOL					DoCalcRotateFrame(double t,
		const gp_Dir& T0,const gp_Dir& N0,const gp_Dir& B0,
		gp_Dir& T,gp_Dir& N,gp_Dir& B);

	BOOL					DoCalcSloanFrame(double t,
		const gp_Dir& T0,const gp_Dir& N0,const gp_Dir& B0,
		gp_Dir& T,gp_Dir& N,gp_Dir& B);

public:
	Handle(Geom_Curve)			m_aCurve;//����
	double						m_df,m_dl;//������Χ
	double						m_nSamps;//���������
	double						m_dstep;//����
	std::vector<FFDCurvePointFrame>	m_frmArray;

protected:
	BOOL						m_bSetStartNormal;
	gp_Dir						m_startNormal;
};

