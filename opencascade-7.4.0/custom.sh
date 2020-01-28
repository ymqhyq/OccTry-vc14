#!/bin/bash
#
# Mandatory 3rd-parties switches
export HAVE_FREETYPE=true
export HAVE_TCLTK=true
# Optional 3rd-parties switches
export HAVE_FREEIMAGE=true
export HAVE_TBB=true
export HAVE_VTK=true
export HAVE_FFMPEG=true
export HAVE_RAPIDJSON=true
export CHECK_QT4=true
export CHECK_JDK=true
#
export CSF_OPT_INC="${PRODUCTS_PATH}/rapidjson-1.1.0/include:${PRODUCTS_PATH}/ffmpeg-3.1/include:${PRODUCTS_PATH}/VTK-6.1.0-new/include/vtk-6.1:${PRODUCTS_PATH}/tbb42_20140416oss/include:${PRODUCTS_PATH}/freeimage-3.17.0/include:${PRODUCTS_PATH}/freetype-2.5.5/include:${PRODUCTS_PATH}/freetype-2.5.5/include/freetype2:${PRODUCTS_PATH}/tcltk-8.6.0/include:${PRODUCTS_PATH}/tcltk-8.6.0/include:"
export CSF_OPT_LIB64="${PRODUCTS_PATH}/ffmpeg-3.1/lib:${PRODUCTS_PATH}/VTK-6.1.0-new/lib/:${PRODUCTS_PATH}/tbb42_20140416oss/lib/intel64/gcc4.4:${PRODUCTS_PATH}/freeimage-3.17.0/lib:${PRODUCTS_PATH}/freetype-2.5.5/lib:${PRODUCTS_PATH}/tcltk-8.6.0/lib:${PRODUCTS_PATH}/tcltk-8.6.0/lib:"
export CSF_OPT_BIN64=""
export CSF_OPT_INC="${CASROOT}/inc:${CSF_OPT_INC}"
export CSF_OPT_LIB64="${CASROOT}/lib:${CSF_OPT_LIB64}"
export CSF_OPT_BIN64="${CASROOT}/bin"
