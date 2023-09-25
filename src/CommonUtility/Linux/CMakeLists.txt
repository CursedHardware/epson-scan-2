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
PROJECT(commonutility)

add_definitions(-DBOOST_NO_CXX11_RVALUE_REFERENCES=1)

# ビルドするためにインクルードするヘッダーファイルの場所
INCLUDE_DIRECTORIES(
../
../../
../../../thirdparty/rapidjson/include/
)

ADD_LIBRARY( commonutility SHARED
../../Platform.cpp
../utils/ESJsonUtils.cpp
../DbgLog.cpp
../stdafx.cpp
../ESImageInfo.cpp
../utils/ESBuffer.cpp
../utils/ESFile.cpp
../utils/ESJsonUtils.cpp
../utils/Utilities.cpp
../utils/stringutils.cpp
../utils/CritSec.cpp
../utils/ESMutex.cpp
../utils/PathUtils.cpp
)

target_link_libraries( commonutility
	pthread
	dl
	${Boost_LIBRARIES}	
)

install(TARGETS commonutility DESTINATION ${EPSON_INSTALL_ROOT}${EPSON_INSTALL_PATH})

