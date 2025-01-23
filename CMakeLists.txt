cmake_minimum_required(VERSION 3.19)

project(pentobi CXX)
set(PENTOBI_VERSION 26.0)

include(GNUInstallDirs)

option(PENTOBI_BUILD_GUI "Build GUI" ON)
option(PENTOBI_BUILD_GTP "Build GTP interface" OFF)
option(BUILD_TESTING "Build tests" OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(BUILD_TESTING)
  enable_testing()
endif()

if(UNIX)
  add_custom_target(dist
    COMMAND git archive --prefix=pentobi-${PENTOBI_VERSION}/ HEAD
    | xz -e > ${CMAKE_BINARY_DIR}/pentobi-${PENTOBI_VERSION}.tar.xz
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

find_package(Threads)
add_subdirectory(libboardgame_base)
add_subdirectory(libboardgame_mcts)
add_subdirectory(libpentobi_base)
add_subdirectory(libpentobi_mcts)
if(BUILD_TESTING)
    add_subdirectory(libboardgame_test)
endif()
if(PENTOBI_BUILD_GTP)
    add_subdirectory(libboardgame_gtp)
    add_subdirectory(libpentobi_gtp)
    add_subdirectory(pentobi_gtp)
    if(UNIX)
        add_subdirectory(twogtp)
    else()
        message(STATUS "Not building twogtp, needs POSIX")
    endif()
    add_subdirectory(learn_tool)
endif()
if(PENTOBI_BUILD_GUI)
    find_package(Gettext 0.19.6 REQUIRED)
    find_program(RSVG_CONVERT rsvg-convert REQUIRED)
    set(CMAKE_AUTORCC ON)
    find_package(Qt6 6.5 REQUIRED
        COMPONENTS Concurrent LinguistTools QuickControls2 Xml)
    qt_standard_project_setup(REQUIRES 6.5)
    add_subdirectory(pentobi)
endif()
