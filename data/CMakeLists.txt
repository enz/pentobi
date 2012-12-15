if(UNIX AND NOT APPLE)

  configure_file(pentobi.desktop.in pentobi.desktop @ONLY)
  configure_file(pentobi.thumbnailer.in pentobi.thumbnailer @ONLY)
  install(FILES ../src/pentobi/icons/pentobi.png
    DESTINATION share/icons/hicolor/48x48/apps)
  install(FILES ../src/pentobi/icons/pentobi-16.png
    DESTINATION share/icons/hicolor/16x16/apps RENAME pentobi.png)
  install(FILES ../src/pentobi/icons/pentobi-32.png
    DESTINATION share/icons/hicolor/32x32/apps RENAME pentobi.png)
  install(FILES ../src/pentobi/icons/pentobi.svg
    DESTINATION share/icons/hicolor/scalable/apps)
  install(FILES application-x-blokus-sgf.png
    DESTINATION share/icons/hicolor/48x48/mimetypes)
  install(FILES application-x-blokus-sgf-16.png
    DESTINATION share/icons/hicolor/16x16/mimetypes
    RENAME application-x-blokus-sgf.png)
  install(FILES application-x-blokus-sgf.svg
    DESTINATION share/icons/hicolor/scalable/mimetypes)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.desktop
    DESTINATION share/applications)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.thumbnailer
    DESTINATION share/thumbnailers)
  install(FILES pentobi-mime.xml DESTINATION share/mime/packages)

endif(UNIX AND NOT APPLE)
