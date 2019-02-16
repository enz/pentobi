//-----------------------------------------------------------------------------
/** @file pentobi/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include <QApplication>
#include <QIcon>
#include <QQuickStyle>
#include <QtQml>
#include <QTranslator>
#include "AnalyzeGameModel.h"
#include "AndroidUtils.h"
#include "GameModel.h"
#include "ImageProvider.h"
#include "PlayerModel.h"
#include "RatingModel.h"
#include "SyncSettings.h"
#include "libboardgame_base/Log.h"

#ifndef Q_OS_ANDROID
#include <QCommandLineParser>
#endif

#ifndef PENTOBI_OPEN_HELP_EXTERNALLY
#include <QtWebView>
#endif

//-----------------------------------------------------------------------------

namespace {

#ifdef Q_OS_ANDROID

int mainAndroid()
{
    QQmlApplicationEngine engine;
    engine.addImageProvider(QStringLiteral("pentobi"), new ImageProvider);
    auto ctx = engine.rootContext();
    ctx->setContextProperty(QStringLiteral("initialFile"), QString());
    ctx->setContextProperty(QStringLiteral("isDesktop"), false);
#ifdef QT_DEBUG
    ctx->setContextProperty(QStringLiteral("isDebug"), true);
#else
    ctx->setContextProperty(QStringLiteral("isDebug"), false);
#endif
    ctx->setContextProperty(QStringLiteral("openHelpExternally"), false);
    engine.load(QStringLiteral("qrc:///qml/Main.qml"));
    if (engine.rootObjects().empty())
        return 1;
    return QGuiApplication::exec();
}

#else // ! defined(Q_OS_ANDROID)

int mainDesktop()
{
    QIcon icon(QStringLiteral(":/pentobi_icon/pentobi-128.svg"));
    QGuiApplication::setWindowIcon(icon);
    QGuiApplication::setDesktopFileName(
                QStringLiteral("io.sourceforge.pentobi"));
    QCommandLineParser parser;
    parser.setApplicationDescription(
                QCoreApplication::translate(
                    "main",
                    "computer opponent for the board game Blokus"));
    auto maxSupportedLevel = Player::max_supported_level;
    QCommandLineOption optionMaxLevel(
                QStringLiteral("maxlevel"),
                //: Description for command line option --maxlevel
                QCoreApplication::translate(
                    "main", "Set maximum level to <n>."),
                QStringLiteral("n"),
                QString::number(PlayerModel::maxLevel));
    parser.addOption(optionMaxLevel);
    QCommandLineOption optionNoBook(
                QStringLiteral("nobook"),
                //: Description for command line option --nobook
                QCoreApplication::translate(
                    "main", "Do not use opening books."));
    QCommandLineOption optionMobile(
                QStringLiteral("mobile"),
                //: Description for command line option --mobile
                QCoreApplication::translate(
                    "main", "Use layout optimized for smartphones."));
    parser.addOption(optionMobile);
    parser.addOption(optionNoBook);
    QCommandLineOption optionNoDelay(
                QStringLiteral("nodelay"),
                //: Description for command line option --nodelay
                QCoreApplication::translate(
                    "main", "Do not delay fast computer moves."));
    parser.addOption(optionNoDelay);
    QCommandLineOption optionSeed(
                QStringLiteral("seed"),
                //: Description for command line option --seed
                QCoreApplication::translate(
                    "main", "Set random seed to <n>."),
                QStringLiteral("n"));
    parser.addOption(optionSeed);
    QCommandLineOption optionThreads(
                QStringLiteral("threads"),
                //: Description for command line option --threads
                QCoreApplication::translate(
                    "main", "Use <n> threads (0=auto)."),
                QStringLiteral("n"));
    parser.addOption(optionThreads);
#ifndef LIBBOARDGAME_DISABLE_LOG
    QCommandLineOption optionVerbose(
                QStringLiteral("verbose"),
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
        if (QQuickStyle::name().isEmpty() && isDesktop)
            QQuickStyle::setStyle(QStringLiteral("Fusion"));
        QQmlApplicationEngine engine;
        engine.addImageProvider(QStringLiteral("pentobi"), new ImageProvider);
        auto ctx = engine.rootContext();
        ctx->setContextProperty(QStringLiteral("initialFile"), initialFile);
        ctx->setContextProperty(QStringLiteral("isDesktop"), isDesktop);
#ifdef QT_DEBUG
        ctx->setContextProperty(QStringLiteral("isDebug"), true);
#else
        ctx->setContextProperty(QStringLiteral("isDebug"), false);
#endif
#ifdef PENTOBI_HELP_DIR
        ctx->setContextProperty(QStringLiteral("helpDir"),
                                QString::fromLocal8Bit(PENTOBI_HELP_DIR));
#else
        ctx->setContextProperty(QStringLiteral("helpDir"), QString());
#endif
#ifdef PENTOBI_OPEN_HELP_EXTERNALLY
        ctx->setContextProperty(QStringLiteral("openHelpExternally"), true);
#else
        ctx->setContextProperty(QStringLiteral("openHelpExternally"), false);
#endif
        engine.load(QStringLiteral("qrc:///qml/Main.qml"));
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
    // We don't use HighDpiScaling on low-DPI Android devices because of
    // QTBUG-69102 and other bugs
    auto density = AndroidUtils::getDensity();
    if (density == 0 || density > 1)
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#else
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QCoreApplication::setOrganizationName(QStringLiteral("Pentobi"));
    QCoreApplication::setApplicationName(QStringLiteral("Pentobi"));
#ifdef VERSION
    QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
#endif
    QGuiApplication app(argc, argv);
#ifndef PENTOBI_OPEN_HELP_EXTERNALLY
    QtWebView::initialize();
#endif
    qmlRegisterType<AnalyzeGameModel>("pentobi", 1, 0, "AnalyzeGameModel");
    qmlRegisterType<AndroidUtils>("pentobi", 1, 0, "AndroidUtils");
    qmlRegisterType<GameModel>("pentobi", 1, 0, "GameModel");
    qmlRegisterType<PlayerModel>("pentobi", 1, 0, "PlayerModel");
    qmlRegisterType<RatingModel>("pentobi", 1, 0, "RatingModel");
    qmlRegisterType<SyncSettings>("pentobi", 1, 0, "SyncSettings");
    qmlRegisterInterface<AnalyzeGameElement>("AnalyzeGameElement");
    qmlRegisterInterface<GameMove>("GameModelMove");
    qmlRegisterInterface<PieceModel>("PieceModel");
#ifndef Q_OS_ANDROID
    // Qt translations needed for QCommandLineParser
    QTranslator qtTranslator;
    qtTranslator.load(
                "qt_" + QLocale::system().name(),
                QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QCoreApplication::installTranslator(&qtTranslator);
#endif
    QTranslator translator;
    translator.load(":qml/i18n/qml_" + QLocale::system().name());
    QCoreApplication::installTranslator(&translator);
#ifdef Q_OS_ANDROID
    return mainAndroid();
#else
    return mainDesktop();
#endif
}

//-----------------------------------------------------------------------------
