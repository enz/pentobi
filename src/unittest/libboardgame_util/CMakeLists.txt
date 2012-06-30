include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

set(unittest_libboardgame_util_STAT_SRCS
  ArrayListTest.cpp
  FastLogTest.cpp
  NullTermListTest.cpp
  )

add_executable(unittest_libboardgame_util
  ${unittest_libboardgame_util_STAT_SRCS})

target_link_libraries(unittest_libboardgame_util
  boardgame_test_main
  boardgame_test
  boardgame_util
  boardgame_sys
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  )

add_test(libboardgame_util unittest_libboardgame_util)
