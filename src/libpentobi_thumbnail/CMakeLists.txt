set(CMAKE_AUTOMOC TRUE)

find_package(Qt5Gui 5.9 REQUIRED)

add_library(pentobi_thumbnail STATIC
    CreateThumbnail.h
    CreateThumbnail.cpp
    )

target_compile_definitions(pentobi_thumbnail PRIVATE
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x051100)

target_link_libraries(pentobi_thumbnail pentobi_paint)
