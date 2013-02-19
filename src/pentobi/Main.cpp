//-----------------------------------------------------------------------------
/** @file pentobi/Main.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QSettings>
#include <QTranslator>
#include "Application.h"
#include "MainWindow.h"
#include "libboardgame_util/Options.h"

#ifdef Q_WS_WIN
#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

using libboardgame_util::set_log_null;
using libboardgame_util::Options;
using libboardgame_util::RandomGenerator;

//-----------------------------------------------------------------------------

namespace {

#ifdef Q_WS_WIN

/** @todo: Move to an extra file to encapsulate the inclusion of <windows.h>,
    which does some ugly stuff, like defining a macro named max that breaks
    standard headers. */
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
    try
    {
        Q_INIT_RESOURCE(libpentobi_gui_resources);
        QCoreApplication::setOrganizationName("Pentobi");
        QCoreApplication::setApplicationName("Pentobi");
        Application app(argc, argv);

        // Allow the user to override installation paths with a config file in
        // the directory of the executable to test it without installation
        QString manualDir;
        QString booksDir;
        QString translationsPentobiDir;
        QString translationsLibPentobiGuiDir;
        QString appDir = QCoreApplication::applicationDirPath();
#ifdef PENTOBI_MANUAL_DIR
        manualDir = PENTOBI_MANUAL_DIR;
#endif
        if (manualDir.isEmpty())
            manualDir = appDir + "/manual";
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
            manualDir = settings.value("ManualDir", manualDir).toString();
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

        vector<string> specs;
        specs.push_back("memory:");
        specs.push_back("nobook");
        specs.push_back("seed|r:");
        specs.push_back("threads:");
        specs.push_back("verbose");
        Options opt(argc, argv, specs);
        size_t memory = 0;
        if (opt.contains("memory"))
        {
            memory = opt.get<size_t>("memory");
            if (memory == 0)
                throw Exception("Value for memory must be greater zero.");
        }
        unsigned threads = 0;
        if (opt.contains("threads"))
        {
            threads = opt.get<unsigned>("threads");
            if (threads == 0)
                throw Exception("Number of threads must be greater zero.");
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
            initialFile = args[0].c_str();
        MainWindow mainWindow(initialFile, manualDir, booksDir, noBook,
                              threads, memory);
        if (opt.contains("seed"))
            mainWindow.setDeterministic();
        mainWindow.show();
        return app.exec();
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    catch (...)
    {
        cerr << "Error: unknown exception\n";
        return 1;
    }
}

//-----------------------------------------------------------------------------
