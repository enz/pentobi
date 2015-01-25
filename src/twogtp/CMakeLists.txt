include_directories(${CMAKE_SOURCE_DIR}/src)

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
)

if (DEFINED CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(twogtp ${CMAKE_THREAD_LIBS_INIT})
endif()

