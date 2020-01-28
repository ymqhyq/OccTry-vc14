#pragma once

#include <vector>
#include "ScCommand.h"

class TcGlyphOutline{
public:
	TcGlyphOutline(HDC hDC,LPLOGFONT lf);

	//返回一个组合对象
	int						GetStringOutLine(LPCTSTR lpszStr,TopTools_SequenceOfShape& seqOfShape);

	//传入的是字符串指针和长度,英文字母通常1个byte,汉语2个byte
	TopoDS_Shape			GetCharOutLine(char *pText,int nlen);

protected:
	//获取一个字符的轮廓对象
	TopoDS_Shape			GetCharOutLine(WCHAR wc);

	TopoDS_Edge				BuildLine(int x,int y);
	TopoDS_Edge				BuildQSpline(int x1,int y1,int x2,int y2);
	TopoDS_Edge				BuildCSpline(int x1,int y1,int x2,int y2,int x3,int y3);
	void					AddPoint(int x,int y);

protected:
	HDC						m_hDC;//
	LPLOGFONT				m_pLogFont;
	WCHAR					m_wcText;	
	std::vector<POINT>		m_pntArray;

	int						m_nXpos;
};

class ScCmdText
{
public:
	ScCmdText(void);
	~ScCmdText(void);
};

class ScCmdOutLineText : public ScCommand{
public:
	ScCmdOutLineText(int nType);

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//接受字符串
	virtual	  int			AcceptInput(const CString& str);

	//接收浮点数
	virtual   int			AcceptInput(double dVal);

public:
	enum{TEXT_WIRE,TEXT_FACE,TEXT_SOLID};

protected:
	enum{S_TEXT,S_ATTR};

	BOOL					BuildText();

	CString					m_szText;//输入文字
	int						m_nType;//类型
	double					m_dHeight;//solid的高度
};