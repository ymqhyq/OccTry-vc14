#include "StdAfx.h"
#include ".\sccmdface.h"

ScCmdFace::ScCmdFace(void)
{
}

ScCmdFace::~ScCmdFace(void)
{
}

/////////////////////////////////////////////////////////////////
// Face 修正

//启动和结束命令
 int		ScCmdFaceFixAll::Begin(Handle(AIS_InteractiveContext) aCtx)
 {
		 ScCommand::Begin(aCtx);

		 if(HasSelFace())
		 {
				 if(FixShape())
						 Done();
				 else
						 End();
		 }
		 else
		 {
				 Prompt("选择一个曲面:");
				 NeedSelect(TRUE);
		 }

		 return 0;
 }

 int		ScCmdFaceFixAll::End()
 {
		 return ScCommand::End();
 }

//点选方式下的消息响应函数
 void		ScCmdFaceFixAll::InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView)
 {
		 if(nState == MINPUT_RIGHT)
		 {
				 End();
		 }
		 else
		 {
				 if(HasSelFace())
						 Done();
				 else
						 End();
		 }
 }

BOOL				ScCmdFaceFixAll::HasSelFace()
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull() && aS.ShapeType() == TopAbs_FACE)
				{
						SelObj(0) = SelectedObj();

						return TRUE;
				}
		}
		return FALSE;
}

BOOL				ScCmdFaceFixAll::FixShape()
{
		TopoDS_Shape aShape = GetObjShape(SelObj(0));
		TopoDS_Face  aFace = TopoDS::Face(aShape);

		//进行修正
		Handle(ShapeFix_Face)   sff = new ShapeFix_Face;

		Handle(ShapeBuild_ReShape) ctx = new ShapeBuild_ReShape;
		ctx->Apply(aFace);

		sff->SetContext(ctx);
		sff->Init(aFace);
		sff->SetPrecision(0.001);

		sff->Perform();

		if(sff->Status(ShapeExtend_DONE))
		{
				TopoDS_Shape aNS  = ctx->Apply(aFace);
				if(!aNS.IsNull() && aNS.ShapeType() == TopAbs_FACE)
				{
						TopoDS_Face aNFace = TopoDS::Face(aNS);
						AfxMessageBox("修正成功.");

						m_AISContext->Remove(SelObj(0), Standard_True);
						SelObj(0) = NULL;
						Display(NewObj(0),aNFace);

						return TRUE;
				}
		}

		return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// 修正face的wire的缝隙
BOOL		ScCmdFaceFixGaps::FixShape()
{
		TopoDS_Shape aShape = GetObjShape(SelObj(0));

		Handle(ShapeFix_Wireframe) SFWF = new ShapeFix_Wireframe(aShape);
		SFWF->SetPrecision(0.1);
		if ( SFWF->FixWireGaps() ) 
		{
				TopoDS_Shape aS = SFWF->Shape();
				if(!aS.IsNull() && aS.ShapeType() == TopAbs_FACE)
				{
						m_AISContext->Remove(SelObj(0), Standard_True);
						SelObj(0) = NULL;
						Display(NewObj(0),aS);

						AfxMessageBox("修正完成.");

						return TRUE;
				}
		}

		return FALSE;
}
