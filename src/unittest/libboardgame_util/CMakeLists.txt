include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(unittest_libboardgame_util
  ArrayListTest.cpp
  FastLogTest.cpp
  NullTermListTest.cpp
  OptionsTest.cpp
  StatisticsTest.cpp
  StringUtilTest.cpp
)

target_link_libraries(unittest_libboardgame_util
  boardgame_test_main
  boardgame_test
  boardgame_util
  boardgame_sys
  )

add_test(libboardgame_util unittest_libboardgame_util)
