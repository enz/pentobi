cmake_minimum_required(VERSION 3.1.0)

project(Pentobi)
set(PENTOBI_VERSION 16.0.x)
set(PENTOBI_RELEASE_DATE 2018-10-18)

cmake_policy(SET CMP0043 NEW)
cmake_policy(SET CMP0071 NEW)

include(GNUInstallDirs)

option(PENTOBI_BUILD_GTP "Build GTP interface" OFF)
option(PENTOBI_BUILD_GUI "Build GUI" ON)
option(PENTOBI_BUILD_TESTS "Build unit tests" OFF)
option(PENTOBI_BUILD_THUMBNAILER "Build Gnome thumbnailer" ON)
option(PENTOBI_BUILD_KDE_THUMBNAILER "Build KDE thumbnailer" OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 14)
if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  add_compile_options(-ffast-math -Wall -Wextra)
endif()

if(PENTOBI_BUILD_TESTS)
  if(PENTOBI_BUILD_KDE_THUMBNAILER)
    configure_file(CTestCustom.cmake ${CMAKE_BINARY_DIR} COPYONLY)
  endif()
  enable_testing()
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

