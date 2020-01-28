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

#define CMS_IDLE			0 //管理器空闲
#define CMS_RUNNING			1 //在执行命令

class ScCmdMgr
{
public:
	ScCmdMgr(void);
	~ScCmdMgr(void);

	//执行命令
	int					RunCommand(ScCommand *pScCmd);
	//终止当前命令执行
	int					StopCommand();
	//清除所有命令
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

	//command需要自己处理select.bAuto:FALSE，command自己处理，否则自动处理
	void				NeedMultiSelect(BOOL bNeed);
	BOOL				IsMultiSelect();

	void				NeedSelect(BOOL bNeed);
	BOOL				IsNeedSelect() const { return m_bNeedSelect; }

public:
	//框选方式下的消息响应函数
	void					DragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView);
	//按下shift下的框选方式
	void					ShiftDragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView);

	//点选方式下的消息响应函数
	void					InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);
	
	//按下shift键下的点选方式
	void					ShiftInputEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//鼠标移动的响应函数
	void					MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);

	//按下shift键下的鼠标移动的响应函数
	void					ShiftMoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView);


public:
	//鼠标消息响应函数
	virtual void		OnLButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void		OnLButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void		OnMouseMove(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void		OnLButtonDblClk(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void		OnRButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void		OnRButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);

protected:
	//外部通知
	//命令完成调用。
	int					CmdFinished();
	//命令中止调用。
	int					CmdTerminated();

	friend class ScCommand;
protected:
	//清除redo列表
	void				ClearRedoList();

protected:
	ScCommand			*m_pScCmd; //当前命令
	ScCommand			*m_pToFreeCmd;//要释放的命令,用于没有完成的cmd.

	TScCmdList			m_scCmdList;//命令列表
	TScCmdList			m_cmdRedoList;//RedoList
	Handle(AIS_InteractiveContext)		m_AISContext;

	ScInputMgr			*m_pInputMgr;
	ScView				*m_pScView;//
	ScUcsMgr			*m_pScUcsMgr;
	ScSnapMgr			*m_pSnapMgr;

	ScUndoMgr		m_undoMgr;//管理undo.

	int					m_nState; //状态，空闲或执行

	BOOL				m_bMultiSelect;
	BOOL				m_bNeedSelect;//是否需要选择
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