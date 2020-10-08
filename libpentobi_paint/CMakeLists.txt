add_library(pentobi_paint STATIC
Paint.cpp
Paint.h
)

target_compile_definitions(pentobi_paint PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000)

target_link_libraries(pentobi_paint pentobi_base Qt5::Gui)
