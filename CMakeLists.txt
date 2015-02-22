cmake_minimum_required(VERSION 2.8.9)

# Use all lower-case for project name because it is used in
# CMAKE_INSTALL_DOCDIR of GNUInstallDirs
project(pentobi)
set(PENTOBI_VERSION 9.0.UNKNOWN)

if(POLICY CMP0020)
  # Remove once we require CMake 2.8.11
  cmake_policy(SET CMP0020 OLD)
endif()
if (POLICY CMP0043)
  # Avoid warning with CMake 3.0.1
  cmake_policy(SET CMP0043 NEW)
endif()

include(CheckIncludeFiles)
include(GNUInstallDirs)

option(PENTOBI_BUILD_TESTS "Build unit tests" OFF)
option(PENTOBI_BUILD_GTP "Build GTP interface" OFF)
option(PENTOBI_BUILD_GUI "Build Qt-based GUI" ON)
option(PENTOBI_BUILD_QML "Build QtQuick-based GUI" OFF)
option(PENTOBI_BUILD_KDE_THUMBNAILER "Build thumbnailer for KDE" OFF)
option(USE_QT5 "Use Qt 5 instead of Qt 4" OFF)

if (PENTOBI_BUILD_KDE_THUMBNAILER AND USE_QT5)
  message(FATAL_ERROR "PENTOBI_BUILD_KDE_THUMBNAILER requires USE_QT5=0")
endif()
if (PENTOBI_BUILD_KDE_THUMBNAILER AND NOT PENTOBI_BUILD_GUI)
  message(FATAL_ERROR
    "PENTOBI_BUILD_KDE_THUMBNAILER requires PENTOBI_BUILD_GUI=1")
endif()
if (PENTOBI_BUILD_QML AND NOT USE_QT5)
  message(FATAL_ERROR "PENTOBI_BUILD_QML requires USE_QT5=1")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DLIBBOARDGAME_DEBUG")

if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    OR (CMAKE_CXX_COMPILER_ID MATCHES "Intel"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
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

find_package(Threads)

if(PENTOBI_BUILD_GUI)
  if (USE_QT5)
    find_package(Qt5Concurrent REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5LinguistTools REQUIRED)
    find_package(Qt5Svg REQUIRED)
  else()
    find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED)
  endif()
endif()
if(PENTOBI_BUILD_QML)
  find_package(Qt5Concurrent 5.3 REQUIRED)
  find_package(Qt5Qml 5.3 REQUIRED)
  find_package(Qt5Gui 5.3 REQUIRED)
  find_package(Qt5Svg 5.3 REQUIRED)
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
add_subdirectory(windows)
