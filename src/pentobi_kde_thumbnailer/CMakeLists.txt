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
  pentobi_kde_thumbnailer
  ${KDE4_KIO_LIBS}
  ${QT_LIBRARIES}
  )

install(TARGETS pentobi-thumbnail DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES pentobi-thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
