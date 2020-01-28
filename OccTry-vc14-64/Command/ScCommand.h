#ifndef _OCCTRY_SC_COMMAND_H_
#define _OCCTRY_SC_COMMAND_H_

#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_SequenceOfInteractive.hxx>
#include <AIS_Shape.hxx>

#include "ScInput.h"

#define SC_OK		0 //成功
#define SC_ERR      -1 //失败

typedef double PNT3D[3] ; // point  type defined in 3 dimension
typedef double (*STR3D)[3] ; // pt array defined in 3 dimension

class ScCmdMgr;
class ScInputMgr;
class ScView;
class ScUcsMgr;
class ScSnapMgr;
class ScUndoMgr;
//class ScInputVar;
//class ScInputReq;

#define SC_INPUT_ERR    -1 //输入数据错误

enum{//输入消息的类型
	MINPUT_LEFT,//左键按下
	MINPUT_RIGHT,//右键按下
	MINPUT_LDBCLK,//左键双击
	MINPUT_RDBCLK,//右键双击
};

enum{
	SEL_OK,//正确选择对象
	SEL_NONE,//没有选中对象
	SEL_LESS,//少选
	SEL_MORE,//多选
};

class ScCommand
{
public:
	ScCommand();
	virtual ~ScCommand();

	//启动命令，开始执行。
	virtual	  int						Begin(Handle(AIS_InteractiveContext) aCtx);
	//结束命令，通常用于终止命令。
	virtual   int						End();

	virtual   void						Reset();

	virtual   BOOL						IsDone() const { return m_bDone; }
	virtual   void						Done();

	//Undo支持，Redo支持
	//是否支持undo。有些命令不支持。
	virtual	  BOOL						CanUndo() const { return m_bCanUndo; }
	virtual	  int						Undo();
	virtual   int						Redo();

	void								Prompt(LPCTSTR lpszText);

	virtual   void						SetState(int nState);
	virtual   int						GetState() const { return m_nState; }
	
	//屏幕像素点转换为模型空间的点
	virtual   gp_Pnt					PixelToModel(int x,int y,BOOL bWCS = TRUE);
	//屏幕点转换为cs的平面上的点
	virtual   gp_Pnt					PixelToCSPlane(Standard_Real x, Standard_Real y,BOOL bWCS = TRUE);

	//坐标转换函数
	int									WCSToUCS(gp_Pnt& pnt);
	int									UCSToWCS(gp_Pnt& pnt);
	int									WCSToUCS(gp_Vec& vec);
	int									UCSToWCS(gp_Vec& vec);

	void								NeedMultiSelect(BOOL bNeed);
	void								NeedSelect(BOOL bNeed);

	//自动捕捉
	void								NeedSnap(BOOL bEnable);
	void								SnapPoint(int nX,int nY,const Handle(V3d_View)& aView);
	gp_Pnt								GetSnapPoint(const gp_Pnt& pnt);

public:
	virtual   void						SetScCmdMgr(ScCmdMgr *pScCmdMgr) { this->m_pScCmdMgr = pScCmdMgr; }
	virtual   void						SetInputMgr(ScInputMgr *pInputMgr) { this->m_pInputMgr = pInputMgr; }
	virtual   void						SetScView(ScView *pScView) { this->m_pScView = pScView; }
	virtual   void						SetUcsMgr(ScUcsMgr *pUcsMgr) { m_pScUcsMgr = pUcsMgr; }
	virtual   void						SetSnapMgr(ScSnapMgr *pSnapMgr) { m_pSnapMgr = pSnapMgr; }
	virtual   void						SetUndoMgr(ScUndoMgr *pUndoMgr) { m_pUndoMgr = pUndoMgr; }

public:
	//输入相关函数，进行了封装,方便调用。
	//需要并开始接收输入
	virtual int							NeedInput(LPCTSTR lpszTitle,int nInputType,DWORD dwWaitTime = 0);
	//需要输入整数
	virtual int							NeedInteger(LPCTSTR lpszDesc);
	//需要输入浮点数
	virtual int							NeedDouble(LPCTSTR lpszDesc);
	//需要3维点
	virtual int							NeedPoint3d(LPCTSTR lpszDesc,BOOL bWCS = TRUE);
	//需要字符串
	virtual int							NeedString(LPCTSTR lpszDesc);
	//结束结束输入
	virtual int							EndInput();

	//显示信息函数
	virtual void						Show(LPCTSTR lpszVal);
	virtual void						Show(int nVal);
	virtual void						Show(double dVal);
	virtual void						Show(gp_Pnt pnt);

	//结束输入的主要函数。
	virtual   int						AcceptInput(const ScInputReq& inputReq,
													const ScInputVar& inputVar);
	//接收整数
	virtual   int						AcceptInput(int nVal) { return SC_OK; }
	//接收浮点数
	virtual   int						AcceptInput(double dVal) { return SC_OK; }
	//接收3维点
	virtual   int						AcceptInput(gp_Pnt& pnt) { return SC_OK; }
	//接收2维点
	virtual   int						AcceptInput(gp_Pnt2d& pnt) { return SC_OK; }
	//接收向量
	virtual   int						AcceptInput(gp_Vec& vec) { return SC_OK; }
	//接受字符串
	virtual	  int						AcceptInput(const CString& str) { return SC_OK; }

public:
	
	//点选方式下的消息响应函数
	virtual	  void						InputEvent(const CPoint& point,int nState,//上述minput定义
													const Handle(V3d_View)& aView);
	//鼠标移动的响应函数
	virtual	  void						MoveEvent(const CPoint& point,
													const Handle(V3d_View)& aView);

	//框选方式下的消息响应函数
	virtual	  void						DragEvent(const CPoint& point,int nMbState,
													const Handle(V3d_View)& aView);
		
	//按下shift键下的点选方式
	virtual	  void						ShiftInputEvent(const CPoint& point,
													const Handle(V3d_View)& aView);

	//按下shift键下的鼠标移动的响应函数
	virtual	  void						ShiftMoveEvent(const CPoint& point,
													const Handle(V3d_View)& aView);
	//按下shift下的框选方式
	virtual	  void						ShiftDragEvent(const CPoint& point,int nMbState,
													const Handle(V3d_View)& aView);

public:
	//鼠标消息响应函数
	virtual void						OnLButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void						OnLButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void						OnMouseMove(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void						OnLButtonDblClk(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void						OnRButtonDown(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);
	virtual void						OnRButtonUp(UINT nFlags, CPoint point,const Handle(V3d_View)& aView);

protected://下面是用于undo的函数
		//开始一个undo项，给出名称
		virtual		int						BeginUndo(LPCTSTR lpszName);
		//记录一个新创建的对象
		virtual      int						UndoAddNewObj(const Handle(AIS_InteractiveObject)& aObj);
		//记录一个删除的对象
		virtual      int						UndoAddDelObj(const Handle(AIS_InteractiveObject)& aObj);
		//结束undo项，提交还是放弃
		virtual     int						EndUndo(BOOL bCommit);

		BOOL								m_bBeginUndo;//是否开始Undo。

protected:
	//从AisCtx中移出临时object，并释放内存。这里关键是释放内存。
	virtual void						RemoveCtxObj(Handle(AIS_InteractiveObject)& aObj,BOOL bUpdate = FALSE);
	virtual	void						SwitchState(int nState,LPCTSTR lpszPrompt,
													BOOL bNeedModelPoint = FALSE);

	//显示一个对象，并记录该对象。以着色模式显示。命令中显示对象，推荐使用该函数。
	virtual void						Display(Handle(AIS_InteractiveObject)& aObj,
												const TopoDS_Shape& aShape,
												BOOL bUpdate = TRUE);

	//显示一个对象，并记录该对象。以着色模式显示。命令中显示对象，推荐使用该函数。
	virtual void						Display(Handle(AIS_InteractiveObject)& aObj,
												const TopoDS_Shape& aShape,
												COLORREF color,//显示颜色
												BOOL bUpdate = TRUE);

	//显示一个对象，并记录该对象。以线框模式显示。命令中显示对象，推荐使用该函数。
	virtual void						WFDisplay(Handle(AIS_InteractiveObject)& aObj,
													const TopoDS_Shape& aShape,
													BOOL bUpdate = TRUE);

	virtual void						Display(const TopoDS_Shape& aShape);
	//获取一个选择的shape，只能是一个
	virtual int							GetOneCurrentObj(Handle(AIS_InteractiveObject)& aObj);
	//获取一个选择的shape，只能是一个
	virtual int							GetOneSelShape(TopoDS_Shape& aShape);
	//获取当前选择的shape。
	virtual int							GetCurrentShape(TopoDS_Shape& aShape);

	//获取对象的shape
	virtual TopoDS_Shape				GetObjShape(Handle(AIS_InteractiveObject)& aObj);

	//根据鼠标点获取对象当前点击的Face
	virtual TopoDS_Face					GetObjPickedFace(Handle(AIS_InteractiveObject)& aObj,
														int nX,int nY,const Handle(V3d_View)& aView);
	
	//根据鼠标点获取对象中据点击点最近的edge
	virtual TopoDS_Edge					GetNearestEdge(Handle(AIS_InteractiveObject)& aObj,
														int nX,int nY,double& t,gp_Pnt& pnt);

protected:
	//对对象选择的一层封装
	void								InitSelect();
	BOOL								MoreSelect();
	void								NextSelect();
	void								ClearSelect();
	TopoDS_Shape						SelectedShape(BOOL bNext = TRUE);
	Handle(AIS_InteractiveObject)		SelectedObj(BOOL bNext = TRUE);

protected:
	Handle(AIS_InteractiveContext)		m_AISContext;
	Handle(V3d_View)					m_View;

	BOOL								m_bDone;//是否执行完成
	ScCmdMgr							*m_pScCmdMgr; //管理器
	ScInputMgr							*m_pInputMgr;//输入管理
	ScView								*m_pScView;//
	ScUcsMgr							*m_pScUcsMgr;
	ScSnapMgr							*m_pSnapMgr;
	ScUndoMgr						*m_pUndoMgr;

	int									m_nState; //当前的状态。每个命令都有不同的状态
	BOOL								m_bCanUndo; //是否可以undo。

protected://对象管理
	//说明：下面seq中记录的都是obj的handle，即引用，如果使用完了，handle不置NULL，可能会
	//      导致handle引用的对象不会被释放，因此使用要仔细。

	//新创建的obj保存在这里.
	AIS_SequenceOfInteractive			m_seqOfNewObj;
	//删除的obj保存在这里,必须保存已经删除的对象，准备删除的对象不要放在这里。
	AIS_SequenceOfInteractive			m_seqOfDelObj;
	//临时显示的obj保存在这里.这些对象最终要删除掉。
	AIS_SequenceOfInteractive			m_seqOfTmpObj;
	//记录选中的对象。如果不使用这里的对象，可以自己在具体command中定义变量。
	AIS_SequenceOfInteractive			m_seqOfSelObj;

	//获取对应的obj，如果没有，创建之。idx 为-1或超出列表范围,则添加新对象到列表中。
	Handle(AIS_InteractiveObject)&		NewObj(int idx);
	Handle(AIS_InteractiveObject)&		DelObj(int idx);
	Handle(AIS_InteractiveObject)&		TmpObj(int idx);
	Handle(AIS_InteractiveObject)&		SelObj(int idx);

	//查找对应的Obj.
	int									FindNewObj(const TopoDS_Shape& aShape);
	void								RemoveNewObj(int idx);
											
	//清除所有新创建的对象。
	virtual		int						RemoveNewObjs();
	//恢复所有删除对象
	virtual		int						RestoreDelObjs();
	//清除所有临时对象
	virtual		int						ClearTmpObjs();
	//清除记录的选中对象，这里只是清除reference。
	virtual		int						ClearSelObjs();

private:
	BOOL								m_bNeedInput;//是否需要输入
	DWORD								m_dwOldTime;
};

inline void	ScCommand::SetState(int nState)
{
	m_nState = nState;
}

#endif