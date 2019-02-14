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
#include <QString>
#include "libboardgame_util/Log.h"
#include "libpentobi_thumbnail/CreateThumbnail.h"

using namespace std;

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    libboardgame_util::LogInitializer log_initializer;
    QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
    QCoreApplication app(argc, argv);
    try
    {
        QCommandLineParser parser;
        QCommandLineOption optionSize(
                    QStringList() << QStringLiteral("s")
                    << QStringLiteral("size"),
                    QStringLiteral(
                        "Generate image with height and width <size>."),
                    QStringLiteral("size"), QStringLiteral("128"));
        parser.addOption(optionSize);
        parser.addHelpOption();
        parser.addVersionOption();
        parser.addPositionalArgument(QStringLiteral("input.blksgf"),
                                     QStringLiteral("Blokus SGF input file"));
        parser.addPositionalArgument(QStringLiteral("output.png"),
                                     QStringLiteral("PNG image output file"));
        parser.process(app);
        auto args = parser.positionalArguments();
        bool ok;
        int size = parser.value(optionSize).toInt(&ok);
        if (! ok || size <= 0)
            throw runtime_error("Invalid image size");
        if (args.size() > 2)
            throw runtime_error("Too many arguments");
        if (args.size() < 2)
            throw runtime_error("Need input and output file argument");
        QImage image(size, size, QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        if (! createThumbnail(args.at(0), size, size, image))
            throw runtime_error("Not a valid Blokus SGF file");
        QImageWriter writer(args.at(1), "png");
        if (! writer.write(image))
            throw runtime_error(writer.errorString().toLocal8Bit().constData());
    }
    catch (const exception& e)
    {
        cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
