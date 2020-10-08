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
    Threads::Threads
    )

