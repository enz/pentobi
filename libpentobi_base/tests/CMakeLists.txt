add_executable(test_libpentobi_base
  BoardConstTest.cpp
  BoardTest.cpp
  BoardUpdaterTest.cpp
  GameTest.cpp
  PentobiTreeTest.cpp
  PentobiSgfUtilTest.cpp
)

target_link_libraries(test_libpentobi_base
    boardgame_test_main
    pentobi_base
    )

add_test(libpentobi_base test_libpentobi_base)
