cmake_minimum_required(VERSION 3.1.0)

project(Pentobi)
set(PENTOBI_VERSION 15.UNKNOWN)
set(PENTOBI_RELEASE_DATE 2018-06-28)

cmake_policy(SET CMP0043 NEW)

include(CheckIncludeFiles)
include(GNUInstallDirs)

option(PENTOBI_BUILD_TESTS "Build unit tests" OFF)
option(PENTOBI_BUILD_GTP "Build GTP interface" OFF)
option(PENTOBI_BUILD_GUI "Build Qt-based GUI" ON)
option(PENTOBI_BUILD_QML "Build QtQuick-based GUI" OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DLIBBOARDGAME_DEBUG")

set(CMAKE_CXX_STANDARD 14)
if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  add_compile_options(-ffast-math -Wall -Wextra)
endif()

check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(sys/times.h HAVE_SYS_TIMES_H)
check_include_files(sys/sysctl.h HAVE_SYS_SYSCTL_H)

if(NOT DEFINED LIBPENTOBI_MCTS_FLOAT_TYPE)
  set(LIBPENTOBI_MCTS_FLOAT_TYPE float)
endif()

if(NOT DEFINED PENTOBI_BOOKS_DIR)
  set(PENTOBI_BOOKS_DIR "${CMAKE_INSTALL_FULL_DATADIR}/pentobi/books")
endif()
if(NOT DEFINED PENTOBI_HELP_DIR)
  set(PENTOBI_HELP_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/help")
endif()
if(NOT DEFINED PENTOBI_TRANSLATIONS)
  set(PENTOBI_TRANSLATIONS "${CMAKE_INSTALL_FULL_DATADIR}/pentobi/translations")
endif()

configure_file(config.h.in config.h)
add_definitions(-DHAVE_CONFIG_H)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/src)

if(PENTOBI_BUILD_TESTS)
  enable_testing()
endif()

find_package(Threads)

if(PENTOBI_BUILD_GUI)
  find_package(Qt5Concurrent 5.6 REQUIRED)
  find_package(Qt5Widgets 5.6 REQUIRED)
  find_package(Qt5LinguistTools 5.6 REQUIRED)
  find_package(Qt5Svg 5.6 REQUIRED)
endif()
if(PENTOBI_BUILD_QML)
  find_package(Qt5Concurrent 5.11 REQUIRED)
  find_package(Qt5Qml 5.11 REQUIRED)
  find_package(Qt5Gui 5.11 REQUIRED)
  find_package(Qt5Svg 5.11 REQUIRED)
  find_package(Qt5WebView 5.11 REQUIRED)
endif()

if(UNIX)
  add_custom_target(dist
    COMMAND git archive --prefix=pentobi-${PENTOBI_VERSION}/ HEAD
    | xz -e > ${CMAKE_BINARY_DIR}/pentobi-${PENTOBI_VERSION}.tar.xz
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(data)

