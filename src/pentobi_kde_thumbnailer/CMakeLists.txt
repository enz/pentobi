if (USE_QT5)


find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

find_package(KF5 REQUIRED COMPONENTS KIO)

add_library(pentobi-thumbnail MODULE
  PentobiThumbCreator.h
  PentobiThumbCreator.cpp
)

target_link_libraries(pentobi-thumbnail
  pentobi_kde_thumbnailer
  KF5::KIOWidgets
)


else(USE_QT5)


find_package(KDE4 REQUIRED)

include(KDE4Defaults)

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${KDE4_INCLUDES}
  ${QT_INCLUDES}
)

kde4_add_plugin(pentobi-thumbnail
  PentobiThumbCreator.h
  PentobiThumbCreator.cpp
)

target_link_libraries(pentobi-thumbnail
  pentobi_kde_thumbnailer
  ${KDE4_KIO_LIBS}
)

endif(USE_QT5)


install(TARGETS pentobi-thumbnail DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES pentobi-thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
