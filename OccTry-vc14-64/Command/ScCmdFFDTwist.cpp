#include "StdAfx.h"

#include "ScBRepLib.h"

#include "FFDObj.h"
#include "FFDTool.h"
#include "FFDAlgo.h"

#include ".\sccmdffdtwist.h"

ScCmdFFDTwist::ScCmdFFDTwist(void)
{
	m_rotrad = 0.0;
}

ScCmdFFDTwist::~ScCmdFFDTwist(void)
{
}

//启动和结束命令
int	  ScCmdFFDTwist::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	if(HasSelObj())
	{
		NeedSnap(TRUE);
		SwitchState(S_AX_BEGIN,"旋转轴起始点:");
	}
	else
	{
		NeedSelect(TRUE);
		SwitchState(S_OBJ,"选择一个对象:");
	}

	return 0;
}

int	  ScCmdFFDTwist::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdFFDTwist::InputEvent(const CPoint& point,int nState,//上述minput定义
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
		case S_OBJ:
			{
				if(HasSelObj())
				{
					NeedSnap(TRUE);
					SwitchState(S_AX_BEGIN,"旋转轴起始点:");
				}
			}
			break;
		case S_AX_BEGIN:
			{
				m_pntBegin = PixelToCSPlane(point.x,point.y);
				SwitchState(S_AX_END,"旋转轴终点:");
			}
			break;
		case S_AX_END:
			{
				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_pntBegin) < 1e-6)
					return;

				m_pntEnd = pnt;
				SwitchState(S_BASEDIR,"旋转起始位置;");
			}
			break;
		case S_BASEDIR:
			{
				if(!CalcRotatePlnPoint(point.x,point.y,TRUE))
					return;

				m_prePnt = m_basePnt;
				SwitchState(S_ANGLE,"旋转角度");
			}
			break;
		case S_ANGLE:
			{
				if(DoTwist(FALSE))
				{
					Done();
				}
				else
				{
					End();
				}
			}
			break;
		default:
			break;
		}
	}
}

//鼠标移动的响应函数
void	ScCmdFFDTwist::MoveEvent(const CPoint& point,
								  const Handle(V3d_View)& aView)
{
	switch(GetState())
	{
		case S_AX_END:
			{
				gp_Pnt pnt = PixelToCSPlane(point.x,point.y);
				if(pnt.Distance(m_pntBegin) < 1e-6)
					return;

				ShowAxis(pnt);
			}
			break;
		case S_BASEDIR:
			{
				if(!CalcRotatePlnPoint(point.x,point.y,TRUE))
					return;

				ShowBaseVec(m_basePnt);
			}
			break;
		case S_ANGLE:
			{
				if(!CalcRotatePlnPoint(point.x,point.y,FALSE))
					return;

				//计算旋转角度
				gp_Vec nv(m_pntEnd,m_newPnt),nv2(m_pntEnd,m_prePnt);
				double dAng = nv2.AngleWithRef(nv,gp_Vec(m_pntBegin,m_pntEnd));//方向很重要
				m_rotrad += dAng;

				ShowBaseVec(m_newPnt);
				DoTwist(TRUE);

				m_prePnt = m_newPnt;
			}
			break;
		default:
			break;
	}
}

//根据空间点计算旋转平面上的点.
BOOL	ScCmdFFDTwist::CalcRotatePlnPoint(int nx,int ny,BOOL bBase)
{
	gp_Pnt pnt = PixelToModel(nx,ny);
	if(pnt.Distance(m_pntEnd) < 1e-6)
		return FALSE;

	//旋转方向vec需要和旋转轴垂直,即在旋转平面内
	gp_Vec basVec(m_pntEnd,pnt),axVec(m_pntBegin,m_pntEnd);
	if(basVec.IsParallel(axVec,0.001))//两线平行,无法计算
		return FALSE;

	double dVal = basVec.Dot(axVec);//验证是否垂直
	if(fabs(dVal) > 1e-10)
	{
		gp_Vec nVec = basVec.Crossed(axVec);
		basVec = axVec.Crossed(nVec);
	}
	
	//实现baseVec可能比较长,需要计算距离pnt比较近的点
	gp_Pnt npnt;
	Handle(Geom_Line) aLin = new Geom_Line(m_pntEnd,gp_Dir(basVec));
	GeomAPI_ProjectPointOnCurve prj(pnt,aLin);
	if(prj.NbPoints() > 0)
	{
		npnt = prj.NearestPoint();
	}
	else
	{
		npnt.SetCoord(m_pntEnd.X() + basVec.X(),m_pntEnd.Y() + basVec.Y(),m_pntEnd.Z() + basVec.Z());
	}


	if(bBase)
	{
		m_basePnt = npnt;
		m_baseVec = basVec;
	}
	else
	{
		m_newPnt = npnt;
	}

	return TRUE;
}


BOOL	ScCmdFFDTwist::HasSelObj()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aShape = SelectedShape(FALSE);
		if(ScBRepLib::IsFace(aShape) || ScBRepLib::IsShell(aShape) ||
			ScBRepLib::IsSolid(aShape))
		{
			SelObj(0) = SelectedObj();

			return TRUE;
		}
	}

	return FALSE;
}

void	ScCmdFFDTwist::ShowAxis(const gp_Pnt& pnt)
{
	RemoveCtxObj(TmpObj(0));

	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_pntBegin,pnt);
	Display(TmpObj(0),aE);
}

void	ScCmdFFDTwist::ShowBaseVec(const gp_Pnt& pnt)
{
	RemoveCtxObj(TmpObj(1));

	TopoDS_Edge aE = BRepBuilderAPI_MakeEdge(m_pntEnd,pnt);
	Display(TmpObj(1),aE);
}

BOOL	ScCmdFFDTwist::DoTwist(BOOL bTmp)
{
	TopoDS_Shape aS = GetObjShape(SelObj(0));

	RemoveCtxObj(TmpObj(2));

	FFDSurfObj obj;
	if(!obj.InitShape(aS,40,40))
	{
		AfxMessageBox("Init obj failed.");
		return FALSE;
	}

	FFDTwistTool tool;
	if(!tool.Init(m_pntBegin,m_pntEnd,m_baseVec))
	{
		return FALSE;
	}

	tool.Rotate(m_rotrad,FALSE);

	FFDTwistAlgo algo;
	if(!algo.Init(&obj,&tool))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if(algo.Deform() && algo.IsDone())
	{
		TopoDS_Shape aNS = algo.Shape();

		if(bTmp)
		{
			Display(TmpObj(2),aNS);
		}
		else
		{
			Display(NewObj(0),aNS);
			BeginUndo("twist");
			UndoAddNewObj(NewObj(0));
			EndUndo(TRUE);
		}

		return TRUE;
	}

	return FALSE;
}