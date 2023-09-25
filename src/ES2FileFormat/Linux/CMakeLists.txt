#  Copyright (C) 2018-2022  SEIKO EPSON CORPORATION
#
#  License: LGPL-2.1+
#  Author : SEIKO EPSON CORPORATION
#
#  This file is part of the 'Epson Scan 2' package.
#	 This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.

#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#  
#  CMakeLists.txt -- template and derived ESC/I ESC/I-2 protocol commands

PROJECT(es2fileformat)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(STATIC_GLIB_LOCATION)
INCLUDE_DIRECTORIES(
../
../../
../../../Libraries/rapidjson/include
../../CommonUtility
../../CommonUtility/utils
../../Include/ES2FileFormat
../../Include/ES2ScanEngine
../../Include/ES2Command
../Header/IO
../Header/Reader
../Header/Writer
#${STATIC_GLIB_LOCATION}/libjpeg-header
${STATIC_GLIB_LOCATION}/tiff-header 
#${STATIC_GLIB_LOCATION}/png-header
${Boost_INCLUDE_DIRS}
)
else()
INCLUDE_DIRECTORIES(
../
../../
../../../Libraries/rapidjson/include
../../CommonUtility
../../CommonUtility/utils
../../Include/ES2FileFormat
../../Include/ES2ScanEngine
../../Include/ES2Command
../Header/IO
../Header/Reader
../Header/Writer
${JPEG_INCLUDE_DIR}
${TIFF_INCLUDE_DIR}
${PNG_INCLUDE_DIR}
${Boost_INCLUDE_DIRS}
)
endif()


ADD_LIBRARY( es2fileformat SHARED
../../Platform.cpp
../Source/JpegUtil/FKJpegDecodeUtil.cpp
../Source/JpegUtil/FKJpegEncodeUtil.cpp
../Source/PngUtil/FKPngEncodeUtil.cpp
../Source/TiffUtil/FKTiffEncodeUtil.cpp
../Source/TiffUtil/FKTiffJpegEncodeUtil.cpp
../Source/BmpUtil/FKBmpUtil.cpp
../Source/Base/FKReader.cpp
../Source/Base/FKWriter.cpp
../Source/Writer/FKJpegWriter.cpp
../Source/Writer/FKPngWriter.cpp
../Source/Writer/FKTiffWriter.cpp
../Source/Reader/FKJpegReader.cpp
../Source/IO/FKDestination.cpp
../Source/IO/FKSource.cpp
../FileFormatKit.cpp
../Source/Writer/FKPnmWriter.cpp
../Source/PnmUtil/FKPnmUtil.cpp
../Source/Writer/FKBmpWriter.cpp
)

if(STATIC_GLIB_LOCATION)

#add_library(libturbojpeg
#    STATIC
#    IMPORTED
  #  )

#add_library(libpng16
#    STATIC
#    IMPORTED
  #  )

add_library(libtiff
    STATIC
    IMPORTED
    )

#set_target_properties(libturbojpeg
#    PROPERTIES
#    IMPORTED_LOCATION ${STATIC_GLIB_LOCATION}/${ARCHITECTURE}/libturbojpeg.a
  #  )

#set_target_properties(libpng16
#    PROPERTIES
#    IMPORTED_LOCATION ${STATIC_GLIB_LOCATION}/${ARCHITECTURE}/libpng16.a
  #  )

set_target_properties(libtiff
    PROPERTIES
    IMPORTED_LOCATION ${STATIC_GLIB_LOCATION}/../Environment_dependency/fileformat/${PKG_TYPE}/${ARCHITECTURE}/libtiff.a
    )

endif()


if(STATIC_GLIB_LOCATION)
target_link_libraries( es2fileformat
	pthread
	dl
  libjpeg.so
  libpng12.so
  libtiff
	commonutility
)

else()

target_link_libraries( es2fileformat
	pthread
	dl
    ${JPEG_LIBRARIES}
    ${TIFF_LIBRARIES}
    ${PNG_LIBRARIES}
	commonutility
)

endif()

install(TARGETS es2fileformat DESTINATION ${EPSON_INSTALL_ROOT}${EPSON_INSTALL_PATH})


