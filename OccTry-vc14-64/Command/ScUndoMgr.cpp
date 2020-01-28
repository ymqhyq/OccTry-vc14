#include "StdAfx.h"
#include ".\scundomgr.h"

//////////////////////////////////////////////////////////////////////////
//
ScUndoItem::~ScUndoItem()
{
		m_seqOfNewObj.Clear();
		m_seqOfDelObj.Clear();
}

BOOL		ScUndoItem::IsEmpty() const
{
		if(m_seqOfNewObj.Length() == 0  &&
				m_seqOfDelObj.Length() == 0)
				return TRUE;

		return FALSE;
}

ScUndoMgr::ScUndoMgr(void)
{
		this->m_nUndoMax = 64;//Ĭ��
		this->m_pCurItem = NULL;
}

ScUndoMgr::~ScUndoMgr(void)
{
}

int			ScUndoMgr::BeginUndo(LPCTSTR lpszName)
{
		if(m_pCurItem)
		{
				ASSERT(FALSE);
				EndUndo(FALSE);
		}

		m_pCurItem = new ScUndoItem;
		if(lpszName)
				m_pCurItem->m_strName = lpszName;

		return 0;
}

int			ScUndoMgr::AddNewObj(const Handle(AIS_InteractiveObject)& aObj)
{
		if(!m_pCurItem)
		{
				ASSERT(FALSE);
				return -1;
		}

		for(int ix = 1;ix <= m_pCurItem->m_seqOfNewObj.Length();ix++)
		{
				Handle(AIS_InteractiveObject) aTmpObj = m_pCurItem->m_seqOfNewObj.Value(ix);
				if(aTmpObj == aObj)
				{
						ASSERT(FALSE);
						return 0;
				}
		}

		m_pCurItem->m_seqOfNewObj.Append(aObj);

		return 0;
}

int			ScUndoMgr::AddDelObj(const Handle(AIS_InteractiveObject)& aObj)
{
		if(!m_pCurItem)
		{
				ASSERT(FALSE);
				return -1;
		}

		for(int ix = 1;ix <= m_pCurItem->m_seqOfDelObj.Length();ix++)
		{
				Handle(AIS_InteractiveObject) aTmpObj = m_pCurItem->m_seqOfDelObj.Value(ix);
				if(aTmpObj == aObj)
				{
						ASSERT(FALSE);
						return 0;
				}
		}

		m_pCurItem->m_seqOfDelObj.Append(aObj);

		return 0;
}

int			ScUndoMgr::EndUndo(BOOL bCommit)
{
		if(!m_pCurItem)
				return 0;

		if(m_pCurItem->IsEmpty() || !bCommit)
		{
				delete m_pCurItem;
				m_pCurItem = NULL;
				return 0;
		}

		m_undoList.push_back(m_pCurItem);
		m_pCurItem = NULL;

		//����Ƿ񳬽��ˡ�
		int nsize = m_undoList.size();
		if(nsize >= m_nUndoMax)
		{
				ScUndoItem *ptItem = m_undoList.front();
				m_undoList.pop_front();
				delete ptItem;
		}

		return 0;
}

//ִ��һ��undo
int			ScUndoMgr::Undo()
{
		if(m_undoList.empty())
				return 0;

		ScUndoItem *pItem = m_undoList.back();
		m_undoList.pop_back();

		//ɾ�������Ķ���
		int ix;
		for(ix = 1;ix <= pItem->m_seqOfNewObj.Length();ix ++)
		{
				Handle(AIS_InteractiveObject) aTmpObj = pItem->m_seqOfNewObj.Value(ix);
				m_AISContext->Remove(aTmpObj,Standard_False);
		}
		pItem->m_seqOfNewObj.Clear();

		//��������Ѿ�ɾ���Ķ���
		for(ix = 1;ix <= pItem->m_seqOfDelObj.Length();ix ++)
		{
				Handle(AIS_InteractiveObject) aObj = pItem->m_seqOfDelObj.Value(ix);
				m_AISContext->SetDisplayMode(aObj,1,Standard_False);
				m_AISContext->Display(aObj,Standard_False);
				m_AISContext->Redisplay(aObj,-1,Standard_False);
				//���´���ᵼ��undoֻ��һ���������ܶಽ����Ϊ�����ӵĶ����������һ��undo�ж���
				//���ڴ������µ�obj����ԭ���������Ϣ�Ѿ���һ���ˡ�
				/*Handle(AIS_Shape) aAIShape = Handle(AIS_Shape)::DownCast(aObj);
				if(!aAIShape.IsNull())
				{
						TopoDS_Shape aShape = aAIShape->Shape();
						if(!aShape.IsNull())
						{
								Handle(AIS_Shape) aNObj = new AIS_Shape(aShape);
								m_AISContext->Display(aNObj,Standard_False);
						}
				}*/
		}
		pItem->m_seqOfDelObj.Clear();

		m_AISContext->UpdateCurrentViewer();
		delete pItem;

		return 0;
}