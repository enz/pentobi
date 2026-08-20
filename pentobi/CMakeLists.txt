add_custom_command(
    OUTPUT help/help.qrc
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/help/help.qrc" help
    DEPENDS pentobi-help help/help.qrc
)

qt_add_executable(pentobi WIN32 Main.cpp)

if (WIN32)
    target_sources(pentobi "windows/pentobi.rc")
endif()

qt_add_qml_module(pentobi
    URI PentobiGui
    QML_FILES
    +Basic/CommentBase.qml
    +Basic/PentobiToolBarBase.qml
    +Fusion/MainBase.qml
    +Fusion/PentobiScrollView.qml
    +Material/MainBase.qml
    +Material/PentobiMenuBase.qml
    +org.kde.breeze/PentobiToolButtonBase.qml
    +org.kde.breeze/PentobiButtonBase.qml
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
    CommentBase.qml
    ComputerDialog.qml
    DialogButtonBoxOkCancel.qml
    DialogButtonBoxClose.qml
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
    MainBase.qml
    MainMenu.qml
    MessageDialog.qml
    MoveAnnotationDialog.qml
    NavigationButtons.qml
    NavigationPanel.qml
    OpenDialog.qml
    PentobiBusyIndicator.qml
    PentobiButton.qml
    PentobiButtonBase.qml
    PentobiDialog.qml
    PentobiMenu.qml
    PentobiMenuBase.qml
    PentobiScrollView.qml
    PentobiToolBar.qml
    PentobiToolBarBase.qml
    PentobiToolButton.qml
    PentobiToolButtonBase.qml
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
    "${CMAKE_CURRENT_BINARY_DIR}/help/help.qrc"
    ../opening_books/pentobi_books.qrc

    RESOURCES
    icon/pentobi-128.svg
    icons/dark/pentobi-newgame.svg
    icons/dark/pentobi-undo.svg
    icons/dark/pentobi-previous-variation.svg
    icons/dark/pentobi-play.svg
    icons/dark/pentobi-menu.svg
    icons/dark/pentobi-rated-game.svg
    icons/dark/pentobi-end.svg
    icons/dark/pentobi-backward.svg
    icons/dark/pentobi-computer-colors.svg
    icons/dark/pentobi-beginning.svg
    icons/dark/pentobi-forward10.svg
    icons/dark/pentobi-next-variation.svg
    icons/dark/pentobi-stop.svg
    icons/dark/pentobi-backward10.svg
    icons/dark/pentobi-forward.svg
    icons/light/pentobi-newgame.svg
    icons/light/pentobi-undo.svg
    icons/light/pentobi-previous-variation.svg
    icons/light/pentobi-play.svg
    icons/light/pentobi-menu.svg
    icons/light/pentobi-rated-game.svg
    icons/light/pentobi-end.svg
    icons/light/pentobi-backward.svg
    icons/light/pentobi-computer-colors.svg
    icons/light/pentobi-beginning.svg
    icons/light/pentobi-forward10.svg
    icons/light/pentobi-next-variation.svg
    icons/light/pentobi-stop.svg
    icons/light/pentobi-backward10.svg
    icons/light/pentobi-forward.svg
    piece-manipulator.svg
    piece-manipulator-dark.svg
    piece-manipulator-legal.svg
    piece-manipulator-legal-dark.svg
)

qt_add_translations(pentobi
    TS_OUTPUT_DIRECTORY i18n
    NO_GENERATE_PLURALS_TS_FILE # Not needed yet
    LRELEASE_OPTIONS -removeidentical -nounfinished
    LUPDATE_OPTIONS -locations none
)

if(ANDROID)
    target_link_libraries(pentobi PRIVATE Qt::CorePrivate)
    math(EXPR PENTOBI_ANDROID_CODE
        "${PENTOBI_VERSION_MAJOR} * 1000000 + ${PENTOBI_VERSION_MINOR} * 1000")
    if(PENTOBI_IS_RELEASE)
        math(EXPR PENTOBI_ANDROID_CODE "${PENTOBI_ANDROID_CODE} + 10")
    endif()
    if(ANDROID_ABI STREQUAL "arm64-v8a")
        math(EXPR PENTOBI_ANDROID_CODE "${PENTOBI_ANDROID_CODE} + 1")
    elseif(ANDROID_ABI STREQUAL "x86")
        math(EXPR PENTOBI_ANDROID_CODE "${PENTOBI_ANDROID_CODE} + 2")
    elseif(ANDROID_ABI STREQUAL "x86_64")
        math(EXPR PENTOBI_ANDROID_CODE "${PENTOBI_ANDROID_CODE} + 3")
    elseif(NOT STREQUAL ANDROID_ABI "armeabi-v7a")
        message(FATAL_ERROR "Unknown Android ABI: ${ANDROID_ABI}")
    endif()
    set_target_properties(pentobi PROPERTIES
        QT_ANDROID_APP_ICON "@drawable/icon"
        QT_ANDROID_APP_NAME "Pentobi"
        QT_ANDROID_PACKAGE_NAME "net.sf.pentobi"
        QT_ANDROID_VERSION_CODE ${PENTOBI_ANDROID_CODE}
        QT_ANDROID_VERSION_NAME ${PENTOBI_VERSION}
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
else()
    install(TARGETS pentobi)
endif()

target_compile_definitions(pentobi PRIVATE
    QT_DISABLE_DEPRECATED_UP_TO=0x061100
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

add_subdirectory(help)
if(UNIX AND NOT ANDROID)
    add_subdirectory(unix)
endif()
