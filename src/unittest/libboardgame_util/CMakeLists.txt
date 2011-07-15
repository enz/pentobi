include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

set(unittest_libboardgame_util_STAT_SRCS
  Main.cpp
  ArrayListTest.cpp
  FastLogTest.cpp
  NullTermListTest.cpp
  )

add_executable(unittest_libboardgame_util
  ${unittest_libboardgame_util_STAT_SRCS})

target_link_libraries(unittest_libboardgame_util
  boardgame_test
  boardgame_util
  boardgame_sys
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  )

add_test(libboardgame_util unittest_libboardgame_util)
