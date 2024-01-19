# Binary gettext files

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/po/LINGUAS" linguas)
string(REGEX REPLACE "\n" ";" linguas "${linguas}")

foreach(lang ${linguas})
    add_custom_command(OUTPUT ${lang}.mo
        COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" -o ${lang}.mo
        "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.po"
        DEPENDS po/${lang}.po
        )
    list(APPEND po_files po/${lang}.po)
endforeach()

# Icons

foreach(sz 16 22 48 128)
    add_custom_command(OUTPUT pentobi-${sz}.png
        COMMAND "${RSVG_CONVERT}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../icon/pentobi-${sz}.svg"
        > pentobi-${sz}.png
        DEPENDS ../icon/pentobi-${sz}.svg)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi-${sz}.png"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${sz}x${sz}/apps
        RENAME pentobi.png)
endforeach()
add_custom_command(OUTPUT pentobi-24.png
    COMMAND "${RSVG_CONVERT}" -z 0.5
    "${CMAKE_CURRENT_SOURCE_DIR}/../icon/pentobi-48.svg"
    > pentobi-24.png
    DEPENDS ../icon/pentobi-48.svg)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi-24.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/24x24/apps
    RENAME pentobi.png)
add_custom_command(OUTPUT pentobi-32.png
    COMMAND "${RSVG_CONVERT}" -z 0.25
    "${CMAKE_CURRENT_SOURCE_DIR}/../icon/pentobi-128.svg"
    > pentobi-32.png
    DEPENDS ../icon/pentobi-128.svg)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi-32.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps
    RENAME pentobi.png)
add_custom_command(OUTPUT pentobi-64.png
    COMMAND "${RSVG_CONVERT}" -z 0.5
    "${CMAKE_CURRENT_SOURCE_DIR}/../icon/pentobi-128.svg"
    > pentobi-64.png
    DEPENDS ../icon/pentobi-128.svg)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi-64.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps
    RENAME pentobi.png)
add_custom_command(OUTPUT pentobi-256.png
    COMMAND "${RSVG_CONVERT}" -z 2
    "${CMAKE_CURRENT_SOURCE_DIR}/../icon/pentobi-128.svg"
    > pentobi-256.png
    DEPENDS ../icon/pentobi-128.svg)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi-256.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps
    RENAME pentobi.png)
install(FILES ../icon/pentobi-128.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps
    RENAME pentobi.svg)

foreach(sz 16 22 48 128)
    add_custom_command(
        OUTPUT io.sourceforge.pentobi.application-x-blokus-sgf-${sz}.png
        COMMAND "${RSVG_CONVERT}"
        "${CMAKE_CURRENT_SOURCE_DIR}/io.sourceforge.pentobi.application-x-blokus-sgf-${sz}.svg"
        > io.sourceforge.pentobi.application-x-blokus-sgf-${sz}.png
        DEPENDS io.sourceforge.pentobi.application-x-blokus-sgf-${sz}.svg)
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.application-x-blokus-sgf-${sz}.png"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${sz}x${sz}/mimetypes
        RENAME io.sourceforge.pentobi.application-x-blokus-sgf.png)
endforeach()
add_custom_command(
    OUTPUT io.sourceforge.pentobi.application-x-blokus-sgf-24.png
    COMMAND "${RSVG_CONVERT}" -z 0.5
    "${CMAKE_CURRENT_SOURCE_DIR}/io.sourceforge.pentobi.application-x-blokus-sgf-48.svg"
    > io.sourceforge.pentobi.application-x-blokus-sgf-24.png
    DEPENDS io.sourceforge.pentobi.application-x-blokus-sgf-48.svg)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.application-x-blokus-sgf-24.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/24x24/mimetypes
    RENAME io.sourceforge.pentobi.application-x-blokus-sgf.png)
add_custom_command(
    OUTPUT io.sourceforge.pentobi.application-x-blokus-sgf-32.png
    COMMAND "${RSVG_CONVERT}" -z 0.25
    "${CMAKE_CURRENT_SOURCE_DIR}/io.sourceforge.pentobi.application-x-blokus-sgf-128.svg"
    > io.sourceforge.pentobi.application-x-blokus-sgf-32.png
    DEPENDS io.sourceforge.pentobi.application-x-blokus-sgf-128.svg)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.application-x-blokus-sgf-32.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/mimetypes
    RENAME io.sourceforge.pentobi.application-x-blokus-sgf.png)
add_custom_command(
    OUTPUT io.sourceforge.pentobi.application-x-blokus-sgf-64.png
    COMMAND "${RSVG_CONVERT}" -z 0.5
    "${CMAKE_CURRENT_SOURCE_DIR}/io.sourceforge.pentobi.application-x-blokus-sgf-128.svg"
    > io.sourceforge.pentobi.application-x-blokus-sgf-64.png
    DEPENDS io.sourceforge.pentobi.application-x-blokus-sgf-128.svg)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.application-x-blokus-sgf-64.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/mimetypes
    RENAME io.sourceforge.pentobi.application-x-blokus-sgf.png)
add_custom_command(
    OUTPUT io.sourceforge.pentobi.application-x-blokus-sgf-256.png
    COMMAND "${RSVG_CONVERT}" -z 2
    "${CMAKE_CURRENT_SOURCE_DIR}/io.sourceforge.pentobi.application-x-blokus-sgf-128.svg"
    > io.sourceforge.pentobi.application-x-blokus-sgf-256.png
    DEPENDS io.sourceforge.pentobi.application-x-blokus-sgf-128.svg)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.application-x-blokus-sgf-256.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/mimetypes
    RENAME io.sourceforge.pentobi.application-x-blokus-sgf.png)
install(
    FILES io.sourceforge.pentobi.application-x-blokus-sgf-128.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/mimetypes
    RENAME io.sourceforge.pentobi.application-x-blokus-sgf.svg)

# Desktop entry

add_custom_command(OUTPUT io.sourceforge.pentobi.desktop
    COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" --desktop
    -d "${CMAKE_CURRENT_SOURCE_DIR}/po"
    --template "${CMAKE_CURRENT_SOURCE_DIR}/io.sourceforge.pentobi.desktop.in"
    -o io.sourceforge.pentobi.desktop
    DEPENDS io.sourceforge.pentobi.desktop.in
    ${po_files} po/LINGUAS)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.desktop
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")

# AppData

add_custom_command(OUTPUT io.sourceforge.pentobi.appdata.xml
    COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" --xml
    -d "${CMAKE_CURRENT_SOURCE_DIR}/po"
    --template "${CMAKE_CURRENT_SOURCE_DIR}/io.sourceforge.pentobi.appdata.xml.in"
    -o io.sourceforge.pentobi.appdata.xml
    DEPENDS io.sourceforge.pentobi.appdata.xml.in ${po_files} po/LINGUAS
)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.appdata.xml"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/metainfo"
)

# Shared MIME info

add_custom_command(OUTPUT io.sourceforge.pentobi.xml
    COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" --xml
    -d "${CMAKE_CURRENT_SOURCE_DIR}/po"
    --template "${CMAKE_CURRENT_SOURCE_DIR}/io.sourceforge.pentobi.xml.in"
    -o io.sourceforge.pentobi.xml
    DEPENDS io.sourceforge.pentobi.xml.in ${po_files} po/LINGUAS
    )
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.xml"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/mime/packages")

# Target

add_custom_target(pentobi-unix ALL DEPENDS
    io.sourceforge.pentobi.appdata.xml
    io.sourceforge.pentobi.application-x-blokus-sgf-16.png
    io.sourceforge.pentobi.application-x-blokus-sgf-22.png
    io.sourceforge.pentobi.application-x-blokus-sgf-24.png
    io.sourceforge.pentobi.application-x-blokus-sgf-32.png
    io.sourceforge.pentobi.application-x-blokus-sgf-48.png
    io.sourceforge.pentobi.application-x-blokus-sgf-64.png
    io.sourceforge.pentobi.application-x-blokus-sgf-128.png
    io.sourceforge.pentobi.application-x-blokus-sgf-256.png
    io.sourceforge.pentobi.desktop
    io.sourceforge.pentobi.xml
    pentobi-16.png
    pentobi-22.png
    pentobi-24.png
    pentobi-32.png
    pentobi-48.png
    pentobi-64.png
    pentobi-128.png
    pentobi-256.png
    )
