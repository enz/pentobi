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

QT += concurrent quickcontrols2 svg webview
android {
    QT += androidextras
}

INCLUDEPATH += ..
CONFIG += c++14
CONFIG += qtquickcompiler
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x051100
DEFINES += QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
DEFINES += VERSION=\"\\\"15.x\\\"\"
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
    AnalyzeGameModel.cpp \
    AndroidUtils.cpp \
    GameModel.cpp \
    Main.cpp \
    PieceModel.cpp \
    PlayerModel.cpp \
    RatingModel.cpp \
    ../libboardgame_base/CoordPoint.cpp \
    ../libboardgame_base/Rating.cpp \
    ../libboardgame_base/RectTransform.cpp \
    ../libboardgame_base/StringRep.cpp \
    ../libboardgame_util/Abort.cpp \
    ../libboardgame_util/Assert.cpp \
    ../libboardgame_util/Barrier.cpp \
    ../libboardgame_util/CpuTimeSource.cpp \
    ../libboardgame_util/IntervalChecker.cpp \
    ../libboardgame_util/Log.cpp \
    ../libboardgame_util/RandomGenerator.cpp \
    ../libboardgame_util/StringUtil.cpp \
    ../libboardgame_util/TimeIntervalChecker.cpp \
    ../libboardgame_util/Timer.cpp \
    ../libboardgame_util/TimeSource.cpp \
    ../libboardgame_util/WallTimeSource.cpp \
    ../libboardgame_sgf/Reader.cpp \
    ../libboardgame_sgf/SgfError.cpp \
    ../libboardgame_sgf/SgfNode.cpp \
    ../libboardgame_sgf/SgfTree.cpp \
    ../libboardgame_sgf/SgfUtil.cpp \
    ../libboardgame_sgf/TreeReader.cpp \
    ../libboardgame_sgf/TreeWriter.cpp \
    ../libboardgame_sgf/Writer.cpp \
    ../libboardgame_sys/CpuTime.cpp \
    ../libboardgame_sys/Memory.cpp \
    ../libpentobi_base/Board.cpp \
    ../libpentobi_base/BoardConst.cpp \
    ../libpentobi_base/BoardUpdater.cpp \
    ../libpentobi_base/BoardUtil.cpp \
    ../libpentobi_base/Book.cpp \
    ../libpentobi_base/CallistoGeometry.cpp \
    ../libpentobi_base/Color.cpp \
    ../libpentobi_base/Game.cpp \
    ../libpentobi_base/GembloQGeometry.cpp \
    ../libpentobi_base/GembloQTransform.cpp \
    ../libpentobi_base/NexosGeometry.cpp \
    ../libpentobi_base/NodeUtil.cpp \
    ../libpentobi_base/PentobiSgfUtil.cpp \
    ../libpentobi_base/PentobiTreeWriter.cpp \
    ../libpentobi_base/PieceInfo.cpp \
    ../libpentobi_base/PieceTransformsClassic.cpp \
    ../libpentobi_base/PieceTransformsGembloQ.cpp \
    ../libpentobi_base/PieceTransformsTrigon.cpp \
    ../libpentobi_base/PointState.cpp \
    ../libpentobi_base/StartingPoints.cpp \
    ../libpentobi_base/SymmetricPoints.cpp \
    ../libpentobi_base/TreeUtil.cpp \
    ../libpentobi_base/TrigonGeometry.cpp \
    ../libpentobi_base/TrigonTransform.cpp \
    ../libpentobi_base/Variant.cpp \
    ../libpentobi_base/PlayerBase.cpp \
    ../libpentobi_base/PentobiTree.cpp \
    ../libpentobi_mcts/AnalyzeGame.cpp \
    ../libpentobi_mcts/History.cpp \
    ../libpentobi_mcts/LocalPoints.cpp \
    ../libpentobi_mcts/Player.cpp \
    ../libpentobi_mcts/PriorKnowledge.cpp \
    ../libpentobi_mcts/Search.cpp \
    ../libpentobi_mcts/SharedConst.cpp \
    ../libpentobi_mcts/State.cpp \
    ../libpentobi_mcts/Util.cpp \
    ../libpentobi_mcts/StateUtil.cpp

RESOURCES += \
    ../books/pentobi_books.qrc \
    ../icon/pentobi_icon.qrc \
    ../../doc/help.qrc \
    qml/themes/themes.qrc \
    qml/i18n/translations.qrc \
    resources.qrc

HEADERS += \
    AnalyzeGameModel.h \
    AndroidUtils.h \
    GameModel.h \
    PieceModel.h \
    PlayerModel.h \
    RatingModel.h \
    ../libboardgame_base/CoordPoint.h \
    ../libboardgame_base/Geometry.h \
    ../libboardgame_base/GeometryUtil.h \
    ../libboardgame_base/Grid.h \
    ../libboardgame_base/Marker.h \
    ../libboardgame_base/Point.h \
    ../libboardgame_base/PointTransform.h \
    ../libboardgame_base/Rating.h \
    ../libboardgame_base/RectGeometry.h \
    ../libboardgame_base/RectTransform.h \
    ../libboardgame_base/StringRep.h \
    ../libboardgame_base/Transform.h \
    ../libboardgame_mcts/Atomic.h \
    ../libboardgame_mcts/LastGoodReply.h \
    ../libboardgame_mcts/Node.h \
    ../libboardgame_mcts/PlayerMove.h \
    ../libboardgame_mcts/SearchBase.h \
    ../libboardgame_mcts/Tree.h \
    ../libboardgame_mcts/TreeUtil.h \
    ../libboardgame_util/Abort.h \
    ../libboardgame_util/ArrayList.h \
    ../libboardgame_util/Assert.h \
    ../libboardgame_util/Barrier.h \
    ../libboardgame_util/CpuTimeSource.h \
    ../libboardgame_util/FmtSaver.h \
    ../libboardgame_util/IntervalChecker.h \
    ../libboardgame_util/Log.h \
    ../libboardgame_util/MathUtil.h \
    ../libboardgame_util/Options.h \
    ../libboardgame_util/RandomGenerator.h \
    ../libboardgame_util/Statistics.h \
    ../libboardgame_util/StringUtil.h \
    ../libboardgame_util/TimeIntervalChecker.h \
    ../libboardgame_util/Timer.h \
    ../libboardgame_util/TimeSource.h \
    ../libboardgame_util/Unused.h \
    ../libboardgame_util/WallTimeSource.h \
    ../libboardgame_sgf/Reader.h \
    ../libboardgame_sgf/SgfError.h \
    ../libboardgame_sgf/SgfNode.h \
    ../libboardgame_sgf/SgfTree.h \
    ../libboardgame_sgf/SgfUtil.h \
    ../libboardgame_sgf/TreeReader.h \
    ../libboardgame_sgf/Writer.h \
    ../libboardgame_sys/Compiler.h \
    ../libboardgame_sys/CpuTime.h \
    ../libboardgame_sys/Memory.h \
    ../libpentobi_base/Board.h \
    ../libpentobi_base/BoardConst.h \
    ../libpentobi_base/BoardUpdater.h \
    ../libpentobi_base/BoardUtil.h \
    ../libpentobi_base/Book.h \
    ../libpentobi_base/Color.h \
    ../libpentobi_base/ColorMap.h \
    ../libpentobi_base/ColorMove.h \
    ../libpentobi_base/Game.h \
    ../libpentobi_base/GembloQGeometry.h \
    ../libpentobi_base/GembloQTransform.h \
    ../libpentobi_base/Geometry.h \
    ../libpentobi_base/Grid.h \
    ../libpentobi_base/Marker.h \
    ../libpentobi_base/Move.h \
    ../libpentobi_base/MoveInfo.h \
    ../libpentobi_base/MoveList.h \
    ../libpentobi_base/MoveMarker.h \
    ../libpentobi_base/MovePoints.h \
    ../libpentobi_base/NexosGeometry.h \
    ../libpentobi_base/NodeUtil.h \
    ../libpentobi_base/PentobiTree.h \
    ../libpentobi_base/Piece.h \
    ../libpentobi_base/PieceInfo.h \
    ../libpentobi_base/PieceMap.h \
    ../libpentobi_base/PieceTransforms.h \
    ../libpentobi_base/PieceTransformsClassic.h \
    ../libpentobi_base/PieceTransformsGembloQ.h \
    ../libpentobi_base/PieceTransformsTrigon.h \
    ../libpentobi_base/PlayerBase.h \
    ../libpentobi_base/Point.h \
    ../libpentobi_base/PointList.h \
    ../libpentobi_base/PointState.h \
    ../libpentobi_base/PrecompMoves.h \
    ../libpentobi_base/Setup.h \
    ../libpentobi_base/PentobiSgfUtil.h \
    ../libpentobi_base/StartingPoints.h \
    ../libpentobi_base/SymmetricPoints.h \
    ../libpentobi_base/TreeUtil.h \
    ../libpentobi_base/TrigonGeometry.h \
    ../libpentobi_base/TrigonTransform.h \
    ../libpentobi_base/Variant.h \
    ../libpentobi_mcts/AnalyzeGame.h \
    ../libpentobi_mcts/Float.h \
    ../libpentobi_mcts/History.h \
    ../libpentobi_mcts/Player.h \
    ../libpentobi_mcts/PlayoutFeatures.h \
    ../libpentobi_mcts/PriorKnowledge.h \
    ../libpentobi_mcts/Search.h \
    ../libpentobi_mcts/SearchParamConst.h \
    ../libpentobi_mcts/SharedConst.h \
    ../libpentobi_mcts/State.h \
    ../libpentobi_mcts/StateUtil.h \
    ../libpentobi_mcts/Util.h

lupdate_only {
SOURCES += \
    qml/*.qml \
    qml/*.js
}

TRANSLATIONS = \
    qml/i18n/qml_de.ts \
    qml/i18n/qml_fr.ts \
    qml/i18n/qml_nb_NO.ts

qtPrepareTool(LRELEASE, lrelease)
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$LRELEASE -removeidentical -nounfinished ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm

OTHER_FILES += \
    android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
