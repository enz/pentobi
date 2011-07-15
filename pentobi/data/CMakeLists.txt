if(UNIX AND NOT APPLE)
  configure_file(pentobi.desktop.in pentobi.desktop @ONLY)
  configure_file(pentobi.schemas.in pentobi.schemas @ONLY)
  install(FILES pentobi.png DESTINATION share/icons/hicolor/48x48/apps)
  install(FILES application-x-blokus-sgf.png
    DESTINATION share/icons/hicolor/48x48/mimetypes)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.desktop
    DESTINATION share/applications)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.schemas
    DESTINATION etc/gconf/schemas)
  install(FILES pentobi-mime.xml DESTINATION share/mime/packages)
endif()
