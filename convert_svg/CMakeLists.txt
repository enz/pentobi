find_package(Qt5Gui REQUIRED)

add_executable(convert-svg Main.cpp)

target_link_libraries(convert-svg Qt5::Gui)
