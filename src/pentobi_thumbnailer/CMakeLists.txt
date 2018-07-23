add_executable(pentobi-thumbnailer Main.cpp)

target_link_libraries(pentobi-thumbnailer
  pentobi_thumbnail
  pentobi_gui
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
  )

target_link_libraries(pentobi-thumbnailer Qt5::Gui)

install(TARGETS pentobi-thumbnailer DESTINATION ${CMAKE_INSTALL_BINDIR})
