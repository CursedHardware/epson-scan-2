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
PROJECT(emulator)

# CMakeのバージョン
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
SET(CMAKE_C_FLAGS "")
SET(CMAKE_CXX_FLAGS "-std=c++11")

# CMakeのバージョン
# デバッグビルド用オプション
SET(CMAKE_C_FLAGS_DEBUG "-g -O0")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++11 -O0")

# ビルドのタイプはリリース
SET(CMAKE_BUILD_TYPE Debug)

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../bin)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# ビルドするためにインクルードするヘッダーファイルの場所
INCLUDE_DIRECTORIES(
../
../../
../../Include
../../Include/ES2Command
../../CommonUtility
../../ES2Command
../../../Libraries/rapidjson/include
)

ADD_LIBRARY( emulator SHARED
../Emulator.cpp
../../ES2Command/Image/ESScannedImage.cpp
../EmulatorSettings.cpp
)

target_link_libraries( emulator
	pthread
	dl
	commonutility
	boost_filesystem
	boost_regex
)


# 共有ライブラリのインストール指定
INSTALL(TARGETS emulator LIBRARY DESTINATION lib)
