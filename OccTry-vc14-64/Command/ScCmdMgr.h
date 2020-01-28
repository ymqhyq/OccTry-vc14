#pragma once

#include <afxtempl.h>
#include <AIS_InteractiveContext.hxx>
#include "ScCommand.h"
#include "ScUndoMgr.h"

typedef  CList<ScCommand *,ScCommand *>  TScCmdList;

class ScInputMgr;
class ScView;
class ScUcsMgr;
class ScSnapMgr;

#define CMS_IDLE			0 //����������
#define CMS_RUNNING			1 //��ִ������

class ScCmdMgr
{
public:
	ScCmdMgr(void);
	~ScCmdMgr(void);

	//ִ������
	int					RunCommand(ScCommand *pScCmd);
	//��ֹ��ǰ����ִ��
	int					StopCommand();
	//�����������
	void				RemoveAll();

	//Undo,Redo
	int					UndoCommand();
	int					RedoCommand();

	ScCommand*			CurrentCmd() const { return m_pScCmd; }
	BOOL				IsCmdRunning() const { return m_nState != CMS_IDLE; }
	int					MgrState() const { return m_nState; }

	void				SetAISContext(Handle(AIS_InteractiveContext) aCtx);
	void				SetScView(ScView *pScView);
	void				SetInputMgr(ScInputMgr *pInputMgr);
	void				SetUcsMgr(ScUcsMgr *pUcsMgr);
	void				SetSnapMgr(ScSnapMgr *pSnapMgr);

	ScUndoMgr&   UndoMgr() { return m_undoMgr; }

	//command��Ҫ�Լ�����select.bAuto:FALSE��command�Լ����������Զ�����
	void				NeedMultiSelect(BOOL bNeed);
	BOOL				IsMultiSelect();

	void				NeedSelect(BOOL bNeed);
	BOOL				IsNeedSelect() const { return m_bNeedSelect; }

public:
	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	void					DragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView);
	//����shift�µĿ�ѡ��ʽ
	void					ShiftDragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView);

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	void					InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//����shift���µĵ�ѡ��ʽ
	void					ShiftInputEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//����ƶ�����Ӧ����
	void					MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//����shift���µ�����ƶ�����Ӧ����
	void					ShiftMoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);


public:
	//�����Ϣ��Ӧ����
	virtual void		OnLButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void		OnLButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void		OnMouseMove(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void		OnLButtonDblClk(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void		OnRButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void		OnRButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);

protected:
	//�ⲿ֪ͨ
	//������ɵ��á�
	int					CmdFinished();
	//������ֹ���á�
	int					CmdTerminated();

	friend class ScCommand;
protected:
	//���redo�б�
	void				ClearRedoList();

protected:
	ScCommand			*m_pScCmd; //��ǰ����
	ScCommand			*m_pToFreeCmd;//Ҫ�ͷŵ�����,����û����ɵ�cmd.

	TScCmdList			m_scCmdList;//�����б�
	TScCmdList			m_cmdRedoList;//RedoList
	Handle(AIS_InteractiveContext)		m_AISContext;

	ScInputMgr			*m_pInputMgr;
	ScView				*m_pScView;//
	ScUcsMgr			*m_pScUcsMgr;
	ScSnapMgr			*m_pSnapMgr;

	ScUndoMgr		m_undoMgr;//����undo.

	int					m_nState; //״̬�����л�ִ��

	BOOL				m_bMultiSelect;
	BOOL				m_bNeedSelect;//�Ƿ���Ҫѡ��
};

inline void		ScCmdMgr::SetAISContext(Handle(AIS_InteractiveContext) aCtx)
{
	m_AISContext = aCtx;
	m_undoMgr.SetAISContext(aCtx);
}

inline void		ScCmdMgr::SetScView(ScView *pScView)
{
	m_pScView = pScView;
}

inline void		ScCmdMgr::SetInputMgr(ScInputMgr *pInputMgr)
{
	m_pInputMgr = pInputMgr;
}

inline void		ScCmdMgr::SetUcsMgr(ScUcsMgr *pUcsMgr)
{
	m_pScUcsMgr = pUcsMgr;
}

inline void		ScCmdMgr::SetSnapMgr(ScSnapMgr *pSnapMgr)
{
	m_pSnapMgr = pSnapMgr;
}

inline void		ScCmdMgr::NeedMultiSelect(BOOL bNeed)
{
	m_bMultiSelect = bNeed;
}

inline BOOL		ScCmdMgr::IsMultiSelect()
{
	return m_bMultiSelect;
}

inline void		ScCmdMgr::NeedSelect(BOOL bNeed)
{
	m_bNeedSelect = bNeed;
}