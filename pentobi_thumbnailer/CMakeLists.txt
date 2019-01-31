add_executable(pentobi-thumbnailer Main.cpp)

target_link_libraries(pentobi-thumbnailer pentobi_thumbnail)

configure_file(pentobi.thumbnailer.in pentobi.thumbnailer @ONLY)
configure_file(pentobi-thumbnailer.6.in pentobi-thumbnailer.6 @ONLY)

install(TARGETS pentobi-thumbnailer DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.thumbnailer
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/thumbnailers)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi-thumbnailer.6
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)
