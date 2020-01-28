#include "StdAfx.h"
#include ".\sccmdfillet.h"

ScCmdFillet::ScCmdFillet(BOOL bConstRadius)
{
	this->m_bConstRadius = bConstRadius;
}

ScCmdFillet::~ScCmdFillet(void)
{
}

//启动和结束命令
int		ScCmdFillet::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);
	RemoveList();

	SwitchState(S_SHAPE,"选择要倒角的对象:");
	return 0;
}

int		ScCmdFillet::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdFillet::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		//m_AISContext->CloseLocalContex();//yxk20191207

		TopoDS_Shape aS = BuildNewShape();
		if(!aS.IsNull())
		{
			m_aShapeObj->Set(aS);
			m_AISContext->Redisplay(m_aShapeObj,Standard_True);
		}
		Done();
	}
	else
	{
		switch(GetState())
		{
		case S_SHAPE:
			{
				m_AISContext->InitCurrent();
				if(m_AISContext->MoreCurrent())
				{
					Handle(AIS_Shape) aObj = Handle(AIS_Shape)::DownCast(
						m_AISContext->Current());
					if(!aObj.IsNull())
					{
						m_aShapeObj = aObj;
						TopoDS_Shape aShape = aObj->Shape();
						if(!aShape.IsNull())
						{
							m_aOldShape = aShape;
							//
							SwitchState(S_EDGE,"选择对象上的一条边,右键结束:");
							//m_AISContext->OpenLocalContext();//yxk20191207
							m_AISContext->Activate(aObj,2);
						}
					}
				}
			}
			break;
		case S_EDGE:
			{
				m_AISContext->InitSelected();
				if(m_AISContext->MoreSelected())
				{
					ScFltEdge *pEdge = new ScFltEdge();

					pEdge->_edge = TopoDS::Edge(m_AISContext->SelectedShape());
		
					m_aEdgeList.AddTail(pEdge);

					SwitchState(S_R1,"输入倒角半径");
					EndInput();
					NeedInput("倒角半径",INPUT_DOUBLE,100);
				}
			}
			break;
		default:
			break;
		}
	}
}

//鼠标移动的响应函数
void	ScCmdFillet::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
	case S_SHAPE:
	case S_EDGE:
		m_AISContext->MoveTo(point.x,point.y,aView, Standard_True);
		break;
	default:
		break;
	}
}

//接收浮点数
int		ScCmdFillet::AcceptInput(double dVal)
{
	switch(GetState())
	{
	case S_R1:
		{
			//这里应当严格检查。TODO:
			if(dVal <= 0.0)
				return SC_INPUT_ERR;

			ScFltEdge *pEdge = m_aEdgeList.GetTail();
			ASSERT(pEdge);
			pEdge->_r1 = dVal;

			EndInput();
			if(this->m_bConstRadius)
			{
				SwitchState(S_EDGE,"选择对象上的一条边,右键结束:");
			}
			else
			{
				SwitchState(S_R2,"输入倒角半径2:");
				NeedInput("倒角半径2",INPUT_DOUBLE,100);
			}
		}
		break;
	case S_R2:
		{
			//这里应当严格检查。TODO:
			if(dVal <= 0.0)
				return SC_INPUT_ERR;

			ScFltEdge *pEdge = m_aEdgeList.GetTail();
			ASSERT(pEdge);
			pEdge->_r2 = dVal;

			EndInput();
			SwitchState(S_EDGE,"选择对象上的一条边,右键结束:");
		}
		break;
	default:
		break;
	}
	return 0;
}

void	ScCmdFillet::RemoveList()
{
	if(m_aEdgeList.IsEmpty())
		return;

	ScFltEdge *pEdge = m_aEdgeList.RemoveHead();
	while(pEdge)
	{
		delete pEdge;
		pEdge = m_aEdgeList.RemoveHead();
	}
}

TopoDS_Shape	ScCmdFillet::BuildNewShape()
{
	TopoDS_Shape aS;
	if(m_aEdgeList.GetCount() == 0)
		return aS;
	//
	BRepFilletAPI_MakeFillet aFillet(m_aOldShape);

	POSITION pos = m_aEdgeList.GetHeadPosition();
	while(pos)
	{
		ScFltEdge *pEdge = m_aEdgeList.GetNext(pos);
		if(this->m_bConstRadius)
		{
			aFillet.Add(pEdge->_r1,pEdge->_edge);
		}
		else
		{
			aFillet.Add(pEdge->_r1,pEdge->_r2,pEdge->_edge);
		}
	}

	try
	{
		aS = aFillet.Shape();
	}
	catch(Standard_Failure)
	{
		AfxMessageBox("倒角失败");
	}

	return aS;
}