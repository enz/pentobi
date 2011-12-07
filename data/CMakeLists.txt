if(UNIX AND NOT APPLE)

  configure_file(pentobi.desktop.in pentobi.desktop @ONLY)
  configure_file(pentobi.thumbnailer.in pentobi.thumbnailer @ONLY)
  install(FILES pentobi.png DESTINATION share/icons/hicolor/48x48/apps)
  install(FILES pentobi.svg DESTINATION share/icons/hicolor/scalable/apps)
  install(FILES application-x-blokus-sgf.png
    DESTINATION share/icons/hicolor/48x48/mimetypes)
  install(FILES application-x-blokus-sgf.svg
    DESTINATION share/icons/hicolor/scalable/mimetypes)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.desktop
    DESTINATION share/applications)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.thumbnailer
    DESTINATION share/thumbnailers)
  install(FILES pentobi-mime.xml DESTINATION share/mime/packages)

  if(PENTOBI_REGISTER_GNOME2_THUMBNAILER)
    configure_file(pentobi.schemas.in pentobi.schemas @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.schemas
      DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/gconf/schemas)
  endif()

endif(UNIX AND NOT APPLE)
