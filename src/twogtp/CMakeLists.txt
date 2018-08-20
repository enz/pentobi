find_package(Threads)

add_executable(twogtp
  Analyze.h
  Analyze.cpp
  FdStream.h
  FdStream.cpp
  GtpConnection.h
  GtpConnection.cpp
  Main.cpp
  Output.h
  Output.cpp
  OutputTree.h
  OutputTree.cpp
  TwoGtp.h
  TwoGtp.cpp
)

target_link_libraries(twogtp
  pentobi_base
  boardgame_sgf
  boardgame_base
  boardgame_util
  boardgame_sys
  Threads::Threads
)

