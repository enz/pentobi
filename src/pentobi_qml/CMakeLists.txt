set(CMAKE_AUTOMOC TRUE)

find_package(Qt5Concurrent 5.11 REQUIRED)
find_package(Qt5QuickCompiler REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5LinguistTools 5.11 REQUIRED)
find_package(Qt5Svg 5.11 REQUIRED)
find_package(Qt5WebView 5.11 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)

qt5_add_translation(pentobi_qml_QM
    qml/i18n/qml_de.ts
    qml/i18n/qml_fr.ts
    qml/i18n/qml_nb_NO.ts
    )

file(COPY qml/i18n/translations.qrc DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
qtquick_compiler_add_resources(pentobi_qml_RC_SRCS
    resources.qrc
    ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
    qml/themes/themes.qrc
    ../books/pentobi_books.qrc
    ../pentobi/help.qrc
    )

add_executable(pentobi_qml WIN32
    ${pentobi_qml_QM}
    ${pentobi_qml_RC_SRCS}
    AnalyzeGameModel.h
    AnalyzeGameModel.cpp
    GameModel.h
    GameModel.cpp
    HelpFileExtractor.h
    HelpFileExtractor.cpp
    Main.cpp
    PieceModel.h
    PieceModel.cpp
    PlayerModel.h
    PlayerModel.cpp
    RatingModel.h
    RatingModel.cpp
    )

target_compile_definitions(pentobi_qml PRIVATE
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x051100)

target_link_libraries(pentobi_qml
    pentobi_mcts
    pentobi_base
    boardgame_base
    boardgame_sgf
    boardgame_util
    boardgame_sys
    Qt5::Concurrent
    Qt5::Gui
    Qt5::Qml
    Qt5::Svg
    Qt5::WebView
    Threads::Threads
    )

add_subdirectory(qml)
