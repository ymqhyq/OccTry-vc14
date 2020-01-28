#include "StdAfx.h"

#include <BRepAlgo_Common.hxx>
#include <BRepAlgo_Fuse.hxx>
#include <BRepAlgo_Cut.hxx>

#include ".\sccmdboolean.h"

ScCmdBoolean::ScCmdBoolean(void)
{
}

ScCmdBoolean::~ScCmdBoolean(void)
{
}

//////////////////////////////////////////////////////////////////////////
//
ScCmdBool::ScCmdBool()
{
}

//启动和结束命令
int	ScCmdBool::Begin(Handle(AIS_InteractiveContext) aCtx)
{
		ScCommand::Begin(aCtx);

		this->NeedMultiSelect(TRUE);

		if(HasSelObj1())
		{
				SwitchState(S_SHAPE2,"选择第二个对象:");
		}
		else
		{
				SwitchState(S_SHAPE1,"选择第一个对象:");
		}

		return 0;
}

int	ScCmdBool::End()
{
		this->ClearSelObjs();
		return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdBool::InputEvent(const CPoint& point,int nState,
							  const Handle(V3d_View)& aView)
{
		if(nState == MINPUT_RIGHT)
		{
				End();
		}
		else
		{
				//	m_AISContext->ShiftSelect();//多选
				switch(GetState())
				{
				case S_SHAPE1:
						{
								if(HasSelObj1())
								{
										SwitchState(S_SHAPE2,"选择第二个对象:");
								}
						}
						break;
				case S_SHAPE2:
						{
								if(HasSelObj2())
								{
										MakeBoolean();
								}
						}
						break;
				default:
						break;
				}
		}
}

//不可选点和edge和compound compsolid.
BOOL		ScCmdBool::HasSelObj1()
{
		InitSelect();
		if(MoreSelect())
		{
				TopoDS_Shape aSS = SelectedShape(FALSE);
				if(!aSS.IsNull() && ( aSS.ShapeType() != TopAbs_VERTEX &&
						aSS.ShapeType() != TopAbs_EDGE && aSS.ShapeType() != TopAbs_COMPOUND
						&& aSS.ShapeType() != TopAbs_COMPSOLID))
				{
						SelObj(0) = SelectedObj(FALSE);

						return TRUE;
				}
		}

		return FALSE;
}

BOOL		ScCmdBool::HasSelObj2()
{
		InitSelect();
		while(MoreSelect())
		{
				if(SelObj(0) != SelectedObj(FALSE))
				{
						TopoDS_Shape aSS = SelectedShape(FALSE);
						if(!aSS.IsNull() && ( aSS.ShapeType() != TopAbs_VERTEX &&
								aSS.ShapeType() != TopAbs_EDGE && aSS.ShapeType() != TopAbs_COMPOUND
								&& aSS.ShapeType() != TopAbs_COMPSOLID))
						{
								SelObj(1) = SelectedObj(FALSE);

								return TRUE;
						}
				}

				NextSelect();
		}

		return FALSE;
}

void	ScCmdBool::MakeBoolean()
{
		//进行boolean运算，删除旧的，并生成两个新的。
		TopoDS_Shape aS1 = GetObjShape(SelObj(0));
		TopoDS_Shape aS2 = GetObjShape(SelObj(1));
		
		TopoDS_Shape aNewShape = BuildNewShape(aS1,aS2);

		if(!aNewShape.IsNull())
		{
				m_AISContext->Remove(SelObj(0),FALSE);
				m_AISContext->Remove(SelObj(1),FALSE);
				Display(NewObj(0),aNewShape);

				Done();
		}
		else
		{
				this->ClearSelObjs();
				End();
		}
}

TopoDS_Shape  ScCmdBool::BuildNewShape(const TopoDS_Shape& aS1,
									   const TopoDS_Shape& aS2)
{
		TopoDS_Shape aS;
		return aS;
}

static TopoDS_Shape FixShape(const TopoDS_Shape& aS)
{
		TopoDS_Shape aFs;
		Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape;
		sfs->Init ( aS );

		sfs->SetPrecision(0.01);
		sfs->Perform();
		aFs = sfs->Shape();

		return aFs;
}

TopoDS_Shape  ScCmdBoolCommon::BuildNewShape(const TopoDS_Shape& aS1,
											 const TopoDS_Shape& aS2)
{
		TopoDS_Shape aS;
		try{
				DWORD dwBegin = ::GetTickCount();
				aS = BRepAlgoAPI_Common(aS1,aS2);
				//aS = BRepAlgo_Common(FixShape(m_aShape1),FixShape(m_aShape2));
				DTRACE("\n command use %d ms.",GetTickCount() - dwBegin);
				Show((double)GetTickCount() - dwBegin);
				CString szMsg;
				szMsg.Format("\n command use %d ms.",GetTickCount() - dwBegin);
				AfxMessageBox(szMsg);
		}catch(Standard_Failure)
		{
				AfxMessageBox("交运算失败.");
		}
		return aS;
}

TopoDS_Shape  ScCmdBoolFuse::BuildNewShape(const TopoDS_Shape& aS1,
										   const TopoDS_Shape& aS2)
{
		TopoDS_Shape aS;
		try{
				aS = BRepAlgoAPI_Fuse(aS1,aS2);
		}catch(Standard_Failure)
		{
				AfxMessageBox("并运算失败.");
		}
		return aS;
}

TopoDS_Shape  ScCmdBoolCut::BuildNewShape(const TopoDS_Shape& aS1,
										  const TopoDS_Shape& aS2)
{
		TopoDS_Shape aS;
		try{
				aS = BRepAlgoAPI_Cut(aS1,aS2);
		}catch(Standard_Failure)
		{
				AfxMessageBox("差运算失败.");
		}
		return aS;
}

TopoDS_Shape  ScCmdBoolCommonOld::BuildNewShape(const TopoDS_Shape& aS1,
											 const TopoDS_Shape& aS2)
{
		TopoDS_Shape aS;
		try{
				DWORD dwBegin = ::GetTickCount();
				aS = BRepAlgo_Common(aS1,aS2);
				//aS = BRepAlgo_Common(FixShape(m_aShape1),FixShape(m_aShape2));
				DTRACE("\n command use %d ms.",GetTickCount() - dwBegin);
				Show((double)GetTickCount() - dwBegin);
				CString szMsg;
				szMsg.Format("\n command use %d ms.",GetTickCount() - dwBegin);
				AfxMessageBox(szMsg);
		}catch(Standard_Failure)
		{
				AfxMessageBox("交运算失败.");
		}
		return aS;
}

TopoDS_Shape  ScCmdBoolFuseOld::BuildNewShape(const TopoDS_Shape& aS1,
										   const TopoDS_Shape& aS2)
{
		TopoDS_Shape aS;
		try{
				aS = BRepAlgo_Fuse(aS1,aS2);
		}catch(Standard_Failure)
		{
				AfxMessageBox("并运算失败.");
		}
		return aS;
}

TopoDS_Shape  ScCmdBoolCutOld::BuildNewShape(const TopoDS_Shape& aS1,
										  const TopoDS_Shape& aS2)
{
		TopoDS_Shape aS;
		try{
				aS = BRepAlgo_Cut(aS1,aS2);
		}catch(Standard_Failure)
		{
				AfxMessageBox("差运算失败.");
		}
		return aS;
}

//
 void		ScCmdBoolCommonSections::MakeBoolean()
 {
		 //进行boolean运算，删除旧的，并生成两个新的。
		 TopoDS_Shape aS1 = GetObjShape(SelObj(0));
		 TopoDS_Shape aS2 = GetObjShape(SelObj(1));

		 try{
				 BRepAlgoAPI_Common  bcomm(aS1,aS2);
				 //获取交线列表
				 TopoDS_Compound comp;
				 BRep_Builder  BB;
				 BB.MakeCompound(comp);

				const TopTools_ListOfShape& shapeList = bcomm.SectionEdges();
				TopTools_ListIteratorOfListOfShape it(shapeList);
				for ( ; it.More(); it.Next()) 
				{
						BB.Add(comp,it.Value());
				}

				Display(NewObj(0),comp);

				Done();
		 }catch(Standard_Failure)
		 {
				 AfxMessageBox("交运算失败.");
		 }
 }


ScCmdBoolCommonGenObjs::ScCmdBoolCommonGenObjs(BOOL bFirst)
{
		m_bFirst = bFirst;
}

 void			ScCmdBoolCommonGenObjs::MakeBoolean()
 {
		 //进行boolean运算，删除旧的，并生成两个新的。
		 TopoDS_Shape aS1 = GetObjShape(SelObj(0));
		 TopoDS_Shape aS2 = GetObjShape(SelObj(1));

		 try{
				 BRepAlgoAPI_Common  bcomm(aS1,aS2);
				 //获取交线列表
				 TopoDS_Compound comp;
				 BRep_Builder  BB;
				 BB.MakeCompound(comp);

				 TopoDS_Shape aS = aS1;
				 if(!m_bFirst)
						 aS = aS2;
				 const TopTools_ListOfShape& shapeList = bcomm.Generated(aS);
				 TopTools_ListIteratorOfListOfShape it(shapeList);
				 for ( ; it.More(); it.Next()) 
				 {
						 BB.Add(comp,it.Value());
				 }

				 Display(NewObj(0),comp);

				 Done();
		 }catch(Standard_Failure)
		 {
				 AfxMessageBox("交运算失败.");
		 }
 }
