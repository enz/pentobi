if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

set(pentobi_thumbnail_STAT_SRCS
  CreateThumbnail.cpp
  )

add_library(pentobi_thumbnail STATIC
  ${pentobi_thumbnail_STAT_SRCS}
)

if (USE_QT5)
  qt5_use_modules(pentobi_thumbnail Widgets)
endif()
