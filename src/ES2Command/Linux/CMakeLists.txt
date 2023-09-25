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
PROJECT(es2command)

add_definitions(-DBOOST_NO_CXX11_RVALUE_REFERENCES=1)

if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)  
endif(COMMAND cmake_policy)

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../bin)

# ビルドのタイプはリリース
#SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_C_FLAGS_DEBUG "-g -O0")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++11 -Wno-multichar -O0")
SET(CMAKE_C_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++11 -Wno-multichar")

# ビルドするためにインクルードするヘッダーファイルの場所
INCLUDE_DIRECTORIES(
../../../thirdparty/rapidjson/include/
../../
../../Include/ES2Command
../Src
)

ADD_LIBRARY( es2command SHARED
../../Platform.cpp
../Src/API/ES2CommandAPI.cpp
../Src/Command/ModelInfo.cpp
../Src/Command/Environment.cpp
../Src/Command/ESScanner.cpp
../Src/Command/ESScanner_Capability.cpp
../Src/Command/Base/CommandBase.cpp
../Src/Command/ESCI/ESCICommand.cpp
../Src/Command/ESCI/ESCIAccessor.cpp
../Src/Command/ESCI/ESCIScanner.cpp
../Src/Command/ESCI/ESCIScanner_Accessor.cpp
../Src/Command/ESCI/ESCIScanner_Capability.cpp
../Src/Command/ESCI2/ESCI2Command.cpp
../Src/Command/ESCI2/ESCI2Accessor.cpp
../Src/Command/ESCI2/ESCI2DataConstructor.cpp
../Src/Command/ESCI2/ESCI2DataEnumerator.cpp
../Src/Command/ESCI2/ESCI2Scanner.cpp
../Src/Command/ESCI2/ESCI2Scanner_Accessor.cpp
../Src/Command/ESCI2/ESCI2Scanner_Capability.cpp
../Src/Command/ESCI2/ESCI2JSONPerser.cpp
../Src/Interface/interfaceFactory.cpp
../Src/Interface/interface.cpp
../Src/Interface/usb/usbInterfaceImpl.cpp
../Src/Interface/ipc/ipcInterfaceImpl.cpp
../Src/Image/ESScannedImage.cpp
../Src/Image/ESCI2ScannedImage.cpp
../Src/Utils/ESUtils.cpp
../../../thirdparty/sha1/sha1.c
)

target_link_libraries( es2command
	pthread
	dl
	commonutility
	${Boost_LIBRARIES}
	usb-1.0
)

install(TARGETS es2command DESTINATION ${EPSON_INSTALL_ROOT}${EPSON_INSTALL_PATH})
