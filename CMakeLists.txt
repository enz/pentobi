cmake_minimum_required(VERSION 2.8.9)
project(Pentobi)
set(PENTOBI_VERSION 5.UNKNOWN)

include(CheckIncludeFiles)

option(PENTOBI_BUILD_TESTS "Build unit tests" OFF)
option(PENTOBI_BUILD_GTP "Build GTP interface" OFF)
option(PENTOBI_BUILD_GUI "Build Qt-based GUI" ON)
option(PENTOBI_BUILD_KDE_THUMBNAILER
  "Build thumbnailer for KDE (also requires PENTOBI_BUILD_GUI)" OFF)
option(PENTOBI_REGISTER_GNOME2_THUMBNAILER
  "Install files for registering thumbnailer for old versions of Gnome" ON)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DLIBBOARDGAME_DEBUG)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  add_definitions(-std=c++0x)
  add_definitions(-ffast-math)
endif()

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

if(MINGW)
  # -std=c++0x makes MinGW define __STRICT_ANSI__, which also hides
  # some functions that we use in some Windows-specific code, like fdopen()
  add_definitions(-U__STRICT_ANSI__)
  add_definitions(-DWINVER=0x0501)
  set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,--large-address-aware")
endif()

check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(sys/times.h HAVE_SYS_TIMES_H)
check_include_files(sys/sysctl.h HAVE_SYS_SYSCTL_H)

set(PENTOBI_BOOST_COMPONENTS system filesystem program_options thread)
# On MinGW, the thread library needs chrono (last tested with Boost 1.51)
if (MINGW)
  set(PENTOBI_BOOST_COMPONENTS ${PENTOBI_BOOST_COMPONENTS} chrono)
endif()
# Package Threads is needed because some versions of CMake/FindBoost fail to
# include -lpthread in Boost_LIBRARIES, which is required by libboost_thread
# (e.g. this occurs with CMake 2.8.9/MinGW 3.18). We solve this by adding
# CMAKE_THREAD_LIBS_INIT to the link libraries.
find_package(Threads)
find_package(Boost 1.49 REQUIRED COMPONENTS ${PENTOBI_BOOST_COMPONENTS})
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found")
endif()
if(MINGW AND Boost_USE_STATIC_LIBS)
  # Boost static libs generate link errors with MinGW if BOOST_THREAD_USE_LIB
  # is not explicetly defined (last tested with Boost 1.52)
  add_definitions(-DBOOST_THREAD_USE_LIB)
endif()

if(NOT DEFINED LIBBOARDGAME_MCTS_FLOAT_TYPE)
  set(LIBBOARDGAME_MCTS_FLOAT_TYPE float)
endif()
if(NOT DEFINED PENTOBI_BOOKS_DIR)
  if(UNIX AND NOT APPLE)
    set(PENTOBI_BOOKS_DIR
      "${CMAKE_INSTALL_PREFIX}/share/games/pentobi/books")
  endif()
endif()
if(NOT DEFINED PENTOBI_MANUAL_DIR)
  if(UNIX AND NOT APPLE)
    set(PENTOBI_MANUAL_DIR
      "${CMAKE_INSTALL_PREFIX}/share/games/pentobi/manual")
  endif()
endif()
if(NOT DEFINED PENTOBI_TRANSLATIONS)
  if(UNIX AND NOT APPLE)
    set(PENTOBI_TRANSLATIONS
      "${CMAKE_INSTALL_PREFIX}/share/games/pentobi/translations")
  endif()
endif()
configure_file(config-cmake.h.in config.h)
add_definitions(-DHAVE_CONFIG_H)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(PENTOBI_BUILD_TESTS)
  enable_testing()
endif()

if(PENTOBI_BUILD_GUI)
  find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
endif()

if(UNIX)
add_custom_target(dist
  COMMAND git archive --prefix=pentobi-${PENTOBI_VERSION}/ HEAD
  | gzip --best > ${CMAKE_BINARY_DIR}/pentobi-${PENTOBI_VERSION}.tar.gz
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(UNIX AND NOT APPLE)
  add_custom_target(post-install
    COMMAND
    update-desktop-database ${CMAKE_INSTALL_PREFIX}/share/applications\;
    update-mime-database ${CMAKE_INSTALL_PREFIX}/share/mime\;
  )
endif()

add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(windows_installer)
