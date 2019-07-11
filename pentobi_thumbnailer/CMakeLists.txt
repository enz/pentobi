find_package(Qt5Core 5.11 REQUIRED)
find_package(Qt5LinguistTools 5.11 REQUIRED)
find_package(Gettext 0.18 REQUIRED)
find_program(ITSTOOL itstool)
if(NOT ITSTOOL)
    message(FATAL_ERROR "itstool not found")
endif()
find_program(XSLTPROC xsltproc)
if(NOT XSLTPROC)
    message(FATAL_ERROR "xsltproc not found")
endif()
find_path(DOCBOOK_XSL manpages/docbook.xsl
    HINTS /usr/share/xml/docbook/stylesheet/docbook-xsl
    /usr/local/share/xml/docbook/stylesheet/docbook-xsl
    /usr/share/xsl/docbook /usr/local/share/xsl/docbook
    )
if(NOT DOCBOOK_XSL)
    message(FATAL_ERROR "docbook-xsl not found")
endif()

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/po/LINGUAS" linguas)
string(REGEX REPLACE "\n" ";" linguas "${linguas}")

set(man_files "pentobi-thumbnailer.6")
foreach(lang ${linguas})
    list(APPEND man_files ${lang}/pentobi-thumbnailer.6)
endforeach()

foreach(lang ${linguas})
    add_custom_command(OUTPUT ${lang}.mo
        COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" -o ${lang}.mo
        "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.po"
        DEPENDS po/${lang}.po
        )
    list(APPEND po_files po/${lang}.po)
    list(APPEND mo_files ${lang}.mo)
endforeach()

qt5_add_translation(pentobi_thumbnailer_QM
    i18n/de.ts
    i18n/es.ts
    OPTIONS -removeidentical -nounfinished
    )
add_custom_command(
    OUTPUT "translations.qrc"
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/i18n/translations.qrc .
    DEPENDS i18n/translations.qrc ${pentobi_thumbnailer_QM}
    )

qt5_add_resources(pentobi_thumbnailer_RC_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
    )

add_executable(pentobi-thumbnailer
    Main.cpp
    ${pentobi_thumbnailer_RC_SRCS}
    ${man_files}
    )

target_link_libraries(pentobi-thumbnailer pentobi_thumbnail)

target_compile_definitions(pentobi-thumbnailer PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x051200
    VERSION="${PENTOBI_VERSION}"
    )

configure_file(pentobi.thumbnailer.in pentobi.thumbnailer @ONLY)

install(TARGETS pentobi-thumbnailer DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.thumbnailer
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/thumbnailers)

# Man page

configure_file(pentobi-thumbnailer-manpage.docbook.in
    pentobi-thumbnailer-manpage.docbook @ONLY)
add_custom_command(OUTPUT pentobi-thumbnailer.6
    COMMAND "${XSLTPROC}" --nonet --novalid --path "${DOCBOOK_XSL}/manpages"
    "${CMAKE_CURRENT_SOURCE_DIR}/../pentobi/unix/manpage.xsl"
    pentobi-thumbnailer-manpage.docbook
    DEPENDS
    "${CMAKE_CURRENT_BINARY_DIR}/pentobi-thumbnailer-manpage.docbook"
    ../pentobi/unix/manpage.xsl
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pentobi-thumbnailer.6"
    DESTINATION "${CMAKE_INSTALL_MANDIR}/man6")
foreach(lang ${linguas})
    add_custom_command(OUTPUT ${lang}/pentobi-thumbnailer-manpage.docbook
        COMMAND ${CMAKE_COMMAND} -E make_directory ${lang}
        COMMAND "${ITSTOOL}" -l ${lang} -m ${lang}.mo
        -o ${lang}/pentobi-thumbnailer-manpage.docbook
        -i "${CMAKE_CURRENT_SOURCE_DIR}/../pentobi/unix/manpage.its"
        pentobi-thumbnailer-manpage.docbook
        DEPENDS ${lang}.mo "${CMAKE_CURRENT_BINARY_DIR}/pentobi-thumbnailer-manpage.docbook"
        )
    add_custom_command(OUTPUT ${lang}/pentobi-thumbnailer.6
        COMMAND "${XSLTPROC}" --nonet --novalid
        --path "${DOCBOOK_XSL}/manpages" -o ${lang}/
        "${CMAKE_CURRENT_SOURCE_DIR}/../pentobi/unix/manpage.xsl"
        ${lang}/pentobi-thumbnailer-manpage.docbook
        DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/${lang}/pentobi-thumbnailer-manpage.docbook"
        ../pentobi/unix/manpage.xsl
        )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${lang}/pentobi-thumbnailer.6"
        DESTINATION "${CMAKE_INSTALL_MANDIR}/${lang}/man6")
endforeach()
