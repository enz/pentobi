qt5_add_translation(pentobi_QM
    qml/i18n/qml_de.ts
    qml/i18n/qml_es.ts
    qml/i18n/qml_fr.ts
    qml/i18n/qml_nb_NO.ts
    qml/i18n/qml_zh_CN.ts
    OPTIONS -removeidentical -nounfinished
    )
add_custom_command(
    OUTPUT "translations.qrc"
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/i18n/translations.qrc" .
    DEPENDS qml/i18n/translations.qrc ${pentobi_QM}
    )

set(pentobi_shared_src
    AnalyzeGameModel.h
    AnalyzeGameModel.cpp
    AndroidUtils.h
    AndroidUtils.cpp
    GameModel.h
    GameModel.cpp
    ImageProvider.h
    ImageProvider.cpp
    Main.cpp
    PieceModel.h
    PieceModel.cpp
    PlayerModel.h
    PlayerModel.cpp
    RatingModel.h
    RatingModel.cpp
    SyncSettings.h
    "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc"
    ../opening_books/pentobi_books.qrc
    icon/pentobi_icon.qrc
    resources.qrc
    qml/themes/themes.qrc
    )

if(ANDROID)
    add_custom_command(
        OUTPUT docbook/help.qrc
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/docbook/help.qrc" docbook
        DEPENDS pentobi-help docbook/help.qrc
        )
    add_library(pentobi SHARED
        ${pentobi_shared_src}
        "${CMAKE_CURRENT_BINARY_DIR}/docbook/help.qrc"
        )
    target_link_libraries(pentobi Qt5::AndroidExtras)
else()
    add_executable(pentobi WIN32
        ${pentobi_shared_src}
        resources_desktop.qrc)
    install(TARGETS pentobi DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

file(GLOB qml_SRC "qml/*.qml" "qml/*.js" "qml/i18n/*.ts" "qml/themes/*/*.qml")
target_sources(pentobi PRIVATE ${qml_SRC})

target_compile_definitions(pentobi PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
    QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    PENTOBI_HELP_DIR="${CMAKE_INSTALL_FULL_DATAROOTDIR}/help"
    VERSION="${PENTOBI_VERSION}"
    )

target_link_libraries(pentobi
    pentobi_paint
    pentobi_mcts
    Qt5::Concurrent
    Qt5::Qml
    Qt5::QuickControls2
    Qt5::Svg
    Threads::Threads
    )

if(Qt5WebView_FOUND AND NOT PENTOBI_OPEN_HELP_EXTERNALLY)
    target_link_libraries(pentobi Qt5::WebView)
else()
    if(NOT Qt5WebView_FOUND AND NOT PENTOBI_OPEN_HELP_EXTERNALLY)
        message(FATAL_ERROR
            "Qt5WebView not found. Install Qt5WebView or set"
            " PENTOBI_OPEN_HELP_EXTERNALLY to make Pentobi use an external"
            " browser for displaying help.")
    endif()
    target_compile_definitions(pentobi PRIVATE PENTOBI_OPEN_HELP_EXTERNALLY)
endif()

add_subdirectory(docbook)
if(UNIX AND NOT ANDROID)
    add_subdirectory(unix)
endif()
