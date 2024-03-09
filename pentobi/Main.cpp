//-----------------------------------------------------------------------------
/** @file pentobi/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include <QGuiApplication>
#include <QIcon>
#include <QQuickStyle>
#include <QtGlobal>
#include <QtQml>
#include <QTranslator>
#include "AnalyzeGameModel.h"
#include "AndroidUtils.h"
#include "DocbookReader.h"
#include "GameModel.h"
#include "ImageProvider.h"
#include "PlayerModel.h"
#include "RatingModel.h"
#include "RecentFiles.h"
#include "SyncSettings.h"
#include "libboardgame_base/Log.h"

#ifndef Q_OS_ANDROID
#include <QCommandLineParser>
#include <QLibraryInfo>
#endif

using namespace Qt::StringLiterals;

//-----------------------------------------------------------------------------

namespace {

#ifdef Q_OS_ANDROID

int mainAndroid()
{
    QQuickStyle::setStyle("Material"_L1);
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
    engine.load("qrc:///qml/Main.qml"_L1);
    if (engine.rootObjects().empty())
        return 1;
    return QGuiApplication::exec();
}

#else // ! defined(Q_OS_ANDROID)

int mainDesktop()
{
    QIcon::setThemeName("pentobi"_L1);
    QIcon icon(":/icon/pentobi-48.png"_L1);
    QGuiApplication::setWindowIcon(icon);
    QGuiApplication::setDesktopFileName("io.sourceforge.pentobi"_L1);
    QCommandLineParser parser;
    parser.setApplicationDescription(
                QCoreApplication::translate(
                    "main",
                    "computer opponent for the board game Blokus"));
    auto maxSupportedLevel = Player::max_supported_level;
    QCommandLineOption optionMaxLevel(
                "maxlevel"_L1,
                //: Description for command line option --maxlevel
                QCoreApplication::translate(
                    "main", "Set maximum level to <n>."),
                "n"_L1,
                QString::number(PlayerModel::maxLevel));
    parser.addOption(optionMaxLevel);
    QCommandLineOption optionNoBook(
                "nobook"_L1,
                //: Description for command line option --nobook
                QCoreApplication::translate(
                    "main", "Do not use opening books."));
    QCommandLineOption optionMobile(
                "mobile"_L1,
                //: Description for command line option --mobile
                QCoreApplication::translate(
                    "main", "Use layout optimized for smartphones."));
    parser.addOption(optionMobile);
    parser.addOption(optionNoBook);
    QCommandLineOption optionNoDelay(
                "nodelay"_L1,
                //: Description for command line option --nodelay
                QCoreApplication::translate(
                    "main", "Do not delay fast computer moves."));
    parser.addOption(optionNoDelay);
    QCommandLineOption optionSeed(
                "seed"_L1,
                //: Description for command line option --seed
                QCoreApplication::translate(
                    "main", "Set random seed to <n>."),
                "n"_L1);
    parser.addOption(optionSeed);
    QCommandLineOption optionThreads(
                "threads"_L1,
                //: Description for command line option --threads
                QCoreApplication::translate(
                    "main", "Use <n> threads (0=auto)."),
                "n"_L1);
    parser.addOption(optionThreads);
#ifndef LIBBOARDGAME_DISABLE_LOG
    QCommandLineOption optionVerbose(
                "verbose"_L1,
                //: Description for command line option --verbose
                QCoreApplication::translate(
                    "main",
                    "Print logging information to standard error."));
    parser.addOption(optionVerbose);
#endif
    parser.addPositionalArgument(
                //: Name of command line argument.
                QCoreApplication::translate("main", "file.blksgf"),
                QCoreApplication::translate(
                    "main",
                    //: Description of command line argument.
                    "Blokus SGF file to open (optional)."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(*QCoreApplication::instance());
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
            throw QCoreApplication::translate(
                    "main", "--maxlevel must be between 1 and %1")
                .arg(maxSupportedLevel);
        PlayerModel::maxLevel = maxLevel;
        if (parser.isSet(optionSeed))
        {
            auto seed = parser.value(optionSeed).toUInt(&ok);
            if (! ok)
                throw QCoreApplication::translate(
                        "main", "--seed must be a positive number");
            libboardgame_base::RandomGenerator::set_global_seed(seed);
        }
        if (parser.isSet(optionThreads))
        {
            auto nuThreads = parser.value(optionThreads).toUInt(&ok);
            if (! ok)
                throw QCoreApplication::translate(
                        "main", "--threads must be a positive number");
            PlayerModel::nuThreads = nuThreads;
        }
        bool isDesktop = ! parser.isSet(optionMobile);
        QString initialFile;
        auto args = parser.positionalArguments();
        if (args.size() > 1)
            throw QCoreApplication::translate("main", "Too many arguments");
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
        engine.load("qrc:///qml/Main.qml"_L1);
        if (engine.rootObjects().empty())
            return 1;
        return QGuiApplication::exec();
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
#elif defined(Q_OS_WIN)
    qputenv("QT_QUICK_CONTROLS_STYLE", "Universal");
#else
    qputenv("QT_QUICK_CONTROLS_STYLE", "Fusion");
#endif
#ifdef Q_OS_LINUX
    // QML cache does not reliably get updated if installed via Flatpak
    // (last tested with flatpak 1.14.4, org.kde.Platform 6.5/6.6)
    qputenv("QML_DISABLE_DISK_CACHE", "1");
#endif
    QCoreApplication::setOrganizationName("Pentobi"_L1);
    QCoreApplication::setApplicationName("Pentobi"_L1);
#ifdef VERSION
    QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
#endif
    QGuiApplication app(argc, argv);
    qmlRegisterType<AnalyzeGameModel>("pentobi", 1, 0, "AnalyzeGameModel");
    qmlRegisterType<AndroidUtils>("pentobi", 1, 0, "AndroidUtils");
    qmlRegisterType<DocbookReader>("pentobi", 1, 0, "DocbookReader");
    qmlRegisterType<GameModel>("pentobi", 1, 0, "GameModel");
    qmlRegisterType<PlayerModel>("pentobi", 1, 0, "PlayerModel");
    qmlRegisterType<RatingModel>("pentobi", 1, 0, "RatingModel");
    qmlRegisterType<RecentFiles>("pentobi", 1, 0, "RecentFiles");
    qmlRegisterType<SyncSettings>("pentobi", 1, 0, "SyncSettings");
    qmlRegisterUncreatableType<AnalyzeGameElement>(
                "pentobi", 1, 0, "AnalyzeGameElement", {});
    qmlRegisterUncreatableType<GameMove>("pentobi", 1, 0, "GameMove", {});
    qmlRegisterUncreatableType<PieceModel>("pentobi", 1, 0, "PieceModel", {});
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
    return mainAndroid();
#else
    return mainDesktop();
#endif
}

//-----------------------------------------------------------------------------
