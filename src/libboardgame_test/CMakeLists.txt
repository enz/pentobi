add_library(boardgame_test STATIC
  Test.h
  Test.cpp
)

target_include_directories(boardgame_test PUBLIC ..)

target_link_libraries(boardgame_test boardgame_util)
