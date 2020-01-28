#include "StdAfx.h"
#include <BRepBndLib.hxx>
#include "ScBRepLib.h"
#include "ScView.h"
#include ".\sccmdtext.h"

ScCmdText::ScCmdText(void)
{
}

ScCmdText::~ScCmdText(void)
{
}

TcGlyphOutline::TcGlyphOutline(HDC hDC,LPLOGFONT lf)
{
	ASSERT(hDC && lf);
	m_hDC = hDC;
	m_pLogFont = lf;
}

static double FixedValue(int nVal)
{
	return nVal * 1.0/65536;
}

//返回一个组合对象
int		TcGlyphOutline::GetStringOutLine(LPCTSTR lpszStr,TopTools_SequenceOfShape& seqOfShape)
{
	ASSERT(lpszStr);
	int nC = 0;

	int nBufSize = ::MultiByteToWideChar(CP_ACP,0,lpszStr,strlen(lpszStr),NULL,0);
	if(nBufSize <= 0)
		return -1;

	WCHAR *pwcBuf = new WCHAR[nBufSize + 1];
	::MultiByteToWideChar(CP_ACP,0,lpszStr,strlen(lpszStr),pwcBuf,nBufSize + 1);

	//转入字体
	CFont lfFont;
	if(!lfFont.CreateFontIndirect(m_pLogFont))
	{
		ASSERT(FALSE);
		return -1;
	}

	HGDIOBJ hOldFont = ::SelectObject(m_hDC,lfFont.GetSafeHandle());

	double x1,y1,z1,x2,y2,z2;
	for(int ix = 0;ix < nBufSize;ix ++)
	{
		TopoDS_Shape aS = GetCharOutLine(pwcBuf[ix]);
		if(!aS.IsNull())
		{
			//计算新的位置
			Bnd_Box bb;
			BRepBndLib::Add(aS,bb);

			bb.Get(x1,y1,z1,x2,y2,z2);
			if(ix == 0)
			{
				m_nXpos = x2 + (x2 - x1)/5.0;
				nC ++;
				seqOfShape.Append(aS);
			}
			else
			{
				//移动aS
				gp_Trsf trsf;
				trsf.SetTranslation(gp_Pnt(x1,y1,z1),gp_Pnt(m_nXpos,y1,z1));
				BRepBuilderAPI_Transform trans(trsf);
				trans.Perform(aS);
				if(trans.IsDone())
				{
					aS = trans.Shape();
					if(!aS.IsNull())
					{
						nC ++;
						seqOfShape.Append(aS);
					}
				}

				m_nXpos += (x2 - x1) * 1.2;
			}
		}
	}

	::SelectObject(m_hDC,hOldFont);

	return nC;
}

//传入的是字符串指针和长度,英文字母通常1个byte,汉语2个byte
TopoDS_Shape	TcGlyphOutline::GetCharOutLine(char *pText,int nlen)
{
	TopoDS_Shape aS;

	//先将字符串转成Unicode
	WCHAR wcBuf[8];

	MultiByteToWideChar( CP_ACP, 0, pText,
		nlen, wcBuf,   
		sizeof(wcBuf)/sizeof(wcBuf[0]) );
	
	//转入字体
	CFont lfFont;
	if(!lfFont.CreateFontIndirect(m_pLogFont))
	{
		ASSERT(FALSE);
		return aS;
	}

	HGDIOBJ hOldFont = ::SelectObject(m_hDC,lfFont.GetSafeHandle());
	aS = GetCharOutLine(wcBuf[0]);
	::SelectObject(m_hDC,hOldFont);

	return aS;
}

//获取一个字符的轮廓对象
//字体已经装入.
TopoDS_Shape		TcGlyphOutline::GetCharOutLine(WCHAR wc)
{
	TopoDS_Compound aC;
	BRep_Builder BB;
	BB.MakeCompound(aC);

	GLYPHMETRICS gm;
	MAT2 mat;

	memset(&mat, 0, sizeof(mat));
	mat.eM11.value = 1;
	mat.eM12.value = 0;
	mat.eM21.value = 0;
	mat.eM22.value = 1;

	//获取对应的轮廓信息
	DWORD dwSize = ::GetGlyphOutlineW(m_hDC,wc,GGO_NATIVE,&gm,0,NULL,&mat);
	if(dwSize == 0 || dwSize == GDI_ERROR)
	{
		DWORD err = GetLastError();

		ASSERT(FALSE);
		return aC;
	}

	TopoDS_Edge aE;

	BYTE *pbOutline = new BYTE[dwSize];
	DWORD dwDataSize = ::GetGlyphOutlineW(m_hDC,wc,GGO_NATIVE,&gm,dwSize,pbOutline,&mat);

	//对获取信息进行解码
	const TTPOLYGONHEADER *pHeader = (const TTPOLYGONHEADER *)pbOutline;
	int nTolSize = dwDataSize;
	int iC = 0;
	while(nTolSize > sizeof(TTPOLYGONHEADER))
	{
		int nCurSize = pHeader->cb;

		nTolSize -= nCurSize;
		if(nTolSize < 0)
		{
			ASSERT(FALSE);
			break;
		}

		if(pHeader->dwType != TT_POLYGON_TYPE)
		{
			ASSERT(FALSE);
			break;
		}

		TopoDS_Shape aW;
		BRepBuilderAPI_MakeWire mw;

		m_pntArray.clear();
		const POINT * p = (const POINT *) & pHeader->pfxStart;

		AddPoint(p[0].x, p[0].y);
		const TTPOLYCURVE *pCurve = (const TTPOLYCURVE *)&pHeader[1];
		while((BYTE *)pCurve < (BYTE *)(((LPSTR)pHeader) + nCurSize))
		{
			int i;
			p = (const POINT *)pCurve->apfx;

			switch(pCurve->wType)
			{
			case TT_PRIM_LINE:
				{
					for(i = 0;i < pCurve->cpfx;i ++)
					{
						aE = BuildLine(p[i].x,p[i].y);
						if(!aE.IsNull())
							mw.Add(aE);
					}
				}
				break;
			case TT_PRIM_QSPLINE:
				{
					ASSERT(pCurve->cpfx >= 2);
					for(i = 0;i < pCurve->cpfx - 1;i ++)
					{
						if(i == pCurve->cpfx - 2)
							aE = BuildQSpline(p[i].x,p[i].y,p[i+1].x,p[i+1].y);
						else
							aE = BuildQSpline(p[i].x,p[i].y,(p[i].x + p[i+1].x)/2,
							(p[i].y + p[i+1].y)/2);
						if(!aE.IsNull())
							mw.Add(aE);
					}
				}
				break;
			case TT_PRIM_CSPLINE:
				{
					ASSERT((pCurve->cpfx % 3) == 0);
					for(i = 0;i < pCurve->cpfx;i+=3)
					{
						aE = BuildCSpline(p[i].x,p[i].y,p[i+1].x,p[i+1].y,p[i+2].x,p[i+2].y);
						if(!aE.IsNull())
							mw.Add(aE);
					}
				}
				break;
			default:
				break;
			}

			pCurve = (const TTPOLYCURVE *)&pCurve->apfx[pCurve->cpfx];
		}

		POINT ph = m_pntArray.at(0),pt = m_pntArray.at(m_pntArray.size() - 1);
		if(ph.x != pt.x || ph.y != pt.y)
		{
			aE = BRepBuilderAPI_MakeEdge(gp_Pnt(FixedValue(pt.x),FixedValue(pt.y),0),
				gp_Pnt(FixedValue(ph.x),FixedValue(ph.y),0));
			if(!aE.IsNull())
			{
				mw.Add(aE);
			}
		}

		try{
			aW = mw.Wire();
			BB.Add(aC,aW);
		}catch(Standard_Failure){

		}

		iC ++;
		pHeader = (const TTPOLYGONHEADER *)((BYTE *)pHeader + nCurSize);
	}

	DTRACE("\n %d coutour.----------",iC);

	delete [] pbOutline;

	return aC;
}

TopoDS_Edge		TcGlyphOutline::BuildLine(int x,int y)
{
	TopoDS_Edge aE;
	POINT  p0 = m_pntArray.at(m_pntArray.size() - 1);
	aE = BRepBuilderAPI_MakeEdge(gp_Pnt(FixedValue(p0.x),FixedValue(p0.y),0),
		gp_Pnt(FixedValue(x),FixedValue(y),0));

	AddPoint(x,y);

	return aE;
}

TopoDS_Edge		TcGlyphOutline::BuildQSpline(int x1,int y1,int x2,int y2)
{
	TopoDS_Edge aE;
	POINT  p0 = m_pntArray.at(m_pntArray.size() - 1);
	
	return BuildCSpline((p0.x+2*x1)/3,(p0.y+2*y1)/3,(2*x1+x2)/3,(2*y1+y2)/3,x2,y2);
}



TopoDS_Edge		TcGlyphOutline::BuildCSpline(int x1,int y1,int x2,int y2,int x3,int y3)
{
	TopoDS_Edge aE;

	POINT  p0 = m_pntArray.at(m_pntArray.size() - 1);

	TColgp_Array1OfPnt arpnt(1,4);
	arpnt.SetValue(1,gp_Pnt(FixedValue(p0.x),FixedValue(p0.y),0.0));
	arpnt.SetValue(2,gp_Pnt(FixedValue(x1),FixedValue(y1),0.0));
	arpnt.SetValue(3,gp_Pnt(FixedValue(x2),FixedValue(y2),0.0));
	arpnt.SetValue(4,gp_Pnt(FixedValue(x3),FixedValue(y3),0.0));

	Handle(Geom_BezierCurve) aCur = new Geom_BezierCurve(arpnt);
	if(!aCur.IsNull())
	{
		aE = BRepBuilderAPI_MakeEdge(aCur);
	}

	AddPoint(x1,y1);
	AddPoint(x2,y2);
	AddPoint(x3,y3);

	return aE;
}

void	TcGlyphOutline::AddPoint(int x,int y)
{
	POINT p;
	p.x = x;p.y = y;
	m_pntArray.push_back(p);
}

//////////////////////////////////////////////////////////////////////////
//
ScCmdOutLineText::ScCmdOutLineText(int nType)
{
	ASSERT(nType == TEXT_WIRE || nType == TEXT_FACE || nType == TEXT_SOLID);
	m_nType = nType;
}

//启动和结束命令
int		ScCmdOutLineText::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedString("输入文本:");
	SwitchState(S_TEXT,"输入文本:");

	return 0;
}

int		ScCmdOutLineText::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdOutLineText::InputEvent(const CPoint& point,int nState,
								   const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
	}
}

//接受字符串
int		ScCmdOutLineText::AcceptInput(const CString& str)
{
	if(str.IsEmpty())
		return 0;

	if(GetState() == S_TEXT)
	{
		m_szText = str;

		if(m_nType == TEXT_SOLID)
		{
			EndInput();
			SwitchState(S_ATTR,"输入高度:");
			NeedDouble("高度:");
		}
		else
		{
			if(BuildText())
				Done();
			else
				End();
		}
	}

	return 0;
}

//接收浮点数
int		ScCmdOutLineText::AcceptInput(double dVal)
{
	if(GetState() == S_ATTR)
	{
		if(dVal > 0)
		{
			m_dHeight = dVal;

			if(BuildText())
				Done();
			else
				End();

		}
	}
	return 0;
}

BOOL	ScCmdOutLineText::BuildText()
{
	CFontDialog dlg;
	LOGFONT lf;
	if(dlg.DoModal() == IDOK)
	{
		dlg.GetCurrentFont(&lf);
	}
	else
	{
		End();
		return 0;
	}

	ASSERT(this->m_pScView->m_pViewWnd);
    HDC hDC = ::GetDC(this->m_pScView->m_pViewWnd->GetSafeHwnd());
	ASSERT(hDC);

	TopTools_SequenceOfShape ss;
	TcGlyphOutline gol(hDC,&lf);
	
	int nC = gol.GetStringOutLine(m_szText,ss);
	if(nC > 0)
	{
		if(m_nType == TEXT_FACE || m_nType == TEXT_SOLID)
		{
			TopTools_ListOfShape aFList;
			for(int ix = 1;ix <= ss.Length();ix ++)
			{
				TopoDS_Shape aS = ss.Value(ix);

				TopTools_ListOfShape alist;
				TopExp_Explorer ex;
				for(ex.Init(aS,TopAbs_WIRE);ex.More();ex.Next())
				{
					alist.Append(ex.Current());
				}
				//创建face
				ScBRepLib::BuildPlnFace(alist,aFList);
			}

			if(m_nType == TEXT_FACE)
			{
				ss.Clear();
				TopTools_ListIteratorOfListOfShape lit;
				for(lit.Initialize(aFList);lit.More();lit.Next())
				{
					ss.Append(lit.Value());
				}
			}
			else
			{
				//生成solid
				ss.Clear();
				TopTools_ListIteratorOfListOfShape lit;
				for(lit.Initialize(aFList);lit.More();lit.Next())
				{
					TopoDS_Face aF = TopoDS::Face(lit.Value());
					//拉伸成solid
					gp_Ax2 ax;
					gp_Dir zD = ax.Direction();
					gp_Vec aVec;

					aVec.SetCoord(m_dHeight * zD.X(),m_dHeight * zD.Y(),m_dHeight * zD.Z());
					
					TopoDS_Shape aNS = BRepPrimAPI_MakePrism(aF,aVec);
					if(!aNS.IsNull())
					{
						ss.Append(aNS);
					}
				}
			}
		}
		
		BeginUndo("OutLine text");
		for(int ix = 1;ix <= ss.Length();ix ++)
		{
			TopoDS_Shape aS = ss.Value(ix);
			Display(NewObj(ix - 1),aS);
			UndoAddNewObj(NewObj(ix - 1));
		}
		EndUndo(TRUE);

		return TRUE;
	}
	
	return FALSE;
}