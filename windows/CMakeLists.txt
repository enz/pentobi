# Build the NSIS installer
# We assume dynamic linking and add a custom target that runs makensis and
# uses windeployqt to include the Qt libraries.

get_target_property(QMAKE Qt5::qmake LOCATION)
find_program(WINDEPLOYQT windeployqt.exe HINTS "${QMAKE}")

set(X86 "(x86)")
find_program(MAKENSIS makensis
  PATHS "$ENV{ProgramFiles}\\NSIS" "$ENV{ProgramFiles${X86}}\\NSIS")

file(MAKE_DIRECTORY deploy)
add_custom_target(nsis
  COMMAND del /s /q deploy
  COMMAND ${WINDEPLOYQT} --dir deploy --release --no-svg $<TARGET_FILE:pentobi>
  COMMAND ${MAKENSIS} install.nsis
)

configure_file(install.nsis.in install.nsis @ONLY)
