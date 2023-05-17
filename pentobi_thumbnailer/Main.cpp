//-----------------------------------------------------------------------------
/** @file pentobi_thumbnailer/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include <iostream>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QImage>
#include <QImageWriter>
#include <QLocale>
#include <QLibraryInfo>
#include <QString>
#include <QTranslator>
#include "libboardgame_base/Log.h"
#include "CreateThumbnail.h"

using namespace Qt::StringLiterals;
using namespace std;

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    libboardgame_base::LogInitializer log_initializer;
    QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
    QCoreApplication app(argc, argv);

    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + QLocale::system().name(),
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        QCoreApplication::installTranslator(&qtTranslator);

    QTranslator translator;
    if (translator.load(":i18n/" + QLocale::system().name()))
        QCoreApplication::installTranslator(&translator);

    try
    {
        QCommandLineParser parser;
        parser.setApplicationDescription(
                    QCoreApplication::translate(
                        "main",
                        "thumbnailer for Blokus game records as used by Pentobi"));
        QCommandLineOption optionSize(
                    QStringList() << "s"_L1 << "size"_L1,
                    //: Description for command line option --size
                    QCoreApplication::translate(
                        "main",
                        "Generate image with height and width <size>."),
                    //: Value name for command line option --size
                    QCoreApplication::translate("main", "size"),
                    "128"_L1);
        parser.addOption(optionSize);
        parser.addHelpOption();
        parser.addVersionOption();
        parser.addPositionalArgument(
                    //: Name of input file command line argument.
                    QCoreApplication::translate("main", "input.blksgf"),
                    QCoreApplication::translate(
                        "main",
                        //: Description of input file command line argument.
                        "Blokus SGF input file."));
        parser.addPositionalArgument(
                    //: Name of output image file command line argument.
                    QCoreApplication::translate("main", "output.png"),
                    QCoreApplication::translate(
                        "main",
                        //: Description of output file command line argument.
                        "PNG image output file."));
        parser.process(app);
        auto args = parser.positionalArguments();
        bool ok;
        int size = parser.value(optionSize).toInt(&ok);
        if (! ok || size <= 0)
            throw QCoreApplication::translate("main", "Invalid image size");
        if (args.size() > 2)
            throw QCoreApplication::translate("main", "Too many arguments");
        if (args.size() < 2)
            throw QCoreApplication::translate(
                    "main", "Need input and output file argument");
        QImage image(size, size, QImage::Format_ARGB32);
        if (! createThumbnail(args.at(0), size, size, image))
            throw QCoreApplication::translate(
                    "main", "Thumbnail creation failed");
        QImageWriter writer(args.at(1), "png");
        if (! writer.write(image))
            throw writer.errorString();
    }
    catch (const QString& s)
    {
        cerr << s.toLocal8Bit().constData() << '\n';
        return 1;
    }
    catch (const exception& e)
    {
        cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
