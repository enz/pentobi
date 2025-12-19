file(READ "${CMAKE_CURRENT_SOURCE_DIR}/po/LINGUAS" linguas)
string(REGEX REPLACE "\n" ";" linguas "${linguas}")

set(DOCBOOK_SRC
    docbook/index.xml
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
    list(APPEND index_files "help/index_${lang}.xml")
    add_custom_command(OUTPUT "help/index_${lang}.xml"
        COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}"
        --xml --template "${CMAKE_CURRENT_SOURCE_DIR}/docbook/index.xml"
        -l "${lang}" -o "help/index_${lang}.xml" --replace-text
        "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.po"
        DEPENDS docbook/index.xml
        )
endforeach()

add_custom_command(OUTPUT "help/index.xml"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/docbook" "help"
    DEPENDS ${DOCBOOK_SRC}
    )

add_custom_target(pentobi-help ALL DEPENDS "help/index.xml" ${index_files})
