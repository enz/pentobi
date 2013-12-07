include_directories(${CMAKE_SOURCE_DIR}/src)

set(unittest_libboardgame_util_STAT_SRCS
  ArrayListTest.cpp
  FastLogTest.cpp
  NullTermListTest.cpp
  OptionsTest.cpp
  StringUtilTest.cpp
  )

add_executable(unittest_libboardgame_util
  ${unittest_libboardgame_util_STAT_SRCS})

target_link_libraries(unittest_libboardgame_util
  boardgame_test_main
  boardgame_test
  boardgame_util
  boardgame_sys
  )

add_test(libboardgame_util unittest_libboardgame_util)
