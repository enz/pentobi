set(CMAKE_AUTOMOC TRUE)

find_package(Qt5Concurrent 5.11 REQUIRED)
find_package(Qt5QuickCompiler REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5LinguistTools 5.11 REQUIRED)
find_package(Qt5Svg 5.11 REQUIRED)
find_package(Qt5WebView 5.11 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)

qt5_add_translation(pentobi_QM
    qml/i18n/qml_de.ts
    qml/i18n/qml_fr.ts
    qml/i18n/qml_nb_NO.ts
    )

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/qml/i18n/translations.qrc"
    "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/qml/i18n/translations.qrc"
    )

# Don't use qtquick_compiler_add_resources for dynamically generated files, it
# doesn't check dependencies like qt5_add_resources (last tested in Qt 5.11.1)
qt5_add_resources(pentobi_RC_TRANSL_SRCS
    "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc"
    )

qtquick_compiler_add_resources(pentobi_RC_SRCS
    resources.qrc
    qml/themes/themes.qrc
    ../books/pentobi_books.qrc
    ../icon/pentobi_icon.qrc
    ../../doc/help.qrc
    )

add_executable(pentobi WIN32
    ${pentobi_QM}
    ${pentobi_RC_SRCS}
    ${pentobi_RC_TRANSL_SRCS}
    AnalyzeGameModel.h
    AnalyzeGameModel.cpp
    AndroidUtils.h
    AndroidUtils.cpp
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

if(MINGW AND (CMAKE_SIZEOF_VOID_P EQUAL "4"))
    set_target_properties(pentobi PROPERTIES LINK_FLAGS -Wl,--large-address-aware)
endif()

target_compile_definitions(pentobi PRIVATE
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x051100)

target_link_libraries(pentobi
    pentobi_mcts
    pentobi_base
    boardgame_base
    boardgame_sgf
    boardgame_util
    boardgame_sys
    Qt5::Concurrent
    Qt5::Gui
    Qt5::Qml
    Qt5::QuickControls2
    Qt5::Svg
    Qt5::WebView
    Threads::Threads
    )

install(TARGETS pentobi DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory(qml)
