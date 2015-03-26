//-----------------------------------------------------------------------------
/** @file pentobi/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QSettings>
#include <QStyle>
#include <QTranslator>
#include "Application.h"
#include "MainWindow.h"
#include "ShowMessage.h"
#include "libboardgame_util/Options.h"
#include "libboardgame_sys/Compiler.h"

#ifdef Q_WS_WIN
#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

using libboardgame_util::set_log_null;
using libboardgame_util::LogInitializer;
using libboardgame_util::OptionError;
using libboardgame_util::Options;
using libboardgame_util::RandomGenerator;
using libboardgame_sys::get_type_name;

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
    QCoreApplication::setOrganizationName("Pentobi");
    QCoreApplication::setApplicationName("Pentobi");
    Application app(argc, argv);
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    Q_INIT_RESOURCE(libpentobi_gui_resources_2x);
#endif
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
        QString appDir = QCoreApplication::applicationDirPath();
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
        if (QFileInfo(overrideConfigFile).exists())
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

        vector<string> specs = {
            "memory:",
            "nobook",
            "nodelay",
            "seed|r:",
            "threads:",
            "verbose"
        };
        Options opt(argc, argv, specs);
        size_t memory = 0;
        if (opt.contains("memory"))
        {
            memory = opt.get<size_t>("memory");
            if (memory == 0)
                throw OptionError("Value for memory must be greater zero.");
        }
        unsigned threads = 0;
        if (opt.contains("threads"))
        {
            threads = opt.get<unsigned>("threads");
            if (threads == 0)
                throw OptionError("Number of threads must be greater zero.");
            if (! libpentobi_mcts::SearchParamConst::multithread
                    && threads > 1)
                throw OptionError("This version of Pentobi was compiled"
                                  " without support for multi-threading.");
        }
        if (! opt.contains("verbose"))
            set_log_null();
#ifdef Q_WS_WIN
        if (opt.contains("verbose"))
            redirectStdErr();
#endif
        if (opt.contains("seed"))
            RandomGenerator::set_global_seed(opt.get<uint32_t>("seed"));
        bool noBook = opt.contains("nobook");
        QString initialFile;
        auto& args = opt.get_args();
        if (! args.empty())
            initialFile = QString::fromLocal8Bit(args[0].c_str());
        QSettings settings;
        auto variantString = settings.value("variant", "").toString();
        Variant variant;
        if (! parse_variant_id(variantString.toLocal8Bit().constData(),
                               variant))
            variant = Variant::classic;
        MainWindow mainWindow(variant, initialFile, helpDir, booksDir, noBook,
                              threads, memory);
        if (opt.contains("seed"))
            mainWindow.setDeterministic();
        if (opt.contains("nodelay"))
            mainWindow.setNoDelay();
        mainWindow.show();
        return app.exec();
    }
    catch (const OptionError& e)
    {
        QMessageBox::critical(nullptr, "Pentobi",
                              "Invalid command line option:\n\n"
                              + QString::fromLocal8Bit(e.what()));
        return 1;
    }
    catch (const exception& e)
    {
        string detailedText = get_type_name(e) + ": " + e.what();
        showFatal(QString::fromLocal8Bit(detailedText.c_str()));
        return 1;
    }
}

//-----------------------------------------------------------------------------
