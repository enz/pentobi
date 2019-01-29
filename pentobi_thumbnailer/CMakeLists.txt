add_executable(pentobi-thumbnailer Main.cpp)

target_link_libraries(pentobi-thumbnailer pentobi_thumbnail)

install(TARGETS pentobi-thumbnailer DESTINATION ${CMAKE_INSTALL_BINDIR})
