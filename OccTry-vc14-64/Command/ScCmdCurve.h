#pragma once

#include <TColgp_SequenceOfPnt.hxx>
#include <AIS_SequenceOfInteractive.hxx>
#include <AIS_InteractiveObject.hxx>
#include "ScCommand.h"
#include "ScCmdFaceBase.h"
#include "ScCmdCurveBase.h"

class ScCmdCurve 
{
public:
	ScCmdCurve(void);
	~ScCmdCurve(void);
};



///////////////////////////////////////////////////////
// 曲线组合
class ScCmdCurveSewing : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_CURVES};

	virtual TopoDS_Shape		BuildNewShape();

protected:
	AIS_SequenceOfInteractive	m_seqOfObj;
	Handle(AIS_Shape)			m_aNewObj;
};

//////////////////////////////////////////////////////////////////////////
// 曲线取消组合
class ScCmdCurveWireToEdge : public ScCmdCurveBase{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

protected:
	BOOL					DoWireToEdge();
};

/////////////////////////////////////////////////////////////
// 曲线到实体的投影
class ScCmdBrepProjection : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_CURVE,S_SHAPE};

	TopoDS_Shape			BuildNewShape();

protected:
	Handle(AIS_InteractiveObject) m_aCurveObj;
	TopoDS_Shape				m_aShape;
	TopoDS_Shape				m_aProjShape;
	Handle(AIS_Shape)			m_aNewObj;
};

////
// 交线
class ScCmdInterCurve : public ScCommand{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_SHAPE1,S_SHAPE2};

	void					BuildInterCurve();

protected:
	TopoDS_Shape			m_aShape1,
							m_aShape2;

};

////////////////////////////////////////////////
// 点生成
class ScCmdPoint :public ScCommand{
public:
	ScCmdPoint(BOOL bPlane = FALSE);

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	BOOL					m_bPlanePoint;//是否平面点
	int						m_nIdx;
};

/////////////////////////////////////////////////////
// 延伸到点
class ScCmdExtendToPoint : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//接收函数
	//接收整数
	virtual   int			AcceptInput(int nVal);

protected:
	enum{S_CURVE,S_POINT,S_DIR,S_CONT};

	gp_Pnt					m_extPnt;//要延伸到的点
	BOOL					m_bAfter;//延伸前端还是后端
};

//////////////////////////////////////////////////////////
// 偏移曲线,使用BRepOffsetAPI_MakeOffset类。
class ScCmdCurveOffset : public ScCommand{
public:
	
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

	//接收函数
	//接收半径
	virtual   int			AcceptInput(double dVal);

protected:
	enum{S_CURVE,S_OFFVAL};
};

/////////////////////////////////////////////////////////////
// 简单基类,主要提供共用方法。
class ScCmdOneFace : public ScCommand{
public:

protected:
	//是否选择了face，结果保存在tmpObj(0)中。
	BOOL				HasSelFace();
};

/////////////////////////////////////////////////////////////
// face的边界
class ScCmdCurveFaceBoundary : public ScCmdOneFace{
public:
	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
										const Handle(V3d_View)& aView);

protected:
	enum{S_FACE,S_CURVE};

	int						m_nNewCurves;

};

//Face的全部3d边界
class ScCmdFaceBoundary3d : public ScCmdFaceBase{
public:

protected:
	virtual BOOL			BuildShape();//生成对应shape。
	virtual BOOL			BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace);
};

//Face的全部参数曲线生成边界
class ScCmdFaceBoundary2d : public ScCmdFaceBase{
public:

protected:
	virtual BOOL			BuildShape();//生成对应shape。
	virtual BOOL			BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace);
};

//Face的全部裁剪环的参数曲线
class ScCmdFaceParamLoop : public ScCmdFaceBase{

protected:
	virtual BOOL			BuildShape();//生成对应shape。
	virtual BOOL			BuildCurve(const TopoDS_Edge& aE,const TopoDS_Face& aFace);
};

//////////////////////////////////////////////////////////////////////////
// 曲线间的交点 BRepExtram_DistShapeShape类
class ScCmdCurveInterPoint : public ScCommand{
public:
		//启动和结束命令
		virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
		virtual   int			End();

		//点选方式下的消息响应函数
		virtual	  void			InputEvent(const CPoint& point,int nState,
				const Handle(V3d_View)& aView);

protected:
		enum{S_CURVE1,S_CURVE2};

		BOOL						HasSelCurve(BOOL bFirst);
		BOOL						DoInter();
};

//////////////////////////////////////////////////////////////////////////
// 曲线活动标架的显式
class ScCmdCurveFrameBase : public ScCommand{
public:

	//启动和结束命令
	virtual	  int			Begin(Handle(AIS_InteractiveContext) aCtx);
	virtual   int			End();

	//点选方式下的消息响应函数
	virtual	  void			InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView);

protected:
	enum{S_CURVE,S_FRAME};

	BOOL					HasSelCurve();
	virtual BOOL			BuildFrame();

	virtual	BOOL			CalcFrame(double t,gp_Dir& T,gp_Dir& N,gp_Dir& B) { return FALSE; }

	Handle(Geom_Curve)		m_aSelCurve;
	TopoDS_Shape			m_aC;//显式frame的对象
	double					m_df,m_dl;
};

//Frenet标架
class ScCmdCurveFrenetFrame : public ScCmdCurveFrameBase{
public:

protected:
	virtual	BOOL			CalcFrame(double t,gp_Dir& T,gp_Dir& N,gp_Dir& B);
};

//旋转标架1
class ScCmdCurveRotateFrame1 : public ScCmdCurveFrameBase{

protected:
	virtual BOOL			BuildFrame();
};

//旋转标架2
class ScCmdCurveRotateFrame2 : public ScCmdCurveFrameBase{

protected:
	virtual BOOL			BuildFrame();
};

//sloan标架
class ScCmdCurveSloanFrame : public ScCmdCurveFrameBase{

protected:
	virtual BOOL			BuildFrame();
};


//双反射标架
class ScCmdCurveDblRefFrame : public ScCmdCurveFrameBase{

protected:
	virtual BOOL			BuildFrame();
};
