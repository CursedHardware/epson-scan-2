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

cmake_minimum_required(VERSION 2.8)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Config.h.in
	  	  	    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Config.h)

add_subdirectory(CommonUtility/Linux)
add_subdirectory(ES2Command/Linux)
add_subdirectory(ES2FileFormat/Linux)
add_subdirectory(Controller)
add_subdirectory(SaneWrapper/Linux)
add_subdirectory(Standalone)
add_subdirectory(DetectAlert)
add_subdirectory(ScanSDK)

