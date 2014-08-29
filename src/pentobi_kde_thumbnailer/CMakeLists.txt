find_package(KDE4 REQUIRED)

include(KDE4Defaults)

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${KDE4_INCLUDES}
  ${QT_INCLUDES}
  )

kde4_add_plugin(pentobi-thumbnail
  PentobiThumbCreator.h
  PentobiThumbCreator.cpp
)

target_link_libraries(pentobi-thumbnail
  pentobi_kde_thumbnailer
  ${KDE4_KIO_LIBS}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  )

install(TARGETS pentobi-thumbnail DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES pentobi-thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
