//-----------------------------------------------------------------------------
/** @file pentobi_qml/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
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
    QString locale = QLocale::system().name();
    QTranslator translatorPentobi;
    translatorPentobi.load("pentobi_qml_" + locale, ":qml/translations");
    app.installTranslator(&translatorPentobi);
    // The translation of standard buttons in QtQuick.Dialogs.MessageDialog
    // is broken on Android (tested with Qt 5.5; QTBUG-43353), so we
    // created our own file, which contains the translations we need.
    QTranslator translatorQt;
    translatorQt.load("replace_qtbase_" + locale, ":qml/translations");
    app.installTranslator(&translatorQt);
    try
    {
        QQmlApplicationEngine engine(QUrl("qrc:///qml/Main.qml"));
        return app.exec();
    }
    catch (const bad_alloc& e)
    {
        // bad_alloc can happen because the player requires a larger
        // amount of memory (e.g. it happened with Pentobi 10.0 on some
        // Android devices).
        auto title = QCoreApplication::translate("main", "Pentobi");
        auto text =
                QCoreApplication::translate("main", "Not enough memory.");
        QMessageBox::critical(nullptr, title, text);
        return 1;
    }
}

//-----------------------------------------------------------------------------
