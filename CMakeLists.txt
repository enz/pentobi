cmake_minimum_required(VERSION 2.8.9)

# Use all lower-case for project name because it is used in
# CMAKE_INSTALL_DOCDIR of GNUInstallDirs
project(pentobi)
set(PENTOBI_VERSION 7.UNKNOWN)

if(POLICY CMP0020)
  # Remove once we require CMake 2.8.11
  cmake_policy(SET CMP0020 OLD)
endif()

include(CheckIncludeFiles)
include(GNUInstallDirs)

option(PENTOBI_BUILD_TESTS "Build unit tests" OFF)
option(PENTOBI_BUILD_GTP "Build GTP interface" OFF)
option(PENTOBI_BUILD_GUI "Build Qt-based GUI" ON)
option(PENTOBI_BUILD_KDE_THUMBNAILER
  "Build thumbnailer for KDE (also requires PENTOBI_BUILD_GUI)" OFF)
option(USE_QT5 "Use Qt 5 instead of Qt 4" OFF)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DLIBBOARDGAME_DEBUG)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    OR (CMAKE_CXX_COMPILER_ID MATCHES "Intel"))
  add_definitions(-std=c++11)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  add_definitions(-ffast-math)
endif()
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE
    -D_SCL_SECURE_NO_WARNINGS)
endif()

check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(sys/times.h HAVE_SYS_TIMES_H)
check_include_files(sys/sysctl.h HAVE_SYS_SYSCTL_H)

if(NOT DEFINED LIBPENTOBI_MCTS_FLOAT_TYPE)
  set(LIBPENTOBI_MCTS_FLOAT_TYPE float)
endif()

# Don't set the Pentobi data dirs on Windows. This is currently needed for
# building a version of Pentobi for the NSIS installer on Windows (see
# directory windows_installer) such that Pentobi will look for data dirs
# relative to the installation directory. (It breaks installing Pentobi on
# Windows with "make install" but we don't support that on Windows anyway.)
if(UNIX)
  if(NOT DEFINED PENTOBI_BOOKS_DIR)
    set(PENTOBI_BOOKS_DIR "${CMAKE_INSTALL_FULL_DATADIR}/pentobi/books")
  endif()
  if(NOT DEFINED PENTOBI_MANUAL_DIR)
    set(PENTOBI_MANUAL_DIR "${CMAKE_INSTALL_FULL_DOCDIR}/manual")
  endif()
  if(NOT DEFINED PENTOBI_TRANSLATIONS)
    set(PENTOBI_TRANSLATIONS
      "${CMAKE_INSTALL_FULL_DATADIR}/pentobi/translations")
  endif()
endif(UNIX)

configure_file(config.h.in config.h)
add_definitions(-DHAVE_CONFIG_H)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(PENTOBI_BUILD_TESTS)
  enable_testing()
endif()

if(PENTOBI_BUILD_GUI)
  if (USE_QT5)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Concurrent REQUIRED)
    find_package(Qt5LinguistTools REQUIRED)
  else()
    find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED)
  endif()
endif()

if(UNIX)
  add_custom_target(dist
    COMMAND git archive --prefix=pentobi-${PENTOBI_VERSION}/ HEAD
    | xz > ${CMAKE_BINARY_DIR}/pentobi-${PENTOBI_VERSION}.tar.xz
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
  add_custom_target(post-install
    COMMAND
    update-mime-database ${CMAKE_INSTALL_FULL_DATAROOTDIR}/mime\;
    update-desktop-database ${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications\;
    )
endif()

install(FILES README NEWS COPYING doc/blksgf/Pentobi-SGF.html
  DESTINATION ${CMAKE_INSTALL_DOCDIR})

add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(windows_installer)
