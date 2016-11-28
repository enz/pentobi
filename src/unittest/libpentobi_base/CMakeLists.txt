add_executable(unittest_libpentobi_base
  BoardConstTest.cpp
  BoardTest.cpp
  BoardUpdaterTest.cpp
  GameTest.cpp
  PentobiTreeTest.cpp
)

target_link_libraries(unittest_libpentobi_base
  boardgame_test_main
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_test
  boardgame_util
  boardgame_sys
  )

add_test(libpentobi_base unittest_libpentobi_base)
