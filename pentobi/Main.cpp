//-----------------------------------------------------------------------------
/** @file pentobi/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QtGlobal>
#include <QTranslator>
#include "ImageProvider.h"
#include "libboardgame_base/Log.h"

#ifdef Q_OS_ANDROID
#include "AndroidUtils.h"
#else
#include <QCommandLineParser>
#include <QLibraryInfo>
#include "PlayerModel.h"
#endif

using namespace Qt::StringLiterals;

//-----------------------------------------------------------------------------

namespace {

#ifdef Q_OS_ANDROID

int mainAndroid(QGuiApplication& app)
{
    QQmlApplicationEngine engine;
    engine.addImageProvider("pentobi"_L1, new ImageProvider);
    auto ctx = engine.rootContext();
    ctx->setContextProperty("globalStyle"_L1, QQuickStyle::name());
    ctx->setContextProperty("initialFile"_L1, AndroidUtils::getInitialFile());
    ctx->setContextProperty("isDesktop"_L1, QVariant(false));
#ifdef QT_DEBUG
    ctx->setContextProperty("isDebug"_L1, QVariant(true));
#else
    ctx->setContextProperty("isDebug"_L1, QVariant(false));
#endif
    QObject::connect(&engine,
                     &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(1); },
                     Qt::QueuedConnection);
    engine.loadFromModule("PentobiGui"_L1, "Main"_L1);
    return app.exec();
}

#else // ! defined(Q_OS_ANDROID)

bool isSmallScreen()
{
    auto size = qApp->screens().at(0)->physicalSize();
    auto width = size.width();
    auto height = size.height();
    auto inches = sqrt(width * width + height * height) / 25.4;
    LIBBOARDGAME_LOG("Main screen ", width, " mm x ", height, " mm, diag ",
                     inches, " in");
    return inches < 10;
}

int mainDesktop(QGuiApplication& app)
{
    QIcon::setThemeName("pentobi"_L1);
    QIcon icon(":/icon/pentobi-48.png"_L1);
    app.setWindowIcon(icon);
    app.setDesktopFileName("io.sourceforge.pentobi"_L1);
    QCommandLineParser parser;
    parser.setApplicationDescription(
                app.translate(
                    "main",
                    "computer opponent for the board game Blokus"));
    QCommandLineOption optionDesktop(
        "desktop"_L1,
        //: Description for command line option --desktop
        app.translate(
            "main", "Use layout optimized for desktop."));
    parser.addOption(optionDesktop);
    auto maxSupportedLevel = Player::max_supported_level;
    QCommandLineOption optionMaxLevel(
                "maxlevel"_L1,
                //: Description for command line option --maxlevel
                app.translate(
                    "main", "Set maximum level to <n>."),
                "n"_L1,
                QString::number(PlayerModel::maxLevel));
    parser.addOption(optionMaxLevel);
    QCommandLineOption optionNoBook(
                "nobook"_L1,
                //: Description for command line option --nobook
                app.translate(
                    "main", "Do not use opening books."));
    QCommandLineOption optionMobile(
                "mobile"_L1,
                //: Description for command line option --mobile
                app.translate(
                    "main", "Use layout optimized for smartphones."));
    parser.addOption(optionMobile);
    parser.addOption(optionNoBook);
    QCommandLineOption optionNoDelay(
                "nodelay"_L1,
                //: Description for command line option --nodelay
                app.translate(
                    "main", "Do not delay fast computer moves."));
    parser.addOption(optionNoDelay);
    QCommandLineOption optionSeed(
                "seed"_L1,
                //: Description for command line option --seed
                app.translate(
                    "main", "Set random seed to <n>."),
                "n"_L1);
    parser.addOption(optionSeed);
    QCommandLineOption optionThreads(
                "threads"_L1,
                //: Description for command line option --threads
                app.translate(
                    "main", "Use <n> threads (0=auto)."),
                "n"_L1);
    parser.addOption(optionThreads);
#ifndef LIBBOARDGAME_DISABLE_LOG
    QCommandLineOption optionVerbose(
                "verbose"_L1,
                //: Description for command line option --verbose
                app.translate(
                    "main",
                    "Print logging information to standard error."));
    parser.addOption(optionVerbose);
#endif
    parser.addPositionalArgument(
                //: Name of command line argument.
                app.translate("main", "file.blksgf"),
                app.translate(
                    "main",
                    //: Description of command line argument.
                    "Blokus SGF file to open (optional)."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(*app.instance());
    try
    {
#ifndef LIBBOARDGAME_DISABLE_LOG
        if (! parser.isSet(optionVerbose))
            libboardgame_base::disable_logging();
#endif
        if (parser.isSet(optionNoBook))
            PlayerModel::noBook = true;
        if (parser.isSet(optionNoDelay))
            PlayerModel::noDelay = true;
        bool ok;
        auto maxLevel = parser.value(optionMaxLevel).toUInt(&ok);
        if (! ok || maxLevel < 1 || maxLevel > maxSupportedLevel)
            throw app.translate(
                    "main", "--maxlevel must be between 1 and %1")
                .arg(maxSupportedLevel);
        PlayerModel::maxLevel = maxLevel;
        if (parser.isSet(optionSeed))
        {
            auto seed = parser.value(optionSeed).toUInt(&ok);
            if (! ok)
                throw app.translate(
                        "main", "--seed must be a positive number");
            libboardgame_base::RandomGenerator::set_global_seed(seed);
        }
        if (parser.isSet(optionThreads))
        {
            auto nuThreads = parser.value(optionThreads).toUInt(&ok);
            if (! ok)
                throw app.translate(
                        "main", "--threads must be a positive number");
            PlayerModel::nuThreads = nuThreads;
        }
        bool isDesktop;
        if (parser.isSet(optionMobile) && parser.isSet(optionDesktop))
            throw app.translate("main",
                                "--mobile and --desktop cannot be used both");
        else if (parser.isSet(optionMobile))
            isDesktop = false;
        else if (parser.isSet(optionDesktop))
            isDesktop = true;
        else
            isDesktop = ! isSmallScreen();
        QString initialFile;
        auto args = parser.positionalArguments();
        if (args.size() > 1)
            throw app.translate("main", "Too many arguments");
        if (! args.empty())
            initialFile = args.at(0);
        LIBBOARDGAME_LOG("Using Qt ", qVersion());
        QQmlApplicationEngine engine;
        engine.addImageProvider("pentobi"_L1, new ImageProvider);
        auto ctx = engine.rootContext();
        ctx->setContextProperty("globalStyle"_L1, QQuickStyle::name());
        ctx->setContextProperty("initialFile"_L1, initialFile);
        ctx->setContextProperty("isDesktop"_L1, QVariant(isDesktop));
#ifdef QT_DEBUG
        ctx->setContextProperty("isDebug"_L1, QVariant(true));
#else
        ctx->setContextProperty("isDebug"_L1, QVariant(false));
#endif
        QObject::connect(&engine,
                         &QQmlApplicationEngine::objectCreationFailed,
                         &app, []() { QCoreApplication::exit(1); },
                         Qt::QueuedConnection);
        engine.loadFromModule("PentobiGui"_L1, "Main"_L1);
        return app.exec();
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
}

#endif // Q_OS_ANDROID

} // namespace

//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    libboardgame_base::LogInitializer log_initializer;
#ifdef Q_OS_ANDROID
    // Rounding on Android uses PassThrough by default which causes rendering
    // errors on some devices when switching fullscreen or orientation and
    // incorrect canvas painting on low-DPI devices with devicePixelRatio<1
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
                Qt::HighDpiScaleFactorRoundingPolicy::Round);
#endif
#ifdef Q_OS_WIN
    qputenv("QT_QUICK_CONTROLS_STYLE", "Universal");
#endif
    QCoreApplication::setOrganizationName("Pentobi"_L1);
    QCoreApplication::setApplicationName("Pentobi"_L1);
#ifdef VERSION
    QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
#endif
    QGuiApplication app(argc, argv);
#ifndef Q_OS_ANDROID
    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + QLocale::system().name(),
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        QCoreApplication::installTranslator(&qtTranslator);
#endif
    QTranslator translator;
    if (translator.load(":qml/i18n/qml_" + QLocale::system().name()))
        QCoreApplication::installTranslator(&translator);
#ifdef Q_OS_ANDROID
    return mainAndroid(app);
#else
    return mainDesktop(app);
#endif
}

//-----------------------------------------------------------------------------
