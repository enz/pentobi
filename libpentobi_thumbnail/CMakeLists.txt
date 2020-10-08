add_library(pentobi_thumbnail STATIC
    CreateThumbnail.h
    CreateThumbnail.cpp
    )

target_compile_definitions(pentobi_thumbnail PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000)

target_link_libraries(pentobi_thumbnail pentobi_paint)
