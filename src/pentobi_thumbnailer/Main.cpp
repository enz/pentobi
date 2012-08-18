//-----------------------------------------------------------------------------
/** @file pentobi_thumbnailer/Main.cpp */
//-----------------------------------------------------------------------------

#include <vector>
#include <boost/program_options.hpp>
#include "libboardgame_util/Exception.h"
#include "libpentobi_thumbnail/CreateThumbnail.h"

using namespace std;
using boost::program_options::command_line_parser;
using boost::program_options::options_description;
using boost::program_options::notify;
using boost::program_options::positional_options_description;
using boost::program_options::store;
using boost::program_options::value;
using boost::program_options::variables_map;
using libboardgame_util::Exception;

//-----------------------------------------------------------------------------

namespace {

void mainFunction(int argc, char* argv[])
{
    int size = 128;
    vector<string> files;
    options_description normal_options("Options");
    normal_options.add_options()
        ("size,s", value<int>(&size), "Image size");
    options_description hidden_options;
    hidden_options.add_options()
        ("files", value<vector<string>>(&files), "input-file output-file");
    options_description all_options;
    all_options.add(normal_options).add(hidden_options);
    positional_options_description positional_options;
    positional_options.add("files", -1);
    variables_map vm;
    store(command_line_parser(argc, argv).options(all_options).
          positional(positional_options).run(), vm);
    notify(vm);
    if (size <= 0)
        throw Exception("Invalid image size");
    if (files.size() > 2)
        throw Exception("Too many file arguments");
    if (files.size() < 2)
        throw Exception("Need input and output file argument");
    QImage image(size, size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    if (! createThumbnail(QString::fromLocal8Bit(files[0].c_str()), size, size,
                          image))
        throw Exception("Thumbnail generation failed");
    QImageWriter writer(QString::fromLocal8Bit(files[1].c_str()), "png");
    if (! writer.write(image))
        throw Exception(writer.errorString().toStdString());
}

} //namespace

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    try
    {
        mainFunction(argc, argv);
    }
    catch (const exception& e)
    {
        cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
