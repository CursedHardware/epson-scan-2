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

# CMakeのバージョン
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
# リリースビルド用オプション
SET(CMAKE_C_FLAGS "-std=C11")
SET(CMAKE_CXX_FLAGS "-std=c++11")

# デバッグビルド用オプション
SET(CMAKE_C_FLAGS_DEBUG "-g -O0")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++11 -O0")

# ビルドのタイプはリリース
SET(CMAKE_BUILD_TYPE Debug)

# プロジェクト名
PROJECT(controller CXX)
# ビルドするためにインクルードするヘッダーファイルの場所
INCLUDE_DIRECTORIES(
./
./../../../Tools/gtest/include
../../Include/Controller
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../Tools/gtest/library/ )

ADD_executable( controller_test 
	ControllerTest.cpp
)
target_link_libraries( controller_test
	gtest
	gtest_main
	pthread
	dl
)


