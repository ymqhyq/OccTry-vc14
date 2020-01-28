#include "StdAfx.h"

#include <AIS_TexturedShape.hxx>

#include "ScDlgTexture.h"
#include ".\sccmddisplay.h"

ScCmdDisplay::ScCmdDisplay(void)
{
}

ScCmdDisplay::~ScCmdDisplay(void)
{
}

//////////////////////////////////////////////////////////
// 对象纹理的实现
// 使用AIS_TextureShape实现

//启动和结束命令
int	ScCmdTexture::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	this->m_bModified = FALSE;
	if(HasSelectedShape())
	{
		SwitchState(S_TEXTURE,"选择纹理.");
		
		DoTexture();
	}
	else
	{
		SwitchState(S_SHAPE,"选择一个对象");
		NeedSelect(TRUE);
	}
	
	return 0;
}

int	ScCmdTexture::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdTexture::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		if(this->m_bModified)
			Done();
		else
			End();
	}
	else
	{
		switch(GetState())
		{
		case S_SHAPE:
			{
				if(HasSelectedShape())
				{
					SwitchState(S_TEXTURE,"选择纹理.");
					
					DoTexture();
				}
			}
			break;
		case S_TEXTURE:
			break;
		default:
			break;
		}
	}
}
	
BOOL	ScCmdTexture::HasSelectedShape()
{
	InitSelect();
	if(!MoreSelect())
		return FALSE;

	TopoDS_Shape aS = SelectedShape(FALSE);
	if(aS.IsNull())
		return FALSE;

	if(aS.ShapeType() == TopAbs_FACE || 
		aS.ShapeType() == TopAbs_SHELL ||
		aS.ShapeType() == TopAbs_SOLID)
	{
		DelObj(0) = SelectedObj(FALSE);

		return TRUE;
	}

	return FALSE;
}

//
// 设置纹理,删除旧的对象，生成新的对象。
//
BOOL	ScCmdTexture::SetTexture(LPCTSTR lpszFile,BOOL bDone)
{
	if(this->m_bModified == FALSE)
	{
		this->m_bModified = TRUE;
		m_AISContext->Remove(DelObj(0), Standard_True);
	}
	else
	{
		RemoveCtxObj(NewObj(0));
	}

	TopoDS_Shape aS = GetObjShape(DelObj(0));
	Handle(AIS_TexturedShape) aTS = new AIS_TexturedShape(aS);
	aTS->SetTextureFileName((Standard_CString)lpszFile);
	NewObj(0) = aTS;

	aTS->SetTextureMapOn();
	m_AISContext->SetDisplayMode(aTS,3,Standard_False);
	m_AISContext->Display(aTS, Standard_True);

	if(bDone)
	{
//		m_AISContext->Remove(DelObj(0));
	}

	
	return TRUE;
}

int		ScCmdTexture::DoTexture()
{
	ScDlgTexture dlgTexture;
	dlgTexture.m_pCmdTexture = this;
	dlgTexture.DoModal();

	if(this->m_bModified)
		Done();
	else
		End();

	return 0;
}