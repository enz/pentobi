include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(boardgame_util_STAT_SRCS
  Abort.cpp
  Assert.cpp
  CpuTime.cpp
  Exception.cpp
  FastLog.cpp
  IntervalChecker.cpp
  Log.cpp
  RandomGenerator.cpp
  StringUtil.cpp
  TimeIntervalChecker.cpp
  Timer.cpp
  TimeSource.cpp
  WallTime.cpp
  )

add_library(boardgame_util STATIC ${boardgame_util_STAT_SRCS})
