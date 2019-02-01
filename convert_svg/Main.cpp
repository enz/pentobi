//-----------------------------------------------------------------------------
/** @file convert/Main.cpp
    Converts images using the Qt library.
    Used for creating PNG icons from the SVG sources at build time.
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include <iostream>
#include <QCoreApplication>
#include <QImageReader>
#include <QImageWriter>

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    try
    {
        if (argc != 3)
            throw QStringLiteral("Need two arguments");
        auto in = QString::fromLocal8Bit(argv[1]);
        auto out = QString::fromLocal8Bit(argv[2]);
        QImageReader reader(in);
        QImage image = reader.read();
        if (image.isNull())
            throw QStringLiteral("%1: %2").arg(in, reader.errorString());
        QImageWriter writer(out);
        if (! writer.write(image))
            throw QStringLiteral("%1: %2").arg(out, writer.errorString());
    }
    catch (const QString& msg)
    {
        std::cerr << msg.toLocal8Bit().constData() << '\n';
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
