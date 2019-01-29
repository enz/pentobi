add_library(boardgame_util STATIC
  Abort.h
  Abort.cpp
  ArrayList.h
  Assert.h
  Assert.cpp
  Barrier.h
  Barrier.cpp
  CpuTimeSource.h
  CpuTimeSource.cpp
  FmtSaver.h
  IntervalChecker.h
  IntervalChecker.cpp
  Log.h
  Log.cpp
  MathUtil.h
  Options.h
  Options.cpp
  RandomGenerator.h
  RandomGenerator.cpp
  Range.h
  Statistics.h
  StringUtil.h
  StringUtil.cpp
  TimeIntervalChecker.h
  TimeIntervalChecker.cpp
  Timer.h
  Timer.cpp
  TimeSource.h
  TimeSource.cpp
  WallTimeSource.h
  WallTimeSource.cpp
)

target_compile_options(boardgame_util PUBLIC
    "$<$<CONFIG:DEBUG>:-DLIBBOARDGAME_DEBUG>")

target_include_directories(boardgame_util PUBLIC ..)

target_link_libraries(boardgame_util boardgame_sys)
