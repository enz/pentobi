//-----------------------------------------------------------------------------
/** @file svgtopng/Main.cpp
    Utility program for converting SVG to PNG icons.
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include <iostream>
#include <QImageReader>
#include <QImageWriter>

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
            throw QString("Usage: svgtopng in.svg out.png");
        QString in = QString::fromLocal8Bit(argv[1]);
        QString out = QString::fromLocal8Bit(argv[2]);
        QImageReader reader(in, "SVG");
        QImage image = reader.read();
        if (image.isNull())
            throw QString("'%1': %2").arg(in).arg(reader.errorString());
        QImageWriter writer(out, "PNG");
        if (! writer.write(image))
            throw QString("'%1': %2").arg(out).arg(writer.errorString());
    }
    catch (const QString& msg)
    {
        std::cerr << msg.toLocal8Bit().data() << '\n';
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
