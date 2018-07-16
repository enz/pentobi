find_package(Qt5Gui REQUIRED)

add_executable(convert Main.cpp)

target_link_libraries(convert Qt5::Gui)
