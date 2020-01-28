#include "stdafx.h"
#include "..\MainFrm.h"
//#include <AIS_Drawer.hxx>//yxk20200104
#include <Prs3d_PointAspect.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include "ScCmdMgr.h"
#include "..\ScView.h"
#include "..\ScInputMgr.h"
#include "..\ScSnapMgr.h"
#include "ScCommand.h"
#include "..\ScInput.h"
#include "ScUndoMgr.h"

ScCommand::ScCommand()
{
	this->m_bDone = FALSE;
	this->m_pScCmdMgr = NULL;
	this->m_pInputMgr = NULL;
	this->m_pScView = NULL;
	this->m_pScCmdMgr = NULL;
	this->m_pSnapMgr = NULL;

	this->m_bNeedInput = FALSE;
	this->m_dwOldTime = 0;
	this->m_bBeginUndo = FALSE;

	this->m_bCanUndo = TRUE;//默认允许undo.
}

ScCommand::~ScCommand()
{
}

int		ScCommand::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	m_AISContext = aCtx;
	this->m_bDone = FALSE;
	return 0;
}

//
// 终止命令执行。
//
int		ScCommand::End()
{
	//清除临时显示对象
	this->ClearTmpObjs();

	//
	this->ClearSelObjs();

	//清除新建对象
	this->RemoveNewObjs();

	//恢复删除的对象
	this->RestoreDelObjs();

	if(m_bBeginUndo)
	{
			EndUndo(FALSE);
	}

	//重置状态
	Reset();

	//通知终止。
	if(this->m_pScCmdMgr)
	{
		this->m_pScCmdMgr->CmdTerminated();
	}

	Prompt("就绪");
	return 0;
}

//
// 恢复到初始状态
//
void	ScCommand::Reset()
{
	//结束输入
	EndInput();
	//
	NeedSnap(FALSE);
	//
	NeedMultiSelect(FALSE);
	NeedSelect(FALSE);
	//
	if(this->m_pScView)
	{
		this->m_pScView->NeedModelPoint(FALSE);
	}

	m_AISContext->ClearCurrents(Standard_True);
}

void    ScCommand::Done()
{
	this->m_bDone = TRUE;
	Prompt("就绪");

	//必要的清除
	//清除临时显示对象
	this->ClearTmpObjs();

	//
	this->ClearSelObjs();

	if(m_bBeginUndo)
	{
			EndUndo(TRUE);
	}

	//通知
	if(this->m_pScCmdMgr)
	{
		this->m_pScCmdMgr->CmdFinished();
	}

	Reset();
}

//
// 简单实现。
//
int		ScCommand::Undo()
{
	if(m_bCanUndo)
	{
		//清除新建对象
		this->RemoveNewObjs();

		//恢复删除的对象
		this->RestoreDelObjs();
	}

	return SC_OK;
}

int		ScCommand::Redo()
{
	return SC_OK;
}

void	ScCommand::Prompt(LPCTSTR lpszText)
{
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if(pFrm)
		pFrm->ShowPrompt(lpszText);
}


//屏幕像素点转换为模型空间的点
gp_Pnt	ScCommand::PixelToModel(int x,int y,BOOL bWCS)
{
	ASSERT(this->m_pScView);
	gp_Pnt pnt = this->m_pScView->PixelToModel(x,y,bWCS);
	//是否自动捕捉,如果允许自动捕捉，使用捕获点。
	if(this->m_pSnapMgr && this->m_pSnapMgr->IsEnable())
	{
		pnt = GetSnapPoint(pnt);
	}
	return pnt;
}

//屏幕点转换为cs的平面上的点
gp_Pnt	ScCommand::PixelToCSPlane(Standard_Real x, Standard_Real y,BOOL bWCS)
{
	ASSERT(this->m_pScView);
	gp_Pnt pnt = this->m_pScView->PixelToCSPlane(x,y,bWCS);
	//是否自动捕捉,如果允许自动捕捉，使用捕获点。
	if(this->m_pSnapMgr && this->m_pSnapMgr->IsEnable())
	{
		pnt = GetSnapPoint(pnt);
	}
	return pnt;
}

//坐标转换函数
int		ScCommand::WCSToUCS(gp_Pnt& pnt)
{
	ASSERT(this->m_pScView);
	return this->m_pScView->WCSToUCS(pnt);
}

int		ScCommand::UCSToWCS(gp_Pnt& pnt)
{
	ASSERT(this->m_pScView);
	return this->m_pScView->UCSToWCS(pnt);
}

int		ScCommand::WCSToUCS(gp_Vec& vec)
{
	ASSERT(this->m_pScView);
	return this->m_pScView->WCSToUCS(vec);
}

int		ScCommand::UCSToWCS(gp_Vec& vec)
{
	ASSERT(this->m_pScView);
	return this->m_pScView->UCSToWCS(vec);
}

void	ScCommand::NeedMultiSelect(BOOL bNeed)
{
	if(this->m_pScCmdMgr)
	{
		this->m_pScCmdMgr->NeedMultiSelect(bNeed);
	}
}

void	ScCommand::NeedSelect(BOOL bNeed)
{
	if(this->m_pScCmdMgr)
	{
		this->m_pScCmdMgr->NeedSelect(bNeed);
	}
}

//自动捕捉
void	ScCommand::NeedSnap(BOOL bEnable)
{
	if(this->m_pSnapMgr)
	{
		this->m_pSnapMgr->Enable(bEnable);
	}
}

void	ScCommand::SnapPoint(int nX,int nY,const Handle(V3d_View)& aView)
{
	return;
	if(this->m_pSnapMgr)
	{
		this->m_pSnapMgr->Snap(nX,nY,aView);
	}
}

gp_Pnt	ScCommand::GetSnapPoint(const gp_Pnt& pnt)
{
	gp_Pnt snapPnt = pnt;
	if(this->m_pSnapMgr)
	{
		gp_Pnt tp;
		if(this->m_pSnapMgr->GetSnapPoint(pnt,tp))
		{
			snapPnt = tp;
		}
	}

	return snapPnt;
}
////////////////////////////////////////////////////////////////
//
//需要并开始接收输入
int		ScCommand::NeedInput(LPCTSTR lpszTitle,int nInputType,DWORD dwWaitTime)
{
	if(this->m_bNeedInput)//
	{
		EndInput();
	}

#ifdef _DLG_INPUT
	if(this->m_pInputMgr)
	{
		//如果不为0，则采用自己定义的等待时间,以便立即显示输入框。
		if(dwWaitTime != 0)
		{
			this->m_dwOldTime = this->m_pInputMgr->WaitTime();
			this->m_pInputMgr->SetWaitTime(dwWaitTime);
		}
		//需放后面。
		this->m_pInputMgr->NeedInput(lpszTitle,nInputType);
		this->m_bNeedInput = TRUE;
	}
#else
	if(this->m_pInputMgr)
	{
		ScInputReq req(nInputType,lpszTitle,this);
		//需放后面。
		this->m_pInputMgr->NeedInput(req);
		this->m_bNeedInput = TRUE;
	}
#endif

	return 0;
}

//需要输入整数
int		ScCommand::NeedInteger(LPCTSTR lpszDesc)
{
	ASSERT(lpszDesc);
	if(this->m_bNeedInput)//
	{
		EndInput();
	}

	if(this->m_pInputMgr)
	{
		ScInputReq req(INPUT_INTEGER,lpszDesc,this);
		this->m_pInputMgr->NeedInput(req);
		this->m_bNeedInput = TRUE;
	}
	
	return 0;
}

//需要输入浮点数
int		ScCommand::NeedDouble(LPCTSTR lpszDesc)
{
	ASSERT(lpszDesc);
	if(this->m_bNeedInput)//
	{
		EndInput();
	}

	if(this->m_pInputMgr)
	{
		ScInputReq req(INPUT_DOUBLE,lpszDesc,this);
		this->m_pInputMgr->NeedInput(req);
		this->m_bNeedInput = TRUE;
	}
	
	return 0;
}

//需要3维点
int		ScCommand::NeedPoint3d(LPCTSTR lpszDesc,BOOL bWCS)
{
	ASSERT(lpszDesc);
	if(this->m_bNeedInput)//
	{
		EndInput();
	}

	if(this->m_pInputMgr)
	{
		ScInputReq req(INPUT_POINT3D,lpszDesc,this);
		req._bWCS = bWCS;
		this->m_pInputMgr->NeedInput(req);
		this->m_bNeedInput = TRUE;
	}
	
	return 0;
}

//需要字符串
int		ScCommand::NeedString(LPCTSTR lpszDesc)
{
	ASSERT(lpszDesc);
	if(this->m_bNeedInput)//
	{
		EndInput();
	}

	if(this->m_pInputMgr)
	{
		ScInputReq req(INPUT_STRING,lpszDesc,this);
		this->m_pInputMgr->NeedInput(req);
		this->m_bNeedInput = TRUE;
	}

	return 0;
}

//结束结束输入
int		ScCommand::EndInput()
{
	if(!this->m_bNeedInput)//允许重复调用
		return 0;

	if(this->m_pInputMgr)
	{
		this->m_pInputMgr->EndInput();
#ifdef _DLG_INPUT
		if(this->m_dwOldTime)
		{
			this->m_pInputMgr->SetWaitTime(m_dwOldTime);
			this->m_dwOldTime = 0;
		}
#endif

		this->m_bNeedInput = FALSE;
	}

	return 0;
}

//显示信息函数
void	ScCommand::Show(LPCTSTR lpszVal)
{
	if(this->m_pInputMgr)
	{
		this->m_pInputMgr->Show(lpszVal);
	}
}

void	ScCommand::Show(int nVal)
{
	if(this->m_pInputMgr)
	{
		this->m_pInputMgr->Show(nVal);
	}
}

void	ScCommand::Show(double dVal)
{
	if(this->m_pInputMgr)
	{
		this->m_pInputMgr->Show(dVal);
	}
}

void	ScCommand::Show(gp_Pnt pnt)
{
	if(this->m_pInputMgr)
	{
		this->m_pInputMgr->Show(pnt);
	}
}

//
// 接收输入的主要函数。可以重载。
//
int		ScCommand::AcceptInput(const ScInputReq& inputReq,
									const ScInputVar& inputVar)
{
	if(inputReq._pCmd != this)
		return SC_ERR;

	if(inputReq._nVarType != inputVar._nVarType)
	{
		ASSERT(FALSE);
		return SC_ERR;
	}

	int iret = SC_OK;
	switch(inputVar._nVarType)
	{
	case INPUT_INTEGER:
		{
			iret = AcceptInput(inputVar._nVal);
		}
		break;
	case INPUT_DOUBLE:
		{
			iret = AcceptInput(inputVar._dVal);
		}
		break;
	case INPUT_POINT3D:
		{
			gp_Pnt pnt = inputVar._pnt;
			if(inputReq._bWCS)
			{
				if(this->m_pScView)
				{
					this->m_pScView->UCSToWCS(pnt);
				}
			}		
			iret = AcceptInput(pnt);
		}
		break;
	case INPUT_POINT2D:
		{
			gp_Pnt2d pnt = inputVar._pnt2d;
			iret = AcceptInput(pnt);
		}
		break;
	case INPUT_VECTOR:
		{
			gp_Vec vec = inputVar._vec;
			if(inputReq._bWCS)
			{
				if(this->m_pScView)
				{
					this->m_pScView->UCSToWCS(vec);
				}
			}
			iret = AcceptInput(vec);
		}
		break;
	case INPUT_STRING:
		{
			iret = AcceptInput(inputVar._strVal);
		}
		break;
	default:
		break;
	}

	return iret;
}


void	ScCommand::SwitchState(int nState,LPCTSTR lpszPrompt,BOOL bNeedModelPoint)
{
	SetState(nState);
	if(lpszPrompt)
	{
		Prompt(lpszPrompt);
	}
	//
	this->m_pScView->NeedModelPoint(bNeedModelPoint);
}

//框选方式下的消息响应函数
void	ScCommand::DragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView)
{
}

//按下shift下的框选方式
void	ScCommand::ShiftDragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView)
{
}

//点选方式下的消息响应函数
void	ScCommand::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
}
	
//按下shift键下的点选方式
void	ScCommand::ShiftInputEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}

//鼠标移动的响应函数
void	ScCommand::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}

//按下shift键下的鼠标移动的响应函数
void	ScCommand::ShiftMoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}


//鼠标消息响应函数
void	ScCommand::OnLButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
}

void	ScCommand::OnLButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
}

void	ScCommand::OnMouseMove(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
}

void	ScCommand::OnLButtonDblClk(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
}

void	ScCommand::OnRButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
}

void	ScCommand::OnRButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView)
{
}

//////////////////////////////////////////////////////////////////////////
// Undo函数
//开始一个undo项，给出名称
int			ScCommand::BeginUndo(LPCTSTR lpszName)
{
		int  iret = 0;
		if(m_pUndoMgr)
		{
				ASSERT(m_bBeginUndo == FALSE);
				iret = m_pUndoMgr->BeginUndo(lpszName);
				m_bBeginUndo = TRUE;
		}

		return iret;
}

//记录一个新创建的对象
int			ScCommand::UndoAddNewObj(const Handle(AIS_InteractiveObject)& aObj)
{
		int  iret = 0;
		if(m_pUndoMgr)
		{
				ASSERT(m_bBeginUndo);
				iret = m_pUndoMgr->AddNewObj(aObj);
		}

		return iret;
}
//记录一个删除的对象
int			ScCommand::UndoAddDelObj(const Handle(AIS_InteractiveObject)& aObj)
{
		int  iret = 0;
		if(m_pUndoMgr)
		{
				ASSERT(m_bBeginUndo);
				iret = m_pUndoMgr->AddDelObj(aObj);
		}

		return iret;
}
//结束undo项，提交还是放弃
int			ScCommand::EndUndo(BOOL bCommit)
{
		int  iret = 0;
		if(m_pUndoMgr)
		{
				ASSERT(m_bBeginUndo);
				iret = m_pUndoMgr->EndUndo(bCommit);
				m_bBeginUndo = FALSE;
		}

		return iret;
}

//从AisCtx中移出临时object，并释放内存。这里关键是释放内存。
void	ScCommand::RemoveCtxObj(Handle(AIS_InteractiveObject)& aObj,BOOL bUpdate)
{
	if(!aObj.IsNull())
	{
		m_AISContext->Remove(aObj,bUpdate);
		//释放内存，很重要.因为如果aObj是类的属性时，引用将一直存在，不释放，
		//频繁创建临时对象，会导致内存泄漏。
		aObj.Nullify();
	}
}

void	ScCommand::Display(Handle(AIS_InteractiveObject)& aObj,
						   const TopoDS_Shape& aShape,
						   BOOL bUpdate)
{
	if(!aShape.IsNull())
	{
		aObj = new AIS_Shape(aShape);
		m_AISContext->SetDisplayMode(aObj,1,Standard_False);
		//更改点的显示
		//Handle (AIS_Drawer) aDrawer = aObj->Attributes();
		//aDrawer->PointAspect()->SetTypeOfMarker(Aspect_TOM_BALL);
		//m_AISContext->SetLocalAttributes(aObj,aDrawer,Standard_False);
		//显示
		m_AISContext->Display(aObj,bUpdate);
	}
}

//显示一个对象，并记录该对象。以线框模式显示。命令中显示对象，推荐使用该函数。
void	ScCommand::WFDisplay(Handle(AIS_InteractiveObject)& aObj,
								const TopoDS_Shape& aShape,
								BOOL bUpdate)
{
	if(!aShape.IsNull())
	{
		aObj = new AIS_Shape(aShape);
		m_AISContext->SetDisplayMode(aObj,0,Standard_False);
		
		m_AISContext->Display(aObj,bUpdate);
	}
}

//显示一个对象，并记录该对象。以着色模式显示。命令中显示对象，推荐使用该函数。
void	ScCommand::Display(Handle(AIS_InteractiveObject)& aObj,
							const TopoDS_Shape& aShape,
							COLORREF color,//显示颜色
							BOOL bUpdate)
{
	if(!aShape.IsNull())
	{
		aObj = new AIS_Shape(aShape);
		//着色显示.
		m_AISContext->SetDisplayMode(aObj,1,Standard_False);
		//颜色
		Quantity_Color qcol(GetRValue(color)/255.0,GetGValue(color)/255.0,
			GetBValue(color)/255.0,Quantity_TOC_RGB);
		m_AISContext->SetColor(aObj,qcol,Standard_False);
		m_AISContext->Display(aObj,bUpdate);
	}
}


void	ScCommand::Display(const TopoDS_Shape& aShape)
{
}

//获取一个选择的shape，只能是一个
int		ScCommand::GetOneCurrentObj(Handle(AIS_InteractiveObject)& aObj)
{	
	//NbCurrents会初始化选择
	int nCurs = m_AISContext->NbCurrents();
	if(nCurs <= 0)
		return SEL_NONE;

	if(nCurs > 1)
		return SEL_MORE;

	//首先要初始化。
	m_AISContext->InitCurrent();	
	aObj = m_AISContext->Current();

	return SEL_OK;
}

//获取一个选择的shape，只能是一个
int		ScCommand::GetOneSelShape(TopoDS_Shape& aShape)
{
	Handle(AIS_InteractiveObject) aObj;
	int nRes = GetOneCurrentObj(aObj);
	if(nRes == SEL_OK)
	{
		Handle(AIS_Shape) aAIShape = Handle(AIS_Shape)::DownCast(aObj);
		if(!aAIShape.IsNull())
			aShape = aAIShape->Shape();
	}

	return nRes;
}

//获取当前选择的shape。
int		ScCommand::GetCurrentShape(TopoDS_Shape& aShape)
{
	//这里Selected当有LocalCtx时选择其中的obj，当没有时选择
	//当前的obj。
	m_AISContext->InitSelected();
	if(m_AISContext->MoreSelected())
	{
		aShape = m_AISContext->SelectedShape();
		if(!aShape.IsNull())
			return SEL_OK;
	}

	return SEL_NONE;
}

//获取对象的shape
TopoDS_Shape	ScCommand::GetObjShape(Handle(AIS_InteractiveObject)& aObj)
{
	TopoDS_Shape aShape;
	if(aObj.IsNull())
		return aShape;

	Handle(AIS_Shape) aAIShape = Handle(AIS_Shape)::DownCast(aObj);
	if(!aAIShape.IsNull())
		aShape = aAIShape->Shape();

	return aShape;
}

//根据鼠标点获取对象当前点击的Face
TopoDS_Face		ScCommand::GetObjPickedFace(Handle(AIS_InteractiveObject)& aObj,
									int nX,int nY,const Handle(V3d_View)& aView)
{
	TopoDS_Face aFace;
	if(aObj.IsNull())
		return aFace;

	//m_AISContext->OpenLocalContext();//yxk20191207
	m_AISContext->Activate(aObj,4);//face

	m_AISContext->MoveTo(nX,nY,aView, Standard_True);
	m_AISContext->Select(Standard_True);

	InitSelect();
	if(MoreSelect())
	{
		aFace = TopoDS::Face(SelectedShape(FALSE));
	}

	//m_AISContext->CloseLocalContex();//yxk20191207

	return aFace;
}

//根据鼠标点获取对象中据点击点最近的edge
TopoDS_Edge		ScCommand::GetNearestEdge(Handle(AIS_InteractiveObject)& aObj,
														int nX,int nY,double& t,gp_Pnt& pnt)
{
	TopoDS_Edge aEdge;
	if(aObj.IsNull())
		return aEdge;

	TopoDS_Shape aS = GetObjShape(aObj);
	if(aS.IsNull())
		return aEdge;

	double dist = 1e10;//尽量大
	TopExp_Explorer ex;
	for(ex.Init(aS,TopAbs_EDGE);ex.More();ex.Next())
	{
		TopoDS_Edge aE = TopoDS::Edge(ex.Current());
		if(aE.IsNull())
			continue;

		double tt;
		gp_Pnt ppnt;
		double dst;
		if(this->m_pScView->GetEdgeNearestPoint(aE,nX,nY,tt,ppnt,dst))
		{
			if(dst < dist)
			{
				dist = dst;
				aEdge = aE;
				t = tt;
				pnt = ppnt;
			}
		}
	}

	return aEdge;
}


void	ScCommand::InitSelect()
{
	m_AISContext->InitSelected();
}

BOOL	ScCommand::MoreSelect()
{
	if(m_AISContext->MoreSelected())
		return TRUE;
	return FALSE;
}

void	ScCommand::NextSelect()
{
	m_AISContext->NextSelected();
}

void	ScCommand::ClearSelect()
{
	m_AISContext->ClearCurrents(Standard_True);//yxk20200104
}

TopoDS_Shape	ScCommand::SelectedShape(BOOL bNext)
{
	TopoDS_Shape aShape = m_AISContext->SelectedShape();
	if(bNext)
	{
		NextSelect();
	}

	return aShape;
}

Handle(AIS_InteractiveObject) ScCommand::SelectedObj(BOOL bNext)
{
	Handle(AIS_InteractiveObject) aObj = m_AISContext->SelectedInteractive();
	if(bNext)
	{
		NextSelect();
	}

	return aObj;
}

//////////////////////////////////////////////////////////
//
//
// 获取对应的obj，如果没有，创建之。
// 注意：seq范围为[1,len]
//
Handle(AIS_InteractiveObject)&	ScCommand::NewObj(int idx)
{
	int nLen = this->m_seqOfNewObj.Length();
	if(idx < 0)//负数，表明向最后添加新的对象。
	{
		idx = nLen;
	}
	if(nLen < idx + 1)
	{//需要生成新的对象
		for(int ix = nLen + 1;ix <= idx + 1;ix ++)
		{
			Handle(AIS_InteractiveObject) aObj;//空引用。
			this->m_seqOfNewObj.Append(aObj);
		}
	}

	return this->m_seqOfNewObj.ChangeValue(idx + 1);
}

Handle(AIS_InteractiveObject)&	ScCommand::DelObj(int idx)
{
	int nLen = this->m_seqOfDelObj.Length();
	if(idx < 0)//负数，表明向最后添加新的对象。
	{
		idx = nLen;
	}
	if(nLen < idx + 1)
	{//需要生成新的对象
		for(int ix = nLen + 1;ix <= idx + 1;ix ++)
		{
			Handle(AIS_InteractiveObject) aObj;//空引用。
			this->m_seqOfDelObj.Append(aObj);
		}
	}

	return this->m_seqOfDelObj.ChangeValue(idx + 1);
}

Handle(AIS_InteractiveObject)&	ScCommand::TmpObj(int idx)
{
	int nLen = this->m_seqOfTmpObj.Length();
	if(idx < 0)//负数，表明向最后添加新的对象。
	{
		idx = nLen;
	}
	if(nLen < idx + 1)
	{//需要生成新的对象
		for(int ix = nLen + 1;ix <= idx + 1;ix ++)
		{
			Handle(AIS_InteractiveObject) aObj;//空引用。
			this->m_seqOfTmpObj.Append(aObj);
		}
	}

	return this->m_seqOfTmpObj.ChangeValue(idx + 1);
}

Handle(AIS_InteractiveObject)&	ScCommand::SelObj(int idx)
{
	int nLen = this->m_seqOfSelObj.Length();
	if(idx < 0)//负数，表明向最后添加新的对象。
	{
		idx = nLen;
	}
	if(nLen < idx + 1)
	{//需要生成新的对象
		for(int ix = nLen + 1;ix <= idx + 1;ix ++)
		{
			Handle(AIS_InteractiveObject) aObj;//空引用。
			this->m_seqOfSelObj.Append(aObj);
		}
	}

	return this->m_seqOfSelObj.ChangeValue(idx + 1);
}


//查找对应的Obj.
int	ScCommand::FindNewObj(const TopoDS_Shape& aShape)
{
	int idx = -1;
	for(int ix = 1;ix <= m_seqOfNewObj.Length();ix ++)
	{
		TopoDS_Shape aS = Handle(AIS_Shape)::DownCast(m_seqOfNewObj.Value(ix))->Shape();
		if(!aS.IsNull() && aS.IsSame(aShape))
		{
			idx = ix - 1;
			break;
		}
	}

	return idx;
}

void	ScCommand::RemoveNewObj(int idx)
{
	ASSERT((idx >= 0) && (idx < m_seqOfNewObj.Length()));
	this->m_seqOfNewObj.ChangeValue(idx + 1).Nullify();
	m_seqOfNewObj.Remove(idx + 1);
}

//
// 清除所有新创建的对象。
//
int		ScCommand::RemoveNewObjs()
{
	if(this->m_seqOfNewObj.IsEmpty())
		return SC_OK;

	//从显示中删除
	for(int ix = 1;ix <= this->m_seqOfNewObj.Length();ix ++)
	{
		m_AISContext->Remove(m_seqOfNewObj.Value(ix),Standard_False);
	}
	//
	this->m_seqOfNewObj.Clear();
	//更新
	m_AISContext->UpdateCurrentViewer();

	return SC_OK;
}

//恢复所有删除对象
int		ScCommand::RestoreDelObjs()
{
	if(this->m_seqOfDelObj.IsEmpty())
		return SC_OK;

	//重新显示.可能是使用erase删除或remove 删除的，要区分对待。
	for(int ix = 1;ix <= this->m_seqOfDelObj.Length();ix++)
	{
		Handle(AIS_InteractiveObject) aObj = m_seqOfDelObj.Value(ix);
		//
	//yxk20200104
		//if(m_AISContext->IsInCollector(aObj))
		//{
		//	m_AISContext->DisplayFromCollector(aObj,Standard_False);
		//}
		//else
		//{
			m_AISContext->Display(aObj,Standard_False);
		//}
	//endyxk
	}
	//
	this->m_seqOfDelObj.Clear();
	//更新
	m_AISContext->UpdateCurrentViewer();

	return SC_OK;
}

//清除所有临时对象
int		ScCommand::ClearTmpObjs()
{
	if(this->m_seqOfTmpObj.IsEmpty())
		return SC_OK;

	//从显示中删除
	for(int ix = 1;ix <= this->m_seqOfTmpObj.Length();ix ++)
	{
		//只有显示的，才移除
		if(m_AISContext->IsDisplayed(m_seqOfTmpObj.Value(ix)))
			m_AISContext->Remove(m_seqOfTmpObj.Value(ix), Standard_False);

		m_seqOfTmpObj.ChangeValue(ix).Nullify();
	}
	//
	this->m_seqOfTmpObj.Clear();

	return SC_OK;
}

//清除记录的选中对象，这里只是清除reference。
int		ScCommand::ClearSelObjs()
{
	if(this->m_seqOfSelObj.IsEmpty())
		return SC_OK;

	//从显示中删除
	for(int ix = 1;ix <= this->m_seqOfTmpObj.Length();ix ++)
	{
		//置空。
		m_seqOfTmpObj.ChangeValue(ix).Nullify();
	}
	//
	this->m_seqOfTmpObj.Clear();

	return SC_OK;
}