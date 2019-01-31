set(CMAKE_AUTOMOC TRUE)

find_package(Threads)
find_package(Qt5Concurrent 5.11 REQUIRED)
find_package(Qt5QuickControls2 5.11 REQUIRED)
find_package(Qt5LinguistTools 5.11 REQUIRED)
find_package(Qt5Svg 5.11 REQUIRED)
find_package(Qt5QuickCompiler REQUIRED)
find_package(Qt5WebView 5.11)

qt5_add_translation(pentobi_QM
    qml/i18n/qml_de.ts
    qml/i18n/qml_fr.ts
    qml/i18n/qml_nb_NO.ts
    qml/i18n/qml_zh_CN.ts
    OPTIONS -removeidentical -nounfinished
    )
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/qml/i18n/translations.qrc"
    "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/qml/i18n/translations.qrc" ${pentobi_QM}
    )

qt5_add_resources(pentobi_RC_SRCS
    "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc"
    ../books/pentobi_books.qrc
    icon/pentobi_icon.qrc
    icon/pentobi_icon_desktop.qrc
    )

qtquick_compiler_add_resources(pentobi_RC_SRCS_QML
    resources.qrc
    resources_desktop.qrc
    qml/themes/themes.qrc
    )

add_executable(pentobi WIN32
    ${pentobi_RC_SRCS}
    ${pentobi_RC_SRCS_QML}
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
    )

file(GLOB qml_SRC "qml/*.qml" "qml/*.js" "qml/i18n/*.ts" "qml/themes/*/*.qml")
target_sources(pentobi PRIVATE ${qml_SRC})

target_compile_definitions(pentobi PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x051200
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

if (Qt5WebView_FOUND AND NOT PENTOBI_OPEN_HELP_EXTERNALLY)
    target_link_libraries(pentobi Qt5::WebView)
else()
    target_compile_definitions(pentobi PRIVATE PENTOBI_OPEN_HELP_EXTERNALLY)
endif()

configure_file(io.sourceforge.pentobi.desktop.in io.sourceforge.pentobi.desktop @ONLY)
configure_file(io.sourceforge.pentobi.appdata.xml.in io.sourceforge.pentobi.appdata.xml @ONLY)
configure_file(pentobi.6.in pentobi.6 @ONLY)

foreach(icon application-x-blokus-sgf application-x-blokus-sgf-16
        application-x-blokus-sgf-32 application-x-blokus-sgf-64)
  set(icon_svg "${CMAKE_CURRENT_SOURCE_DIR}/${icon}.svg")
  set(icon_png "${CMAKE_CURRENT_BINARY_DIR}/${icon}.png")
  add_custom_command(OUTPUT "${icon_png}"
    COMMAND convert "${icon_svg}" "${icon_png}" DEPENDS "${icon_svg}")
  list(APPEND png_icons "${icon_png}")
endforeach()
foreach(icon pentobi pentobi-16 pentobi-32 pentobi-64)
  set(icon_svg "${CMAKE_CURRENT_SOURCE_DIR}/icon/${icon}.svg")
  set(icon_png "${CMAKE_CURRENT_BINARY_DIR}/${icon}.png")
  add_custom_command(OUTPUT "${icon_png}"
    COMMAND convert "${icon_svg}" "${icon_png}" DEPENDS "${icon_svg}")
  list(APPEND png_icons "${icon_png}")
endforeach()
add_custom_target(data_icons ALL DEPENDS ${png_icons})

install(TARGETS pentobi DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY help DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
    FILES_MATCHING PATTERN "*.css" PATTERN "*.html" PATTERN "*.png"
    PATTERN "*.jpg")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi-16.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/apps
  RENAME pentobi.png)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi-32.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps
  RENAME pentobi.png)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi-64.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps
  RENAME pentobi.png)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icon/pentobi.svg
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/mimetypes)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-16.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/mimetypes
  RENAME application-x-blokus-sgf.png)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-32.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/mimetypes
  RENAME application-x-blokus-sgf.png)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/application-x-blokus-sgf-64.png
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/mimetypes
  RENAME application-x-blokus-sgf.png)
install(FILES application-x-blokus-sgf.svg
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/mimetypes)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.desktop
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install(FILES pentobi-mime.xml
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mime/packages)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.appdata.xml
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pentobi.6
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)
