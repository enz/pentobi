//-----------------------------------------------------------------------------
/** @file pentobi/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QCommandLineParser>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QSettings>
#include <QStyle>
#include <QTranslator>
#include "Application.h"
#include "MainWindow.h"

#ifdef Q_WS_WIN
#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

using libboardgame_util::LogInitializer;
using libboardgame_util::RandomGenerator;

//-----------------------------------------------------------------------------

namespace {

#ifdef Q_WS_WIN

void redirectStdErr()
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    AllocConsole();
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    info.dwSize.Y = 500;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), info.dwSize);
    long stdErrHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    int conHandle = _open_osfhandle(stdErrHandle, _O_TEXT);
    auto f = _fdopen(conHandle, "w");
    *stderr = *f;
    setvbuf(stderr, NULL, _IONBF, 0);
    ios::sync_with_stdio();
}

#endif

} // namespace

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    LogInitializer log_initializer;
    Q_INIT_RESOURCE(libpentobi_gui_resources);
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    Application app(argc, argv);
    app.setOrganizationName("Pentobi");
    app.setApplicationName("Pentobi");
    Q_INIT_RESOURCE(libpentobi_gui_resources_2x);
    try
    {
        // For some reason, labels in the status bar have a border on
        // Windows 7 with Qt 4.8. We don't want that.
        app.setStyleSheet("QStatusBar::item { border: 0px solid black }");

        // Allow the user to override installation paths with a config file in
        // the directory of the executable to test it without installation
        QString helpDir;
        QString booksDir;
        QString translationsPentobiDir;
        QString translationsLibPentobiGuiDir;
        QString appDir = app.applicationDirPath();
#ifdef PENTOBI_HELP_DIR
        helpDir = PENTOBI_HELP_DIR;
#endif
        if (helpDir.isEmpty())
            helpDir = appDir + "/help";
#ifdef PENTOBI_BOOKS_DIR
        booksDir = PENTOBI_BOOKS_DIR;
#endif
        if (booksDir.isEmpty())
            booksDir = appDir + "/books";
#ifdef PENTOBI_TRANSLATIONS
        translationsPentobiDir = PENTOBI_TRANSLATIONS;
        translationsLibPentobiGuiDir = PENTOBI_TRANSLATIONS;
#endif
        if (translationsPentobiDir.isEmpty())
            translationsPentobiDir = appDir + "/translations";
        if (translationsLibPentobiGuiDir.isEmpty())
            translationsLibPentobiGuiDir = appDir + "/translations";
        QString overrideConfigFile = appDir + "/pentobi.conf";
        if (QFileInfo::exists(overrideConfigFile))
        {
            QSettings settings(overrideConfigFile, QSettings::IniFormat);
            helpDir = settings.value("HelpDir", helpDir).toString();
            booksDir = settings.value("BooksDir", booksDir).toString();
            translationsPentobiDir =
                settings.value("TranslationsPentobiDir",
                               translationsPentobiDir).toString();
            translationsLibPentobiGuiDir =
                settings.value("TranslationsLibPentobiGuiDir",
                               translationsLibPentobiGuiDir).toString();
        }

        QTranslator qtTranslator;
        QString qtTranslationPath =
            QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        QString locale = QLocale::system().name();
        qtTranslator.load("qt_" + locale, qtTranslationPath);
        app.installTranslator(&qtTranslator);
        QTranslator libPentobiGuiTranslator;
        libPentobiGuiTranslator.load("libpentobi_gui_" + locale,
                                     translationsLibPentobiGuiDir);
        app.installTranslator(&libPentobiGuiTranslator);
        QTranslator pentobiTranslator;
        pentobiTranslator.load("pentobi_" + locale, translationsPentobiDir);
        app.installTranslator(&pentobiTranslator);

        QCommandLineParser parser;
        auto maxSupportedLevel = Player::max_supported_level;
        QCommandLineOption optionMaxLevel("maxlevel",
                                          "Set maximum level to <n>.", "n",
                                          QString::number(maxSupportedLevel));
        parser.addOption(optionMaxLevel);
        QCommandLineOption optionNoBook("nobook");
        parser.addOption(optionNoBook);
        QCommandLineOption optionNoDelay("nodelay");
        parser.addOption(optionNoDelay);
        QCommandLineOption optionSeed("seed", "Set random seed to <n>.", "n");
        parser.addOption(optionSeed);
        QCommandLineOption optionThreads("threads", "Use <n> threads.", "n");
        parser.addOption(optionThreads);
        QCommandLineOption optionVerbose("verbose");
        parser.addOption(optionVerbose);
        parser.process(app);
        bool ok;
        auto maxLevel = parser.value(optionMaxLevel).toUInt(&ok);
        if (! ok || maxLevel < 1 || maxLevel > maxSupportedLevel)
            throw runtime_error("--maxlevel must be between 1 and "
                                + to_string(maxSupportedLevel));
        unsigned threads = 0;
        if (parser.isSet(optionThreads))
        {
            threads = parser.value(optionThreads).toUInt(&ok);
            if (! ok || threads == 0)
                throw runtime_error("--threads must be greater zero.");
            if (! libpentobi_mcts::SearchParamConst::multithread
                    && threads > 1)
                throw runtime_error("This version of Pentobi was compiled"
                                    " without support for multi-threading.");
        }
        if (! parser.isSet(optionVerbose))
        {
            libboardgame_util::disable_logging();
#ifdef Q_WS_WIN
            redirectStdErr();
#endif
        }
        if (parser.isSet(optionSeed))
        {
            RandomGenerator::ResultType seed =
                    parser.value(optionSeed).toUInt(&ok);
            if (! ok)
                throw runtime_error("--seed must be a positive number");
            RandomGenerator::set_global_seed(seed);
        }
        bool noBook = parser.isSet(optionNoBook);
        QString initialFile;
        auto args = parser.positionalArguments();
        if (! args.empty())
            initialFile = args.at(0);
        QSettings settings;
        auto variantString = settings.value("variant", "").toString();
        Variant variant;
        if (! parse_variant_id(variantString.toLocal8Bit().constData(),
                               variant))
            variant = Variant::duo;
        try
        {
            MainWindow mainWindow(variant, initialFile, helpDir, maxLevel,
                                  booksDir, noBook, threads);
            if (parser.isSet(optionNoDelay))
                mainWindow.setNoDelay();
            mainWindow.show();
            return app.exec();
        }
        catch (bad_alloc&)
        {
            // bad_alloc is an expected error because libpentobi_mcts::Player
            // requires a larger amount of memory and an error message should
            // be shown to the user. It needs to be handled here because it
            // needs the translators being installed for the error message.
            QMessageBox::critical(
                        nullptr, app.translate("main", "Pentobi"),
                        app.translate("main", "Not enough memory."));
        }
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}

//-----------------------------------------------------------------------------
