cmake_minimum_required(VERSION 2.8)
project(Pentobi)
set(PENTOBI_VERSION 0.1.UNKNOWN)

option(PENTOBI_BUILD_TESTS "Build unit tests" OFF)
option(PENTOBI_BUILD_GTP "Build GTP interface" OFF)
option(PENTOBI_BUILD_GUI "Build Qt-based GUI" ON)

# Use optimized build as default for CMAKE_BUILD_TYPE
if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Type of build (None Debug Release RelWithDebInfo MinSizeRel)"
      FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DLIBBOARDGAME_DEBUG)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-std=c++0x)
  add_definitions(-ffast-math)
endif()

# Define CMAKE_INSTALL_SYSCONFDIR  and CMAKE_INSTALL_FULL_SYSCONFDIR as in
# GNUInstallDirs (we don't use GNUInstallDirs yet because it requires CMake
# 2.8.5 and we require only CMake 2.8 so far.
if(NOT DEFINED CMAKE_INSTALL_SYSCONFDIR)
  set(CMAKE_INSTALL_SYSCONFDIR etc CACHE PATH
    "Unix etc directory (absolute path or relative to CMAKE_INSTALL_PREFIX)"
    FORCE)
endif()
if(NOT IS_ABSOLUTE ${CMAKE_INSTALL_SYSCONFDIR})
  set(CMAKE_INSTALL_FULL_SYSCONFDIR
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}")
else()
  set(CMAKE_INSTALL_FULL_SYSCONFDIR "${CMAKE_INSTALL_SYSCONFDIR}")
endif()

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

include(CheckIncludeFiles)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(sys/times.h HAVE_SYS_TIMES_H)

set(Boost_DETAILED_FAILURE_MSG 1)
# Boost versions that were not released when CMake 2.8 came out.
# You might have to update this list if you use a newer version of Boost.
set(Boost_ADDITIONAL_VERSIONS "1.45 1.45.0 1.46 1.46.0 1.46.1 1.47 1.47.0")
# Note: Some older versions of Boost cause compilation errors if compiled with
# GCC option -std=c++0x. It might be possible to compile Pentobi with an older
# version of Boost depending on the version of GCC (e.g. the combination
# GCC 4.4/Boost 1.40 in Ubuntu 10.04 worked but the combination
# GCC 4.5/Boost 1.42 in Ubuntu 11.04 causes compilation errors.
find_package(Boost 1.45 REQUIRED
  COMPONENTS thread system filesystem program_options)
if(MINGW AND Boost_USE_STATIC_LIBS)
  # Boost 1.45 static libs generate link errors with MinGW if
  # BOOST_THREAD_USE_LIB is not explicetly defined.
  add_definitions(-DBOOST_THREAD_USE_LIB)
endif()

# Package Threads is needed because some versions of CMake/FindBoost fail to
# include -lpthread in Boost_LIBRARIES, which is required by libboost_thread.
# (e.g. this occurs with CMake 2.8.4/MinGW 3.18). We solve this by adding
# CMAKE_THREAD_LIBS_INIT to the link libraries.
find_package(Threads)

if(NOT DEFINED LIBBOARDGAME_MCTS_VALUE_TYPE)
    set(LIBBOARDGAME_MCTS_VALUE_TYPE float)
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

add_custom_target(dist
  COMMAND git archive --prefix=pentobi-${PENTOBI_VERSION}/ HEAD
  | gzip --best > ${CMAKE_BINARY_DIR}/pentobi-${PENTOBI_VERSION}.tar.gz
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(post-install
  COMMAND update-desktop-database ${CMAKE_INSTALL_PREFIX}/share/applications
  COMMAND update-mime-database ${CMAKE_INSTALL_PREFIX}/share/mime
  COMMAND env GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
  gconftool-2 --makefile-install-rule
  ${CMAKE_INSTALL_FULL_SYSCONFDIR}/gconf/schemas/pentobi.schemas
  )

add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(windows_installer)
