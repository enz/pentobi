add_executable(pentobi-gtp
  Engine.h
  Engine.cpp
  Main.cpp
)

target_link_libraries(pentobi-gtp
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
  boardgame_gtp
  )

if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(pentobi-gtp ${CMAKE_THREAD_LIBS_INIT})
endif()

if(MINGW AND (CMAKE_SIZEOF_VOID_P EQUAL "4"))
  set_target_properties(pentobi-gtp PROPERTIES LINK_FLAGS -Wl,--large-address-aware)
endif()

