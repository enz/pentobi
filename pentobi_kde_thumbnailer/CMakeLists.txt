find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

find_package(Gettext 0.18 REQUIRED)

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

find_package(KF5 REQUIRED COMPONENTS KIO)

add_library(pentobi-thumbnail MODULE
  PentobiThumbCreator.h
  PentobiThumbCreator.cpp
  io.sourceforge.pentobi.kde_thumbnailer.metainfo.translated.xml
)

target_include_directories(pentobi-thumbnail PRIVATE "${CMAKE_SOURCE_DIR}")

target_link_libraries(pentobi-thumbnail
  pentobi_kde_thumbnailer
  KF5::KIOWidgets
)

install(TARGETS pentobi-thumbnail DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES pentobi-thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)

# MetaInfo

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/po/LINGUAS" linguas)
string(REGEX REPLACE "\n" ";" linguas "${linguas}")
foreach(lang ${linguas})
    list(APPEND po_files po/${lang}.po)
endforeach()

configure_file(io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml.in
    io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml @ONLY)
add_custom_command(
    OUTPUT io.sourceforge.pentobi.kde_thumbnailer.metainfo.translated.xml
    COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" --xml
    -d "${CMAKE_CURRENT_SOURCE_DIR}/po"
    --template io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml
    -o io.sourceforge.pentobi.kde_thumbnailer.metainfo.translated.xml
    DEPENDS
    "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml"
    ${po_files} po/LINGUAS
    )

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.kde_thumbnailer.metainfo.translated.xml"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo
    RENAME io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml
    )
