#include "StdAfx.h"
#include <ProjLib.hxx>
#include <ElsLib.hxx>

#include ".\sctools.h"

ScTools::ScTools(void)
{
}

ScTools::~ScTools(void)
{
}

gp_Pnt ScTools::ConvertClickToPoint(Standard_Real x, Standard_Real y, Handle(V3d_View) aView)
{
	
	V3d_Coordinate XEye,YEye,ZEye,XAt,YAt,ZAt;
	aView->Eye(XEye,YEye,ZEye);//�ӵ�
	aView->At(XAt,YAt,ZAt);//VRCԭ��
	gp_Pnt EyePoint(XEye,YEye,ZEye);
	gp_Pnt AtPoint(XAt,YAt,ZAt);

	gp_Vec EyeVector(EyePoint,AtPoint);
	gp_Dir EyeDir(EyeVector);//����

	gp_Pln PlaneOfTheView = gp_Pln(AtPoint,EyeDir);//��ƽ��

	//��Ļ��ת��Ϊģ�Ϳռ䣨����������ϵ���ĵ㡣
	Standard_Real X,Y,Z;
	aView->Convert((int)x,(int)y,X,Y,Z);
	//������ܲ�����ƽ���ϣ����Ҫ����ͶӰ����ƽ�棬�ټ�����ƽ���ϵĵ㡣
	gp_Pnt ConvertedPoint(X,Y,Z);
	gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView,ConvertedPoint);
	
	gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
									ConvertedPointOnPlane.Y(),
									PlaneOfTheView);

#ifdef _VERBOSE_DEBUG
	DTRACE("\n Eye [%.4f,%.4f,%.4f] At [%.4f,%.4f,%.4f],viewpoint [%.4f,%.4f,%.4f],result [%.4f,%.4f,%.4f].",
		XEye,YEye,ZEye,XAt,YAt,ZAt,X,Y,Z,ResultPoint.X(),ResultPoint.Y(),ResultPoint.Z());
#endif
	return ResultPoint;
}

//��ʾ��ʾ��Ϣ
void	ScTools::Prompt(LPCTSTR lpszText)
{
}

inline Standard_Integer _key(Standard_Integer n1,Standard_Integer n2)
{
  Standard_Integer key = 
    (n2>n1)?(n1<<16)+n2:(n2<<16)+n1;
  return key;
}

//����shape�������������ٶ�shape�Ѿ���mesh�����ˡ�
TopoDS_Shape	ScTools::BuildShapeMesh(const TopoDS_Shape& aShape,double defle)
{
	TopoDS_Compound aComp;
	if(aShape.IsNull())
		return aComp;

	BRep_Builder aBuilder;
	aBuilder.MakeCompound(aComp);

	//�����������
	TopExp_Explorer exFace;
	for(exFace.Init(aShape,TopAbs_FACE);exFace.More();exFace.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(exFace.Current());
		TopLoc_Location aL;

		Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace,aL);

		if(!aTr.IsNull())
		{
			const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
			const Poly_Array1OfTriangle& aTris = aTr->Triangles();

			TopTools_DataMapOfIntegerShape aEdges;
			int nTr = aTr->NbTriangles();
			int n1,n2,n3,nKey;
			gp_Pnt p1,p2;

			//����������
			for(int ix = 1;ix <= nTr;ix ++)
			{
				aTris(ix).Get(n1,n2,n3);

				nKey = _key(n1,n2);
				if(!aEdges.IsBound(nKey))//ÿ��ʾ������ʾ
				{
					p1 = aNodes.Value(n1).Transformed(aL);
					p2 = aNodes.Value(n2).Transformed(aL);
					if(!p1.IsEqual(p2,Precision::Confusion()))
					{
						TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(p1,p2);
						if(!aEdge.IsNull())
						{
							aBuilder.Add(aComp,aEdge);
							aEdges.Bind(nKey,aEdge);
						}
					}
				}

				nKey = _key(n2,n3);
				if(!aEdges.IsBound(nKey))//ÿ��ʾ������ʾ
				{
					p1 = aNodes.Value(n2).Transformed(aL);
					p2 = aNodes.Value(n3).Transformed(aL);
					if(!p1.IsEqual(p2,Precision::Confusion()))
					{
						TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(p1,p2);
						if(!aEdge.IsNull())
						{
							aBuilder.Add(aComp,aEdge);
							aEdges.Bind(nKey,aEdge);
						}
					}
				}

				nKey = _key(n3,n1);
				if(!aEdges.IsBound(nKey))//ÿ��ʾ������ʾ
				{
					p1 = aNodes.Value(n3).Transformed(aL);
					p2 = aNodes.Value(n1).Transformed(aL);
					if(!p1.IsEqual(p2,Precision::Confusion()))
					{
						TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(p1,p2);
						if(!aEdge.IsNull())
						{
							aBuilder.Add(aComp,aEdge);
							aEdges.Bind(nKey,aEdge);
						}
					}
				}
			}//end for
		}//end if
	}//end for	

	return aComp;
}

//
// ��ȡface�Ĳü�����Ϣ
//
int	ScTools::FaceTrmLoopInfo(const TopoDS_Face& aFace,CStringList& lineList,
								 int nLevel,double dTol,double dPTol)
{
	int iret = 0;
	CString szLine;
	TopExp_Explorer  ex;

	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
	if(aSurf.IsNull())
		return FALSE;

	szLine = "begin get face trim loop info:";
	lineList.AddTail(szLine);

	CStringList detailList;//��ϸ��Ϣ,�������ɵ���ϸ��Ϣ
	CStringList summList;
	//����wire
	int iC = 0;
	for(ex.Init(aFace,TopAbs_WIRE);ex.More();ex.Next())
	{
		TopoDS_Wire aWire = TopoDS::Wire(ex.Current());
		if(aWire.IsNull())
			continue;

		szLine.Format("Wire %d:",iC++);
		lineList.AddTail(szLine);
		//����wire��
		Handle(ShapeFix_Wire) aSFW = 
			new ShapeFix_Wire(aWire,aFace,Precision::Confusion());
		//����,ShapeExtend_WireData��֤��wire��Ϊ�����edge.�������ڲ���ȷ�Ļ�.
		aSFW->FixReorder();

		//��ȡ���������ݡ�
		Handle(ShapeExtend_WireData) aSEWD = aSFW->WireData();
		int nE = aSEWD->NbEdges();
		int nEC = 0;
		double dWTol = 0.0;

		//��¼�����Ϣ
		TColgp_SequenceOfPnt seqOfV,seqOf3d,seqOfCalc; 
		TColgp_SequenceOfPnt2d seqOf2d;
		gp_Pnt ptb,pte;
		gp_Pnt ptb2,pte2;
		gp_Pnt2d pptb,ppte;
		BOOL bCheckCPC = TRUE,bCheckVC = TRUE,bCheckVPC = TRUE,bCheckParam = TRUE;
		double dMaxEDev = 0.0;

		for(int ix = 1;ix <= nE;ix ++)
		{
			TopoDS_Edge aE = aSEWD->Edge(ix);
			if(aE.IsNull())
				continue;

			BOOL bReversed = aE.Orientation() == TopAbs_REVERSED;
			if(bReversed)
				szLine.Format("Edge %d: REVERSED",nEC ++);
			else
				szLine.Format("Edge %d: FORWARD",nEC ++);
			detailList.AddTail(szLine);
			
			//���м��
			ShapeAnalysis_Edge sae;
			BOOL bCCP,bVC,bVPC,bParam;
			bCCP = sae.CheckCurve3dWithPCurve(aE,aFace);

			if(bCCP)
				bCheckCPC = FALSE;
			
			if(!bCCP)
				szLine = " CheckCurve3dWithPCurve pass.";
			else
				szLine = " CheckCurve3dWithPCurve fail.";

			
			bVC = sae.CheckVerticesWithCurve3d(aE,dTol);
			if(bVC)  bCheckVC = FALSE;
			
			if(!bVC)
				szLine += " CheckVerticesWithCurve3d pass.";
			else
				szLine += " CheckVerticesWithCurve3d fail.";
			
			bVPC = sae.CheckVerticesWithPCurve(aE,aFace,dTol);
			if(bVPC) bCheckVPC = FALSE;
			
			if(!bVPC)
				szLine += " CheckVerticesWithPCurve pass.";
			else
				szLine += " CheckVerticesWithPCurve fail.";
			
			double maxdev;
			bParam = sae.CheckSameParameter(aE,maxdev);
			if(maxdev > dMaxEDev) dMaxEDev = maxdev;
			if(bParam) bCheckParam = FALSE;
			
			CString szv;
			if(!bParam)
				szv.Format(" CheckSameParameter pass,dev %f.",maxdev);
			else
				szv.Format(" CheckSameParameter fail,dev %f.",maxdev);
			szLine += szv;
		
			detailList.AddTail(szLine);

			
			//���Vertex��Ϣ
			int nVC = 0;
			TopExp_Explorer exv;
			for(exv.Init(aE,TopAbs_VERTEX);exv.More();exv.Next())
			{
				TopoDS_Vertex aV = TopoDS::Vertex(exv.Current());
				if(aV.IsNull())
					continue;

				gp_Pnt pnt = BRep_Tool::Pnt(aV);
				seqOfV.Append(pnt);

				//Tol
				double dTol = BRep_Tool::Tolerance(aV);
				if(dTol > dWTol)
					dWTol = dTol;
			}

			//3D������Ϣ
			double dF,dL;
			Handle(Geom_Curve) aCur3d = BRep_Tool::Curve(aE,dF,dL);
			if(aCur3d.IsNull())
			{
				szLine = "has no curve3d";
				detailList.AddTail(szLine);
			}
			else
			{
				szLine = "3d curve:";
				detailList.AddTail(szLine);

				ptb = aCur3d->Value(dF);
				pte = aCur3d->Value(dL);

				//Ҫ�����Ϸ���
				if(bReversed)
				{
					seqOf3d.Append(pte);
					seqOf3d.Append(ptb);
				}
				else
				{
					seqOf3d.Append(ptb);
					seqOf3d.Append(pte);
				}
			}
			
			//2d������Ϣ
			Handle(Geom2d_Curve) aCur2d = BRep_Tool::CurveOnSurface(aE,aFace,dF,dL);
			if(aCur2d.IsNull())
			{
				szLine = "has no param curve";
				detailList.AddTail(szLine);
			}
			else
			{
				szLine = "param curve:";
				detailList.AddTail(szLine);

				pptb = aCur2d->Value(dF);
				ppte = aCur2d->Value(dL);
				ptb2 = aSurf->Value(pptb.X(),pptb.Y());
				pte2 = aSurf->Value(ppte.X(),ppte.Y());

				if(bReversed)
				{
					seqOfCalc.Append(pte2);
					seqOfCalc.Append(ptb2);
					seqOf2d.Append(ppte);
					seqOf2d.Append(pptb);
				}
				else
				{
					seqOfCalc.Append(ptb2);
					seqOfCalc.Append(pte2);
					seqOf2d.Append(pptb);
					seqOf2d.Append(ppte);
				}
			}
	
			szLine = "Edge end.";
			detailList.AddTail(szLine);
		}

		szLine.Format("  Wire vertex Tolerance : %f",dWTol);
		summList.AddTail(szLine);

		//����ͼ�����
		int ic = 0;
		//vertex ��Ϣ
		szLine = "Vertex list:";
		detailList.AddTail(szLine);
		szLine = "";
		for(ic = 1;ic <= seqOfV.Length();ic ++)
		{
			gp_Pnt pt = seqOfV.Value(ic);
			szLine.Format("    Vertex:[%f,%f,%f]",pt.X(),pt.Y(),pt.Z());
			detailList.AddTail(szLine);
		}
		
		//2d������Ϣ
		szLine.Format("Param curve pnt list:");
		detailList.AddTail(szLine);
		
		double dGap = 0.0,dMax2dGap = 0.0,dMax3dGap = 0.0,dMaxCalGap = 0.0;
		int nLen = seqOf2d.Length();
		for(ic = 1;ic <= nLen;ic += 2)
		{
			pptb = seqOf2d.Value(ic);
			ppte = seqOf2d.Value(ic + 1);

			if(ic == 1)
			{
				//���һ���͵�һ����
				dGap = seqOf2d.Value(nLen).Distance(pptb);
			}
			else 
			{
				dGap = seqOf2d.Value(ic - 1).Distance(pptb);
			}
			if(dGap > dMax2dGap)
			{
				dMax2dGap = dGap;
			}

			szLine.Format("		begin:[%f,%f],end[%f,%f],gap with prev point %f",
					pptb.X(),pptb.Y(),ppte.X(),ppte.Y(),dGap);
			detailList.AddTail(szLine);
		}

		//2d����3d����Ϣ
		szLine.Format("3d point from Param curve list:");
		detailList.AddTail(szLine);
		
		nLen = seqOfCalc.Length();
		for(ic = 1;ic <= nLen;ic += 2)
		{
			ptb = seqOfCalc.Value(ic);
			pte = seqOfCalc.Value(ic + 1);

			if(ic == 1)
			{
				dGap = seqOfCalc.Value(nLen).Distance(ptb);
			}
			else
			{
				dGap = seqOfCalc.Value(ic - 1).Distance(ptb);
			}
			if(dGap > dMaxCalGap)
				dMaxCalGap = dGap;

			szLine.Format("		begin:[%f,%f,%f],end:[%f,%f,%f],gap with prev point %f",
					ptb.X(),ptb.Y(),ptb.Z(),pte.X(),pte.Y(),pte.Z(),dGap);
			detailList.AddTail(szLine);
		}

		//3d curve��Ϣ
		szLine.Format("3d curve point list:");
		detailList.AddTail(szLine);
		
		nLen = seqOf3d.Length();
		for(ic = 1;ic <= nLen;ic += 2)
		{
			ptb = seqOf3d.Value(ic);
			pte = seqOf3d.Value(ic + 1);

			if(ic == 1)
			{
				dGap = seqOf3d.Value(nLen).Distance(ptb);
			}
			else
			{
				dGap = seqOf3d.Value(ic - 1).Distance(ptb);
			}

			if(dGap > dMax3dGap)
				dMax3dGap = dGap;

			szLine.Format("		begin:[%f,%f,%f],end:[%f,%f,%f],gap with prev point %f",
					ptb.X(),ptb.Y(),ptb.Z(),pte.X(),pte.Y(),pte.Z(),dGap);
			detailList.AddTail(szLine);
		}

		//�����
		if(!bCheckCPC || !bCheckVC || !bCheckVPC || !bCheckParam)
			iret |= FACE_TRMLOOP_CHECK_ERR;

		double dGapTol = Precision::Confusion();
		if(dMax2dGap > dGapTol || dMaxCalGap > dGapTol || dMax3dGap > dGapTol)
			iret |= FACE_TRMLOOP_GAP_ERR;

		if(dMax2dGap > 0.001 || dMaxCalGap > 0.001 || dMax3dGap > 0.001)
			iret |= FACE_TRMLOOP_FATAL_ERR;


		if(!bCheckCPC || !bCheckVC || !bCheckVPC || !bCheckParam)
		{
			if(bCheckCPC)
				szLine = " CheckCurve3dWithPCurve pass.";
			else
				szLine = " CheckCurve3dWithPCurve fail.";

			if(bCheckVC)
				szLine += " CheckVerticesWithCurve3d pass.";
			else
				szLine += " CheckVerticesWithCurve3d fail.";

			if(bCheckVPC)
				szLine += " CheckVerticesWithPCurve pass.";
			else
				szLine += " CheckVerticesWithPCurve fail.";

			CString szv;
			if(bCheckParam)
				szv.Format(" CheckSameParameter pass,dev %f.",dMaxEDev);
			else
				szv.Format(" CheckSameParameter fail,dev %f.",dMaxEDev);
			szLine += szv;

			summList.AddTail(szLine);
		}

		szLine.Format(" max 2d gap : %.8f,max calc gap : %.8f,max 3d gap : %.8f",
				dMax2dGap,dMaxCalGap,dMax3dGap);
		summList.AddTail(szLine);
		
		if(nLevel == 0 || ((nLevel == 1) && (iret != 0)) ||
			((nLevel == 2) && (dMax2dGap > 0.001 || dMaxCalGap > 0.001 || dMax3dGap > 0.001)))
		{//�����ϸ��Ϣ
			POSITION pos = summList.GetHeadPosition();
			while(pos)
			{
				szLine = summList.GetNext(pos);
				lineList.AddTail(szLine);
			}

			if(nLevel != 1)
			{
				pos = detailList.GetHeadPosition();
				while(pos)
				{
					szLine = detailList.GetNext(pos);
					lineList.AddTail(szLine);
				}
			}
		}

		summList.RemoveAll();
		detailList.RemoveAll();
		
		szLine = "Wire end.";
		lineList.AddTail(szLine);
	}

	szLine = "Face end.";
	lineList.AddTail(szLine);

	return iret;
}

static char   arTypName[][20] = {
	"COMPOUND","COMPSOLID",
	"SOLID","SHELL",
	"FACE","WIRE",
	"EDGE","VERTEX",
	"SHAPE"
};

char*	ScTools::ShapeTypeName(TopAbs_ShapeEnum eType)
{
	return (char *)arTypName[(int)eType];
}

//��ȡbnd_box�İ˸��ǵ�
BOOL	ScTools::GetBoxCorner(const Bnd_Box& bb,TColgp_SequenceOfPnt& pnts)
{
	if(bb.IsVoid())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	double x1,y1,z1,x2,y2,z2;
	bb.Get(x1,y1,z1,x2,y2,z2);

	pnts.Append(gp_Pnt(x1,y1,z1));
	pnts.Append(gp_Pnt(x1,y1,z2));
	pnts.Append(gp_Pnt(x1,y2,z1));
	pnts.Append(gp_Pnt(x1,y2,z2));
	pnts.Append(gp_Pnt(x2,y2,z1));
	pnts.Append(gp_Pnt(x2,y2,z2));
	pnts.Append(gp_Pnt(x2,y1,z1));
	pnts.Append(gp_Pnt(x2,y1,z2));

	return TRUE;
}