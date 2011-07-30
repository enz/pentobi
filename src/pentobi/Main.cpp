//-----------------------------------------------------------------------------
/** @file pentobi/Main.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <boost/program_options.hpp>
#include <QtGui>
#include "MainWindow.h"

using boost::program_options::command_line_parser;
using boost::program_options::options_description;
using boost::program_options::positional_options_description;
using boost::program_options::store;
using boost::program_options::value;
using boost::program_options::variables_map;
using libboardgame_util::set_log_null;

//-----------------------------------------------------------------------------

namespace {

void loadLibPentobiGuiTranslator(QTranslator& translator)
{
    QString locale = QLocale::system().name();
    QString file = "libpentobi_gui_" + locale;
    QString appDir = QCoreApplication::applicationDirPath();
    // Search for translation file in the following directories in this order
    // 1. AppDir (Windows installation)
    // 2. AppDir/../libpentobi_gui (CMake build on Unix or with MinGW)
    // 3. AppDir/../../libpentobi_gui (CMake build with MSVC)
    // 4. DATADIR/pentobi/translations (Unix installation)
    if (translator.load(file, appDir))
        return;
#ifndef _MSC_VER
    if (translator.load(file, appDir + "/../libpentobi_gui"))
        return;
#endif
#ifdef _MSC_VER
    if (translator.load(file, appDir + "/../../libpentobi_gui"))
        return;
#endif
#ifdef DATADIR
    translator.load(file, QString(DATADIR) + "/pentobi/translations");
#endif
}

void loadPentobiTranslator(QTranslator& translator)
{
    QString locale = QLocale::system().name();
    QString file = "pentobi_" + locale;
    QString appDir = QCoreApplication::applicationDirPath();
    // Search for translation file in the following directories in this order
    // 1. AppDir (Windows installation or CMake build on Unix or with MinGW)
    // 2. AppDir/.. (CMake build with MSVC)
    // 3. DATADIR/pentobi/translations (Unix installation)
#ifndef _MSC_VER
    if (translator.load(file, appDir))
        return;
#endif
#ifdef _MSC_VER
    if (translator.load(file, appDir + "/.."))
        return;
#endif
#ifdef DATADIR
    translator.load(file, QString(DATADIR) + "/pentobi/translations");
#endif
}

 } // namespace

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    try
    {
        Q_INIT_RESOURCE(libpentobi_gui_resources);
        QCoreApplication::setOrganizationName("Pentobi");
        QCoreApplication::setApplicationName("Pentobi");
        QApplication app(argc, argv);
        QTranslator qtTranslator;
        QString qtTranslationPath =
            QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        QString locale = QLocale::system().name();
        qtTranslator.load("qt_" + locale, qtTranslationPath);
        app.installTranslator(&qtTranslator);
        QTranslator libPentobiGuiTranslator;
        loadLibPentobiGuiTranslator(libPentobiGuiTranslator);
        app.installTranslator(&libPentobiGuiTranslator);
        QTranslator pentobiTranslator;
        loadPentobiTranslator(pentobiTranslator);
        app.installTranslator(&pentobiTranslator);
        vector<string> arguments;
        options_description normal_options("Options");
        normal_options.add_options()
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
        if (! vm.count("verbose"))
            set_log_null();
        QString initialFile;
        if (arguments.size() > 0)
            initialFile = arguments[0].c_str();
        MainWindow mainWindow(initialFile);
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
