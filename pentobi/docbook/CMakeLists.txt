file(READ "${CMAKE_CURRENT_SOURCE_DIR}/po/LINGUAS" linguas)
string(REGEX REPLACE "\n" ";" linguas "${linguas}")

set(DOCBOOK_SRC
    custom.xsl
    index.docbook
    figures/analysis.jpg
    figures/board_callisto.png
    figures/board_classic.png
    figures/board_duo.png
    figures/board_gembloq.png
    figures/board_nexos.png
    figures/board_trigon.jpg
    figures/pieces_callisto.png
    figures/pieces_gembloq.jpg
    figures/pieces_junior.png
    figures/pieces_nexos.png
    figures/pieces.png
    figures/pieces_trigon.jpg
    figures/position_callisto.png
    figures/position_classic.png
    figures/position_duo.png
    figures/position_gembloq.png
    figures/position_nexos.png
    figures/position_trigon.jpg
    figures/rating.jpg
    figures/stylesheet.css
    )

foreach(lang ${linguas})
    add_custom_command(OUTPUT ${lang}.mo
        COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" -o ${lang}.mo
        "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.po"
        DEPENDS po/${lang}.po
        )
    list(APPEND index_files "help/${lang}/index.html")
    add_custom_command(OUTPUT "index_${lang}.docbook"
        COMMAND "${ITSTOOL}" -m ${lang}.mo -o "index_${lang}.docbook"
        "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook"
        DEPENDS index.docbook ${lang}.mo
        )
    add_custom_command(OUTPUT "help/${lang}/index.html"
        COMMAND "${XSLTPROC}" --nonet --novalid --path "${DOCBOOKXSL_DIR}/html"
        --stringparam l10n.gentext.language ${lang}
        -o "help/${lang}/" "${CMAKE_CURRENT_SOURCE_DIR}/custom.xsl"
        index_${lang}.docbook
        DEPENDS "index_${lang}.docbook" ${DOCBOOK_SRC}
        )
endforeach()

add_custom_command(OUTPUT "help/C/index.html"
    COMMAND "${XSLTPROC}" --nonet --novalid --path "${DOCBOOKXSL_DIR}/html"
    -o "help/C/" "${CMAKE_CURRENT_SOURCE_DIR}/custom.xsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook"
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/figures/*.png" "help/C/"
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/figures/*.jpg" "help/C/"
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/figures/stylesheet.css" "help/C/"
    DEPENDS ${DOCBOOK_SRC}
    )

add_custom_target(pentobi-help ALL DEPENDS "help/C/index.html" ${index_files})

if(NOT ANDROID)
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/help"
        DESTINATION ${CMAKE_INSTALL_FULL_DOCDIR} FILES_MATCHING PATTERN "*.css"
        PATTERN "*.html" PATTERN "*.png" PATTERN "*.jpg")
endif()
