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

# プロジェクト名
PROJECT(sane-epsonscan2)

add_definitions(-DBOOST_NO_CXX11_RVALUE_REFERENCES=1)

if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)  
endif(COMMAND cmake_policy)

INCLUDE_DIRECTORIES(
../
../../
../../Standalone
../../bin
../../CommonUtility
../../CommonUtility/utils
)

ADD_LIBRARY( sane-epsonscan2 SHARED
../backend.cpp
../epsonscan.cpp
../list.cpp
../message.cpp
../../Standalone/supervisor.cpp
../../Standalone/saveimagepathlist.cpp
../../Standalone/devicelist.cpp
../../Standalone/defaultsettings.cpp
)

target_link_libraries( sane-epsonscan2
	pthread
	dl
	commonutility
)

target_compile_definitions(sane-epsonscan2 PRIVATE BUILDSANE=1)

install(TARGETS sane-epsonscan2 DESTINATION ${EPSON_INSTALL_ROOT}${EPSON_INSTALL_PATH})
