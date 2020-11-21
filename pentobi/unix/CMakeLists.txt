# The AppData ITS file (named metainfo.its or appdata.its) is usually installed
# in the gettext folder but provided but the appdata package. We want to check
# its existence already at configuration time.
get_filename_component(GETTEXT_BIN_DIR ${GETTEXT_MSGFMT_EXECUTABLE} DIRECTORY)
get_filename_component(GETTEXT_INSTALL_DIR ${GETTEXT_BIN_DIR} DIRECTORY)
find_file(METAINFO_ITS NAMES metainfo.its appdata.its
    HINTS ${GETTEXT_INSTALL_DIR}/share/gettext/its
    )
if(NOT METAINFO_ITS)
    message(FATAL_ERROR
        "metainfo.its not found. Install appstream and/or use"
        " -DMETAINFO_ITS=<file> to define the location of the metainfo.its"
        " or appdata.its file."
        )
endif()

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

add_custom_command(OUTPUT pentobi.png
    COMMAND "${RSVG_CONVERT}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../icon/pentobi-48.svg"
    > pentobi.png
    DEPENDS ../icon/pentobi-48.svg)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps)
install(FILES ../icon/pentobi-128.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps
    RENAME pentobi.svg)

add_custom_command(OUTPUT application-x-blokus-sgf.png
    COMMAND "${RSVG_CONVERT}"
    "${CMAKE_CURRENT_SOURCE_DIR}/application-x-blokus-sgf-48.svg"
    > application-x-blokus-sgf.png
    DEPENDS application-x-blokus-sgf-48.svg)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf.png"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/mimetypes)
install(FILES application-x-blokus-sgf-128.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/mimetypes
    RENAME application-x-blokus-sgf.svg)

# Desktop entry

configure_file(io.sourceforge.pentobi.desktop.in
    io.sourceforge.pentobi.desktop.in.2 @ONLY)
add_custom_command(OUTPUT io.sourceforge.pentobi.desktop
    COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" --desktop
    -d "${CMAKE_CURRENT_SOURCE_DIR}/po"
    --template io.sourceforge.pentobi.desktop.in.2
    -o io.sourceforge.pentobi.desktop
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.desktop.in.2"
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
    ${METAINFO_ITS} ${po_files} po/LINGUAS
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
    COMMAND "${XSLTPROC}" --nonet --novalid --path "${DOCBOOKXSL_DIR}/manpages"
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
        COMMAND "${XSLTPROC}" --nonet --novalid
        --path "${DOCBOOKXSL_DIR}/manpages" -o ${lang}/
        "${CMAKE_CURRENT_SOURCE_DIR}/manpage.xsl"
        ${lang}/pentobi-manpage.docbook
        DEPENDS manpage.xsl ${lang}/pentobi-manpage.docbook
        )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${lang}/pentobi.6"
        DESTINATION "${CMAKE_INSTALL_MANDIR}/${lang}/man6")
endforeach()

# User manual

install(DIRECTORY help DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
    FILES_MATCHING PATTERN "*.css" PATTERN "*.html" PATTERN "*.png"
    PATTERN "*.jpg")

# Target

add_custom_target(pentobi-unix ALL DEPENDS
    io.sourceforge.pentobi.desktop
    io.sourceforge.pentobi.appdata.translated.xml
    pentobi-mime.xml
    pentobi.png
    application-x-blokus-sgf.png
    ${man_files}
    )
