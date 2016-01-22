//-----------------------------------------------------------------------------
/** @file pentobi_qml/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>
#include <QTranslator>
#include <QtQml>
#include "GameModel.h"
#include "PlayerModel.h"
#include "libboardgame_util/Log.h"

using libboardgame_util::RandomGenerator;

//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    libboardgame_util::LogInitializer log_initializer;
    QCoreApplication::setOrganizationName("Pentobi");
    QCoreApplication::setApplicationName("Pentobi");
    qmlRegisterType<GameModel>("pentobi", 1, 0, "GameModel");
    qmlRegisterType<PlayerModel>("pentobi", 1, 0, "PlayerModel");
    qmlRegisterInterface<PieceModel>("PieceModel");
    QApplication app(argc, argv);
    QString locale = QLocale::system().name();
    QTranslator translatorPentobi;
    translatorPentobi.load("qml_" + locale, ":qml/i18n");
    app.installTranslator(&translatorPentobi);
    // The translation of standard buttons in QtQuick.Dialogs.MessageDialog
    // is broken on Android (tested with Qt 5.5; QTBUG-43353), so we
    // created our own file, which contains the translations we need.
    QTranslator translatorQt;
    translatorQt.load("replace_qtbase_" + locale, ":qml/i18n");
    app.installTranslator(&translatorQt);
    QCommandLineParser parser;
    QCommandLineOption optionSeed("seed", "Set random seed to <n>.", "n");
    parser.addOption(optionSeed);
    QCommandLineOption optionVerbose("verbose");
    parser.addOption(optionVerbose);
    parser.process(app);
    try
    {
        bool ok;
        if (parser.isSet(optionSeed))
        {
            auto seed = parser.value(optionSeed).toUInt(&ok);
            if (! ok)
                throw runtime_error("--seed must be a positive number");
            RandomGenerator::set_global_seed(seed);
        }
#if LIBBOARDGAME_DISABLE_LOG
        if (parser.isSet(optionVerbose))
            throw runtime_error("This version of Pentobi was compiled"
                                " without support for logging.");
#else
        if (! parser.isSet(optionVerbose))
            libboardgame_util::disable_logging();
#endif
        QQmlApplicationEngine engine(QUrl("qrc:///qml/Main.qml"));
        return app.exec();
    }
    catch (const bad_alloc&)
    {
        // bad_alloc is an expected error because the player requires a larger
        // amount of memory.
        auto title = QCoreApplication::translate("main", "Pentobi");
        auto text =
                QCoreApplication::translate("main", "Not enough memory.");
        QMessageBox::critical(nullptr, title, text);
        return 1;
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}

//-----------------------------------------------------------------------------
