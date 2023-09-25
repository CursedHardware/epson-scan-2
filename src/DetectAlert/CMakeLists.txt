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

PROJECT(detectalert)
cmake_minimum_required (VERSION 2.8.10.2)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

INCLUDE_DIRECTORIES(
../
../../
../../../
../Include
../CommonUtility
../CommonUtility/utils
)

file(GLOB_RECURSE UI_FILES *.ui)
file(GLOB_RECURSE QRC_FILES *.qrc)

add_executable(detectalert
	detectalert.cpp
    main.cpp
    translationstring.cpp	
	${UI_FILES}
   	${QRC_FILES}
)

if (${CMAKE_VERSION} LESS 3.0)
    qt5_wrap_ui(FORMS ${UI_FILES})
endif()

set_target_properties(detectalert PROPERTIES OUTPUT_NAME "detectalert")

target_link_libraries(detectalert ${QT_LIBRARIES}
	pthread
	dl
	${Boost_LIBRARIES}	
	commonutility
)

QT5_USE_MODULES(detectalert Widgets)
install(TARGETS detectalert DESTINATION "${EPSON_INSTALL_ROOT}${EPSON_INSTALL_PATH}")

