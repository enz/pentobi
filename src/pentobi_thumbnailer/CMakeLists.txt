if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

set(pentobi_thumbnailer_SRCS
  Main.cpp
  )

add_executable(pentobi-thumbnailer ${pentobi_thumbnailer_SRCS})

if (USE_QT5)
  qt5_use_modules(pentobi-thumbnailer Widgets)
endif()

target_link_libraries(pentobi-thumbnailer
  pentobi_thumbnail
  pentobi_gui
  ${QT_QTGUI_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIB_DEPENDENCIES}
  ${QT_QTCORE_LIB_DEPENDENCIES}
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
  )

install(TARGETS pentobi-thumbnailer DESTINATION ${CMAKE_INSTALL_BINDIR})
