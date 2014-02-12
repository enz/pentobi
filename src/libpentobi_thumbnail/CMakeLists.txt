if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(pentobi_thumbnail STATIC
  CreateThumbnail.cpp
  CreateThumbnail.h
)

if (USE_QT5)
  qt5_use_modules(pentobi_thumbnail Widgets)
endif()
