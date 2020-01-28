// File generated by CPPExt (Transient)
//
//                     Copyright (C) 1991 - 2000 by  
//                      Matra Datavision SA.  All rights reserved.
//  
//                     Copyright (C) 2001 - 2004 by
//                     Open CASCADE SA.  All rights reserved.
// 
// This file is part of the Open CASCADE Technology software.
//
// This software may be distributed and/or modified under the terms and
// conditions of the Open CASCADE Public License as defined by Open CASCADE SA
// and appearing in the file LICENSE included in the packaging of this file.
//  
// This software is distributed on an "AS IS" basis, without warranty of any
// kind, and Open CASCADE SA hereby disclaims all such warranties,
// including without limitation, any warranties of merchantability, fitness
// for a particular purpose or non-infringement. Please see the License for
// the specific terms and conditions governing rights and limitations under the
// License.

#ifndef _Handle_SCMESH_DataSource_HeaderFile
#define _Handle_SCMESH_DataSource_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
//yxk20200104
//#ifndef _Handle_MeshVS_DataSource_HeaderFile
//#include <Handle_MeshVS_DataSource.hxx>
//#endif
//endyxk
class Standard_Transient;
//yxk20200104
//class Handle_Standard_Type;
//class Handle(MeshVS_DataSource);
class ScMesh_DataSource;
//Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(ScMesh_DataSource);
//endyxk

class Handle(ScMesh_DataSource) : public Handle(MeshVS_DataSource) {
  public:
    Handle(ScMesh_DataSource)():Handle(MeshVS_DataSource)() {} 
    Handle(ScMesh_DataSource)(const Handle(ScMesh_DataSource)& aHandle) : Handle(MeshVS_DataSource)(aHandle) 
     {
     }

    Handle(ScMesh_DataSource)(const ScMesh_DataSource* anItem) : Handle(MeshVS_DataSource)((MeshVS_DataSource *)anItem) 
     {
     }

    Handle(ScMesh_DataSource)& operator=(const Handle(ScMesh_DataSource)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(ScMesh_DataSource)& operator=(const ScMesh_DataSource* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    ScMesh_DataSource* operator->() const
     {
      return (ScMesh_DataSource *)ControlAccess();
     }

//   Standard_EXPORT ~Handle(ScMesh_DataSource)();
 
   Standard_EXPORT static const Handle(ScMesh_DataSource) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif