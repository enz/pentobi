file(READ "${CMAKE_CURRENT_SOURCE_DIR}/po/LINGUAS" linguas)
string(REGEX REPLACE "\n" ";" linguas "${linguas}")

set(DOCBOOK_SRC
    docbook/index.docbook
    docbook/analysis.png
    docbook/board_callisto.png
    docbook/board_classic.png
    docbook/board_duo.png
    docbook/board_gembloq.png
    docbook/board_nexos.png
    docbook/board_trigon.png
    docbook/pieces_callisto.png
    docbook/pieces_gembloq.png
    docbook/pieces_junior.png
    docbook/pieces_nexos.png
    docbook/pieces.png
    docbook/pieces_trigon.png
    docbook/position_callisto.png
    docbook/position_classic.png
    docbook/position_duo.png
    docbook/position_gembloq.png
    docbook/position_nexos.png
    docbook/position_trigon.png
    docbook/rating.png
    )

foreach(lang ${linguas})
    add_custom_command(OUTPUT ${lang}.mo
        COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" -o ${lang}.mo
        "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.po"
        DEPENDS po/${lang}.po
        )
    list(APPEND index_files "help/index_${lang}.docbook")
    add_custom_command(OUTPUT "help/index_${lang}.docbook"
        COMMAND "${ITSTOOL}" -m ${lang}.mo -o "help/index_${lang}.docbook"
        "${CMAKE_CURRENT_SOURCE_DIR}/docbook/index.docbook"
        DEPENDS docbook/index.docbook ${lang}.mo
        )
endforeach()

add_custom_command(OUTPUT "help/index.docbook"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/docbook" "help"
    DEPENDS ${DOCBOOK_SRC}
    )

add_custom_target(pentobi-help ALL DEPENDS "help/index.docbook" ${index_files})
