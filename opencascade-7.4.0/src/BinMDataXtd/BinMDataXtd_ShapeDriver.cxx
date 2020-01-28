// Created on: 2004-05-13
// Created by: Sergey ZARITCHNY
// Copyright (c) 2004-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <BinMDataXtd_ShapeDriver.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataXtd_ShapeDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataXtd_ShapeDriver
//purpose  : 
//=======================================================================
BinMDataXtd_ShapeDriver::BinMDataXtd_ShapeDriver
                        (const Handle(Message_Messenger)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataXtd_Shape)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataXtd_ShapeDriver::NewEmpty() const
{
  return new TDataXtd_Shape();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataXtd_ShapeDriver::Paste
                                (const BinObjMgt_Persistent&,
                                 const Handle(TDF_Attribute)&,
                                 BinObjMgt_RRelocationTable& ) const
{return Standard_True;}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataXtd_ShapeDriver::Paste (const Handle(TDF_Attribute)&,
                                       BinObjMgt_Persistent&,
                                       BinObjMgt_SRelocationTable&  ) const
{}
