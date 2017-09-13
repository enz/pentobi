//-----------------------------------------------------------------------------
/** @file libpentobi_gui/HelpWindow.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "HelpWindow.h"

#include <QApplication>
#include <QAction>
#include <QDesktopWidget>
#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QTextBrowser>
#include "libboardgame_util/Log.h"

//-----------------------------------------------------------------------------

HelpWindow::HelpWindow(QWidget* parent, const QString& title,
                       const QString& mainPage)
    : QMainWindow(parent)
{
    LIBBOARDGAME_LOG("Loading ", mainPage.toLocal8Bit().constData());
    setWindowTitle(title);
    if (QIcon::hasThemeIcon("help-browser"))
        setWindowIcon(QIcon::fromTheme("help-browser"));
    m_mainPageUrl = QUrl::fromLocalFile(mainPage);
    auto browser = new QTextBrowser;
    setCentralWidget(browser);
    browser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    browser->setSource(m_mainPageUrl);
    auto actionBack = new QAction(tr("Back"), this);
    actionBack->setShortcut(QKeySequence::Back);
    actionBack->setToolTip(tr("Show previous page in history"));
    actionBack->setEnabled(false);
    actionBack->setIcon(QIcon::fromTheme("go-previous"));
    connect(actionBack, &QAction::triggered, browser, &QTextBrowser::backward);
    connect(browser, &QTextBrowser::backwardAvailable, actionBack, &QAction::setEnabled);
    auto actionForward = new QAction(tr("Forward"), this);
    actionForward->setShortcut(QKeySequence::Forward);
    actionForward->setToolTip(tr("Show next page in history"));
    actionForward->setEnabled(false);
    actionForward->setIcon(QIcon::fromTheme("go-next"));
    connect(actionForward, &QAction::triggered, browser, &QTextBrowser::forward);
    connect(browser, &QTextBrowser::forwardAvailable, actionForward, &QAction::setEnabled);
    auto actionClose = new QAction("", this);
    actionClose->setShortcut(QKeySequence::Close);
    connect(actionClose, &QAction::triggered, this, &HelpWindow::hide);
    addAction(actionClose);
    browser->addAction(actionBack);
    browser->addAction(actionForward);
    browser->setContextMenuPolicy(Qt::ActionsContextMenu);
    QSettings settings;
    restoreGeometry(settings.value("helpwindow_geometry").toByteArray());
}

QString HelpWindow::findMainPage(const QString& helpDir, const QString& appName)
{
    auto locale = QLocale::system().name();
    auto path = QString("%1/%2/%3/index.html").arg(helpDir, locale, appName);
    if (QFile(path).exists())
        return path;
    path = QString("%1/%2/%3/index.html")
            .arg(helpDir, locale.split("_")[0], appName);
    if (QFile(path).exists())
        return path;
    return QString("%1/C/%3/index.html").arg(helpDir, appName);
}

void HelpWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("helpwindow_geometry", saveGeometry());
    QMainWindow::closeEvent(event);
}

QSize HelpWindow::sizeHint() const
{
    auto geo = QApplication::desktop()->availableGeometry(this);
    return QSize(geo.width() * 4 / 10, geo.height() * 9 / 10);
}

//-----------------------------------------------------------------------------
