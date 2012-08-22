# libpentobi_kde_thumbnailer contains the files needed by
# the pentobi_kde_thumbnailer plugin compiled with shared library options
# (-fPIC) because this is required for building shared libraries on
# some targets (e.g. x86_64).
#
# The alternative would be to add -fPIC to the global compiler flags even for
# executables but this slows down Pentobi's search by 10% on some targets.
#
# Adding the source files directly to pentobi_kde_thumbnailer/CMakeList.txt is
# not possible because the KDE CMake macros add -fno-exceptions to the
# compiler flags which causes errors in the Pentobi sources that use
# exceptions (which should be fine as long as no exceptions are thrown
# from the thumbnailer plugin functions).

include(${QT_USE_FILE})

add_definitions(-fPIC)

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(pentobi_kde_thumbnailer_STAT_SRCS
  ../libboardgame_util/Assert.cpp
  ../libboardgame_util/Exception.cpp
  ../libboardgame_util/Log.cpp
  ../libboardgame_util/StringUtil.cpp
  ../libboardgame_base/SpreadsheetStringRep.cpp
  ../libboardgame_base/Transform.cpp
  ../libboardgame_sgf/InvalidPropertyValue.cpp
  ../libboardgame_sgf/InvalidTree.cpp
  ../libboardgame_sgf/MissingProperty.cpp
  ../libboardgame_sgf/Node.cpp
  ../libboardgame_sgf/Reader.cpp
  ../libboardgame_sgf/Tree.cpp
  ../libboardgame_sgf/TreeReader.cpp
  ../libboardgame_sgf/Util.cpp
  ../libboardgame_sgf/Writer.cpp
  ../libpentobi_base/Board.cpp
  ../libpentobi_base/BoardConst.cpp
  ../libpentobi_base/BoardUpdater.cpp
  ../libpentobi_base/BoardUtil.cpp
  ../libpentobi_base/PieceInfo.cpp
  ../libpentobi_base/PieceTransforms.cpp
  ../libpentobi_base/PieceTransformsClassic.cpp
  ../libpentobi_base/PieceTransformsTrigon.cpp
  ../libpentobi_base/SgfUtil.cpp
  ../libpentobi_base/SymmetricPoints.cpp
  ../libpentobi_base/Tree.cpp
  ../libpentobi_base/Variant.cpp
  ../libpentobi_gui/BoardPainter.cpp
  ../libpentobi_gui/Util.cpp
  ../libpentobi_thumbnail/CreateThumbnail.cpp
  )

add_library(pentobi_kde_thumbnailer STATIC
  ${pentobi_kde_thumbnailer_STAT_SRCS}
)
