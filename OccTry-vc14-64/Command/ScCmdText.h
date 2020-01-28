#pragma once

#include <vector>
#include "ScCommand.h"

class TcGlyphOutline{
public:
	TcGlyphOutline(HDC hDC,LPLOGFONT lf);

	//����һ����϶���
	int						GetStringOutLine(LPCTSTR lpszStr,TopTools_SequenceOfShape& seqOfShape);

	//��������ַ���ָ��ͳ���,Ӣ����ĸͨ��1��byte,����2��byte
	TopoDS_Shape			GetCharOutLine(char *pText,int nlen);

protected:
	//��ȡһ���ַ�����������
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

	//�����ͽ�������
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//��ѡ��ʽ�µ���Ϣ��Ӧ����
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

	//�����ַ���
	virtual	  int			AcceptInput(const CString& str);

	//���ո�����
	virtual   int			AcceptInput(double dVal);

public:
	enum{TEXT_WIRE,TEXT_FACE,TEXT_SOLID};

protected:
	enum{S_TEXT,S_ATTR};

	BOOL					BuildText();

	CString					m_szText;//��������
	int						m_nType;//����
	double					m_dHeight;//solid�ĸ߶�
};