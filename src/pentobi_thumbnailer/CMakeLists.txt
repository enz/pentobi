if(UNIX AND NOT APPLE)

  include(${QT_USE_FILE})

  include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
    )

  link_directories(${Boost_LIBRARY_DIRS})

  set(pentobi_thumbnailer_SRCS
    Main.cpp
    )

  add_executable(pentobi-thumbnailer ${pentobi_thumbnailer_SRCS})

  target_link_libraries(pentobi-thumbnailer
    pentobi_gui
    ${QT_LIBRARIES}
    pentobi_base
    boardgame_base
    boardgame_sgf
    boardgame_util
    boardgame_sys
    ${Boost_LIBRARIES}
    )

  install(TARGETS pentobi-thumbnailer DESTINATION games)

endif(UNIX AND NOT APPLE)
