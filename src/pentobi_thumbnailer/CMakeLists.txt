if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  )

link_directories(${Boost_LIBRARY_DIRS})

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
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
  )

install(TARGETS pentobi-thumbnailer DESTINATION ${CMAKE_INSTALL_BINDIR})
