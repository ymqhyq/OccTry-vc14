#ifndef _OCCTRY_SC_COMMAND_H_
#define _OCCTRY_SC_COMMAND_H_

#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_SequenceOfInteractive.hxx>
#include <AIS_Shape.hxx>

#include "ScInput.h"

#define SC_OK		0 //�ɹ�
#define SC_ERR      -1 //ʧ��

typedef double PNT3D[3] ; // point  type defined in 3 dimension
typedef double (*STR3D)[3] ; // pt array defined in 3 dimension

class ScCmdMgr;
class ScInputMgr;
class ScView;
class ScUcsMgr;
class ScSnapMgr;
class ScUndoMgr;
//class ScInputVar;
//class ScInputReq;

#define SC_INPUT_ERR    -1 //�������ݴ���

enum{//������Ϣ������
	MINPUT_LEFT,//�������
	MINPUT_RIGHT,//�Ҽ�����
	MINPUT_LDBCLK,//���˫��
	MINPUT_RDBCLK,//�Ҽ�˫��
};

enum{
	SEL_OK,//��ȷѡ�����
	SEL_NONE,//û��ѡ�ж���
	SEL_LESS,//��ѡ
	SEL_MORE,//��ѡ
};

class ScCommand
{
public:
	ScCommand();
	virtual ~ScCommand();

	//���������ʼִ�С�
	virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
	//�������ͨ��������ֹ���
	virtual   int						End();

	virtual   void						Reset();

	virtual   BOOL						IsDone() const { return m_bDone; }
	virtual   void						Done();

	//Undo֧�֣�Redo֧��
	//�Ƿ�֧��undo����Щ���֧�֡�
	virtual	  BOOL						CanUndo() const { return m_bCanUndo; }
	virtual	  int						Undo();
	virtual   int						Redo();

	void								Prompt(LPCTSTR lpszText);

	virtual   void						SetState(int nState);
	virtual   int						GetState() const { return m_nState; }
	
	//��Ļ���ص�ת��Ϊģ�Ϳռ�ĵ�
	virtual   gp_Pnt					PixelToModel(int x,int y,BOOL bWCS = TRUE);
	//��Ļ��ת��Ϊcs��ƽ���ϵĵ�
	virtual   gp_Pnt					PixelToCSPlane(Standard_Real x, Standard_Real y,BOOL bWCS = TRUE);

	//����ת������
	int									WCSToUCS(gp_Pnt& pnt);
	int									UCSToWCS(gp_Pnt& pnt);
	int									WCSToUCS(gp_Vec& vec);
	int									UCSToWCS(gp_Vec& vec);

	void								NeedMultiSelect(BOOL bNeed);
	void								NeedSelect(BOOL bNeed);

	//�Զ���׽
	void								NeedSnap(BOOL bEnable);
	void								SnapPoint(int nX,int nY,const Handle(V3d_View)& aView);
	gp_Pnt								GetSnapPoint(const gp_Pnt& pnt);

public:
	virtual   void						SetScCmdMgr(ScCmdMgr *pScCmdMgr) { this->m_pScCmdMgr = pScCmdMgr; }
	virtual   void						SetInputMgr(ScInputMgr *pInputMgr) { this->m_pInputMgr = pInputMgr; }
	virtual   void						SetScView(ScView *pScView) { this->m_pScView = pScView; }
	virtual   void						SetUcsMgr(ScUcsMgr *pUcsMgr) { m_pScUcsMgr = pUcsMgr; }
	virtual   void						SetSnapMgr(ScSnapMgr *pSnapMgr) { m_pSnapMgr = pSnapMgr; }
	virtual   void						SetUndoMgr(ScUndoMgr *pUndoMgr) { m_pUndoMgr = pUndoMgr; }

public:
	//������غ����������˷�װ,������á�
	//��Ҫ����ʼ��������
	virtual int							NeedInput(LPCTSTR lpszTitle,int nInputType,DWORD dwWaitTime = 0);
	//��Ҫ��������
	virtual int							NeedInteger(LPCTSTR lpszDesc);
	//��Ҫ���븡����
	virtual int							NeedDouble(LPCTSTR lpszDesc);
	//��Ҫ3ά��
	virtual int							NeedPoint3d(LPCTSTR lpszDesc,BOOL bWCS = TRUE);
	//��Ҫ�ַ���
	virtual int							NeedString(LPCTSTR lpszDesc);
	//������������
	virtual int							EndInput();

	//��ʾ��Ϣ����
	virtual void						Show(LPCTSTR lpszVal);
	virtual void						Show(int nVal);
	virtual void						Show(double dVal);
	virtual void						Show(gp_Pnt pnt);

	//�����������Ҫ������
	virtual   int						AcceptInput(const ScInputReq& inputReq,
													const ScInputVar& inputVar);
	//��������
	virtual   int						AcceptInput(int nVal) { return SC_OK; }
	//���ո�����
	virtual   int						AcceptInput(double dVal) { return SC_OK; }
	//����3ά��
	virtual   int						AcceptInput(gp_Pnt& pnt) { return SC_OK; }
	//����2ά��
	virtual   int						AcceptInput(gp_Pnt2d& pnt) { return SC_OK; }
	//��������
	virtual   int						AcceptInput(gp_Vec& vec) { return SC_OK; }
	//�����ַ���
	virtual	  int						AcceptInput(const CString& str) { return SC_OK; }

public:
	
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void						InputEvent(const CPoint& point,int nState,//����minput����
													const Handle(V3d_View)& aView);
	//����ƶ�����Ӧ����
	virtual	  void						MoveEvent(const CPoint& point,
													const Handle(V3d_View)& aView);

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void						DragEvent(const CPoint& point,int nMbState,
													const Handle(V3d_View)& aView);
		
	//����shift���µĵ�ѡ��ʽ
	virtual	  void						ShiftInputEvent(const CPoint& point,
													const Handle(V3d_View)& aView);

	//����shift���µ�����ƶ�����Ӧ����
	virtual	  void						ShiftMoveEvent(const CPoint& point,
													const Handle(V3d_View)& aView);
	//����shift�µĿ�ѡ��ʽ
	virtual	  void						ShiftDragEvent(const CPoint& point,int nMbState,
													const Handle(V3d_View)& aView);

public:
	//�����Ϣ��Ӧ����
	virtual void						OnLButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void						OnLButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void						OnMouseMove(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void						OnLButtonDblClk(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void						OnRButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void						OnRButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);

protected://����������undo�ĺ���
		//��ʼһ��undo���������
		virtual		int						BeginUndo(LPCTSTR lpszName);
		//��¼һ���´����Ķ���
		virtual      int						UndoAddNewObj(const Handle(AIS_InteractiveObject)& aObj);
		//��¼һ��ɾ���Ķ���
		virtual      int						UndoAddDelObj(const Handle(AIS_InteractiveObject)& aObj);
		//����undo��ύ���Ƿ���
		virtual     int						EndUndo(BOOL bCommit);

		BOOL								m_bBeginUndo;//�Ƿ�ʼUndo��

protected:
	//��AisCtx���Ƴ���ʱobject�����ͷ��ڴ档����ؼ����ͷ��ڴ档
	virtual void						RemoveCtxObj(Handle(AIS_InteractiveObject)& aObj,BOOL bUpdate = FALSE);
	virtual	void						SwitchState(int nState,LPCTSTR lpszPrompt,
													BOOL bNeedModelPoint = FALSE);

	//��ʾһ�����󣬲���¼�ö�������ɫģʽ��ʾ����������ʾ�����Ƽ�ʹ�øú�����
	virtual void						Display(Handle(AIS_InteractiveObject)& aObj,
												const TopoDS_Shape& aShape,
												BOOL bUpdate = TRUE);

	//��ʾһ�����󣬲���¼�ö�������ɫģʽ��ʾ����������ʾ�����Ƽ�ʹ�øú�����
	virtual void						Display(Handle(AIS_InteractiveObject)& aObj,
												const TopoDS_Shape& aShape,
												COLORREF color,//��ʾ��ɫ
												BOOL bUpdate = TRUE);

	//��ʾһ�����󣬲���¼�ö������߿�ģʽ��ʾ����������ʾ�����Ƽ�ʹ�øú�����
	virtual void						WFDisplay(Handle(AIS_InteractiveObject)& aObj,
													const TopoDS_Shape& aShape,
													BOOL bUpdate = TRUE);

	virtual void						Display(const TopoDS_Shape& aShape);
	//��ȡһ��ѡ���shape��ֻ����һ��
	virtual int							GetOneCurrentObj(Handle(AIS_InteractiveObject)& aObj);
	//��ȡһ��ѡ���shape��ֻ����һ��
	virtual int							GetOneSelShape(TopoDS_Shape& aShape);
	//��ȡ��ǰѡ���shape��
	virtual int							GetCurrentShape(TopoDS_Shape& aShape);

	//��ȡ�����shape
	virtual TopoDS_Shape				GetObjShape(Handle(AIS_InteractiveObject)& aObj);

	//���������ȡ����ǰ�����Face
	virtual TopoDS_Face					GetObjPickedFace(Handle(AIS_InteractiveObject)& aObj,
														int nX,int nY,const Handle(V3d_View)& aView);
	
	//���������ȡ�����оݵ���������edge
	virtual TopoDS_Edge					GetNearestEdge(Handle(AIS_InteractiveObject)& aObj,
														int nX,int nY,double& t,gp_Pnt& pnt);

protected:
	//�Զ���ѡ���һ���װ
	void								InitSelect();
	BOOL								MoreSelect();
	void								NextSelect();
	void								ClearSelect();
	TopoDS_Shape						SelectedShape(BOOL bNext = TRUE);
	Handle(AIS_InteractiveObject)		SelectedObj(BOOL bNext = TRUE);

protected:
	Handle(AIS_InteractiveContext)		m_AISContext;
	Handle(V3d_View)					m_View;

	BOOL								m_bDone;//�Ƿ�ִ�����
	ScCmdMgr							*m_pScCmdMgr; //������
	ScInputMgr							*m_pInputMgr;//�������
	ScView								*m_pScView;//
	ScUcsMgr							*m_pScUcsMgr;
	ScSnapMgr							*m_pSnapMgr;
	ScUndoMgr						*m_pUndoMgr;

	int									m_nState; //��ǰ��״̬��ÿ������в�ͬ��״̬
	BOOL								m_bCanUndo; //�Ƿ����undo��

protected://�������
	//˵��������seq�м�¼�Ķ���obj��handle�������ã����ʹ�����ˣ�handle����NULL�����ܻ�
	//      ����handle���õĶ��󲻻ᱻ�ͷţ����ʹ��Ҫ��ϸ��

	//�´�����obj����������.
	AIS_SequenceOfInteractive			m_seqOfNewObj;
	//ɾ����obj����������,���뱣���Ѿ�ɾ���Ķ���׼��ɾ���Ķ���Ҫ�������
	AIS_SequenceOfInteractive			m_seqOfDelObj;
	//��ʱ��ʾ��obj����������.��Щ��������Ҫɾ������
	AIS_SequenceOfInteractive			m_seqOfTmpObj;
	//��¼ѡ�еĶ��������ʹ������Ķ��󣬿����Լ��ھ���command�ж��������
	AIS_SequenceOfInteractive			m_seqOfSelObj;

	//��ȡ��Ӧ��obj�����û�У�����֮��idx Ϊ-1�򳬳��б�Χ,������¶����б��С�
	Handle(AIS_InteractiveObject)&		NewObj(int idx);
	Handle(AIS_InteractiveObject)&		DelObj(int idx);
	Handle(AIS_InteractiveObject)&		TmpObj(int idx);
	Handle(AIS_InteractiveObject)&		SelObj(int idx);

	//���Ҷ�Ӧ��Obj.
	int									FindNewObj(const TopoDS_Shape& aShape);
	void								RemoveNewObj(int idx);
											
	//��������´����Ķ���
	virtual		int						RemoveNewObjs();
	//�ָ�����ɾ������
	virtual		int						RestoreDelObjs();
	//���������ʱ����
	virtual		int						ClearTmpObjs();
	//�����¼��ѡ�ж�������ֻ�����reference��
	virtual		int						ClearSelObjs();

private:
	BOOL								m_bNeedInput;//�Ƿ���Ҫ����
	DWORD								m_dwOldTime;
};

inline void	ScCommand::SetState(int nState)
{
	m_nState = nState;
}

#endif