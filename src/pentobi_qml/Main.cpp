//-----------------------------------------------------------------------------
/** @file pentobi_qml/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include <QApplication>
#include <QtQml>
#include "BoardModel.h"
#include "PlayerModel.h"
#include "libboardgame_util/Log.h"

//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    libboardgame_util::LogInitializer log_initializer;
    qmlRegisterType<BoardModel>("pentobi", 1, 0, "BoardModel");
    qmlRegisterType<PlayerModel>("pentobi", 1, 0, "PlayerModel");
    qmlRegisterInterface<PieceModel>("PieceModel");
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///pentobi_qml/qml/Main.qml")));
    return app.exec();
}

//-----------------------------------------------------------------------------
