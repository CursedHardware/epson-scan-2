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

PROJECT(sdksample)
cmake_minimum_required (VERSION 2.8.10.2)

set(CMAKE_BUILD_TYPE "Release")

SET(CMAKE_C_FLAGS_DEBUG "-g -O0 -pthread")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++11 -O0 -pthread")
SET(CMAKE_C_FLAGS_RELEASE "-O3 -pthread")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++11 -pthread")# -fsanitize=address -fsanitize=leak")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DBOOST_NO_CXX11_RVALUE_REFERENCES=1)

INCLUDE_DIRECTORIES(
)

add_executable(sdksample
./ESL_SAMPLE.cpp
./EslStub.cpp
)

target_link_libraries( sdksample
	dl
)

install(TARGETS sdksample DESTINATION /home/build/workspace/trunk/epsonscan2/src/ScanSDK/Src/SDK/SAMPLE/)

