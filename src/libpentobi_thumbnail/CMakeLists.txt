include(${QT_USE_FILE})

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(pentobi_thumbnail_STAT_SRCS
  CreateThumbnail.cpp
  )

add_library(pentobi_thumbnail STATIC
  ${pentobi_thumbnail_STAT_SRCS}
)
