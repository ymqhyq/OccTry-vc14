// File:	ScMesh_DataSource.cxx
// Created:	Thu June 10 2004
// Author:	Alexander SOLOVYOV
// Copyright:	 Open CASCADE 2004

#include "StdAfx.h"

#include <MeshVS_EntityType.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlMesh_SequenceOfMeshTriangle.hxx>
#include <StlMesh_MeshTriangle.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TColStd_DataMapOfIntegerReal.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>

#include "ScMesh_DataSource.hxx"
//================================================================
// Function : Constructor
// Purpose  :
//================================================================
ScMesh_DataSource::ScMesh_DataSource( const Handle( StlMesh_Mesh )& aMesh )
{
  myMesh = aMesh;

  if( !myMesh.IsNull() )
  {
    const TColgp_SequenceOfXYZ& aCoords = myMesh->Vertices();
    Standard_Integer len = aCoords.Length(), i, j;
    myNodeCoords = new TColStd_HArray2OfReal(1, len, 1, 3);
    cout << "Nodes : " << len << endl;

    gp_XYZ xyz;

    for( i = 1; i <= len; i++ )
    {
      myNodes.Add( i );
      xyz = aCoords(i);

      myNodeCoords->SetValue(i, 1, xyz.X());
      myNodeCoords->SetValue(i, 2, xyz.Y());
      myNodeCoords->SetValue(i, 3, xyz.Z());
    }

    const StlMesh_SequenceOfMeshTriangle& aSeq = myMesh->Triangles();
    len = aSeq.Length();
    myElemNormals = new TColStd_HArray2OfReal(1, len, 1, 3);
    myElemNodes = new TColStd_HArray2OfInteger(1, len, 1, 3);
    
    cout << "Elements : " << len << endl;
    
    for( i = 1; i <= len; i++ )
    {
      myElements.Add( i );
      Handle( StlMesh_MeshTriangle ) aTriangle = aSeq.Value( i );
      Standard_Integer V[3]; Standard_Real nx, ny, nz;
      
      aTriangle->GetVertexAndOrientation( V[0], V[1], V[2], nx, ny, nz );
      
      for( j = 0; j < 3; j++ )
      {
	myElemNodes->SetValue(i, j+1, V[j]);
      }
      
      myElemNormals->SetValue(i, 1, nx);
      myElemNormals->SetValue(i, 2, ny);
      myElemNormals->SetValue(i, 3, nz);
    }
  }
  cout << "Construction is finished" << endl;
}

//================================================================
// Function : GetGeom
// Purpose  :
//================================================================
Standard_Boolean ScMesh_DataSource::GetGeom
   ( const Standard_Integer ID, const Standard_Boolean IsElement,
    TColStd_Array1OfReal& Coords, Standard_Integer& NbNodes,
    MeshVS_EntityType& Type ) const
{
  if( myMesh.IsNull() )
    return Standard_False;

  if( IsElement )
  {
    if( ID>=1 && ID<=myElements.Extent() )
    {
      Type = MeshVS_ET_Face;
      NbNodes = 3;
      
      for( Standard_Integer i = 1, k = 1; i <= 3; i++ )
      {
	Standard_Integer IdxNode = myElemNodes->Value(ID, i);
	for(Standard_Integer j = 1; j <= 3; j++, k++ )
	  Coords(k) = myNodeCoords->Value(IdxNode, j);
      }
      
      return Standard_True;
    }
    else
      return Standard_False;
  }
  else
    if( ID>=1 && ID<=myNodes.Extent() )
    {
      Type = MeshVS_ET_Node;
      NbNodes = 1;
      
      Coords( 1 ) = myNodeCoords->Value(ID, 1);
      Coords( 2 ) = myNodeCoords->Value(ID, 2);
      Coords( 3 ) = myNodeCoords->Value(ID, 3);
      return Standard_True;
    }
    else
      return Standard_False;
}

//================================================================
// Function : GetGeomType
// Purpose  :
//================================================================
Standard_Boolean ScMesh_DataSource::GetGeomType
	( const Standard_Integer,
	  const Standard_Boolean IsElement,
	  MeshVS_EntityType& Type ) const
{
  if( IsElement )
  {
    Type = MeshVS_ET_Face;
    return Standard_True;
  }
  else
  {
    Type = MeshVS_ET_Node;
    return Standard_True;
  }
}

//================================================================
// Function : GetAddr
// Purpose  :
//================================================================
Standard_Address ScMesh_DataSource::GetAddr
	( const Standard_Integer, const Standard_Boolean ) const
{
  return NULL;
}

//================================================================
// Function : GetNodesByElement
// Purpose  :
//================================================================
Handle(TColStd_HArray1OfInteger) ScMesh_DataSource::GetNodesByElement
	( const Standard_Integer ID ) const
{
  if( myMesh.IsNull() )
    return 0;
  
  if( ID>=1 && ID<=myElements.Extent() )
  {
    Handle(TColStd_HArray1OfInteger) anArr = new TColStd_HArray1OfInteger (1, 3);
    anArr->SetValue (1, myElemNodes->Value(ID, 1 ) );
    anArr->SetValue (2, myElemNodes->Value(ID, 2 ) );
    anArr->SetValue (3, myElemNodes->Value(ID, 3 ) );
    return anArr;
  }
  return 0;
}

//================================================================
// Function : GetAllNodes
// Purpose  :
//================================================================
const TColStd_PackedMapOfInteger& ScMesh_DataSource::GetAllNodes() const
{
  return myNodes;
}

//================================================================
// Function : GetAllElements
// Purpose  :
//================================================================
const TColStd_PackedMapOfInteger& ScMesh_DataSource::GetAllElements() const
{
  return myElements;
}

//================================================================
// Function : GetNormal
// Purpose  :
//================================================================
Standard_Boolean ScMesh_DataSource::GetNormal
	( const Standard_Integer Id, const Standard_Integer Max,
	  Standard_Real& nx, Standard_Real& ny,Standard_Real& nz ) const
{
  if( myMesh.IsNull() )
    return Standard_False;
  
  if( Id>=1 && Id<=myElements.Extent() && Max>=3 )
  {
    nx = myElemNormals->Value(Id, 1);
    ny = myElemNormals->Value(Id, 2);
    nz = myElemNormals->Value(Id, 3);
    return Standard_True;
  }
  else
    return Standard_False;
}

 Handle_Standard_Type& ScMesh_DataSource_Type_()
{

	static Handle_Standard_Type aType1 = STANDARD_TYPE(MeshVS_DataSource);
	static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
	static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);


	static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
	static Handle_Standard_Type _aType = new Standard_Type("ScMesh_DataSource",
		sizeof(ScMesh_DataSource),
		1,
		(Standard_Address)_Ancestors,
		(Standard_Address)NULL);

	return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(ScMesh_DataSource) Handle(ScMesh_DataSource)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
	Handle(ScMesh_DataSource) _anOtherObject;

	if (!AnObject.IsNull()) {
		if (AnObject->IsKind(STANDARD_TYPE(ScMesh_DataSource))) {
			_anOtherObject = Handle(ScMesh_DataSource)((Handle(ScMesh_DataSource)&)AnObject);
		}
	}

	return _anOtherObject ;
}
const Handle(Standard_Type)& ScMesh_DataSource::DynamicType() const 
{ 
	return STANDARD_TYPE(ScMesh_DataSource) ; 
}
//Standard_Boolean ScMesh_DataSource::IsKind(const Handle(Standard_Type)& AType) const 
//{ 
//  return (STANDARD_TYPE(ScMesh_DataSource) == AType || MeshVS_DataSource::IsKind(AType)); 
//}
//Handle_ScMesh_DataSource::~Handle_ScMesh_DataSource() {}
