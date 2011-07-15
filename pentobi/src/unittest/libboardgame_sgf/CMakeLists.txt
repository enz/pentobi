include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

set(unittest_libboardgame_sgf_STAT_SRCS
  Main.cpp
  NodeTest.cpp
  TreeReaderTest.cpp
  UtilTest.cpp
  )

add_executable(unittest_libboardgame_sgf
  ${unittest_libboardgame_sgf_STAT_SRCS})

target_link_libraries(unittest_libboardgame_sgf
  boardgame_test
  boardgame_util
  boardgame_sgf
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  )

add_test(libboardgame_sgf unittest_libboardgame_sgf)
