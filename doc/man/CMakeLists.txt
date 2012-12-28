configure_file(pentobi.6.in pentobi.6 @ONLY)
configure_file(pentobi-thumbnailer.6.in pentobi-thumbnailer.6 @ONLY)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/pentobi.6
  ${CMAKE_CURRENT_BINARY_DIR}/pentobi-thumbnailer.6
  DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)
