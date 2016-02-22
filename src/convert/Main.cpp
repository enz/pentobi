//-----------------------------------------------------------------------------
/** @file convert/Main.cpp
    Utility program for converting icons between image formats.
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
    QCoreApplication app(argc, argv); // Initializes the locale subsystem
    try
    {
        QString in, out;
        bool createHdpi = false;
        int nuArg = 0;
        for (int i = 1; i < argc; ++i)
        {
            QString arg = QString::fromLocal8Bit(argv[i]);
            if (arg == "--hdpi")
            {
                createHdpi = true;
                continue;
            }
            else if (arg.startsWith('-'))
                throw QString("Unknown option %1").arg(arg);
            if (nuArg == 0)
                in = arg;
            else if (nuArg == 1)
                out = arg;
            ++nuArg;
        }
        if (nuArg != 2)
            throw QString("Need two arguments");

        QImageReader reader(in);
        QImage image = reader.read();
        if (image.isNull())
            throw QString("%1: %2").arg(in, reader.errorString());
        QSize size = image.size();
        if (createHdpi)
        {
            QImageReader reader(in);
            reader.setScaledSize(2 * size);
            QImage image = reader.read();
            if (image.isNull())
                throw QString("%1: %2").arg(in, reader.errorString());
            QImageWriter writer(out);
            if (! writer.write(image))
                throw QString("%1: %2").arg(out, writer.errorString());
        }
        else
        {
            QImageWriter writer(out);
            if (! writer.write(image))
                throw QString("%1: %2").arg(out, writer.errorString());
        }
    }
    catch (const QString& msg)
    {
        std::cerr << msg.toLocal8Bit().constData() << '\n';
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
