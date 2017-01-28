add_library(pentobi_thumbnail STATIC
  CreateThumbnail.h
  CreateThumbnail.cpp
)

target_compile_definitions(pentobi_thumbnail PRIVATE
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050200)

target_link_libraries(pentobi_thumbnail Qt5::Widgets)
