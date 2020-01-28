#pragma once

#include "ScCommand.h"

class FFDSurfObj;
class FFDSurfTool;
class FFDSurfAlgo;

//////////////////////////////////////////////////////////////////////////
// ��¼���浽���������ͶӰ��Ϣ
class TcSurfPrjInfo{
public:
	TcSurfPrjInfo();
	~TcSurfPrjInfo();

	BOOL					Init(int nus,int nvs);

	void					SetDist(int iu,int iv,double dist);
	double					GetDist(int iu,int iv);
	
	void					SetPrjUV(int iu,int iv,double u,double v);
	gp_Pnt2d				GetPrjUV(int iu,int iv);

	int						IndexOf(int iu,int iv);

public:
	Handle(Geom_BSplineSurface)		m_cpySurf;//������surf.

	int						m_nus,m_nvs;//���Ƶ����
	double					*m_pDstArray;//����
	STR3D					m_uvArray;//����

	BOOL					m_bInit;
};

class ScCmdSurfFFD : public ScCommand
{
public:
	ScCmdSurfFFD(void);
	~ScCmdSurfFFD(void);

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
	enum{S_OBJ,S_SELPNT,S_BASPNT,S_CHANGING};

	BOOL					HasSelObj();
	BOOL					InitDefrom();
	BOOL					ShowCtrlPnts();
	BOOL					ChangeToolSurf(BOOL bTmp);
	BOOL					DoDeform(BOOL bDone);

protected:
	Handle(Geom_BSplineSurface)		m_toolSurf,m_tmpSurf;
	TColgp_SequenceOfPnt			m_seqOfPnt;//ѡ�е�
	gp_Pnt							m_basPnt,m_chgPnt;//������µ�
	BOOL							m_bChanged;//

	TcSurfPrjInfo					*m_pSurfPrjInfo;

	FFDSurfAlgo				*m_pFFDAlgo;
	FFDSurfObj				*m_pFFDObj;
	FFDSurfTool				*m_pFFDTool;
};
