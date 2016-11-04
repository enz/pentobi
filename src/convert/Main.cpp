//-----------------------------------------------------------------------------
/** @file convert/Main.cpp
    Utility program for converting icons between image formats.
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include <iostream>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QImageReader>
#include <QImageWriter>

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    try
    {
        QCommandLineParser parser;
        QCommandLineOption optionHdpi("hdpi");
        parser.addOption(optionHdpi);
        parser.process(app);
        auto args = parser.positionalArguments();
        if (args.size() != 2)
            throw QString("Need two arguments");
        auto in = args.at(0);
        auto out = args.at(1);
        QImageReader reader(in);
        QImage image = reader.read();
        if (image.isNull())
            throw QString("%1: %2").arg(in, reader.errorString());
        if (parser.isSet(optionHdpi))
        {
            QImageReader reader(in);
            reader.setScaledSize(2 * image.size());
            image = reader.read();
            if (image.isNull())
                throw QString("%1: %2").arg(in, reader.errorString());
        }
        QImageWriter writer(out);
        if (! writer.write(image))
            throw QString("%1: %2").arg(out, writer.errorString());
    }
    catch (const QString& msg)
    {
        std::cerr << msg.toLocal8Bit().constData() << '\n';
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
