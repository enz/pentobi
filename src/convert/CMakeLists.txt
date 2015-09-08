if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

add_executable(convert Main.cpp)

# We currently assume static linking with MinGW/Qt4 because this is how we
# build the binaries for the Windows installer. Unfortunately, the following
# lines will break the compilation with dynamic linking and MinGW/Qt4.
if (NOT USE_QT5 AND MINGW)
  add_definitions(-DQT_STATICPLUGIN)
  if(QT_DEBUG)
    target_link_libraries(convert "${QT_PLUGINS_DIR}/imageformats/libqsvgd.a")
  else()
    target_link_libraries(convert "${QT_PLUGINS_DIR}/imageformats/libqsvg.a")
  endif()
endif()

if (USE_QT5)
  target_link_libraries(convert Qt5::Widgets)
else()
  target_link_libraries(convert
    ${QT_QTSVG_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIB_DEPENDENCIES}
    ${QT_QTCORE_LIB_DEPENDENCIES}
    )
endif()
