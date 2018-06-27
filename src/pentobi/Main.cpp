//-----------------------------------------------------------------------------
/** @file pentobi/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QSettings>
#include <QStyle>
#include <QTranslator>
#include "MainWindow.h"

#ifdef Q_OS_WIN
#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

using libboardgame_util::LogInitializer;
using libboardgame_util::RandomGenerator;

//-----------------------------------------------------------------------------

namespace {

void redirectStdErr()
{
#ifdef Q_OS_WIN
    CONSOLE_SCREEN_BUFFER_INFO info;
    AllocConsole();
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    info.dwSize.Y = 500;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), info.dwSize);
    auto stdErrHandle = (intptr_t)GetStdHandle(STD_ERROR_HANDLE);
    int conHandle = _open_osfhandle(stdErrHandle, _O_TEXT);
    auto f = _fdopen(conHandle, "w");
    *stderr = *f;
    setvbuf(stderr, NULL, _IONBF, 0);
    ios::sync_with_stdio();
#endif
}

} // namespace

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    LogInitializer log_initializer;
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);
    app.setOrganizationName("Pentobi");
    app.setApplicationName("Pentobi");
    try
    {
        // For some reason, labels in the status bar have a border on
        // Windows 7 with Qt 4.8. We don't want that.
        app.setStyleSheet("QStatusBar::item { border: 0px solid black }");

#ifdef PENTOBI_HELP_DIR
        QString helpDir = PENTOBI_HELP_DIR;
#else
        QString helpDir;
#endif
#ifdef PENTOBI_BOOKS_DIR
        QString booksDir = PENTOBI_BOOKS_DIR;
#else
        QString booksDir;
#endif
#ifdef PENTOBI_TRANSLATIONS
        QString translationsPentobiDir = PENTOBI_TRANSLATIONS;
        QString translationsLibPentobiGuiDir = PENTOBI_TRANSLATIONS;
#else
        QString translationsPentobiDir;
        QString translationsLibPentobiGuiDir;
#endif
        // Allow the user to override installation paths with a config file in
        // the directory of the executable to test it without installation
        auto overrideConfigFile = app.applicationDirPath() + "/pentobi.conf";
        if (QFileInfo::exists(overrideConfigFile))
        {
            QSettings settings(overrideConfigFile, QSettings::IniFormat);
            helpDir = settings.value("HelpDir").toString();
            booksDir = settings.value("BooksDir").toString();
            translationsPentobiDir =
                settings.value("TranslationsPentobiDir").toString();
            translationsLibPentobiGuiDir =
                settings.value("TranslationsLibPentobiGuiDir").toString();
        }

        QString locale = QLocale::system().name();

        // qtbase_minimal is included for languages not yet supported by Qt
        // and contains only the translations for qtbase that we need
        QTranslator qtMinimalTranslator;
        qtMinimalTranslator.load("qtbase_minimal_" + locale,
                                 translationsPentobiDir);
        app.installTranslator(&qtMinimalTranslator);

        QTranslator qtTranslator;
        QString qtTranslationPath =
            QLibraryInfo::location(QLibraryInfo::TranslationsPath);
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
        QCommandLineOption optionNoBook("nobook", "Do not use opening books.");
        parser.addOption(optionNoBook);
        QCommandLineOption optionNoDelay(
                    "nodelay", "Do not delay fast computer moves.");
        parser.addOption(optionNoDelay);
        QCommandLineOption optionSeed("seed", "Set random seed to <n>.", "n");
        parser.addOption(optionSeed);
        QCommandLineOption optionThreads("threads", "Use <n> threads.", "n");
        parser.addOption(optionThreads);
        QCommandLineOption optionVerbose(
                    "verbose", "Print logging information to standard error.");
        parser.addOption(optionVerbose);
        parser.addPositionalArgument("file.blksgf",
                                     "Blokus SGF file to open (optional).");
        parser.addHelpOption();
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
            libboardgame_util::disable_logging();
        else
            redirectStdErr();
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
            MainWindow mainWindow(nullptr, variant, initialFile, helpDir,
                                  maxLevel, booksDir, noBook, threads);
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
