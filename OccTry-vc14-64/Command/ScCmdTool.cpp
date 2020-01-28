#include "StdAfx.h"
#include "ScTools.h"
#include <GeomProjLib.hxx>
#include <BRepTools.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepAlgo.hxx>
#include ".\sccmdtool.h"

ScCmdTool::ScCmdTool(void)
{
}

ScCmdTool::~ScCmdTool(void)
{
}

/////////////////////////////////////////////////////////
// ��ȡFace�Ĳü�����Ϣ
//

//���������ʼִ�С�
int	ScCmdFaceTrmLoopInfo::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelShape())
	{
		SwitchState(S_INFO,"ѡ��һ����");
		//m_AISContext->OpenLocalContext();//yxk20191207
		m_AISContext->Activate(TmpObj(0),4);
	}
	else
	{
		SwitchState(S_SHAPE,"ѡ��һ������");
	}

	return 0;
}

//�������ͨ��������ֹ���
int	ScCmdFaceTrmLoopInfo::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void		ScCmdFaceTrmLoopInfo::InputEvent(const CPoint& point,int nState,//����minput����
													const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
	}
	else
	{
		switch(GetState())
		{
		case S_SHAPE:
			{
				if(HasSelShape())
				{
					SwitchState(S_INFO,"ѡ��һ����");
					//m_AISContext->OpenLocalContext();//yxk20191207
					m_AISContext->Activate(TmpObj(0),4);
				}
			}
			break;
		case S_INFO:
			{
				InitSelect();
				if(MoreSelect())
				{
					TopoDS_Face aF = TopoDS::Face(SelectedShape(FALSE));
					CStringList list;
					ScTools::FaceTrmLoopInfo(aF,list,0);

					//��ʾ��Ϣ,�ȼ�����Ϣ����ʾ
					CString szMsg;
					POSITION pos = list.GetHeadPosition();
					while(pos)
					{
						CString szl = list.GetNext(pos);
						szMsg += szl + "\n";
					}

					CStdioFile sf;
					if(sf.Open("d:\\~tmp.txt",CFile::modeCreate | CFile::modeWrite))
					{
						sf.WriteString(szMsg);
						sf.Close();
					}

					//���ļ�
					ShellExecute(NULL, "open", "d:\\~tmp.txt", NULL, NULL, SW_SHOWNORMAL);

					//m_AISContext->CloseLocalContex();//yxk20191207
					TmpObj(0) = NULL;

					End();
				}
			}
			break;
		default:
			break;
		}
	}
}

BOOL	ScCmdFaceTrmLoopInfo::HasSelShape()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(!aS.IsNull() && 
			((aS.ShapeType() == TopAbs_FACE) || 
			(aS.ShapeType() == TopAbs_SHELL) ||
			(aS.ShapeType() == TopAbs_SOLID)))
		{
			TmpObj(0) = SelectedObj();

			return TRUE;
		}
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// ��face����3d���������µĲ������ߡ�
// ��3d���ߺͲ������߲�ƥ���ʱ��

//���������ʼִ�С�
int	ScCmdToolCurve2d::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelShape())
	{
		BuildCurve2d();
	}
	else
	{
		SwitchState(S_SHAPE,"ѡ��һ������");
	}

	return 0;
}

//�������ͨ��������ֹ���
int	ScCmdToolCurve2d::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void		ScCmdToolCurve2d::InputEvent(const CPoint& point,int nState,//����minput����
													const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
	}
	else
	{
		switch(GetState())
		{
		case S_SHAPE:
			{
				if(HasSelShape())
				{
					BuildCurve2d();
				}
			}
			break;
		default:
			break;
		}
	}
}

void	ScCmdToolCurve2d::BuildCurve2d()
{
	TopoDS_Face aFace = TopoDS::Face(GetObjShape(TmpObj(0)));
	TopExp_Explorer ex,exw;
	int nChg = 0;

	for(ex.Init(aFace,TopAbs_WIRE);ex.More();ex.Next())
	{
		TopoDS_Wire aW = TopoDS::Wire(ex.Current());

		for(exw.Init(aW,TopAbs_EDGE);exw.More();exw.Next())
		{
			TopoDS_Edge aE = TopoDS::Edge(exw.Current());

			ShapeAnalysis_Edge sae;
			double maxdev = 0.0;
			sae.CheckSameParameter(aE,maxdev,10);
			if(maxdev > 0.001)//��������Ҫ��������ȡ�����
			{
				DTRACE("\n Edge CheckSameParameter faild,maxdev %f.",maxdev);
				//�ɾɵ�edge�����µ����ߡ�
				double df,dl;
				Handle(Geom_Curve) aCur3d = BRep_Tool::Curve(aE,df,dl);
				Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
				/*if(!aCur3d->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
				{
					aCur3d = new Geom_TrimmedCurve(aCur3d,df,dl);
				}*/
			/*	Handle(Geom_Curve) aProjC = GeomProjLib::Project(aCur3d,aSurf);
				if(!aProjC.IsNull())
				{*/
				//
				//Handle(Geom2d_Curve) aCur2d = GeomProjLib::Curve2d(
				//	aCur3d,df,dl,aSurf);
				Handle(Geom2d_Curve) aCur2d;
#ifndef _CURVE_2D
				double dtol = 0.0001;
				aCur2d = GeomProjLib::Curve2d(aCur3d,df,dl,aSurf,dtol);
				{
#else
				ShapeConstruct_ProjectCurveOnSurface sproj;
				sproj.Init(aSurf,0.0001);
				if(sproj.Perform(aCur3d,df,dl,aCur2d))
				{
#endif
				if(!aCur2d.IsNull())
				{
					ShapeBuild_Edge sbe;
					sbe.ReplacePCurve(aE,aCur2d,aFace);
					nChg ++;
				}
				}
				//}
			}
		}
	}

	if(nChg > 0)
	{
		m_AISContext->Redisplay(TmpObj(0),Standard_True);
	}

	TmpObj(0) = NULL;

	Done();
}

BOOL	ScCmdToolCurve2d::HasSelShape()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(!aS.IsNull() && 
			(aS.ShapeType() == TopAbs_FACE))
		{
			TmpObj(0) = SelectedObj();

			return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////
// ����߽����ʸ��ʾ��

//���������ʼִ�С�
int		ScCmdFaceBndTang::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	this->m_bBuilded = FALSE;
	return ScCmdFaceBase::Begin(aCtx);
}

//�������ͨ��������ֹ���
int	ScCmdFaceBndTang::End()
{
	TmpObj(0) = NULL;
	m_aFace.Nullify();

	if(m_bLocalCtx)
	{
		m_bLocalCtx = FALSE;
		//m_AISContext->CloseLocalContex();//yxk20191207
	}

	this->RemoveNewObjs();

	return ScCommand::End();
}

BOOL	ScCmdFaceBndTang::BuildShape()//���ɶ�Ӧshape��
{
	if(this->m_bBuilded)
		return TRUE;

	this->m_bBuilded = TRUE;

	//����face��wire�����ɶ�Ӧ��edge�����ߡ�ÿ��curve�������
	TopExp_Explorer exf,exw;
	int nC = 0;

	for(exf.Init(m_aFace,TopAbs_WIRE);exf.More();exf.Next())
	{
		TopoDS_Wire aW = TopoDS::Wire(exf.Current());
		if(aW.IsNull())
			continue;

		for(exw.Init(aW,TopAbs_EDGE);exw.More();exw.Next())
		{
			TopoDS_Edge aE = TopoDS::Edge(exw.Current());
			if(aE.IsNull())
				continue;

			nC ++;
			//����edge��Ӧ����ʸ
			double df,dl;
			Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE,df,dl);
			if(aCur.IsNull())
				continue;

			aCur = Handle(Geom_Curve)::DownCast(aCur->Copy());

			//�����е����ʸ��
			double dm = (df + dl)/2.0;
			gp_Pnt p,p2;
			gp_Vec dp;
			aCur->D1(dm,p,dp);
			p2.SetX(p.X() + dp.X());
			p2.SetY(p.Y() + dp.Y());
			p2.SetZ(p.Z() + dp.Z());

			//������ʸ
			TopoDS_Edge aNE = BRepBuilderAPI_MakeEdge(p,p2);

			//��ʾ��ͷ�������С�����ߵı���������
			GeomAdaptor_Curve AC(aCur);
			double arsize = GCPnts_AbscissaPoint::Length(AC,df,dl);
			arsize /= 10.0;
			if(arsize > 5.0) arsize = 5.0;
			TopoDS_Shape aCS = BRepPrimAPI_MakeCone(gp_Ax2(p2,gp_Dir(dp)),arsize,0,arsize * 2.0);

			if(!aNE.IsNull())
			{
				Display(NewObj(-1),aNE);
				Display(NewObj(-1),aCS);
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////
// ����ķ�ʸ

//���������ʼִ�С�
int	ScCmdFaceNormal::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	this->m_bBuilded = FALSE;
	return ScCmdFaceBase::Begin(aCtx);
}

//�������ͨ��������ֹ���
int	ScCmdFaceNormal::End()
{
	TmpObj(0) = NULL;
	m_aFace.Nullify();

	if(m_bLocalCtx)
	{
		m_bLocalCtx = FALSE;
		//m_AISContext->CloseLocalContex();//yxk20191207
	}

	this->RemoveNewObjs();

	return ScCommand::End();
}

BOOL	ScCmdFaceNormal::BuildShape()//���ɶ�Ӧshape��
{
	if(this->m_bBuilded)
		return TRUE;

	this->m_bBuilded = TRUE;

	Handle(Geom_Surface) aSurf = BRep_Tool::Surface(m_aFace);
	if(aSurf.IsNull())
		return FALSE;

	double U1,U2,V1,V2;
	BRepTools::UVBounds(m_aFace,U1,U2,V1,V2);
	//���е�
	double u = (U1 + U2)/2.0;
	double v = (V1 + V2)/2.0;

	//ȡֵ
	gp_Pnt pt,pt2;
	gp_Vec d1u,d1v;
	aSurf->D1(u,v,pt,d1u,d1v);

	//���㷨ʸ
	gp_Vec norm = d1u.Crossed(d1v);
	if(norm.Magnitude() < Precision::Confusion())
		return FALSE;
	pt2.SetCoord(pt.X() + norm.X(),pt.Y() + norm.Y(),pt.Z() + norm.Z());

	//
	//���ɷ�ʸ
	TopoDS_Edge aNE = BRepBuilderAPI_MakeEdge(pt,pt2);

	//���ɼ�ͷ
	GProp_GProps gprop;
	BRepGProp::SurfaceProperties(m_aFace,gprop);
	Standard_Real Area = gprop.Mass();

	double arsize = sqrt(Area)/10.0;
	if(arsize > 5.0)
		arsize = 5.0;

	TopoDS_Shape aCS = BRepPrimAPI_MakeCone(gp_Ax2(pt2,gp_Dir(norm)),arsize,0,arsize * 2.0);

	Display(NewObj(0),aNE);
	Display(NewObj(1),aCS);

	return TRUE;
}


//////////////////////////////////////////////////////////
// ���ߵķ�ʸ����ʾ
//���������ʼִ�С�
int		ScCmdCurveTang::Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		if(HasSelCurve())
		{
				BuildShape();
		}
		else
		{
				SwitchState(S_SELCURVE,"ѡ��һ������:");
				NeedSelect(TRUE);
		}

		return 0;
}

//�������ͨ��������ֹ���
int	   ScCmdCurveTang::End()
{
		TmpObj(0) = NULL;

		this->RemoveNewObjs();

		return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
 void	ScCmdCurveTang::InputEvent(const CPoint& point,int nState,//����minput����
				const Handle(V3d_View)& aView)
 {
		 if(nState == MINPUT_RIGHT)
		 {
				 End();
		 }
		 else
		 {
				 if(HasSelCurve())
				 {
						 BuildShape();
				 }
		 }
 }

 BOOL		ScCmdCurveTang::HasSelCurve()
 {
		 InitSelect();
		 if(MoreSelect())
		 {
				 TopoDS_Shape aS = SelectedShape(FALSE);
				 if(!aS.IsNull() && ((aS.ShapeType() == TopAbs_EDGE) || (
						 aS.ShapeType() == TopAbs_WIRE)))
				 {
						 TmpObj(0) = SelectedObj(FALSE);

						 return TRUE;
				 }
		 }
		return FALSE;
 }

 // Ϊѡ������߻�wire���ɲ���ʾ��ʸ
BOOL	ScCmdCurveTang::BuildShape()//���ɶ�Ӧshape��
{
		TopoDS_Shape aS = GetObjShape(TmpObj(0));
		TopExp_Explorer ex;

		for(ex.Init(aS,TopAbs_EDGE);ex.More();ex.Next())
		{
				TopoDS_Edge aE = TopoDS::Edge(ex.Current());
				if(aE.IsNull())
						continue;

				//����edge��Ӧ����ʸ
				double df,dl;
				Handle(Geom_Curve) aCur = BRep_Tool::Curve(aE,df,dl);
				if(aCur.IsNull())
						continue;

				aCur = Handle(Geom_Curve)::DownCast(aCur->Copy());

				//�����е����ʸ��
				double dm = (df + dl)/2.0;
				gp_Pnt p,p2;
				gp_Vec dp;
				aCur->D1(dm,p,dp);
				p2.SetX(p.X() + dp.X());
				p2.SetY(p.Y() + dp.Y());
				p2.SetZ(p.Z() + dp.Z());

				//������ʸ
				TopoDS_Edge aNE = BRepBuilderAPI_MakeEdge(p,p2);

				//��ʾ��ͷ�������С�����ߵı���������
				GeomAdaptor_Curve AC(aCur);
				double arsize = GCPnts_AbscissaPoint::Length(AC,df,dl);
				arsize /= 10.0;
				if(arsize > 5.0) arsize = 5.0;
				TopoDS_Shape aCS = BRepPrimAPI_MakeCone(gp_Ax2(p2,gp_Dir(dp)),arsize,0,arsize * 2.0);

				if(!aNE.IsNull())
				{
						Display(NewObj(-1),aNE);
						Display(NewObj(-1),aCS);
				}
		}

		return FALSE;
}

/////////////////////////////////////////////////////////////////
// ������Ļ���

//���������ʼִ�С�
 int		ScCmdCheckBase::Begin(Handle(AIS_InteractiveContext) aCtx)
 {
		 ScCommand::Begin(aCtx);

		 if(HasSelObject())
		 {
				 Check();
				 End();
		 }
		 else
		 {
				 Prompt("ѡ��һ������:");
				 NeedSelect(TRUE);
		 }

		 return 0;
 }

//�������ͨ��������ֹ���
int		ScCmdCheckBase::End()
{
		SelObj(0) = NULL;
		return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
 void		ScCmdCheckBase::InputEvent(const CPoint& point,int nState,//����minput����
				const Handle(V3d_View)& aView)
 {
		if(nState == MINPUT_RIGHT)
		{
				End();
		}
		else
		{
				if(HasSelObject())
				{
						Check();
						End();
				}
		}
 }

BOOL			ScCmdCheckBase::HasSelObject()
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull())
				{
						SelObj(0) = SelectedObj();

						return TRUE;
				}
		}
		return FALSE;
}

BOOL		ScCmdCheckBase::Check()
{
		return FALSE;
}

BOOL		ScCmdCheckValid::Check()
{
		TopoDS_Shape aShape = GetObjShape(SelObj(0));

		if(BRepAlgo::IsValid(aShape))
				AfxMessageBox("��Ч����.");
		else
				AfxMessageBox("��Ч����.");

		return FALSE;
}

BOOL			ScCmdCheckTopoValid::Check()
{
		TopoDS_Shape aShape = GetObjShape(SelObj(0));

		if(BRepAlgo::IsTopologicallyValid(aShape))
				AfxMessageBox("��Ч���˶���.");
		else
				AfxMessageBox("��Ч���˶���.");

		return FALSE;
}

BOOL		ScCmdCheckClosed::Check()
{
		TopoDS_Shape aShape = GetObjShape(SelObj(0));

		if(BRep_Tool::IsClosed(aShape))
				AfxMessageBox("�պ϶���");
		else
				AfxMessageBox("�Ǳպ϶���");

		return FALSE;
}
