if(USE_QT5)
  get_target_property(QMAKE Qt5::qmake LOCATION)
  execute_process(COMMAND ${QMAKE} -query QT_INSTALL_TRANSLATIONS
    OUTPUT_VARIABLE QT_TRANSLATIONS_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

configure_file(install.nsis.in install.nsis @ONLY)
