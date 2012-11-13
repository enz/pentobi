//-----------------------------------------------------------------------------
/** @file pentobi/Main.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <boost/program_options.hpp>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QSettings>
#include <QTranslator>
#include "Application.h"
#include "MainWindow.h"

#ifdef Q_WS_WIN
#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

using boost::program_options::command_line_parser;
using boost::program_options::options_description;
using boost::program_options::positional_options_description;
using boost::program_options::store;
using boost::program_options::value;
using boost::program_options::variables_map;
using libboardgame_util::set_log_null;
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
    FILE* f = _fdopen(conHandle, "w");
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
        vector<string> arguments;
        options_description normal_options("Options");
        uint32_t seed;
        size_t memory = 0;
        normal_options.add_options()
            ("memory", value<>(&memory), "memory to allocate for search trees")
            ("nobook", "do not use opening book")
            ("seed,r", value<uint32_t>(&seed), "set random seed")
            ("verbose", "print logging messages");
        options_description hidden_options;
        hidden_options.add_options()
            ("input-file", value<vector<string>>(&arguments),
             "input file");
        options_description all_options;
        all_options.add(normal_options).add(hidden_options);
        positional_options_description positional_options;
        positional_options.add("input-file", -1);
        variables_map vm;
        store(command_line_parser(argc, argv).options(all_options).
              positional(positional_options).run(), vm);
        boost::program_options::notify(vm);
        if (memory == 0 && vm.count("memory"))
            throw Exception("Value for memory must be greater zero.");
        if (! vm.count("verbose"))
            set_log_null();
        if (vm.count("seed"))
            RandomGenerator::set_global_seed(seed);
#ifdef Q_WS_WIN
        if (vm.count("verbose"))
            redirectStdErr();
#endif
        bool noBook = (vm.count("nobook") != 0);
        QString initialFile;
        if (! arguments.empty())
            initialFile = arguments[0].c_str();
        MainWindow mainWindow(initialFile, manualDir, booksDir, noBook,
                              memory);
        if (vm.count("seed"))
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
