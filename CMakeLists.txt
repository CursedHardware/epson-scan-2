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

project (epsonscan2)
set (CMAKE_INSTALL_PREFIX "/usr")
cmake_minimum_required (VERSION 2.8.12.2)

include(GNUInstallDirs)

if (DEFINED ENV{BUILD_TYPE})

if (${BUILD_TYPE} STREQUAL "Release")
set(CMAKE_BUILD_TYPE Release)
elseif (${BUILD_TYPE} STREQUAL "Debug")
set(CMAKE_BUILD_TYPE Debug)
endif ()

else ()
set(CMAKE_BUILD_TYPE Release)
endif()

if (DEFINED ENV{EPSON_VERSION})
else ()
set(EPSON_VERSION -1.0.0.0-1)
endif()

SET(CMAKE_C_FLAGS_DEBUG "-g -O0")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++11 -O0")
SET(CMAKE_C_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++11")

### Boost settings ########

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
find_package(Boost 1.36.0 REQUIRED)
message(" Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}") 
message(" Boost_LIBRARIES: ${Boost_LIBRARIES}")  
message(" Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}") 

##############################

#### Dependents package ######

if(STATIC_GLIB_LOCATION)
else()
find_package (JPEG REQUIRED)
find_package (PNG REQUIRED)
find_package (TIFF REQUIRED)
find_package (ZLIB REQUIRED)

message(" JPEG_INCLUDE_DIRS: ${JPEG_INCLUDE_DIR}")  
message(" JPEG_LIBRARY_DIRS: ${JPEG_LIBRARIES}")  

message("TIFF INCLUDE : ${TIFF_INCLUDE_DIR}")
message("TIFF LIBRARY : ${TIFF_LIBRARIES}")

message("PNG INCLUDE : ${PNG_INCLUDE_DIR}")
message("PNG LIBRARY : ${PNG_LIBRARIES}")
endif()

message("ZLIB INCLUDE : ${ZLIB_INCLUDE_DIR}")
message("ZLIB LIBRARY : ${ZLIB_LIBRARIES}")

############################

####  PATH Settings #######
set(EPSONSCAN_PATH epsonscan2/)
set(COMMON_ETC_PATH ${CMAKE_INSTALL_FULL_SYSCONFDIR})
set(EPSON_WORK_PATH /tmp/epsonWork/)
set(EPSON_SETTINGS_PATH $ENV{HOME}/.epsonscan2/)

SET (CMAKE_INSTALL_PREFIX /usr)
set(EPSON_INSTALL_PATH ${CMAKE_INSTALL_FULL_LIBDIR}/epsonscan2/)
set(COMMON_SHARE_PATH ${CMAKE_INSTALL_FULL_DATAROOTDIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /tmp/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /tmp/build)

message("EPSONSCAN_PATH ${EPSONSCAN_PATH}")
message("COMMON_ETC_PATH ${COMMON_ETC_PATH}")
message("EPSON_WORK_PATH ${EPSON_WORK_PATH}")
message("EPSON_SETTINGS_PATH ${EPSON_SETTINGS_PATH}")
message("EPSON_INSTALL_PATH ${EPSON_INSTALL_PATH}")
message("COMMON_SHARE_PATH ${COMMON_SHARE_PATH}")

EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
message("Library architecture ${ARCHITECTURE}")

SET(CMAKE_INSTALL_RPATH "${EPSON_INSTALL_PATH}")

###################################

add_subdirectory(src)

######## install settings #########

install(DIRECTORY Resources DESTINATION ${EPSON_INSTALL_ROOT}${EPSON_INSTALL_PATH})

install(FILES epsonscan2.rules DESTINATION ${EPSON_INSTALL_ROOT}/lib/udev/rules.d/ RENAME 60-epsonscan2.rules)
install(FILES epsonscan2 DESTINATION ${EPSON_INSTALL_ROOT}/etc/sane.d/dll.d)
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${EPSON_INSTALL_ROOT}${CMAKE_INSTALL_FULL_LIBDIR}/sane/)")
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ../epsonscan2/libsane-epsonscan2.so ${EPSON_INSTALL_ROOT}${CMAKE_INSTALL_FULL_LIBDIR}/sane/libsane-epsonscan2.so.1)")
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ../epsonscan2/libsane-epsonscan2.so ${EPSON_INSTALL_ROOT}${CMAKE_INSTALL_FULL_LIBDIR}/sane/libsane-epsonscan2.so.1.0.0)")

install(FILES changelog.Debian DESTINATION ${EPSON_INSTALL_ROOT}${CMAKE_INSTALL_PREFIX}/share/doc/epsonscan2${EPSON_VERSION})

install(FILES NEWS DESTINATION ${EPSON_INSTALL_ROOT}${CMAKE_INSTALL_PREFIX}/share/doc/epsonscan2${EPSON_VERSION})
install(FILES AUTHORS DESTINATION  ${EPSON_INSTALL_ROOT}${CMAKE_INSTALL_PREFIX}/share/doc/epsonscan2${EPSON_VERSION})
install(FILES README DESTINATION  ${EPSON_INSTALL_ROOT}${CMAKE_INSTALL_PREFIX}/share/doc/epsonscan2${EPSON_VERSION})
install(FILES COPYING DESTINATION ${EPSON_INSTALL_ROOT}${CMAKE_INSTALL_PREFIX}/share/doc/epsonscan2${EPSON_VERSION})
install(FILES LICENSE DESTINATION ${EPSON_INSTALL_ROOT}${CMAKE_INSTALL_PREFIX}/share/doc/epsonscan2${EPSON_VERSION})

