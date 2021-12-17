qt_add_translation(pentobi_QM
    qml/i18n/qml_de.ts
    qml/i18n/qml_es.ts
    qml/i18n/qml_fr.ts
    qml/i18n/qml_nb_NO.ts
    qml/i18n/qml_ru.ts
    qml/i18n/qml_zh_CN.ts
    OPTIONS -removeidentical -nounfinished
    )
add_custom_command(
    OUTPUT translations.qrc
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/i18n/translations.qrc" .
    DEPENDS qml/i18n/translations.qrc ${pentobi_QM}
    )

# Images and icons to be used with QtQuick Image. Here we use a sourceSize four
# times larger than the device-indepenent size to support high-dpi images.
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/icon")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qml/icons")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qml/themes/dark")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qml/themes/light")
set(pentobi_images
    icon/pentobi-128
    qml/icons/filedialog-folder
    qml/themes/dark/menu
    qml/themes/dark/menu-desktop
    qml/themes/dark/pentobi-backward
    qml/themes/dark/pentobi-backward10
    qml/themes/dark/pentobi-beginning
    qml/themes/dark/pentobi-computer-colors
    qml/themes/dark/pentobi-end
    qml/themes/dark/pentobi-forward
    qml/themes/dark/pentobi-forward10
    qml/themes/dark/pentobi-newgame
    qml/themes/dark/pentobi-next-variation
    qml/themes/dark/pentobi-play
    qml/themes/dark/pentobi-previous-variation
    qml/themes/dark/pentobi-rated-game
    qml/themes/dark/pentobi-stop
    qml/themes/dark/pentobi-undo
    qml/themes/dark/piece-manipulator-desktop
    qml/themes/dark/piece-manipulator-desktop-legal
    qml/themes/dark/piece-manipulator
    qml/themes/dark/piece-manipulator-legal
    qml/themes/light/menu
    qml/themes/light/menu-desktop
    qml/themes/light/pentobi-backward
    qml/themes/light/pentobi-backward10
    qml/themes/light/pentobi-beginning
    qml/themes/light/pentobi-computer-colors
    qml/themes/light/pentobi-end
    qml/themes/light/pentobi-forward
    qml/themes/light/pentobi-forward10
    qml/themes/light/pentobi-newgame
    qml/themes/light/pentobi-next-variation
    qml/themes/light/pentobi-play
    qml/themes/light/pentobi-previous-variation
    qml/themes/light/pentobi-rated-game
    qml/themes/light/pentobi-stop
    qml/themes/light/pentobi-undo
    qml/themes/light/piece-manipulator-desktop
    qml/themes/light/piece-manipulator-desktop-legal
    qml/themes/light/piece-manipulator
    qml/themes/light/piece-manipulator-legal
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

# Icons to be used with QtQuickControls2 ToolButton.icon. Here we cannot
# specify a sourceSize different from size, so we use a custom icon theme to
# support high-dpi icons.
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/icons/pentobi/16x16")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/icons/pentobi/16x16@2")
set(pentobi_icons
    filedialog-newfolder
    filedialog-parent
    )
foreach(icon ${pentobi_icons})
    add_custom_command(OUTPUT "icons/pentobi/16x16/${icon}.png"
        COMMAND "${RSVG_CONVERT}"
        "${CMAKE_CURRENT_SOURCE_DIR}/icons/${icon}.svg"
        > "icons/pentobi/16x16/${icon}.png"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/icons/${icon}.svg"
        )
    list(APPEND icons_png "icons/pentobi/16x16/${icon}.png")
    add_custom_command(OUTPUT "icons/pentobi/16x16@2/${icon}.png"
        COMMAND "${RSVG_CONVERT}" -x 2 -y 2
        "${CMAKE_CURRENT_SOURCE_DIR}/icons/${icon}.svg"
        > "icons/pentobi/16x16@2/${icon}.png"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/icons/${icon}.svg"
        )
    list(APPEND icons_png "icons/pentobi/16x16@2/${icon}.png")
endforeach()
add_custom_command(
    OUTPUT icons_desktop.qrc
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/icons_desktop.qrc" .
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/icons_desktop.qrc" ${images_png}
    ${icons_png}
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
    RecentFiles.h
    RecentFiles.cpp
    SyncSettings.h
    "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc"
    "${CMAKE_CURRENT_BINARY_DIR}/icons.qrc"
    ../opening_books/pentobi_books.qrc
    resources.qrc
    )

if(ANDROID)
    add_custom_command(
        OUTPUT docbook/help.qrc
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/docbook/help.qrc" docbook
        DEPENDS pentobi-help docbook/help.qrc
        )
    qt_add_executable(pentobi
        MANUAL_FINALIZATION
        ${pentobi_shared_src}
        "${CMAKE_CURRENT_BINARY_DIR}/docbook/help.qrc"
        )
    target_link_libraries(pentobi PRIVATE Qt::CorePrivate)
    set_property(TARGET pentobi APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
    qt_finalize_executable(pentobi)
else()
    add_executable(pentobi WIN32
        ${pentobi_shared_src}
        "${CMAKE_CURRENT_BINARY_DIR}/icons_desktop.qrc"
        resources_desktop.qrc
        )
    install(TARGETS pentobi)
endif()

target_compile_definitions(pentobi PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
    QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    PENTOBI_HELP_DIR="${CMAKE_INSTALL_FULL_DOCDIR}/help"
    VERSION="${PENTOBI_VERSION}"
    )

target_link_libraries(pentobi PRIVATE
    pentobi_paint
    pentobi_mcts
    Qt::Concurrent
    Qt::Qml
    Qt::QuickControls2
    Threads::Threads
    )

add_subdirectory(docbook)
if(UNIX AND NOT ANDROID)
    add_subdirectory(unix)
endif()
