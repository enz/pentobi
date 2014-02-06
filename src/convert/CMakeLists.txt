if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(convert Main.cpp)

# CMake 2.8.9 doesn't seem to support finding plugin paths yet but
# we need to add qsvg to the linked libraries if static linking is
# used as by our current build of the Windows installer with MinGW.
# This should be fixed once CMake properly supports plugins because it
# breaks the build with dynamic libraries on MinGW and doesn't solve
# the problem with static libraries on other platforms.
if (MINGW)
  add_definitions(-DQT_STATICPLUGIN)
  if(QT_DEBUG)
    target_link_libraries(convert "${QT_PLUGINS_DIR}/imageformats/libqsvgd.a")
  else()
    target_link_libraries(convert "${QT_PLUGINS_DIR}/imageformats/libqsvg.a")
  endif()
endif()

if (USE_QT5)
  qt5_use_modules(convert Widgets)
else()
  target_link_libraries(convert
    ${QT_QTSVG_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIB_DEPENDENCIES}
    ${QT_QTCORE_LIB_DEPENDENCIES}
    )
endif()
