#include "StdAfx.h"

#include <MeshVS_Mesh.hxx>
//#include <StlMesh_Mesh.hxx>//yxk20200104
//#include <XSDRAWSTLVRML_DataSource.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>

#include "ScMesh_DataSource.hxx"
#include ".\sccmdmeshimport.h"

ScCmdMeshImport::ScCmdMeshImport(void)
{
}

ScCmdMeshImport::~ScCmdMeshImport(void)
{
}

ScCmdMeshImportMs3d::ScCmdMeshImportMs3d()
{

}

//启动和结束命令
int	ScCmdMeshImportMs3d::Begin(Handle(AIS_InteractiveContext) aCtx)
{
	ScCommand::Begin(aCtx);

	//简单的测试，构造一个三棱椎。
	Handle(StlMesh_Mesh) aMesh = new StlMesh_Mesh;
	aMesh->AddDomain();
	int v1,v2,v3,v4;
	v1 = aMesh->AddVertex(0,0,0);//0
	v2 = aMesh->AddVertex(20,0,0);//1
	v3 = aMesh->AddVertex(0,50,0);//2
	v4 = aMesh->AddVertex(0,0,20);//3

	aMesh->AddTriangle(v1,v3,v2,0,0,-1);
	aMesh->AddTriangle(v1,v4,v3,-1,0,0);
	aMesh->AddTriangle(v1,v2,v4,0,-1,0);
	aMesh->AddTriangle(v2,v3,v4,1,1,1);

	Handle(ScMesh_DataSource) aDS = new ScMesh_DataSource(aMesh);
	Handle(MeshVS_Mesh) aVM = new MeshVS_Mesh;
	Handle(MeshVS_MeshPrsBuilder) mB = new MeshVS_MeshPrsBuilder(aVM,MeshVS_DMF_OCCMask,aDS);
	aVM->SetDataSource(aDS);
	aVM->AddBuilder(mB, Standard_True);

	m_AISContext->Display(aVM);

	End();

	return 0;
}

int	ScCmdMeshImportMs3d::End()
{
	return ScCommand::End();
}

//点选方式下的消息响应函数
void	ScCmdMeshImportMs3d::InputEvent(const CPoint& point,int nState,
		const Handle(V3d_View)& aView)
{

}
