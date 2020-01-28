#pragma once

#include <AIS_InteractiveContext.hxx>
#include <AIS_SequenceOfInteractive.hxx>
#include <AIS_Shape.hxx>
#include <list>


//一个undo的项
class ScUndoItem{
public:
		ScUndoItem(){
				m_strName = "Undo command";
		}
		~ScUndoItem();

		BOOL								IsEmpty() const;

public:
		CString												m_strName;//undo的名称
		AIS_SequenceOfInteractive				m_seqOfNewObj;//新创建的对象
		AIS_SequenceOfInteractive				m_seqOfDelObj;//删除的对象
};

//暂时只支持undo。
class ScUndoMgr
{
public:
		ScUndoMgr(void);
		~ScUndoMgr(void);

		void									SetAISContext(Handle(AIS_InteractiveContext)& aCtx);

		//设置undo的最大次数
		void									SetLimit(int nMax);

		BOOL								IsEmpty() const;

		//开始一个undo项
		int										BeginUndo(LPCTSTR lpszName);
		int										AddNewObj(const Handle(AIS_InteractiveObject)& aObj);
		int										AddDelObj(const Handle(AIS_InteractiveObject)& aObj);
		int										EndUndo(BOOL bCommit);

		//执行一个undo
		int										Undo();

protected:
		Handle(AIS_InteractiveContext)			m_AISContext;

		std::list<ScUndoItem *>							m_undoList;//undo队列
		int																m_nUndoMax;//undo的最大次数

		ScUndoItem											*m_pCurItem;//当前的项。
};

inline  void		ScUndoMgr::SetAISContext(Handle(AIS_InteractiveContext)& aCtx)
{
		m_AISContext = aCtx;
}

//设置undo的最大次数
inline	void     ScUndoMgr::SetLimit(int nMax)
{
		m_nUndoMax = nMax;
}

inline BOOL	 ScUndoMgr::IsEmpty() const
{
		return (m_undoList.size() == 0);
}