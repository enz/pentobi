include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

set(unittest_libboardgame_sgf_STAT_SRCS
  NodeTest.cpp
  TreeTest.cpp
  TreeIteratorTest.cpp
  TreeReaderTest.cpp
  UtilTest.cpp
  )

add_executable(unittest_libboardgame_sgf
  ${unittest_libboardgame_sgf_STAT_SRCS})

target_link_libraries(unittest_libboardgame_sgf
  boardgame_test_main
  boardgame_test
  boardgame_sgf
  boardgame_util
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  )

add_test(libboardgame_sgf unittest_libboardgame_sgf)
