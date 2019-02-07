find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

find_package(Gettext 0.18 REQUIRED)
find_program(ITSTOOL itstool)
if(NOT ITSTOOL)
    message(FATAL_ERROR "itstool not found")
endif()

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

find_package(KF5 REQUIRED COMPONENTS KIO)

add_library(pentobi-thumbnail MODULE
  PentobiThumbCreator.h
  PentobiThumbCreator.cpp
  io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml
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
    add_custom_command(OUTPUT ${lang}.mo
        COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" -o ${lang}.mo
        "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.po"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}.po"
        )
    list(APPEND mo_files ${lang}.mo)
endforeach()

configure_file(io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml.in
    io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml.in.2 @ONLY)
add_custom_command(
    OUTPUT io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml
    COMMAND "${ITSTOOL}"
    -j io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml.in.2
    -o io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml ${mo_files}
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml.in.2"
    ${mo_files} "${CMAKE_CURRENT_SOURCE_DIR}/po/LINGUAS"
    )

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/io.sourceforge.pentobi.kde_thumbnailer.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)
