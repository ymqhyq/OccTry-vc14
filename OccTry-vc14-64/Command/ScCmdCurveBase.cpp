#include "StdAfx.h"
#include "ScBRepLib.h"
#include ".\sccmdcurvebase.h"

ScCmdCurveBase::ScCmdCurveBase(void)
{
}

ScCmdCurveBase::~ScCmdCurveBase(void)
{
}

//�Ƿ�ѡ�ж������ѡ���˶�������󱣴���SelObj(0)�С�
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