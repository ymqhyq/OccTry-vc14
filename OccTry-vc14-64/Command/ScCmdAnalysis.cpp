#include "StdAfx.h"

#include <BRepGProp.hxx>
#include "ScBRepLib.h"
#include ".\sccmdanalysis.h"

ScCmdAnalysis::ScCmdAnalysis(void)
{
}

ScCmdAnalysis::~ScCmdAnalysis(void)
{
}

//////////////////////////////////////////////////////////////////////////
// ���߳���

//�����ͽ�������
int	ScCmdCurveLength::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	NeedSelect(TRUE);
	if(HasSelObj())
	{
		GetLength();
	}
	SwitchState(S_CURVE,"ѡ��һ������:");

	return 0;
}

int	ScCmdCurveLength::End()
{
	return ScCommand::End();
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCmdCurveLength::InputEvent(const CPoint& point,int nState,
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
		case S_CURVE:
			{
				if(HasSelObj())
				{
					GetLength();
				}
			}
			break;
		default:
			break;
		}
	}
}


BOOL	ScCmdCurveLength::HasSelObj()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsEdge(aS) || ScBRepLib::IsWire(aS))
		{
			SelObj(0) = SelectedObj(FALSE);

			return TRUE;
		}
	}
	return FALSE;
}

BOOL	ScCmdCurveLength::GetLength()
{
	TopoDS_Shape aS = GetObjShape(SelObj(0));

	double dtLen = 0.0;
	GProp_GProps prop;
	TopExp_Explorer ex;
	
	for(ex.Init(aS,TopAbs_EDGE);ex.More();ex.Next())
	{
		TopoDS_Edge aE = TopoDS::Edge(ex.Current());
		BRepGProp::LinearProperties(aE,prop);
		dtLen += prop.Mass();
	}

	CString szMsg;
	szMsg.Format("���߳���Ϊ:%f",dtLen);
	AfxMessageBox(szMsg);

	return TRUE;
}
