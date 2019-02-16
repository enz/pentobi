add_library(boardgame_test STATIC
  Test.h
  Test.cpp
)

target_include_directories(boardgame_test PUBLIC ..)

target_link_libraries(boardgame_test boardgame_base)

add_library(boardgame_test_main STATIC Main.cpp)

target_link_libraries(boardgame_test_main boardgame_test)
