set(CMAKE_AUTORCC ON)

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

qt_add_translation(pentobi_thumbnailer_QM
    i18n/de.ts
    i18n/es.ts
    i18n/ru.ts
    OPTIONS -removeidentical -nounfinished
    )
add_custom_command(
    OUTPUT "translations.qrc"
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/i18n/translations.qrc .
    DEPENDS i18n/translations.qrc ${pentobi_thumbnailer_QM}
    )

add_executable(pentobi-thumbnailer
    CreateThumbnail.h
    CreateThumbnail.cpp
    Main.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
    ${man_files}
    )

target_link_libraries(pentobi-thumbnailer pentobi_paint)

target_compile_definitions(pentobi-thumbnailer PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
    VERSION="${PENTOBI_VERSION}"
    )

configure_file(pentobi.thumbnailer.in pentobi.thumbnailer @ONLY)

install(TARGETS pentobi-thumbnailer)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.thumbnailer
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/thumbnailers)

# Man page

configure_file(pentobi-thumbnailer-manpage.docbook.in
    pentobi-thumbnailer-manpage.docbook @ONLY)
add_custom_command(OUTPUT pentobi-thumbnailer.6
    COMMAND "${XSLTPROC}" --nonet --novalid --path "${DOCBOOKXSL_DIR}/manpages"
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
        --path "${DOCBOOKXSL_DIR}/manpages" -o ${lang}/
        "${CMAKE_CURRENT_SOURCE_DIR}/../pentobi/unix/manpage.xsl"
        ${lang}/pentobi-thumbnailer-manpage.docbook
        DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/${lang}/pentobi-thumbnailer-manpage.docbook"
        ../pentobi/unix/manpage.xsl
        )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${lang}/pentobi-thumbnailer.6"
        DESTINATION "${CMAKE_INSTALL_MANDIR}/${lang}/man6")
endforeach()
