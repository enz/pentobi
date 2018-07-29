#############################################################################
# The preferred way of building Pentobi is using CMake. This project file
# exists only because building, deploying and debugging for Android is not
# yet functional for CMake projects in QtCreator.
#############################################################################

lessThan(QT_MAJOR_VERSION, 5) {
    error("Qt >=5.11 required")
}
equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 11) {
    error("Qt >=5.11 required")
}

TEMPLATE = app

QT += qml quick svg concurrent webview quickcontrols2
android {
    QT += androidextras
}

INCLUDEPATH += src
CONFIG += c++14
CONFIG += qtquickcompiler
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x051100
DEFINES += VERSION=\"\\\"15.UNKNOWN\\\"\"
android {
    DEFINES += PENTOBI_LOW_RESOURCES
    QMAKE_CXXFLAGS_RELEASE += -DLIBBOARDGAME_DISABLE_LOG
}
QMAKE_CXXFLAGS_DEBUG += -DLIBBOARDGAME_DEBUG
gcc {
    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE -= -O3
    QMAKE_CXXFLAGS_RELEASE -= -Os
    QMAKE_CXXFLAGS_RELEASE *= -Ofast
}

SOURCES += \
    src/pentobi/AnalyzeGameModel.cpp \
    src/pentobi/AndroidUtils.cpp \
    src/pentobi/GameModel.cpp \
    src/pentobi/HelpFileExtractor.cpp \
    src/pentobi/Main.cpp \
    src/pentobi/PieceModel.cpp \
    src/pentobi/PlayerModel.cpp \
    src/pentobi/RatingModel.cpp \
    src/libboardgame_base/CoordPoint.cpp \
    src/libboardgame_base/Rating.cpp \
    src/libboardgame_base/RectTransform.cpp \
    src/libboardgame_base/StringRep.cpp \
    src/libboardgame_util/Abort.cpp \
    src/libboardgame_util/Assert.cpp \
    src/libboardgame_util/Barrier.cpp \
    src/libboardgame_util/CpuTimeSource.cpp \
    src/libboardgame_util/IntervalChecker.cpp \
    src/libboardgame_util/Log.cpp \
    src/libboardgame_util/RandomGenerator.cpp \
    src/libboardgame_util/StringUtil.cpp \
    src/libboardgame_util/TimeIntervalChecker.cpp \
    src/libboardgame_util/Timer.cpp \
    src/libboardgame_util/TimeSource.cpp \
    src/libboardgame_util/WallTimeSource.cpp \
    src/libboardgame_sgf/Reader.cpp \
    src/libboardgame_sgf/SgfError.cpp \
    src/libboardgame_sgf/SgfNode.cpp \
    src/libboardgame_sgf/SgfTree.cpp \
    src/libboardgame_sgf/SgfUtil.cpp \
    src/libboardgame_sgf/TreeReader.cpp \
    src/libboardgame_sgf/TreeWriter.cpp \
    src/libboardgame_sgf/Writer.cpp \
    src/libboardgame_sys/CpuTime.cpp \
    src/libboardgame_sys/Memory.cpp \
    src/libpentobi_base/Board.cpp \
    src/libpentobi_base/BoardConst.cpp \
    src/libpentobi_base/BoardUpdater.cpp \
    src/libpentobi_base/BoardUtil.cpp \
    src/libpentobi_base/Book.cpp \
    src/libpentobi_base/CallistoGeometry.cpp \
    src/libpentobi_base/Color.cpp \
    src/libpentobi_base/Game.cpp \
    src/libpentobi_base/GembloQGeometry.cpp \
    src/libpentobi_base/GembloQTransform.cpp \
    src/libpentobi_base/NexosGeometry.cpp \
    src/libpentobi_base/NodeUtil.cpp \
    src/libpentobi_base/PentobiSgfUtil.cpp \
    src/libpentobi_base/PentobiTreeWriter.cpp \
    src/libpentobi_base/PieceInfo.cpp \
    src/libpentobi_base/PieceTransformsClassic.cpp \
    src/libpentobi_base/PieceTransformsGembloQ.cpp \
    src/libpentobi_base/PieceTransformsTrigon.cpp \
    src/libpentobi_base/PointState.cpp \
    src/libpentobi_base/StartingPoints.cpp \
    src/libpentobi_base/SymmetricPoints.cpp \
    src/libpentobi_base/TreeUtil.cpp \
    src/libpentobi_base/TrigonGeometry.cpp \
    src/libpentobi_base/TrigonTransform.cpp \
    src/libpentobi_base/Variant.cpp \
    src/libpentobi_base/PlayerBase.cpp \
    src/libpentobi_base/PentobiTree.cpp \
    src/libpentobi_mcts/AnalyzeGame.cpp \
    src/libpentobi_mcts/History.cpp \
    src/libpentobi_mcts/LocalPoints.cpp \
    src/libpentobi_mcts/Player.cpp \
    src/libpentobi_mcts/PriorKnowledge.cpp \
    src/libpentobi_mcts/Search.cpp \
    src/libpentobi_mcts/SharedConst.cpp \
    src/libpentobi_mcts/State.cpp \
    src/libpentobi_mcts/Util.cpp \
    src/libpentobi_mcts/StateUtil.cpp

RESOURCES += \
    src/books/pentobi_books.qrc \
    src/icon/pentobi_icon.qrc \
    doc/help.qrc \
    src/pentobi/qml/themes/themes.qrc \
    src/pentobi/qml/i18n/translations.qrc \
    src/pentobi/resources.qrc

HEADERS += \
    src/pentobi/AnalyzeGameModel.h \
    src/pentobi/AndroidUtils.h \
    src/pentobi/GameModel.h \
    src/pentobi/HelpFileExtractor.h \
    src/pentobi/PieceModel.h \
    src/pentobi/PlayerModel.h \
    src/pentobi/RatingModel.h \
    src/libboardgame_base/CoordPoint.h \
    src/libboardgame_base/Geometry.h \
    src/libboardgame_base/GeometryUtil.h \
    src/libboardgame_base/Grid.h \
    src/libboardgame_base/Marker.h \
    src/libboardgame_base/Point.h \
    src/libboardgame_base/PointTransform.h \
    src/libboardgame_base/Rating.h \
    src/libboardgame_base/RectGeometry.h \
    src/libboardgame_base/RectTransform.h \
    src/libboardgame_base/StringRep.h \
    src/libboardgame_base/Transform.h \
    src/libboardgame_mcts/Atomic.h \
    src/libboardgame_mcts/LastGoodReply.h \
    src/libboardgame_mcts/Node.h \
    src/libboardgame_mcts/PlayerMove.h \
    src/libboardgame_mcts/SearchBase.h \
    src/libboardgame_mcts/Tree.h \
    src/libboardgame_mcts/TreeUtil.h \
    src/libboardgame_util/Abort.h \
    src/libboardgame_util/ArrayList.h \
    src/libboardgame_util/Assert.h \
    src/libboardgame_util/Barrier.h \
    src/libboardgame_util/CpuTimeSource.h \
    src/libboardgame_util/FmtSaver.h \
    src/libboardgame_util/IntervalChecker.h \
    src/libboardgame_util/Log.h \
    src/libboardgame_util/MathUtil.h \
    src/libboardgame_util/Options.h \
    src/libboardgame_util/RandomGenerator.h \
    src/libboardgame_util/Statistics.h \
    src/libboardgame_util/StringUtil.h \
    src/libboardgame_util/TimeIntervalChecker.h \
    src/libboardgame_util/Timer.h \
    src/libboardgame_util/TimeSource.h \
    src/libboardgame_util/Unused.h \
    src/libboardgame_util/WallTimeSource.h \
    src/libboardgame_sgf/Reader.h \
    src/libboardgame_sgf/SgfError.h \
    src/libboardgame_sgf/SgfNode.h \
    src/libboardgame_sgf/SgfTree.h \
    src/libboardgame_sgf/SgfUtil.h \
    src/libboardgame_sgf/TreeReader.h \
    src/libboardgame_sgf/Writer.h \
    src/libboardgame_sys/Compiler.h \
    src/libboardgame_sys/CpuTime.h \
    src/libboardgame_sys/Memory.h \
    src/libpentobi_base/Board.h \
    src/libpentobi_base/BoardConst.h \
    src/libpentobi_base/BoardUpdater.h \
    src/libpentobi_base/BoardUtil.h \
    src/libpentobi_base/Book.h \
    src/libpentobi_base/Color.h \
    src/libpentobi_base/ColorMap.h \
    src/libpentobi_base/ColorMove.h \
    src/libpentobi_base/Game.h \
    src/libpentobi_base/GembloQGeometry.h \
    src/libpentobi_base/GembloQTransform.h \
    src/libpentobi_base/Geometry.h \
    src/libpentobi_base/Grid.h \
    src/libpentobi_base/Marker.h \
    src/libpentobi_base/Move.h \
    src/libpentobi_base/MoveInfo.h \
    src/libpentobi_base/MoveList.h \
    src/libpentobi_base/MoveMarker.h \
    src/libpentobi_base/MovePoints.h \
    src/libpentobi_base/NexosGeometry.h \
    src/libpentobi_base/NodeUtil.h \
    src/libpentobi_base/PentobiTree.h \
    src/libpentobi_base/Piece.h \
    src/libpentobi_base/PieceInfo.h \
    src/libpentobi_base/PieceMap.h \
    src/libpentobi_base/PieceTransforms.h \
    src/libpentobi_base/PieceTransformsClassic.h \
    src/libpentobi_base/PieceTransformsGembloQ.h \
    src/libpentobi_base/PieceTransformsTrigon.h \
    src/libpentobi_base/PlayerBase.h \
    src/libpentobi_base/Point.h \
    src/libpentobi_base/PointList.h \
    src/libpentobi_base/PointState.h \
    src/libpentobi_base/PrecompMoves.h \
    src/libpentobi_base/Setup.h \
    src/libpentobi_base/PentobiSgfUtil.h \
    src/libpentobi_base/StartingPoints.h \
    src/libpentobi_base/SymmetricPoints.h \
    src/libpentobi_base/TreeUtil.h \
    src/libpentobi_base/TrigonGeometry.h \
    src/libpentobi_base/TrigonTransform.h \
    src/libpentobi_base/Variant.h \
    src/libpentobi_mcts/AnalyzeGame.h \
    src/libpentobi_mcts/Float.h \
    src/libpentobi_mcts/History.h \
    src/libpentobi_mcts/Player.h \
    src/libpentobi_mcts/PlayoutFeatures.h \
    src/libpentobi_mcts/PriorKnowledge.h \
    src/libpentobi_mcts/Search.h \
    src/libpentobi_mcts/SearchParamConst.h \
    src/libpentobi_mcts/SharedConst.h \
    src/libpentobi_mcts/State.h \
    src/libpentobi_mcts/StateUtil.h \
    src/libpentobi_mcts/Util.h

lupdate_only {
SOURCES += \
    src/pentobi/qml/*.qml \
    src/pentobi/qml/*.js
}

TRANSLATIONS = \
    src/pentobi/qml/i18n/qml_de.ts \
    src/pentobi/qml/i18n/qml_fr.ts \
    src/pentobi/qml/i18n/qml_nb_NO.ts

qtPrepareTool(LRELEASE, lrelease)
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$LRELEASE -removeidentical -nounfinished ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm

OTHER_FILES += \
    src/pentobi/android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
