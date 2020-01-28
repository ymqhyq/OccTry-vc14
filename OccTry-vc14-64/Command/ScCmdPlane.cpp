#include "StdAfx.h"
#include <BRepBndLib.hxx>
#include ".\sccmdplane.h"

//ScCmdPlane::ScCmdPlane(void)
//{
//}
//
//ScCmdPlane::~ScCmdPlane(void)
//{
//}

//////////////////////////////////////////////////////////////////////////
// 从wire和curve生成的平面

//启动和结束命令
int		ScCmdPlaneFromWire::Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		if(HasSelCurve())
		{
				if(MakePlane())
						Done();
				else
						End();
		}
		else
		{
				NeedSelect(TRUE);
				Prompt("选择一条平面曲线:");
		}

		return 0;
}

int		ScCmdPlaneFromWire::End()
{
		return ScCommand::End();
}

//点选方式下的消息响应函数
void		ScCmdPlaneFromWire::InputEvent(const CPoint& point,int nState,
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
						if(MakePlane())
								Done();
						else
								End();
				}		
		}
}

BOOL			ScCmdPlaneFromWire::HasSelCurve()
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull() && (aS.ShapeType() == TopAbs_WIRE ||
						aS.ShapeType() == TopAbs_EDGE))
				{
						SelObj(0) = SelectedObj();

						return TRUE;
				}
		}
		return FALSE;
}

BOOL			ScCmdPlaneFromWire::MakePlane()
{
		TopoDS_Shape aS = GetObjShape(SelObj(0));
		TopoDS_Wire    aW;
		if(aS.ShapeType() == TopAbs_EDGE)
		{
				aW = BRepBuilderAPI_MakeWire(TopoDS::Edge(aS));
		}
		else
		{
				aW = TopoDS::Wire(aS);
		}

		// Find a surface through the wire
		BRepLib_FindSurface FS(aW, -1,Standard_True);
		if (!FS.Found()) {
				AfxMessageBox("非平面曲线");
				return FALSE;
		}

		//计算包围盒
		Bnd_Box  box;
		BRepBndLib::Add(aW,box);
		double  dLen = ::sqrt(box.SquareExtent());

		//创建平面
		TopoDS_Face aFace = BRepBuilderAPI_MakeFace(FS.Surface(),-dLen,dLen,-dLen,dLen, Precision::Confusion());//yxk20200104增加, Precision::Confusion()

		if(!aFace.IsNull())
		{
				Display(NewObj(0),aFace);
				if(!NewObj(0).IsNull())
				{
						BeginUndo("Plane From Wire");
						UndoAddNewObj(NewObj(0));
						EndUndo(TRUE);
				}
		}
		
		return TRUE;
}
