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

PROJECT(epsonscansdk)
cmake_minimum_required (VERSION 2.8.10.2)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DBOOST_NO_CXX11_RVALUE_REFERENCES=1)

INCLUDE_DIRECTORIES(
../
../../
../../../
../Include
../CommonUtility
../CommonUtility/utils
)

ADD_LIBRARY(epsonscansdk SHARED
#add_executable(epsonscansdk
./Src/SDK/main.cpp
./Src/SDK/ESCANLIB.cpp
./Src/SDK/InstanceMgr.cpp
./Src/SDK/LibHelper.cpp
./Src/SDK/ScanMgr.cpp
./Src/SDK/supervisor.cpp
./Src/SDK/devicelist.cpp
./Src/SDK/saveimagepathlist.cpp
./Src/SDK/configuration.cpp
)

target_link_libraries( epsonscansdk
	pthread
	dl
	commonutility
)

install(TARGETS epsonscansdk DESTINATION ${EPSON_INSTALL_ROOT}${EPSON_INSTALL_PATH})
#install(TARGETS epsonscansdk DESTINATION /home/build/workspace/trunk/epsonscan2/src/ScanSDK/Src/SDK/)

