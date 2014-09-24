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

using libboardgame_util::set_log_null;

//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    libboardgame_util::LogInitializer log_initializer;
#if QT_NO_DEBUG
    set_log_null();
#endif
    QCoreApplication::setOrganizationName("Pentobi");
    QCoreApplication::setApplicationName("Pentobi");
    qmlRegisterType<BoardModel>("pentobi", 1, 0, "BoardModel");
    qmlRegisterType<PlayerModel>("pentobi", 1, 0, "PlayerModel");
    qmlRegisterInterface<PieceModel>("PieceModel");
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:///qml/Main.qml"));
    return app.exec();
}

//-----------------------------------------------------------------------------
