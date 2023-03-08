set(CMAKE_AUTORCC ON)

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
    )

target_link_libraries(pentobi-thumbnailer pentobi_paint)

target_compile_definitions(pentobi-thumbnailer PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060400
    VERSION="${PENTOBI_VERSION}"
    )

configure_file(pentobi.thumbnailer.in pentobi.thumbnailer @ONLY)

install(TARGETS pentobi-thumbnailer)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.thumbnailer
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/thumbnailers)
