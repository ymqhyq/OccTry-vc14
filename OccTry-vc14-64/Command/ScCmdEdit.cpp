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

//启动和结束命令
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
		//删除
	    BeginUndo("Delete");

		int nLen = this->m_seqOfDelObj.Length();
		for(int ix = 1;ix <= nLen;ix ++)
		{
				Handle(AIS_InteractiveObject) aObj = this->m_seqOfDelObj.Value(ix);
				m_AISContext->Remove(aObj,Standard_False);
				UndoAddDelObj(aObj);
		}
		this->m_seqOfDelObj.Clear();
		//最后统一更新
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