#include "StdAfx.h"
#include ".\sccmdedit.h"

ScCmdEdit::ScCmdEdit(void)
{
}

ScCmdEdit::~ScCmdEdit(void)
{
}

///////////////////////////////////////////////////////////////
//

//�����ͽ�������
int		ScCmdDelete::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	int nC = 0;
	InitSelect();
	while(MoreSelect())
	{
		TopoDS_Shape aShape = SelectedShape(FALSE);
		if(!aShape.IsNull())
		{
			DelObj(nC) = SelectedObj(FALSE);
			nC ++;
		}

		NextSelect();
	}

	if(nC == 0)
	{
		End();
	}
	else
	{
		//ɾ��
	    BeginUndo("Delete");

		int nLen = this->m_seqOfDelObj.Length();
		for(int ix = 1;ix <= nLen;ix ++)
		{
				Handle(AIS_InteractiveObject) aObj = this->m_seqOfDelObj.Value(ix);
				m_AISContext->Remove(aObj,Standard_False);
				UndoAddDelObj(aObj);
		}
		this->m_seqOfDelObj.Clear();
		//���ͳһ����
		m_AISContext->UpdateCurrentViewer();

		EndUndo(TRUE);
		//
		Done();
	}

	return 0;
}

int		ScCmdDelete::End()
{
	return ScCommand::End();
}