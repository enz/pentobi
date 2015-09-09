# Build the NSIS installer
# With Qt4, we assume a statically linked binary of Pentobi and only
# generate install.nsis. which can be used for makensis.
# With Qt5, we assume dynamic linking and add a custom target that runs
# makensis and uses windeployqt to include the Qt libraries.

if(USE_QT5)
  get_target_property(QMAKE Qt5::qmake LOCATION)
  execute_process(COMMAND ${QMAKE} -query QT_INSTALL_TRANSLATIONS
    OUTPUT_VARIABLE QT_TRANSLATIONS_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
  find_program(WINDEPLOYQT windeployqt.exe HINTS "${QMAKE}")

  set(X86 "(x86)")
  find_program(MAKENSIS makensis
    PATHS "$ENV{ProgramFiles}\\NSIS" "$ENV{ProgramFiles${X86}}\\NSIS")

  set(INCLUDE_RUNTIME_QT
    "!include \"${CMAKE_SOURCE_DIR}\\windows\\RuntimeQt.nsh\"")

  file(MAKE_DIRECTORY deploy)
  add_custom_target(nsis
    COMMAND del /s /q deploy
    COMMAND ${WINDEPLOYQT} --dir deploy --release --no-svg --no-translations
      $<TARGET_FILE:pentobi>
    COMMAND ${MAKENSIS} install.nsis
  )
endif()

configure_file(install.nsis.in install.nsis @ONLY)
