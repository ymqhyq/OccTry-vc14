// Created on: 1997-11-21
// Created by: ZOV
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _V3d_PositionLight_HeaderFile
#define _V3d_PositionLight_HeaderFile

#include <Graphic3d_Vertex.hxx>
#include <V3d_Light.hxx>
#include <V3d_TypeOfPickLight.hxx>
#include <V3d_TypeOfRepresentation.hxx>

class V3d_Viewer;

//! Base class for Positional, Spot and Directional Light classes.
class V3d_PositionLight : public Graphic3d_CLight
{
  DEFINE_STANDARD_RTTIEXT(V3d_PositionLight, Graphic3d_CLight)
protected:

  //! Protected constructor.
  Standard_EXPORT V3d_PositionLight (Graphic3d_TypeOfLightSource theType,
                                     const Handle(V3d_Viewer)& theViewer);

//! @name hidden properties not applicable to positional light
protected:

  using Graphic3d_CLight::Position;
  using Graphic3d_CLight::SetPosition;

};

DEFINE_STANDARD_HANDLE(V3d_PositionLight, Graphic3d_CLight)

#endif // _V3d_PositionLight_HeaderFile
