configure_file(pentobi.desktop.in pentobi.desktop @ONLY)
configure_file(pentobi.thumbnailer.in pentobi.thumbnailer @ONLY)
install(FILES ../src/pentobi/icons/pentobi.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps)
install(FILES ../src/pentobi/icons/pentobi-16.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/apps
  RENAME pentobi.png)
install(FILES ../src/pentobi/icons/pentobi-32.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps
  RENAME pentobi.png)
install(FILES ../src/pentobi/icons/pentobi.svg
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
install(FILES application-x-blokus-sgf.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/mimetypes)
install(FILES application-x-blokus-sgf-16.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/mimetypes
  RENAME application-x-blokus-sgf.png)
install(FILES application-x-blokus-sgf.svg
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/mimetypes)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.desktop
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.thumbnailer
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/thumbnailers)
install(FILES pentobi-mime.xml
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mime/packages)
