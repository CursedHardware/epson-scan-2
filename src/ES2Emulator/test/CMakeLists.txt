# プロジェクト名
PROJECT(testcommon CXX)

set(BOOST_LIBRARYDIR /usr/lib/x86_64-linux-gnu)
set(BOOST_ROOT /usr/lib/x86_64-linux-gnu)

find_package(Boost COMPONENTS system filesystem REQUIRED)


# CMakeのバージョン
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
SET(CMAKE_C_FLAGS "-std=C11")
SET(CMAKE_CXX_FLAGS "-std=c++11")
LINK_DIRECTORIES(
${CMAKE_BINARY_DIR}
)

# CMakeのバージョン
# デバッグビルド用オプション
SET(CMAKE_C_FLAGS_DEBUG "-g -O0")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++11 -fsanitize=address -O0")

# ビルドのタイプはリリース
SET(CMAKE_BUILD_TYPE Debug)

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../bin)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# ビルドするためにインクルードするヘッダーファイルの場所
INCLUDE_DIRECTORIES(
../
../../
../../CommonUtility
../../Include
../../Include/ES2Command
../../ES2Command
../../../Libraries/rapidjson/include
)

add_executable(testcommon test.cpp)

target_link_libraries( testcommon
	pthread
	dl
	emulator
	boost_regex
       ${Boost_FILESYSTEM_LIBRARY}
       ${Boost_SYSTEM_LIBRARY}
)
