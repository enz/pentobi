add_library(pentobi_gtp STATIC
  Engine.h
  Engine.cpp
)

target_include_directories(pentobi_gtp PUBLIC ..)

target_link_libraries(pentobi_gtp boardgame_gtp pentobi_base)
