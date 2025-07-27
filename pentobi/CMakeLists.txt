qt_add_translation(pentobi_QM
    i18n/qml_de.ts
    i18n/qml_es.ts
    i18n/qml_fr.ts
    i18n/qml_nb_NO.ts
    i18n/qml_ru.ts
    i18n/qml_zh_CN.ts
    OPTIONS -removeidentical -nounfinished
    )
add_custom_command(
    OUTPUT translations.qrc
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/i18n/translations.qrc" .
    DEPENDS i18n/translations.qrc ${pentobi_QM}
    )

# Images and icons to be used with QtQuick Image. We use sourceSize four times
# larger than the device-indepenent size to support high-dpi images.
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/icon")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/icons")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/themes/dark")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/themes/light")
set(pentobi_images
    icon/pentobi-48
    themes/dark/menu
    themes/dark/pentobi-backward
    themes/dark/pentobi-backward10
    themes/dark/pentobi-beginning
    themes/dark/pentobi-computer-colors
    themes/dark/pentobi-end
    themes/dark/pentobi-forward
    themes/dark/pentobi-forward10
    themes/dark/pentobi-newgame
    themes/dark/pentobi-next-variation
    themes/dark/pentobi-play
    themes/dark/pentobi-previous-variation
    themes/dark/pentobi-rated-game
    themes/dark/pentobi-stop
    themes/dark/pentobi-undo
    themes/dark/piece-manipulator-desktop
    themes/dark/piece-manipulator-desktop-legal
    themes/dark/piece-manipulator
    themes/dark/piece-manipulator-legal
    themes/light/menu
    themes/light/pentobi-backward
    themes/light/pentobi-backward10
    themes/light/pentobi-beginning
    themes/light/pentobi-computer-colors
    themes/light/pentobi-end
    themes/light/pentobi-forward
    themes/light/pentobi-forward10
    themes/light/pentobi-newgame
    themes/light/pentobi-next-variation
    themes/light/pentobi-play
    themes/light/pentobi-previous-variation
    themes/light/pentobi-rated-game
    themes/light/pentobi-stop
    themes/light/pentobi-undo
    themes/light/piece-manipulator-desktop
    themes/light/piece-manipulator-desktop-legal
    themes/light/piece-manipulator
    themes/light/piece-manipulator-legal
    )
foreach(image ${pentobi_images})
    add_custom_command(OUTPUT "${image}.png"
        COMMAND "${RSVG_CONVERT}" -x 4 -y 4
        "${CMAKE_CURRENT_SOURCE_DIR}/${image}.svg" > "${image}.png"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${image}.svg"
        )
    list(APPEND images_png "${image}.png")
endforeach()
add_custom_command(
    OUTPUT icons.qrc
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/icons.qrc" .
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/icons.qrc" ${images_png}
    )

add_custom_command(
    OUTPUT icons_desktop.qrc
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/icons_desktop.qrc" .
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/icons_desktop.qrc" ${images_png}
    )

qt_add_executable(pentobi WIN32 Main.cpp)

if (WIN32)
    target_sources(pentobi "windows/pentobi.rc")
endif()

qt_add_qml_module(pentobi
    URI PentobiGui
    QML_FILES
    AboutDialog.qml
    AnalyzeDialog.qml
    AnalyzeGame.qml
    AppearanceDialog.qml
    Board.qml
    BoardContextMenu.qml
    ButtonCancel.qml
    ButtonClose.qml
    ButtonOk.qml
    Comment.qml
    ComputerDialog.qml
    DialogButtonBoxOkCancel.qml
    DialogLoader.qml
    ExportImageDialog.qml
    FatalMessage.qml
    GameInfoDialog.qml
    GameVariantDialog.qml
    GameViewDesktop.qml
    GameViewMobile.qml
    GotoMoveDialog.qml
    HelpDialog.qml
    HelpViewer.qml
    HelpWindow.qml
    ImageSaveDialog.qml
    InitialRatingDialog.qml
    LineSegment.qml
    Main.qml
    MessageDialog.qml
    MenuComputer.qml
    MenuEdit.qml
    MenuGame.qml
    MenuGo.qml
    MenuHelp.qml
    MenuRecentFiles.qml
    MenuTools.qml
    MenuView.qml
    MoveAnnotationDialog.qml
    NavigationButtons.qml
    NavigationPanel.qml
    OpenDialog.qml
    PentobiBusyIndicator.qml
    PentobiButton.qml
    PentobiDialog.qml
    PentobiDialogButtonBox.qml
    PentobiMenu.qml
    PentobiToolBar.qml
    PieceBase.qml
    PieceCallisto.qml
    PieceClassic.qml
    PieceGembloQ.qml
    PieceList.qml
    PieceManipulator.qml
    PieceNexos.qml
    PieceRotationAnimation.qml
    PieceSelectorDesktop.qml
    PieceSelectorMobile.qml
    PieceSwitchedFlipAnimation.qml
    PieceTrigon.qml
    QuarterSquare.qml
    QuestionDialog.qml
    RatingDialog.qml
    RatingGraph.qml
    SaveDialog.qml
    ScoreDisplay.qml
    ScoreElement.qml
    ScoreElement2.qml
    Square.qml
    Theme.qml
    ThemeColorblindDark.qml
    ThemeColorblindLight.qml
    ThemeDark.qml
    ThemeLight.qml
    ThemeSystem.qml
    Triangle.qml
    gameview.js
    main.js

    SOURCES
    AnalyzeGameModel.h
    AnalyzeGameModel.cpp
    AndroidUtils.h
    AndroidUtils.cpp
    DocbookReader.h
    DocbookReader.cpp
    GameModel.h
    GameModel.cpp
    ImageProvider.h
    ImageProvider.cpp
    Paint.h
    Paint.cpp
    PieceModel.h
    PieceModel.cpp
    PlayerModel.h
    PlayerModel.cpp
    RatingModel.h
    RatingModel.cpp
    RecentFiles.h
    RecentFiles.cpp
    SyncSettings.h
    resources.qrc
    "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc"
    "${CMAKE_CURRENT_BINARY_DIR}/icons.qrc"
    "${CMAKE_CURRENT_BINARY_DIR}/icons_desktop.qrc"
    help/help.qrc
    ../opening_books/pentobi_books.qrc
)

if(ANDROID)
    target_link_libraries(pentobi PRIVATE Qt::CorePrivate)
    set_property(TARGET pentobi APPEND PROPERTY
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)
else()
    install(TARGETS pentobi)
endif()

target_compile_definitions(pentobi PRIVATE
    QT_DISABLE_DEPRECATED_UP_TO=0x060900
    QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    VERSION="${PENTOBI_VERSION}"
    )

target_link_libraries(pentobi PRIVATE
    pentobi_mcts
    Qt::Concurrent
    Qt::Qml
    Qt::QuickControls2
    Qt::Xml
    Threads::Threads
    )

if(UNIX AND NOT ANDROID)
    add_subdirectory(unix)
endif()
