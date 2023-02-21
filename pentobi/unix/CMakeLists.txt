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
    list(APPEND mo_files ${lang}.mo)
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
    add_custom_command(OUTPUT application-x-blokus-sgf-${sz}.png
        COMMAND "${RSVG_CONVERT}"
        "${CMAKE_CURRENT_SOURCE_DIR}/application-x-blokus-sgf-${sz}.svg"
        > application-x-blokus-sgf-${sz}.png
        DEPENDS application-x-blokus-sgf-${sz}.svg)
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-${sz}.png"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${sz}x${sz}/mimetypes
        RENAME application-x-blokus-sgf.png)
endforeach()
add_custom_command(OUTPUT application-x-blokus-sgf-24.png
    COMMAND "${RSVG_CONVERT}" -z 0.5
    "${CMAKE_CURRENT_SOURCE_DIR}/application-x-blokus-sgf-48.svg"
    > application-x-blokus-sgf-24.png
    DEPENDS application-x-blokus-sgf-48.svg)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-24.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/24x24/mimetypes
    RENAME application-x-blokus-sgf.png)
add_custom_command(OUTPUT application-x-blokus-sgf-32.png
    COMMAND "${RSVG_CONVERT}" -z 0.25
    "${CMAKE_CURRENT_SOURCE_DIR}/application-x-blokus-sgf-128.svg"
    > application-x-blokus-sgf-32.png
    DEPENDS application-x-blokus-sgf-128.svg)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-32.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/mimetypes
    RENAME application-x-blokus-sgf.png)
add_custom_command(OUTPUT application-x-blokus-sgf-64.png
    COMMAND "${RSVG_CONVERT}" -z 0.5
    "${CMAKE_CURRENT_SOURCE_DIR}/application-x-blokus-sgf-128.svg"
    > application-x-blokus-sgf-64.png
    DEPENDS application-x-blokus-sgf-128.svg)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-64.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/mimetypes
    RENAME application-x-blokus-sgf.png)
add_custom_command(OUTPUT application-x-blokus-sgf-256.png
    COMMAND "${RSVG_CONVERT}" -z 2
    "${CMAKE_CURRENT_SOURCE_DIR}/application-x-blokus-sgf-128.svg"
    > application-x-blokus-sgf-256.png
    DEPENDS application-x-blokus-sgf-128.svg)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-256.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/mimetypes
    RENAME application-x-blokus-sgf.png)
install(FILES application-x-blokus-sgf-128.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/mimetypes
    RENAME application-x-blokus-sgf.svg)

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

configure_file(io.sourceforge.pentobi.appdata.xml.in
    io.sourceforge.pentobi.appdata.xml @ONLY)
add_custom_command(OUTPUT io.sourceforge.pentobi.appdata.translated.xml
    COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" --xml
    -d "${CMAKE_CURRENT_SOURCE_DIR}/po"
    --template io.sourceforge.pentobi.appdata.xml
    -o io.sourceforge.pentobi.appdata.translated.xml
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.appdata.xml"
    ${po_files} po/LINGUAS
    )

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.appdata.translated.xml"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/metainfo"
    RENAME io.sourceforge.pentobi.appdata.xml
    )

# Shared MIME info

# Use itstool until shared-mime-info includes its/loc files for gettext (see
# also https://gitlab.freedesktop.org/xdg/shared-mime-info/merge_requests/4)
add_custom_command(OUTPUT pentobi-mime.xml
    COMMAND "${ITSTOOL}"
    -j "${CMAKE_CURRENT_SOURCE_DIR}/pentobi-mime.xml.in"
    -o pentobi-mime.xml ${mo_files}
    DEPENDS pentobi-mime.xml.in ${mo_files} po/LINGUAS
    )
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi-mime.xml"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/mime/packages")

# Man page

set(man_files "pentobi.6")
foreach(lang ${linguas})
    list(APPEND man_files "${lang}/pentobi.6")
endforeach()

configure_file(pentobi-manpage.docbook.in pentobi-manpage.docbook @ONLY)
add_custom_command(OUTPUT pentobi.6
    COMMAND "${XSLTPROC}" --nonet
    "${CMAKE_CURRENT_SOURCE_DIR}/manpage.xsl" pentobi-manpage.docbook
    DEPENDS manpage.xsl "${CMAKE_CURRENT_BINARY_DIR}/pentobi-manpage.docbook"
    )
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi.6"
    DESTINATION "${CMAKE_INSTALL_MANDIR}/man6")
foreach(lang ${linguas})
    add_custom_command(OUTPUT ${lang}/pentobi-manpage.docbook
        COMMAND ${CMAKE_COMMAND} -E make_directory ${lang}
        COMMAND "${ITSTOOL}" -l ${lang} -m ${lang}.mo
        -o ${lang}/pentobi-manpage.docbook
        -i "${CMAKE_CURRENT_SOURCE_DIR}/manpage.its"
        pentobi-manpage.docbook
        DEPENDS ${lang}.mo "${CMAKE_CURRENT_BINARY_DIR}/pentobi-manpage.docbook"
        )
    add_custom_command(OUTPUT ${lang}/pentobi.6
        COMMAND "${XSLTPROC}" --novalid
        -o ${lang}/ "${CMAKE_CURRENT_SOURCE_DIR}/manpage.xsl"
        ${lang}/pentobi-manpage.docbook
        DEPENDS manpage.xsl ${lang}/pentobi-manpage.docbook
        )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${lang}/pentobi.6"
        DESTINATION "${CMAKE_INSTALL_MANDIR}/${lang}/man6")
endforeach()

# Target

add_custom_target(pentobi-unix ALL DEPENDS
    io.sourceforge.pentobi.desktop
    io.sourceforge.pentobi.appdata.translated.xml
    pentobi-mime.xml
    pentobi-16.png
    pentobi-22.png
    pentobi-24.png
    pentobi-32.png
    pentobi-48.png
    pentobi-64.png
    pentobi-128.png
    pentobi-256.png
    application-x-blokus-sgf-16.png
    application-x-blokus-sgf-22.png
    application-x-blokus-sgf-24.png
    application-x-blokus-sgf-32.png
    application-x-blokus-sgf-48.png
    application-x-blokus-sgf-64.png
    application-x-blokus-sgf-128.png
    application-x-blokus-sgf-256.png
    ${man_files}
    )
