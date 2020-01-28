//////////////////////////////////////////////////////////////////////////
// ʵ��Axial Deformation�����㷨��
#pragma once

#include <vector>
#include "ScCommand.h"

//////////////////////////////////////////////////////////////////////////
//��¼һ���������ɢframe����Ϣ
class TcCurveFrame{
public:
	TcCurveFrame();
	TcCurveFrame(const TcCurveFrame& other);

	TcCurveFrame&	operator=(const TcCurveFrame& other);

	void					Set(double u,const gp_Pnt& pnt,const gp_Dir& T,
								const gp_Dir& N,const gp_Dir& B);

	double					_u;
	gp_Pnt					_O;//�ֲ�ԭ��
	gp_Dir					_dT,_dN,_dB;//tangent,Normal,biNormal
};
class TcCurveRMF{
public:
	TcCurveRMF();

	BOOL					Init(const Handle(Geom_Curve)& aCur,double df,double dl,int nSamps);
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
	std::vector<TcCurveFrame>	m_frmArray;
};

//////////////////////////////////////////////////////////////////////////
//��¼ÿ�������������Ϣ
class ScAxDFSurfInfo{
public:
	ScAxDFSurfInfo();
	~ScAxDFSurfInfo();

	//��ʼ��
	BOOL						Init(int nus,int nvs);

	void						SetUOfCurve(int iu,int iv,double u);
	double						UOfCurve(int iu,int iv);
	void						SetLcsCoord(int iu,int iv,const gp_Pnt& pnt);
	gp_Pnt						LcsCoord(int iu,int iv);

	int							IndexOf(int iu,int iv);

public:
	Handle(Geom_BSplineSurface)			m_cpySurf;//����������
	Handle(Geom_BSplineSurface)			m_tmpSurf;//�м�����,��ʱ��ʽʹ��
	int							m_nus,m_nvs;//u��v����Ƶ����
	double						*m_pUOfCurve;//���Ƶ��ڱ��������ϵĲ���ֵ��
	STR3D						m_pArLcsCoord;//�����ߵ�ľֲ�����ϵ�ڲ��ľֲ�����

	BOOL						m_bInit;//
};

class ScCmdAxDF : public ScCommand
{
public:
	ScCmdAxDF(void);
	~ScCmdAxDF(void);

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
	enum{S_OBJ,S_TOOL,S_SELPNT,S_MOVEPNT};

	BOOL					HasSelObj();
	BOOL					HasSelTool();
	BOOL					InitDeform();//Ϊ��������ʼ��

	void					ShowToolFrame();//��ʽ���ߵĿ��Ƶ㣬�Ա��ڱ༭��

	BOOL					ChangeToolCurve(const gp_Pnt& pnt,BOOL bTmp);
	BOOL					DeformObj(BOOL bTmp);

	ScAxDFSurfInfo			*m_pSurfInfo;//���������Ϣ
	Handle(Geom_BSplineCurve)		m_curTool,m_tmpTool;
	BOOL					m_bChanged;

	TcCurveRMF				*m_pCurRMF;//��¼�����Ϣ

	gp_Pnt					m_selPnt;
};
