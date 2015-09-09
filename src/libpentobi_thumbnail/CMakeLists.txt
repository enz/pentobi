add_library(pentobi_thumbnail STATIC
  CreateThumbnail.h
  CreateThumbnail.cpp
)

target_link_libraries(pentobi_thumbnail Qt5::Widgets)
