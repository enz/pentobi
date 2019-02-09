find_package(Gettext 0.18 REQUIRED)
find_program(ITSTOOL itstool)
if(NOT ITSTOOL)
    message(FATAL_ERROR "itstool not found")
endif()
find_program(XSLTPROC xsltproc)
if(NOT XSLTPROC)
    message(FATAL_ERROR "xsltproc not found")
endif()
find_path(DOCBOOK_XSL "manpages/docbook.xsl"
    HINTS "/usr/share/xml/docbook/stylesheet/docbook-xsl")
if(NOT DOCBOOK_XSL)
    message(FATAL_ERROR "docbook-xsl not found")
endif()

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/po/LINGUAS" linguas)
string(REGEX REPLACE "\n" ";" linguas "${linguas}")

set(man_files "${CMAKE_CURRENT_BINARY_DIR}/pentobi.6")
foreach(lang ${linguas})
    list(APPEND man_files "${CMAKE_CURRENT_BINARY_DIR}/${lang}/pentobi.6")
endforeach()

foreach(icon application-x-blokus-sgf application-x-blokus-sgf-16
        application-x-blokus-sgf-32 application-x-blokus-sgf-64)
    set(icon_svg "${CMAKE_CURRENT_SOURCE_DIR}/${icon}.svg")
    set(icon_png "${CMAKE_CURRENT_BINARY_DIR}/${icon}.png")
    add_custom_command(OUTPUT "${icon_png}"
        COMMAND convert-svg "${icon_svg}" "${icon_png}"
        DEPENDS "${icon_svg}")
    list(APPEND png_icons "${icon_png}")
endforeach()
foreach(icon pentobi pentobi-16 pentobi-32 pentobi-64)
  set(icon_svg "${CMAKE_CURRENT_SOURCE_DIR}/../icon/${icon}.svg")
  set(icon_png "${CMAKE_CURRENT_BINARY_DIR}/${icon}.png")
  add_custom_command(OUTPUT "${icon_png}"
    COMMAND convert-svg "${icon_svg}" "${icon_png}"
    DEPENDS "${icon_svg}")
  list(APPEND png_icons "${icon_png}")
endforeach()

add_custom_target(pentobi-unix ALL DEPENDS
    ${png_icons}
    "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.desktop"
    "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.appdata.xml"
    "${CMAKE_CURRENT_BINARY_DIR}/pentobi-mime.xml"
    "${man_files}"
    "${png_icons}"
    )

foreach(lang ${linguas})
    add_custom_command(OUTPUT ${lang}.mo
        COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" -o ${lang}.mo
        "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.po"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.po"
        )
    list(APPEND po_files po/${lang}.po)
    list(APPEND mo_files ${lang}.mo)
endforeach()

# Desktop entry

configure_file(io.sourceforge.pentobi.desktop.in
    io.sourceforge.pentobi.desktop.in.2 @ONLY)
add_custom_command(OUTPUT io.sourceforge.pentobi.desktop
    COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" --desktop
    -d "${CMAKE_CURRENT_SOURCE_DIR}/po"
    --template io.sourceforge.pentobi.desktop.in.2
    -o io.sourceforge.pentobi.desktop
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.desktop.in.2"
    ${po_files} "${CMAKE_CURRENT_SOURCE_DIR}/po/LINGUAS")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

# AppData

configure_file(io.sourceforge.pentobi.appdata.xml.in
    io.sourceforge.pentobi.appdata.xml.in.2 @ONLY)
add_custom_command(OUTPUT io.sourceforge.pentobi.appdata.xml
    COMMAND "${ITSTOOL}"
    -j io.sourceforge.pentobi.appdata.xml.in.2
    -o io.sourceforge.pentobi.appdata.xml ${mo_files}
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.appdata.xml.in.2"
    ${mo_files} "${CMAKE_CURRENT_SOURCE_DIR}/po/LINGUAS"
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.appdata.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)

# Shared MIME info

add_custom_command(OUTPUT pentobi-mime.xml
    COMMAND "${ITSTOOL}"
    -j "${CMAKE_CURRENT_SOURCE_DIR}/pentobi-mime.xml.in"
    -o pentobi-mime.xml ${mo_files}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/pentobi-mime.xml.in" ${mo_files}
    "${CMAKE_CURRENT_SOURCE_DIR}/po/LINGUAS"
    )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi-mime.xml"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mime/packages)

# Man page

configure_file(pentobi-manpage.docbook.in pentobi-manpage.docbook @ONLY)
add_custom_command(OUTPUT pentobi.6
    COMMAND "${XSLTPROC}" --nonet --path "${DOCBOOK_XSL}/manpages"
    "${CMAKE_CURRENT_SOURCE_DIR}/manpage.xsl" pentobi-manpage.docbook
    DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/manpage.xsl"
    "${CMAKE_CURRENT_BINARY_DIR}/pentobi-manpage.docbook"
    )
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi.6"
    DESTINATION "${CMAKE_INSTALL_MANDIR}/man6")
foreach(lang ${linguas})
    add_custom_command(OUTPUT ${lang}/pentobi-manpage.docbook
        COMMAND mkdir -p ${lang}
        COMMAND "${ITSTOOL}" -l ${lang} -m ${lang}.mo
        -o ${lang}/pentobi-manpage.docbook
        -i "${CMAKE_CURRENT_SOURCE_DIR}/manpage.its"
        pentobi-manpage.docbook
        DEPENDS ${lang}.mo "${CMAKE_CURRENT_BINARY_DIR}/pentobi-manpage.docbook"
        )
    add_custom_command(OUTPUT ${lang}/pentobi.6
        COMMAND "${XSLTPROC}"  --nonet --path "${DOCBOOK_XSL}/manpages"
        -o ${lang}/ "${CMAKE_CURRENT_SOURCE_DIR}/manpage.xsl"
        ${lang}/pentobi-manpage.docbook
        DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/manpage.xsl"
        "${CMAKE_CURRENT_BINARY_DIR}/${lang}/pentobi-manpage.docbook"
        )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${lang}/pentobi.6"
        DESTINATION "${CMAKE_INSTALL_MANDIR}/${lang}/man6")
endforeach()

# App and mimetype icons

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/mimetypes)
foreach(size 16 32 64)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi-${size}.png
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${size}x${size}/apps
        RENAME pentobi.png)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-${size}.png
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${size}x${size}/mimetypes
        RENAME application-x-blokus-sgf.png)
endforeach()
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../icon/pentobi.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
install(FILES application-x-blokus-sgf.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/mimetypes)

# User manual

install(DIRECTORY help DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
    FILES_MATCHING PATTERN "*.css" PATTERN "*.html" PATTERN "*.png"
    PATTERN "*.jpg")
