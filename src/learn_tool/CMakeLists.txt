add_executable(learn-tool Main.cpp)

target_link_libraries(learn-tool
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
)

if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(learn-tool ${CMAKE_THREAD_LIBS_INIT})
endif()
