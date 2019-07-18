find_package(Gettext 0.18 REQUIRED)
find_program(ITSTOOL itstool)
if(NOT ITSTOOL)
    message(FATAL_ERROR "itstool not found")
endif()
find_program(XSLTPROC xsltproc)
if(NOT XSLTPROC)
    message(FATAL_ERROR "xsltproc not found")
endif()
find_path(DOCBOOK_XSL html/chunk.xsl
    HINTS /usr/share/xml/docbook/stylesheet/docbook-xsl
    /usr/local/share/xml/docbook/stylesheet/docbook-xsl
    /usr/share/xsl/docbook /usr/local/share/xsl/docbook
    /usr/share/xml/docbook/stylesheet/nwalsh/current
    /usr/share/sgml/docbook/xsl-ns-stylesheets
    )
if(NOT DOCBOOK_XSL)
    message(FATAL_ERROR
        "docbook-xsl not found. Install docbook-xsl and/or use"
        " -DDOCBOOK_XSL=<dir> to define the location of the docbook-xsl"
        " directory.")
endif()

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
    list(APPEND index_files "help/${lang}/pentobi/index.html")
    add_custom_command(OUTPUT "index_${lang}.docbook"
        COMMAND "${ITSTOOL}" -m ${lang}.mo -o "index_${lang}.docbook"
        "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook"
        DEPENDS index.docbook ${lang}.mo
        )
    add_custom_command(OUTPUT "help/${lang}/pentobi/index.html"
        COMMAND "${XSLTPROC}" --nonet --novalid --path "${DOCBOOK_XSL}/html"
        --stringparam l10n.gentext.language ${lang}
        -o "help/${lang}/pentobi/" "${CMAKE_CURRENT_SOURCE_DIR}/custom.xsl"
        index_${lang}.docbook
        DEPENDS "index_${lang}.docbook" ${DOCBOOK_SRC}
        )
endforeach()

add_custom_command(OUTPUT "help/C/pentobi/index.html"
    COMMAND "${XSLTPROC}" --nonet --novalid --path "${DOCBOOK_XSL}/html"
    -o "help/C/pentobi/" "${CMAKE_CURRENT_SOURCE_DIR}/custom.xsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook"
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/figures/*.png" "help/C/pentobi/"
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/figures/*.jpg" "help/C/pentobi/"
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/figures/stylesheet.css" "help/C/pentobi/"
    DEPENDS ${DOCBOOK_SRC}
    )

add_custom_target(pentobi-help ALL DEPENDS
    "help/C/pentobi/index.html" ${index_files}
    )

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/help"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR} FILES_MATCHING PATTERN "*.css"
    PATTERN "*.html" PATTERN "*.png" PATTERN "*.jpg")
