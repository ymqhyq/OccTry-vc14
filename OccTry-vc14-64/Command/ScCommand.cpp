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

	this->m_bCanUndo = TRUE;//Ĭ������undo.
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
// ��ֹ����ִ�С�
//
int		ScCommand::End()
{
	//�����ʱ��ʾ����
	this->ClearTmpObjs();

	//
	this->ClearSelObjs();

	//����½�����
	this->RemoveNewObjs();

	//�ָ�ɾ���Ķ���
	this->RestoreDelObjs();

	if(m_bBeginUndo)
	{
			EndUndo(FALSE);
	}

	//����״̬
	Reset();

	//֪ͨ��ֹ��
	if(this->m_pScCmdMgr)
	{
		this->m_pScCmdMgr->CmdTerminated();
	}

	Prompt("����");
	return 0;
}

//
// �ָ�����ʼ״̬
//
void	ScCommand::Reset()
{
	//��������
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
	Prompt("����");

	//��Ҫ�����
	//�����ʱ��ʾ����
	this->ClearTmpObjs();

	//
	this->ClearSelObjs();

	if(m_bBeginUndo)
	{
			EndUndo(TRUE);
	}

	//֪ͨ
	if(this->m_pScCmdMgr)
	{
		this->m_pScCmdMgr->CmdFinished();
	}

	Reset();
}

//
// ��ʵ�֡�
//
int		ScCommand::Undo()
{
	if(m_bCanUndo)
	{
		//����½�����
		this->RemoveNewObjs();

		//�ָ�ɾ���Ķ���
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


//��Ļ���ص�ת��Ϊģ�Ϳռ�ĵ�
gp_Pnt	ScCommand::PixelToModel(int x,int y,BOOL bWCS)
{
	ASSERT(this->m_pScView);
	gp_Pnt pnt = this->m_pScView->PixelToModel(x,y,bWCS);
	//�Ƿ��Զ���׽,��������Զ���׽��ʹ�ò���㡣
	if(this->m_pSnapMgr && this->m_pSnapMgr->IsEnable())
	{
		pnt = GetSnapPoint(pnt);
	}
	return pnt;
}

//��Ļ��ת��Ϊcs��ƽ���ϵĵ�
gp_Pnt	ScCommand::PixelToCSPlane(Standard_Real x, Standard_Real y,BOOL bWCS)
{
	ASSERT(this->m_pScView);
	gp_Pnt pnt = this->m_pScView->PixelToCSPlane(x,y,bWCS);
	//�Ƿ��Զ���׽,��������Զ���׽��ʹ�ò���㡣
	if(this->m_pSnapMgr && this->m_pSnapMgr->IsEnable())
	{
		pnt = GetSnapPoint(pnt);
	}
	return pnt;
}

//����ת������
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

//�Զ���׽
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
//��Ҫ����ʼ��������
int		ScCommand::NeedInput(LPCTSTR lpszTitle,int nInputType,DWORD dwWaitTime)
{
	if(this->m_bNeedInput)//
	{
		EndInput();
	}

#ifdef _DLG_INPUT
	if(this->m_pInputMgr)
	{
		//�����Ϊ0��������Լ�����ĵȴ�ʱ��,�Ա�������ʾ�����
		if(dwWaitTime != 0)
		{
			this->m_dwOldTime = this->m_pInputMgr->WaitTime();
			this->m_pInputMgr->SetWaitTime(dwWaitTime);
		}
		//��ź��档
		this->m_pInputMgr->NeedInput(lpszTitle,nInputType);
		this->m_bNeedInput = TRUE;
	}
#else
	if(this->m_pInputMgr)
	{
		ScInputReq req(nInputType,lpszTitle,this);
		//��ź��档
		this->m_pInputMgr->NeedInput(req);
		this->m_bNeedInput = TRUE;
	}
#endif

	return 0;
}

//��Ҫ��������
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

//��Ҫ���븡����
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

//��Ҫ3ά��
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

//��Ҫ�ַ���
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

//������������
int		ScCommand::EndInput()
{
	if(!this->m_bNeedInput)//�����ظ�����
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

//��ʾ��Ϣ����
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
// �����������Ҫ�������������ء�
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

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCommand::DragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView)
{
}

//����shift�µĿ�ѡ��ʽ
void	ScCommand::ShiftDragEvent(const CPoint& point,int nMbState,
										const Handle(V3d_View)& aView)
{
}

//��ѡ��ʽ�µ���Ϣ��Ӧ����
void	ScCommand::InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView)
{
}
	
//����shift���µĵ�ѡ��ʽ
void	ScCommand::ShiftInputEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}

//����ƶ�����Ӧ����
void	ScCommand::MoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}

//����shift���µ�����ƶ�����Ӧ����
void	ScCommand::ShiftMoveEvent(const CPoint& point,
										const Handle(V3d_View)& aView)
{
}


//�����Ϣ��Ӧ����
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
// Undo����
//��ʼһ��undo���������
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

//��¼һ���´����Ķ���
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
//��¼һ��ɾ���Ķ���
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
//����undo��ύ���Ƿ���
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

//��AisCtx���Ƴ���ʱobject�����ͷ��ڴ档����ؼ����ͷ��ڴ档
void	ScCommand::RemoveCtxObj(Handle(AIS_InteractiveObject)& aObj,BOOL bUpdate)
{
	if(!aObj.IsNull())
	{
		m_AISContext->Remove(aObj,bUpdate);
		//�ͷ��ڴ棬����Ҫ.��Ϊ���aObj���������ʱ�����ý�һֱ���ڣ����ͷţ�
		//Ƶ��������ʱ���󣬻ᵼ���ڴ�й©��
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
		//���ĵ����ʾ
		//Handle (AIS_Drawer) aDrawer = aObj->Attributes();
		//aDrawer->PointAspect()->SetTypeOfMarker(Aspect_TOM_BALL);
		//m_AISContext->SetLocalAttributes(aObj,aDrawer,Standard_False);
		//��ʾ
		m_AISContext->Display(aObj,bUpdate);
	}
}

//��ʾһ�����󣬲���¼�ö������߿�ģʽ��ʾ����������ʾ�����Ƽ�ʹ�øú�����
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

//��ʾһ�����󣬲���¼�ö�������ɫģʽ��ʾ����������ʾ�����Ƽ�ʹ�øú�����
void	ScCommand::Display(Handle(AIS_InteractiveObject)& aObj,
							const TopoDS_Shape& aShape,
							COLORREF color,//��ʾ��ɫ
							BOOL bUpdate)
{
	if(!aShape.IsNull())
	{
		aObj = new AIS_Shape(aShape);
		//��ɫ��ʾ.
		m_AISContext->SetDisplayMode(aObj,1,Standard_False);
		//��ɫ
		Quantity_Color qcol(GetRValue(color)/255.0,GetGValue(color)/255.0,
			GetBValue(color)/255.0,Quantity_TOC_RGB);
		m_AISContext->SetColor(aObj,qcol,Standard_False);
		m_AISContext->Display(aObj,bUpdate);
	}
}


void	ScCommand::Display(const TopoDS_Shape& aShape)
{
}

//��ȡһ��ѡ���shape��ֻ����һ��
int		ScCommand::GetOneCurrentObj(Handle(AIS_InteractiveObject)& aObj)
{	
	//NbCurrents���ʼ��ѡ��
	int nCurs = m_AISContext->NbCurrents();
	if(nCurs <= 0)
		return SEL_NONE;

	if(nCurs > 1)
		return SEL_MORE;

	//����Ҫ��ʼ����
	m_AISContext->InitCurrent();	
	aObj = m_AISContext->Current();

	return SEL_OK;
}

//��ȡһ��ѡ���shape��ֻ����һ��
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

//��ȡ��ǰѡ���shape��
int		ScCommand::GetCurrentShape(TopoDS_Shape& aShape)
{
	//����Selected����LocalCtxʱѡ�����е�obj����û��ʱѡ��
	//��ǰ��obj��
	m_AISContext->InitSelected();
	if(m_AISContext->MoreSelected())
	{
		aShape = m_AISContext->SelectedShape();
		if(!aShape.IsNull())
			return SEL_OK;
	}

	return SEL_NONE;
}

//��ȡ�����shape
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

//���������ȡ����ǰ�����Face
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

//���������ȡ�����оݵ���������edge
TopoDS_Edge		ScCommand::GetNearestEdge(Handle(AIS_InteractiveObject)& aObj,
														int nX,int nY,double& t,gp_Pnt& pnt)
{
	TopoDS_Edge aEdge;
	if(aObj.IsNull())
		return aEdge;

	TopoDS_Shape aS = GetObjShape(aObj);
	if(aS.IsNull())
		return aEdge;

	double dist = 1e10;//������
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
// ��ȡ��Ӧ��obj�����û�У�����֮��
// ע�⣺seq��ΧΪ[1,len]
//
Handle(AIS_InteractiveObject)&	ScCommand::NewObj(int idx)
{
	int nLen = this->m_seqOfNewObj.Length();
	if(idx < 0)//�������������������µĶ���
	{
		idx = nLen;
	}
	if(nLen < idx + 1)
	{//��Ҫ�����µĶ���
		for(int ix = nLen + 1;ix <= idx + 1;ix ++)
		{
			Handle(AIS_InteractiveObject) aObj;//�����á�
			this->m_seqOfNewObj.Append(aObj);
		}
	}

	return this->m_seqOfNewObj.ChangeValue(idx + 1);
}

Handle(AIS_InteractiveObject)&	ScCommand::DelObj(int idx)
{
	int nLen = this->m_seqOfDelObj.Length();
	if(idx < 0)//�������������������µĶ���
	{
		idx = nLen;
	}
	if(nLen < idx + 1)
	{//��Ҫ�����µĶ���
		for(int ix = nLen + 1;ix <= idx + 1;ix ++)
		{
			Handle(AIS_InteractiveObject) aObj;//�����á�
			this->m_seqOfDelObj.Append(aObj);
		}
	}

	return this->m_seqOfDelObj.ChangeValue(idx + 1);
}

Handle(AIS_InteractiveObject)&	ScCommand::TmpObj(int idx)
{
	int nLen = this->m_seqOfTmpObj.Length();
	if(idx < 0)//�������������������µĶ���
	{
		idx = nLen;
	}
	if(nLen < idx + 1)
	{//��Ҫ�����µĶ���
		for(int ix = nLen + 1;ix <= idx + 1;ix ++)
		{
			Handle(AIS_InteractiveObject) aObj;//�����á�
			this->m_seqOfTmpObj.Append(aObj);
		}
	}

	return this->m_seqOfTmpObj.ChangeValue(idx + 1);
}

Handle(AIS_InteractiveObject)&	ScCommand::SelObj(int idx)
{
	int nLen = this->m_seqOfSelObj.Length();
	if(idx < 0)//�������������������µĶ���
	{
		idx = nLen;
	}
	if(nLen < idx + 1)
	{//��Ҫ�����µĶ���
		for(int ix = nLen + 1;ix <= idx + 1;ix ++)
		{
			Handle(AIS_InteractiveObject) aObj;//�����á�
			this->m_seqOfSelObj.Append(aObj);
		}
	}

	return this->m_seqOfSelObj.ChangeValue(idx + 1);
}


//���Ҷ�Ӧ��Obj.
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
// ��������´����Ķ���
//
int		ScCommand::RemoveNewObjs()
{
	if(this->m_seqOfNewObj.IsEmpty())
		return SC_OK;

	//����ʾ��ɾ��
	for(int ix = 1;ix <= this->m_seqOfNewObj.Length();ix ++)
	{
		m_AISContext->Remove(m_seqOfNewObj.Value(ix),Standard_False);
	}
	//
	this->m_seqOfNewObj.Clear();
	//����
	m_AISContext->UpdateCurrentViewer();

	return SC_OK;
}

//�ָ�����ɾ������
int		ScCommand::RestoreDelObjs()
{
	if(this->m_seqOfDelObj.IsEmpty())
		return SC_OK;

	//������ʾ.������ʹ��eraseɾ����remove ɾ���ģ�Ҫ���ֶԴ���
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
	//����
	m_AISContext->UpdateCurrentViewer();

	return SC_OK;
}

//���������ʱ����
int		ScCommand::ClearTmpObjs()
{
	if(this->m_seqOfTmpObj.IsEmpty())
		return SC_OK;

	//����ʾ��ɾ��
	for(int ix = 1;ix <= this->m_seqOfTmpObj.Length();ix ++)
	{
		//ֻ����ʾ�ģ����Ƴ�
		if(m_AISContext->IsDisplayed(m_seqOfTmpObj.Value(ix)))
			m_AISContext->Remove(m_seqOfTmpObj.Value(ix), Standard_False);

		m_seqOfTmpObj.ChangeValue(ix).Nullify();
	}
	//
	this->m_seqOfTmpObj.Clear();

	return SC_OK;
}

//�����¼��ѡ�ж�������ֻ�����reference��
int		ScCommand::ClearSelObjs()
{
	if(this->m_seqOfSelObj.IsEmpty())
		return SC_OK;

	//����ʾ��ɾ��
	for(int ix = 1;ix <= this->m_seqOfTmpObj.Length();ix ++)
	{
		//�ÿա�
		m_seqOfTmpObj.ChangeValue(ix).Nullify();
	}
	//
	this->m_seqOfTmpObj.Clear();

	return SC_OK;
}