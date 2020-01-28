#pragma once

#include <vector>

class FFDSurfObj;
class FFDSurfTool;
class FFDLineTool;
class FFDCurveTool;
class FFDSurfFlowTool;
class FFDShearTool;
class FFDTwistTool;
class FFDBendTool;

class FFDAlgo
{
public:
	FFDAlgo(void);
	virtual ~FFDAlgo(void);

	BOOL					IsValid() const { return m_bValid; }
	BOOL					IsDone() const { return m_bDone; }

	TopoDS_Shape			Shape() const { return m_aNewShape; };

protected:
	BOOL					m_bValid;
	BOOL					m_bDone;
	TopoDS_Shape			m_aNewShape;
};

//////////////////////////////////////////////////////////////////////////
//

class FFDSurfToolMapItem{
public:
	FFDSurfToolMapItem(){
		m_u = m_v = 0.0;
		m_dist = 0.0;
	}

	double					m_u,m_v;//���ε��ڻ�׼�����ϵĲ�������
	double					m_dist;//���ε㵽��׼�����������롣
};

//����ͱ��ι��߼��ӳ���ϵ
class FFDSurfToolMap{
public:
	FFDSurfToolMap();
	~FFDSurfToolMap();

	BOOL					Init(int nus,int nvs);

	//��������ͶӰ������������
	BOOL					MapToDatumSurf(FFDSurfTool *pTool);

	void					SetDist(int iu,int iv,double dist);
	double					GetDist(int iu,int iv);

	void					SetPrjUV(int iu,int iv,double u,double v);
	gp_Pnt2d				GetPrjUV(int iu,int iv);

	int						IndexOf(int iu,int iv);

public:
	Handle(Geom_BSplineSurface)			m_aSurf;//Ҫ���ε�����

	int									m_nus,m_nvs;//���Ƶ����
	FFDSurfToolMapItem					*m_pSurfToolMap;//�����Ͽ��Ƶ�ͻ�׼����Ĺ���ӳ�䡣

	BOOL								m_bInit;//�Ƿ��ʼ��
};

class FFDSurfData{
public:
	TopoDS_Face				m_aFace;//����face
	FFDSurfToolMap			m_aMap;//������Ϣ
};

//��������ı���
class FFDSurfAlgo : public FFDAlgo{
public:
	FFDSurfAlgo();
	virtual ~FFDSurfAlgo();

	//�㷨��ʼ��
	BOOL					Init(FFDSurfObj *pObj,FFDSurfTool *pTool);

	//���б���
	BOOL					Deform();


protected:
	FFDSurfObj				*m_pObj;
	FFDSurfTool				*m_pTool;

	//��Ҫ����surf����map��Ĺ�ϵ
	std::vector<FFDSurfData *>			m_surfDatas;//face����Ϣ
};

//////////////////////////////////////////////////////////////////////////
//

//��¼һ�����Ƶ���frame�ڵ�����
class FFDCurveToolMapItem{
public:
	FFDCurveToolMapItem();

	double					m_t;//���Ƶ��Ӧ�������ϵĲ���ֵ��
};

//��¼�������tool�Ķ�Ӧ��ϵ
class FFDCurveToolMap{
public:
	FFDCurveToolMap();
	~FFDCurveToolMap();

	//��ʼ��
	BOOL							Init(const Handle(Geom_BSplineSurface)& aSurf);

	//���úͲ�ѯ����
	void							SetParam(int iu,int iv,double t);
	double							GetParam(int iu,int iv);

	int								IndexOf(int iu,int iv);

public:
	Handle(Geom_BSplineSurface)		m_aSurf;//Ҫ���ε�����

	int								m_nus,m_nvs;//u��v����Ƶ����
	FFDCurveToolMapItem				*m_pMapData;//ӳ�����ݣ�Ϊÿ�����Ƶ��ڶ�Ӧ�ľֲ�����ϵ�ڵ���Ϣ��

	BOOL							m_bInit;//
};

//����������������
class FFDCurveSurfData{
public:
	TopoDS_Face						m_aFace;
	FFDCurveToolMap					m_aMap;
};

//����ֱ�ߵı���
class FFDLineAlgo : public FFDAlgo{
public:
	FFDLineAlgo();
	virtual ~FFDLineAlgo();

	//�㷨��ʼ��
	BOOL					Init(FFDSurfObj *pObj,FFDLineTool *pTool);

	void					Clear();

	//���б���
	BOOL					Deform();


protected:
	FFDCurveSurfData*		GetSurfData(const TopoDS_Face& aFace);

protected:
	FFDSurfObj				*m_pObj;
	FFDLineTool				*m_pTool;

	std::vector<FFDCurveSurfData *>		m_surfDatas;//���������

};

//�������ߵı���
class FFDCurveAlgo : public FFDAlgo{
public:
	FFDCurveAlgo();
	virtual ~FFDCurveAlgo();

	//�㷨��ʼ��
	BOOL					Init(FFDSurfObj *pObj,FFDCurveTool *pTool);

	//���б���
	BOOL					Deform();


protected:
	FFDSurfObj				*m_pObj;
	FFDCurveTool			*m_pTool;


};

//////////////////////////////////////////////////////////////////////////
// ����������
class FFDSurfFlowAlgo : public FFDAlgo{
public:
	FFDSurfFlowAlgo();
	virtual ~FFDSurfFlowAlgo();

	//�㷨��ʼ��
	BOOL					Init(FFDSurfObj *pObj,FFDSurfFlowTool *pTool);

	//���б���
	BOOL					Deform();

protected:
	FFDSurfObj				*m_pObj;
	FFDSurfFlowTool			*m_pTool;
};

//////////////////////////////////////////////////////////////////////////
// shear��б����
class FFDShearAlgo : public FFDAlgo{
public:
	FFDShearAlgo();
	virtual ~FFDShearAlgo();

	//�㷨��ʼ��
	BOOL					Init(FFDSurfObj *pObj,FFDShearTool *pTool);

	//���б���
	BOOL					Deform();

protected:
	FFDSurfObj				*m_pObj;
	FFDShearTool			*m_pTool;
};

//////////////////////////////////////////////////////////////////////////
// Ťת�����㷨
class FFDTwistAlgo : public FFDAlgo{
public:
	FFDTwistAlgo();
	virtual ~FFDTwistAlgo();

	//�㷨��ʼ��
	BOOL					Init(FFDSurfObj *pObj,FFDTwistTool *pTool);

	//���б���
	BOOL					Deform();

protected:
	FFDSurfObj				*m_pObj;
	FFDTwistTool			*m_pTool;
};

//////////////////////////////////////////////////////////////////////////
// ���������㷨,�õ�������������������
class FFDBendAlgo : public FFDAlgo{
public:
	FFDBendAlgo();
	virtual	~FFDBendAlgo();

	//��ʼ��
	BOOL					Init(FFDSurfObj *pObj,FFDBendTool *pTool);

	//���б���
	BOOL					Deform();

protected:
	FFDSurfObj				*m_pObj;
	FFDBendTool				*m_pTool;
};