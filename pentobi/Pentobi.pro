#############################################################################
# The preferred way of building Pentobi is using CMake. This project file
# exists only because building, deploying and debugging for Android is not
# yet functional for CMake projects with Qt <5.15. Note that this project
# does not work with multi-ABI builds, select only a single ABI in the qmake
# build settings (last tested with Qt 5.15.1, QtCreator 4.13.2).
#############################################################################

lessThan(QT_MAJOR_VERSION, 5) {
    error("Qt >=5.12 required")
}
equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 12) {
    error("Qt >=5.12 required")
}

TEMPLATE = app

QT += concurrent quickcontrols2 svg webview
android { QT += androidextras }

INCLUDEPATH += ..
CONFIG += c++17 qtquickcompiler
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES += QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
DEFINES += VERSION=\"\\\"18.2.X\\\"\"
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
    ImageProvider.cpp \
    Main.cpp \
    PieceModel.cpp \
    PlayerModel.cpp \
    RatingModel.cpp \
    ../libboardgame_base/Assert.cpp \
    ../libboardgame_base/Barrier.cpp \
    ../libboardgame_base/CpuTime.cpp \
    ../libboardgame_base/CpuTimeSource.cpp \
    ../libboardgame_base/IntervalChecker.cpp \
    ../libboardgame_base/Log.cpp \
    ../libboardgame_base/Memory.cpp \
    ../libboardgame_base/RandomGenerator.cpp \
    ../libboardgame_base/Rating.cpp \
    ../libboardgame_base/Reader.cpp \
    ../libboardgame_base/RectTransform.cpp \
    ../libboardgame_base/SgfError.cpp \
    ../libboardgame_base/SgfNode.cpp \
    ../libboardgame_base/SgfTree.cpp \
    ../libboardgame_base/SgfUtil.cpp \
    ../libboardgame_base/StringRep.cpp \
    ../libboardgame_base/StringUtil.cpp \
    ../libboardgame_base/TimeIntervalChecker.cpp \
    ../libboardgame_base/Timer.cpp \
    ../libboardgame_base/TimeSource.cpp \
    ../libboardgame_base/Transform.cpp \
    ../libboardgame_base/TreeReader.cpp \
    ../libboardgame_base/TreeWriter.cpp \
    ../libboardgame_base/WallTimeSource.cpp \
    ../libboardgame_base/Writer.cpp \
    ../libpentobi_base/Board.cpp \
    ../libpentobi_base/BoardConst.cpp \
    ../libpentobi_base/BoardUpdater.cpp \
    ../libpentobi_base/BoardUtil.cpp \
    ../libpentobi_base/Book.cpp \
    ../libpentobi_base/CallistoGeometry.cpp \
    ../libpentobi_base/Game.cpp \
    ../libpentobi_base/GembloQGeometry.cpp \
    ../libpentobi_base/GembloQTransform.cpp \
    ../libpentobi_base/NexosGeometry.cpp \
    ../libpentobi_base/NodeUtil.cpp \
    ../libpentobi_base/PentobiSgfUtil.cpp \
    ../libpentobi_base/PentobiTreeWriter.cpp \
    ../libpentobi_base/PieceInfo.cpp \
    ../libpentobi_base/PieceTransforms.cpp \
    ../libpentobi_base/PieceTransformsClassic.cpp \
    ../libpentobi_base/PieceTransformsGembloQ.cpp \
    ../libpentobi_base/PieceTransformsTrigon.cpp \
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
    ../libpentobi_mcts/StateUtil.cpp \
    ../libpentobi_paint/Paint.cpp

RESOURCES += \
    ../opening_books/pentobi_books.qrc \
    icon/pentobi_icon.qrc \
    qml/themes/themes.qrc \
    resources.qrc

!android {
    RESOURCES += resources_desktop.qrc
}

HEADERS += \
    AnalyzeGameModel.h \
    AndroidUtils.h \
    GameModel.h \
    ImageProvider.h \
    PieceModel.h \
    PlayerModel.h \
    RatingModel.h \
    SyncSettings.h \
    ../libboardgame_base/ArrayList.h \
    ../libboardgame_base/Assert.h \
    ../libboardgame_base/Barrier.h \
    ../libboardgame_base/Compiler.h \
    ../libboardgame_base/CoordPoint.h \
    ../libboardgame_base/CpuTime.h \
    ../libboardgame_base/CpuTimeSource.h \
    ../libboardgame_base/FmtSaver.h \
    ../libboardgame_base/Geometry.h \
    ../libboardgame_base/GeometryUtil.h \
    ../libboardgame_base/Grid.h \
    ../libboardgame_base/IntervalChecker.h \
    ../libboardgame_base/Log.h \
    ../libboardgame_base/Marker.h \
    ../libboardgame_base/MathUtil.h \
    ../libboardgame_base/Memory.h \
    ../libboardgame_base/Options.h \
    ../libboardgame_base/Point.h \
    ../libboardgame_base/PointTransform.h \
    ../libboardgame_base/RandomGenerator.h \
    ../libboardgame_base/Rating.h \
    ../libboardgame_base/RectGeometry.h \
    ../libboardgame_base/RectTransform.h \
    ../libboardgame_base/Statistics.h \
    ../libboardgame_base/StringRep.h \
    ../libboardgame_base/StringUtil.h \
    ../libboardgame_base/TimeIntervalChecker.h \
    ../libboardgame_base/Timer.h \
    ../libboardgame_base/TimeSource.h \
    ../libboardgame_base/Transform.h \
    ../libboardgame_base/WallTimeSource.h \
    ../libboardgame_mcts/Atomic.h \
    ../libboardgame_mcts/LastGoodReply.h \
    ../libboardgame_mcts/Node.h \
    ../libboardgame_mcts/PlayerMove.h \
    ../libboardgame_mcts/SearchBase.h \
    ../libboardgame_mcts/Tree.h \
    ../libboardgame_mcts/TreeUtil.h \
    ../libboardgame_base/Reader.h \
    ../libboardgame_base/SgfError.h \
    ../libboardgame_base/SgfNode.h \
    ../libboardgame_base/SgfTree.h \
    ../libboardgame_base/SgfUtil.h \
    ../libboardgame_base/TreeReader.h \
    ../libboardgame_base/Writer.h \
    ../libpentobi_base/Board.h \
    ../libpentobi_base/BoardConst.h \
    ../libpentobi_base/BoardUpdater.h \
    ../libpentobi_base/BoardUtil.h \
    ../libpentobi_base/Book.h \
    ../libpentobi_base/CallistoGeometry.h \
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
    ../libpentobi_mcts/Util.h \
    ../libpentobi_paint/Paint.h

lupdate_only {
SOURCES += \
    qml/*.qml \
    qml/*.js
}

TRANSLATIONS = \
    qml/i18n/qml_de.ts \
    qml/i18n/qml_es.ts \
    qml/i18n/qml_fr.ts \
    qml/i18n/qml_nb_NO.ts \
    qml/i18n/qml_zh_CN.ts

qtPrepareTool(LRELEASE, lrelease)
update_qm.input = TRANSLATIONS
update_qm.output = $$OUT_PWD/${QMAKE_FILE_BASE}.qm
update_qm.commands = $$LRELEASE -removeidentical -nounfinished \
    ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
update_qm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += update_qm

COPY_QRC = \
    qml/i18n/translations.qrc \
    docbook/help.qrc
copy_qrc.input = COPY_QRC
copy_qrc.output = $$OUT_PWD/${QMAKE_FILE_BASE}.qrc
copy_qrc.commands = $$QMAKE_COPY_FILE ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_qrc.variable_out = RESOURCES
QMAKE_EXTRA_COMPILERS += copy_qrc

GEN_HELP_INPUT = docbook/index.docbook
# Currently ignores dependencies on figures, custom.xml, and po files
gen_help.input = GEN_HELP_INPUT
gen_help.output = $$OUT_PWD/help/C/pentobi/index.html
gen_help.commands = ${QMAKE_FILE_IN_PATH}/create-html $$OUT_PWD
gen_help.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += gen_help

OTHER_FILES += android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

ANDROID_ABIS = armeabi-v7a
