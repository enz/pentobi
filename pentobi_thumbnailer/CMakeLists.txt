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

set(man_files "${CMAKE_CURRENT_BINARY_DIR}/pentobi-thumbnailer.6")
foreach(lang ${linguas})
    list(APPEND man_files
        "${CMAKE_CURRENT_BINARY_DIR}/manpage-${lang}/pentobi-thumbnailer.6")
endforeach()

foreach(lang ${linguas})
    add_custom_command(OUTPUT ${lang}.mo
        COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" -o ${lang}.mo
        "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.po"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.po"
        )
    list(APPEND po_files po/${lang}.po)
    list(APPEND mo_files ${lang}.mo)
endforeach()

add_executable(pentobi-thumbnailer Main.cpp ${man_files})

target_link_libraries(pentobi-thumbnailer pentobi_thumbnail)

configure_file(pentobi.thumbnailer.in pentobi.thumbnailer @ONLY)
configure_file(pentobi-thumbnailer.6.in pentobi-thumbnailer.6 @ONLY)

install(TARGETS pentobi-thumbnailer DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.thumbnailer
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/thumbnailers)

# Man page

configure_file(pentobi-thumbnailer-manpage.docbook.in
    pentobi-thumbnailer-manpage.docbook @ONLY)
add_custom_command(OUTPUT pentobi-thumbnailer.6
    COMMAND "${XSLTPROC}" --nonet --path "${DOCBOOK_XSL}/manpages"
    "${CMAKE_CURRENT_SOURCE_DIR}/../pentobi/unix/manpage.xsl"
    pentobi-thumbnailer-manpage.docbook
    DEPENDS
    "${CMAKE_CURRENT_BINARY_DIR}/pentobi-thumbnailer-manpage.docbook"
    "${CMAKE_CURRENT_SOURCE_DIR}/../pentobi/unix/manpage.xsl"
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi-thumbnailer.6"
    DESTINATION "${CMAKE_INSTALL_MANDIR}/man6")
foreach(lang ${linguas})
    add_custom_command(OUTPUT manpage-${lang}/pentobi-thumbnailer-manpage.docbook
        COMMAND mkdir -p manpage-${lang}/
        COMMAND "${ITSTOOL}" -l ${lang} -m ${lang}.mo
        -o manpage-${lang}/pentobi-thumbnailer-manpage.docbook
        -i "${CMAKE_CURRENT_SOURCE_DIR}/../pentobi/unix/manpage.its"
        pentobi-thumbnailer-manpage.docbook
        DEPENDS ${lang}.mo "${CMAKE_CURRENT_BINARY_DIR}/pentobi-thumbnailer-manpage.docbook"
        )
    add_custom_command(OUTPUT manpage-${lang}/pentobi-thumbnailer.6
        COMMAND "${XSLTPROC}"  --nonet --path "${DOCBOOK_XSL}/manpages"
        -o manpage-${lang}/
        "${CMAKE_CURRENT_SOURCE_DIR}/../pentobi/unix/manpage.xsl"
        manpage-${lang}/pentobi-thumbnailer-manpage.docbook
        DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/manpage-${lang}/pentobi-thumbnailer-manpage.docbook"
        "${CMAKE_CURRENT_SOURCE_DIR}/../pentobi/unix/manpage.xsl"
        )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/manpage-${lang}/pentobi-thumbnailer.6"
        DESTINATION "${CMAKE_INSTALL_MANDIR}/${lang}/man6")
endforeach()

