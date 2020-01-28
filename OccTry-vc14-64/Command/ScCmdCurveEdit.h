#pragma once

#include <vector>
#include "ScCommand.h"
#include "ScCmdCurveBase.h"

class ScCmdCurveEdit
{
public:
		ScCmdCurveEdit(void);
		~ScCmdCurveEdit(void);
};

//////////////////////////////////////////////////////////////////////////
// ���߱���ָ�
class ScCmdCurveSplitByPoint : public ScCmdCurveBase{
public:
		//�����ͽ�������
		virtual	  int				Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int				End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

		//����ƶ�����Ӧ����
		virtual	  void			MoveEvent(const CPoint& point,
												const Handle(V3d_View)& aView);

protected:
		enum{S_CURVE,S_POINT};

		BOOL						DoSplit();

		double						m_t;
		gp_Pnt					m_ptClk;//������������������ĵ㡣
		BOOL						m_bSelPnt;
};

//////////////////////////////////////////////////////////////////////////
// ���߱����߷ָ�
class ScCmdCurveSplitByCurve : public ScCmdCurveBase{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);


protected:
		enum{S_CURVE,S_TOOL};

		BOOL						DoSplit();
};

////////////////////////////////////////////////////////////////////////////
// ���߲ü�������ָ�����
class ScCmdCurveTrimSplit : public ScCmdCurveBase{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_CURVE1,S_CURVE2};

		BOOL						DoSplit();
};

////////////////////////////////////////////////////////////////////////////
// ���߲ü������ٲü�
class ScCmdCurveQuickTrim : public ScCmdCurveBase{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_CURVE1,S_CURVE2,S_TRIM};

		BOOL						DoSplit();
		BOOL						DoTrim(int nx,int ny);
		BOOL						DoTrimOver();

		BOOL						HasDel(std::vector<int>&  arx,int ix);//�Ƿ�ix�Ѿ�ɾ��

		TopTools_ListOfShape							m_shapeList1,m_shapeList2;
		TopTools_DataMapOfShapeInteger	m_shapeMap;
		std::vector<int>										m_arIdx1,m_arIdx2;//��¼ɾ��������±�.
		int																m_nNewC;//�´�����ʱ��������
};

////////////////////////////////////////////////////////////////////////////
// 
class ScCmdCurveFillet : public ScCmdCurveBase{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

		//���ո�����
		virtual   int			AcceptInput(double dVal); 
protected:
		enum{S_WIRE,S_VERTEX,S_RADIUS};

		BOOL					HasClosedWire();
		BOOL					MakeFillet();

		TopoDS_Face				m_aFace;
		TopoDS_Vertex			m_aVertex;//Ҫ���ǵĵ�
		double					m_dRadius;//���ǰ뾶
};

////////////////////////////////////////////////////////////////////////////
//
class ScCmdCurveChamfer : public ScCommand{
public:
		//�����ͽ�������
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//��ѡ��ʽ�µ���Ϣ��Ӧ����
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);
};