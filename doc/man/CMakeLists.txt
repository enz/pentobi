configure_file(pentobi.6.in pentobi.6 @ONLY)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/pentobi.6
  DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)

if(PENTOBI_BUILD_THUMBNAILER)
    configure_file(pentobi-thumbnailer.6.in pentobi-thumbnailer.6 @ONLY)
    install(FILES
      ${CMAKE_CURRENT_BINARY_DIR}/pentobi-thumbnailer.6
      DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)
endif()
