find_package(Threads)

add_executable(learn-tool Main.cpp)

target_link_libraries(learn-tool
  pentobi_mcts
  Threads::Threads
)
