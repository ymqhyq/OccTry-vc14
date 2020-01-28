#pragma once

#include <AIS_InteractiveContext.hxx>
#include <AIS_SequenceOfInteractive.hxx>
#include <AIS_Shape.hxx>
#include <list>


//һ��undo����
class ScUndoItem{
public:
		ScUndoItem(){
				m_strName = "Undo command";
		}
		~ScUndoItem();

		BOOL								IsEmpty() const;

public:
		CString												m_strName;//undo������
		AIS_SequenceOfInteractive				m_seqOfNewObj;//�´����Ķ���
		AIS_SequenceOfInteractive				m_seqOfDelObj;//ɾ���Ķ���
};

//��ʱֻ֧��undo��
class ScUndoMgr
{
public:
		ScUndoMgr(void);
		~ScUndoMgr(void);

		void									SetAISContext(Handle(AIS_InteractiveContext)& aCtx);

		//����undo��������
		void									SetLimit(int nMax);

		BOOL								IsEmpty() const;

		//��ʼһ��undo��
		int										BeginUndo(LPCTSTR lpszName);
		int										AddNewObj(const Handle(AIS_InteractiveObject)& aObj);
		int										AddDelObj(const Handle(AIS_InteractiveObject)& aObj);
		int										EndUndo(BOOL bCommit);

		//ִ��һ��undo
		int										Undo();

protected:
		Handle(AIS_InteractiveContext)			m_AISContext;

		std::list<ScUndoItem *>							m_undoList;//undo����
		int																m_nUndoMax;//undo��������

		ScUndoItem											*m_pCurItem;//��ǰ���
};

inline  void		ScUndoMgr::SetAISContext(Handle(AIS_InteractiveContext)& aCtx)
{
		m_AISContext = aCtx;
}

//����undo��������
inline	void     ScUndoMgr::SetLimit(int nMax)
{
		m_nUndoMax = nMax;
}

inline BOOL	 ScUndoMgr::IsEmpty() const
{
		return (m_undoList.size() == 0);
}