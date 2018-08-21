set(CMAKE_AUTOMOC TRUE)

find_package(Threads)
find_package(Qt5Concurrent 5.11 REQUIRED)
find_package(Qt5QuickCompiler REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5LinguistTools 5.11 REQUIRED)
find_package(Qt5Svg 5.11 REQUIRED)
find_package(Qt5WebView 5.11 REQUIRED)

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
    resources_desktop.qrc
    qml/themes/themes.qrc
    ../books/pentobi_books.qrc
    ../icon/pentobi_icon.qrc
    ../icon/pentobi_icon_desktop.qrc
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
    ImageProvider.h
    ImageProvider.cpp
    Main.cpp
    PieceModel.h
    PieceModel.cpp
    PlayerModel.h
    PlayerModel.cpp
    RatingModel.h
    RatingModel.cpp
    )

file(GLOB qml_SRC "qml/*.qml" "qml/*.js" "qml/i18n/*.ts" "qml/themes/*/*.qml")
target_sources(pentobi PRIVATE ${qml_SRC})

target_compile_definitions(pentobi PRIVATE
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x051100
    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)

target_link_libraries(pentobi
    pentobi_paint
    pentobi_mcts
    Qt5::Concurrent
    Qt5::Qml
    Qt5::QuickControls2
    Qt5::Svg
    Qt5::WebView
    Threads::Threads
    )

install(TARGETS pentobi DESTINATION ${CMAKE_INSTALL_BINDIR})
