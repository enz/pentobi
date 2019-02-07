configure_file(io.sourceforge.pentobi.desktop.in
    io.sourceforge.pentobi.desktop @ONLY)
configure_file(io.sourceforge.pentobi.appdata.xml.in
    io.sourceforge.pentobi.appdata.xml @ONLY)
configure_file(pentobi.6.in pentobi.6 @ONLY)

foreach(icon application-x-blokus-sgf application-x-blokus-sgf-16
        application-x-blokus-sgf-32 application-x-blokus-sgf-64)
    set(icon_svg "${CMAKE_CURRENT_SOURCE_DIR}/${icon}.svg")
    set(icon_png "${CMAKE_CURRENT_BINARY_DIR}/${icon}.png")
    add_custom_command(OUTPUT "${icon_png}"
        COMMAND convert-svg "${icon_svg}" "${icon_png}"
        DEPENDS "${icon_svg}")
    list(APPEND png_icons "${icon_png}")
endforeach()
foreach(icon pentobi pentobi-16 pentobi-32 pentobi-64)
  set(icon_svg "${CMAKE_CURRENT_SOURCE_DIR}/../icon/${icon}.svg")
  set(icon_png "${CMAKE_CURRENT_BINARY_DIR}/${icon}.png")
  add_custom_command(OUTPUT "${icon_png}"
    COMMAND convert-svg "${icon_svg}" "${icon_png}"
    DEPENDS "${icon_svg}")
  list(APPEND png_icons "${icon_png}")
endforeach()

add_custom_target(pentobi_unix ALL DEPENDS
    ${png_icons}
    "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.desktop"
    "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.appdata.xml"
    "${CMAKE_CURRENT_BINARY_DIR}/pentobi.6"
    )

install(DIRECTORY help DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
    FILES_MATCHING PATTERN "*.css" PATTERN "*.html" PATTERN "*.png"
    PATTERN "*.jpg")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/mimetypes)
foreach(size 16 32 64)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi-${size}.png
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${size}x${size}/apps
        RENAME pentobi.png)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-${size}.png
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${size}x${size}/mimetypes
        RENAME application-x-blokus-sgf.png)
endforeach()
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../icon/pentobi.svg
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
install(FILES application-x-blokus-sgf.svg
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/mimetypes)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.desktop
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install(FILES pentobi-mime.xml
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mime/packages)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.appdata.xml
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.6
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)