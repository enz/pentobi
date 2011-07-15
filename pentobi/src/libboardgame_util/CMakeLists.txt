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
  Parameters.cpp
  RandomGenerator.cpp
  Statistics.cpp
  StringUtil.cpp
  TeeStream.cpp
  TimeIntervalChecker.cpp
  Timer.cpp
  TimeSource.cpp
  WallTime.cpp
  )

add_library(boardgame_util STATIC ${boardgame_util_STAT_SRCS})
