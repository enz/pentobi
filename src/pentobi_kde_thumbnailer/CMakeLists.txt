find_package(KDE4 REQUIRED)

include(KDE4Defaults)

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${KDE4_INCLUDES}
  ${QT_INCLUDES}
  )

set(pentobi_thumbnail_SRCS PentobiThumbCreator.cpp)

kde4_add_plugin(pentobi-thumbnail ${pentobi_thumbnail_SRCS})

target_link_libraries(pentobi-thumbnail
  ${KDE4_KIO_LIBS}
  pentobi_thumbnail
  pentobi_gui
  ${QT_LIBRARIES}
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  )

install(TARGETS pentobi-thumbnail DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES pentobi-thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
