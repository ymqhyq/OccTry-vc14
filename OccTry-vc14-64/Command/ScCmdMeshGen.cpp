#include "StdAfx.h"

#include <Precision.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <MeshVS_ElementalColorPrsBuilder.hxx>
//yxk20191208
//#include <StlMesh_Mesh.hxx>
//#include <StlTransfer.hxx>
//endyxk
#include "ScMesh_DataSource.hxx"

#include "ScBRepLib.h"
#include ".\sccmdmeshgen.h"

ScCmdMeshGen::ScCmdMeshGen(void)
{
}

ScCmdMeshGen::~ScCmdMeshGen(void)
{
}

//启动和结束命令
int	ScCmdMeshConvertToMesh::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	if(HasSelObj())
	{
		if(DoConvert())
			Done();
		else
			End();
	}
	else
	{
		NeedSelect(TRUE);
		Prompt("选择一个对象(非曲线):");
	}

	return 0;
}

int	ScCmdMeshConvertToMesh::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdMeshConvertToMesh::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{
	if(nState == MINPUT_RIGHT)
	{
		End();
	}
	else
	{
		if(HasSelObj())
		{
			if(DoConvert())
				Done();
			else
				End();
		}
	}
}


BOOL	ScCmdMeshConvertToMesh::HasSelObj()
{
	InitSelect();
	if(MoreSelect())
	{
		TopoDS_Shape aS = SelectedShape(FALSE);
		if(ScBRepLib::IsSolid(aS) || ScBRepLib::IsShell(aS) || ScBRepLib::IsFace(aS))
		{
			SelObj(0) = SelectedObj();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL	ScCmdMeshConvertToMesh::DoConvert()
{
	TopoDS_Shape aShape = GetObjShape(SelObj(0));
	
	Handle(StlMesh_Mesh) aMesh = new StlMesh_Mesh;
	aMesh->AddDomain();//使用一个domain。

	MeshVS_DataMapOfIntegerColor colmap;
	//构造网格对象
	TopExp_Explorer exFace;
	for(exFace.Init(aShape,TopAbs_FACE);exFace.More();exFace.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(exFace.Current());
		TopLoc_Location aL;

		Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace,aL);

		if(!aTr.IsNull())
		{
			const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
			const Poly_Array1OfTriangle& aTris = aTr->Triangles();

			//aMesh->AddDomain();//使用多个domain。

			int nTr = aTr->NbTriangles();
			int n1,n2,n3;
			gp_Pnt p1,p2,p3;

			//遍历三角形
			for(int ix = 1;ix <= nTr;ix ++)
			{
				aTris(ix).Get(n1,n2,n3);
				//获取点
				p1 = aNodes.Value(n1).Transformed(aL);
				p2 = aNodes.Value(n2).Transformed(aL);
				p3 = aNodes.Value(n3).Transformed(aL);
				
				if(p1.IsEqual(p2,Precision::Confusion()) || 
					p2.IsEqual(p3,Precision::Confusion()) ||
					p3.IsEqual(p1,Precision::Confusion()))
					continue;//退化

				//计算面片矢量
				gp_Vec v1(p2.X() - p1.X(),p2.Y() - p1.Y(),p2.Z() - p1.Z());
				gp_Vec v2(p3.X() - p1.X(),p3.Y() - p1.Y(),p3.Z() - p1.Z());
				gp_Vec vn = v1.Crossed(v2);
				if(vn.SquareMagnitude() < Precision::Confusion())
					continue;
				vn.Normalize();
			
				//添加
                n1 = aMesh->AddVertex(p1.X(),p1.Y(),p1.Z());
				n2 = aMesh->AddVertex(p2.X(),p2.Y(),p2.Z());
				n3 = aMesh->AddVertex(p3.X(),p3.Y(),p3.Z());
				colmap.Bind(n1,Quantity_Color(Quantity_NOC_GOLD));
				colmap.Bind(n2,Quantity_Color(Quantity_NOC_GOLD));
				colmap.Bind(n3,Quantity_Color(Quantity_NOC_GOLD));

				aMesh->AddTriangle(n1,n2,n3,vn.X(),vn.Y(),vn.Z());
			}//end for
		}//end if
	}//end for	*/

	//使用现成的函数，处理更完善,不好，为什么？
	//StlTransfer::BuildIncrementalMesh(aShape,0.01,aMesh);

	//显示
	Handle(ScMesh_DataSource) aDS = new ScMesh_DataSource(aMesh);
	Handle(MeshVS_Mesh) aVM = new MeshVS_Mesh;
	Handle(MeshVS_MeshPrsBuilder) mB = new MeshVS_MeshPrsBuilder(aVM,MeshVS_DMF_OCCMask,aDS);
//	Handle(MeshVS_ElementalColorPrsBuilder) aHB = new MeshVS_ElementalColorPrsBuilder(aVM);
//	aHB->SetColors1(colmap);
	aVM->SetDataSource(aDS);
	aVM->AddBuilder(mB, Standard_True);
	//aVM->SetHilighter(aHB);
	aVM->SetColor(Quantity_NOC_GRAY);
	aVM->SetHilighter(Quantity_NOC_GOLD);

	m_AISContext->Remove(SelObj(0),Standard_False);
	m_AISContext->Display(aVM);
	
	BeginUndo("Convert to Mesh");
	UndoAddNewObj(aVM);
	UndoAddDelObj(SelObj(0));
	EndUndo(TRUE);

	return TRUE;
}
