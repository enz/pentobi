include(${QT_USE_FILE})

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(pentobi_SRCS
  MainWindow.cpp
  Main.cpp
  )

set(pentobi_MOC_HDRS
  MainWindow.h
  )
  
set(pentobi_RCS
  resources.qrc
  )

link_directories(${Boost_LIBRARY_DIRS})

qt4_add_resources(pentobi_RC_SRCS ${pentobi_RCS})

qt4_wrap_cpp(pentobi_MOC_SRCS ${pentobi_MOC_HDRS})

if (MINGW)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icon.o
    COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
    -i${CMAKE_CURRENT_SOURCE_DIR}/icon.rc
    -o ${CMAKE_CURRENT_BINARY_DIR}/icon.o)
  set(pentobi_SRCS ${pentobi_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/icon.o)
else()
  set(pentobi_SRCS ${pentobi_SRCS} icon.rc)
endif()

add_executable(pentobi WIN32
  ${pentobi_SRCS}
  ${pentobi_MOC_SRCS}
  ${pentobi_RC_SRCS})

target_link_libraries(pentobi
  pentobi_gui
  ${QT_LIBRARIES}
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  boardgame_sys
  ${Boost_LIBRARIES}
  )

if (WIN32)
target_link_libraries(pentobi
  ${QT_QTMAIN_LIBRARY}
  )
endif()

install(TARGETS pentobi DESTINATION bin)
