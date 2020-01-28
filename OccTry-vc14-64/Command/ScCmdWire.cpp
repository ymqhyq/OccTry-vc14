#include "StdAfx.h"
#include ".\sccmdwire.h"

ScCmdWire::ScCmdWire(void)
{
		
}

ScCmdWire::~ScCmdWire(void)
{
}

//启动和结束命令
int		ScCmdWireFixBase::Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		m_dFixTol = 0.001;

		if(HasSelWire())
		{
				SwitchState(S_TOL,"输入修正误差:");
				NeedSelect(FALSE);
				NeedDouble("修正误差:");
		}
		else
		{
				NeedSelect(TRUE);
				SwitchState(S_WIRE,"选择一条wire:");
		}

		return 0;
}

int		ScCmdWireFixBase::End()
{
		SelObj(0) = NULL;
		return ScCommand::End();
}

//点选方式下的消息响应函数
 void		ScCmdWireFixBase::InputEvent(const CPoint& point,int nState,
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
				 case S_WIRE:
						 {
								 if(HasSelWire())
								 {
										 SwitchState(S_TOL,"输入修正误差:");
										 NeedSelect(FALSE);
										 NeedDouble("修正误差:");
								 }
						 }
						 break;
				 default:
						 break;
				 }
		 }
 }

		//接收浮点数
int		ScCmdWireFixBase::AcceptInput(double dVal)
{
		if(GetState() != S_TOL)
				return -1;

		if(dVal < 0.0)
				return -1;

		m_dFixTol = dVal;

		if(FixShape())
				Done();
		else
				End();

		return 0;
}


BOOL		ScCmdWireFixBase::HasSelWire()
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull() && aS.ShapeType() == TopAbs_WIRE)
				{
						SelObj(0) = SelectedObj(FALSE);

						return TRUE;
				}
		}

		return FALSE;
}

BOOL		ScCmdWireFixBase::FixShape()
{
		return FALSE;
}

///////////////////////////////////////////////////////////////////////
// wire的闭合修正
BOOL			ScCmdWireCloseFix::FixShape()
{
		TopoDS_Wire aWire = TopoDS::Wire(GetObjShape(SelObj(0)));
		if(aWire.IsNull())
				return FALSE;

		if(BRep_Tool::IsClosed(aWire))
		{
				AfxMessageBox("wire闭合,不用修正.");
				return FALSE;
		}

		Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;

		Handle(ShapeBuild_ReShape)  ctx = new ShapeBuild_ReShape;
		ctx->Apply(aWire);

		sfw->SetContext(ctx);
		sfw->Load(aWire);

		sfw->FixClosed(m_dFixTol);
		if(sfw->StatusClosed(ShapeExtend_DONE))
		{
				TopoDS_Shape aNS = ctx->Apply(aWire);
				if(!aNS.IsNull() && aNS.ShapeType() == TopAbs_WIRE)
				{

						TopoDS_Wire aNW = TopoDS::Wire(aNS);
						aNW.Closed(Standard_True);

						m_AISContext->Remove(SelObj(0), Standard_True);
						SelObj(0) = NULL;
						Display(NewObj(0),aNW);

						AfxMessageBox("修正完成.");
						return TRUE;

				}
		}

		return FALSE;
}

/////////////////////////////////////////////////////////////////////////
// wire的修正，使用shapeFix_wire类的相应功能.
BOOL		ScCmdWireFix::	FixShape()
{
		TopoDS_Wire aWire = TopoDS::Wire(GetObjShape(SelObj(0)));
		if(aWire.IsNull())
				return FALSE;

		Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;

		Handle(ShapeBuild_ReShape)  ctx = new ShapeBuild_ReShape;
		ctx->Apply(aWire);

		sfw->SetContext(ctx);
		sfw->Load(aWire);
		sfw->SetPrecision(0.1);

		sfw->Perform();

		if(sfw->LastFixStatus(ShapeExtend_DONE))
		{
				TopoDS_Shape aNS = ctx->Apply(aWire);
				if(!aNS.IsNull() && aNS.ShapeType() == TopAbs_WIRE)
				{

						TopoDS_Wire aNW = TopoDS::Wire(aNS);

						m_AISContext->Remove(SelObj(0), Standard_True);
						SelObj(0) = NULL;
						Display(NewObj(0),aNW);

						AfxMessageBox("修正完成.");
						return TRUE;

				}
		}

		return FALSE;
}

BOOL		ScCmdWireFixGaps::FixShape()
{
		TopoDS_Wire aWire = TopoDS::Wire(GetObjShape(SelObj(0)));
		if(aWire.IsNull())
				return FALSE;

		Handle(ShapeFix_Wireframe) SFWF = new ShapeFix_Wireframe(aWire);
		SFWF->SetPrecision(m_dFixTol);
		if ( SFWF->FixWireGaps() ) 
		{
				TopoDS_Shape aS = SFWF->Shape();
				if(!aS.IsNull() && aS.ShapeType() == TopAbs_WIRE)
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
