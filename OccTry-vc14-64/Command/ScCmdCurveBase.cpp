#include "StdAfx.h"
#include "ScBRepLib.h"
#include ".\sccmdcurvebase.h"

ScCmdCurveBase::ScCmdCurveBase(void)
{
}

ScCmdCurveBase::~ScCmdCurveBase(void)
{
}

//是否选中对象，如果选中了对象，则对象保存在SelObj(0)中。
BOOL			ScCmdCurveBase::HasSelEdge(int idx)
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull() && ScBRepLib::IsEdge(aS))
				{
						SelObj(idx) = SelectedObj(FALSE);

						return TRUE;
				}
		}

		return FALSE;
}

BOOL			ScCmdCurveBase::HasSelWire(int idx)
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull() && ScBRepLib::IsWire(aS))
				{
						SelObj(idx) = SelectedObj(FALSE);

						return TRUE;
				}
		}

		return FALSE;
}

BOOL			ScCmdCurveBase::HasSelEdgeOrWire(int idx)
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape aS = SelectedShape(FALSE);
				if(!aS.IsNull() && (ScBRepLib::IsEdge(aS) || ScBRepLib::IsWire(aS)))
				{
						SelObj(idx) = SelectedObj(FALSE);

						return TRUE;
				}
		}

		return FALSE;
}