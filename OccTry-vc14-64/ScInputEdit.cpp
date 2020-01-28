// ScInputEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "OccTry.h"
#include "ScInputEdit.h"
#include ".\scinputedit.h"
#include "ScInputBar.h"


// ScInputEdit

IMPLEMENT_DYNAMIC(ScInputEdit, CEdit)
ScInputEdit::ScInputEdit()
{
	this->m_nVarType = INPUT_UNKNOWN;
	this->m_bInput = FALSE;
	this->m_pInputBar = NULL;
}

ScInputEdit::~ScInputEdit()
{
}

int		ScInputEdit::BeginInput(int nVarType,ScInputBar *pInputBar)
{
	this->m_nVarType = nVarType;
	this->m_bInput = TRUE;
	this->m_pInputBar = pInputBar;

	SetWindowText("");

	return 0;
}

int		ScInputEdit::EndInput()
{
	this->m_nVarType = INPUT_UNKNOWN;
	this->m_bInput = FALSE;
	this->m_pInputBar = NULL;

	SetWindowText("");

	return 0;
}


BEGIN_MESSAGE_MAP(ScInputEdit, CEdit)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnEnSetfocus)
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_KEYUP()
END_MESSAGE_MAP()



// ScInputEdit 消息处理程序


void ScInputEdit::OnEnSetfocus()
{
	// TODO: 在此添加控件通知处理程序代码
}

//static BOOL  IsChar(UINT nChar,char c)
//{
//	TCHAR cc = (TCHAR)nChar;
//}

//
// 对输入加以限制
//
void ScInputEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	char *nFilter = "+-";
	char *dFilter = "+-.e";
	char *vFilter = "+-.,e ";

	DTRACE("\nOnChar %d",nChar);
	if(this->m_bInput && this->m_nVarType != INPUT_STRING)//对string不限制
	{
		BOOL bDot = TRUE,bComma = FALSE;
		if(this->m_nVarType == INPUT_INTEGER)
			bDot = FALSE;
		if(this->m_nVarType == INPUT_POINT2D ||
			this->m_nVarType == INPUT_POINT3D ||
			this->m_nVarType == INPUT_VECTOR)
			bComma = TRUE;

		if(!isdigit((int)nChar) && (nChar != VK_BACK))
		{
			char *ptr = NULL;
			if(!bDot)
				ptr = nFilter;
			else if(!bComma)
				ptr = dFilter;
			else
				ptr = vFilter;

			BOOL bOk = FALSE;
			while(*ptr)
			{
				BYTE bc = (BYTE)(*ptr);
				if(nChar == bc)
				{
					bOk = TRUE;
					break;
				}
				ptr ++;
			}

			if(!bOk)
				return;//不可输入的字符，不处理
		}
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void ScInputEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	DTRACE("\n ScInputEdit::OnSetFocus...");
	//发送消息模拟全选,此法不可行。
	//CRect rc;
	//GetClientRect(&rc);
	//DWORD lparam = ((rc.top + 4) << 16 ) + rc.left + 4;
	//SendMessage(WM_LBUTTONDBLCLK ,(WPARAM)0,(LPARAM)lparam);

}

void ScInputEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	DTRACE("\nScInputEdit::OnKillFocus。。。");
}

void ScInputEdit::OnSize(UINT nType, int cx, int cy)
{
	CEdit::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}

BOOL ScInputEdit::PreTranslateMessage(MSG* pMsg)
{
	//不再这里加，否则在acceptInput中显示窗口可能会出问题。
	//限制比较多。
	/*if(pMsg->message == WM_KEYUP)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			DTRACE("\n---------enter--------------\n");
			if(this->m_bInput)
				AcceptInput();
		}
	}*/
	return CEdit::PreTranslateMessage(pMsg);
}

//
// 接收输入,要检查输入的有效性。
//
int		ScInputEdit::AcceptInput()
{
	CString szVal;
	this->GetWindowText(szVal);
	if(szVal.IsEmpty())
		return -1;

	ScInput input;
	if(!input.InputString(this->m_nVarType,szVal))
		return -1;

	ScInputVar var;
	var.Set(input);

	if(this->m_pInputBar)
	{
		this->m_pInputBar->AcceptInput(var);
	}

	return 0;
}


void ScInputEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_RETURN)
	{
		DTRACE("\n---------enter--------------\n");
		if(this->m_bInput)
			AcceptInput();
	}

	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}
