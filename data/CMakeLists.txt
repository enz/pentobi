if(PENTOBI_BUILD_GUI)

foreach(icon application-x-blokus-sgf application-x-blokus-sgf-16
        application-x-blokus-sgf-32 application-x-blokus-sgf-64)
  set(icon_svg "${CMAKE_CURRENT_SOURCE_DIR}/${icon}.svg")
  set(icon_png "${CMAKE_CURRENT_BINARY_DIR}/${icon}.png")
  add_custom_command(OUTPUT "${icon_png}"
    COMMAND convert "${icon_svg}" "${icon_png}" DEPENDS "${icon_svg}")
  list(APPEND png_icons "${icon_png}")
endforeach()
foreach(icon pentobi pentobi-16 pentobi-32 pentobi-64)
  set(icon_svg "${CMAKE_SOURCE_DIR}/src/icon/${icon}.svg")
  set(icon_png "${CMAKE_CURRENT_BINARY_DIR}/${icon}.png")
  add_custom_command(OUTPUT "${icon_png}"
    COMMAND convert "${icon_svg}" "${icon_png}" DEPENDS "${icon_svg}")
  list(APPEND png_icons "${icon_png}")
endforeach()
add_custom_target(data_icons ALL DEPENDS ${png_icons})

configure_file(io.sourceforge.pentobi.desktop.in io.sourceforge.pentobi.desktop @ONLY)
configure_file(io.sourceforge.pentobi.appdata.xml.in io.sourceforge.pentobi.appdata.xml @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi-16.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/apps
  RENAME pentobi.png)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi-32.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps
  RENAME pentobi.png)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi-64.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps
  RENAME pentobi.png)
install(FILES ${CMAKE_SOURCE_DIR}/src/icon/pentobi.svg
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/mimetypes)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-16.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/mimetypes
  RENAME application-x-blokus-sgf.png)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-32.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/mimetypes
  RENAME application-x-blokus-sgf.png)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-64.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/mimetypes
  RENAME application-x-blokus-sgf.png)
install(FILES application-x-blokus-sgf.svg
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/mimetypes)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.desktop
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install(FILES pentobi-mime.xml
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mime/packages)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.appdata.xml
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)

endif(PENTOBI_BUILD_GUI)

if(PENTOBI_BUILD_THUMBNAILER)
  configure_file(pentobi.thumbnailer.in pentobi.thumbnailer @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.thumbnailer
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/thumbnailers)
endif()

if(PENTOBI_BUILD_KDE_THUMBNAILER)
  configure_file(io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml.in
    io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)
endif()
