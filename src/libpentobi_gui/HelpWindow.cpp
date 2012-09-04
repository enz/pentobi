//-----------------------------------------------------------------------------
/** @file libpentobi_gui/HelpWindow.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "HelpWindow.h"

#include "libboardgame_util/Log.h"

using libboardgame_util::log;

//-----------------------------------------------------------------------------

namespace {

void setIcon(QAction* action, const QString& name)
{
    action->setIcon(QIcon(QString(":/libpentobi_gui/icons/%1.png").arg(name)));
}

} // namespace

//-----------------------------------------------------------------------------

HelpWindow::HelpWindow(QWidget* parent, const QString& mainPage)
    : QMainWindow(parent)
{
    log() << "Loading " << mainPage.toStdString() << '\n';
    setWindowTitle(tr("Pentobi User Manual"));
    m_mainPageUrl = QUrl::fromLocalFile(mainPage);
    QTextBrowser* browser = new QTextBrowser(this);
    setCentralWidget(browser);
    browser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    browser->setSource(m_mainPageUrl);
    QAction* actionBack = new QAction(tr("Back"), this);
    actionBack->setEnabled(false);
    setIcon(actionBack, "go-previous");
    connect(actionBack, SIGNAL(triggered()), browser, SLOT(backward()));
    connect(browser, SIGNAL(backwardAvailable(bool)),
            actionBack, SLOT(setEnabled(bool)));
    QAction* actionForward = new QAction(tr("Forward"), this);
    actionForward->setEnabled(false);
    setIcon(actionForward, "go-next");
    connect(actionForward, SIGNAL(triggered()), browser, SLOT(forward()));
    connect(browser, SIGNAL(forwardAvailable(bool)),
            actionForward, SLOT(setEnabled(bool)));
    m_actionHome = new QAction(tr("Home"), this);
    m_actionHome->setEnabled(false);
    setIcon(m_actionHome, "go-home");
    connect(m_actionHome, SIGNAL(triggered()), browser, SLOT(home()));
    connect(browser, SIGNAL(sourceChanged(const QUrl&)),
            this, SLOT(handleSourceChanged(const QUrl&)));
    QAction* actionClose = new QAction("", this);
    actionClose->setShortcut(QKeySequence::Close);
    connect(actionClose, SIGNAL(triggered()), this, SLOT(hide()));
    addAction(actionClose);
    QToolBar* toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->addAction(actionBack);
    toolBar->addAction(actionForward);
    toolBar->addAction(m_actionHome);
    addToolBar(toolBar);
    QSettings settings;
    if (! restoreGeometry(settings.value("helpwindow_geometry").toByteArray()))
        adjustSize();
}

QString HelpWindow::findMainPage(QString dir, QString file, QString locale)
{
    QString path;
    path = dir + "/" + locale + "/" + file;
    if (QFile(path).exists())
        return path;
    QStringList list = locale.split("_");
    path = dir + "/" + list[0] + "/" + file;
    if (QFile(path).exists())
        return path;
    return dir + "/en/" + file;
}

void HelpWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("helpwindow_geometry", saveGeometry());
    QMainWindow::closeEvent(event);
}

void HelpWindow::handleSourceChanged(const QUrl& src)
{
    m_actionHome->setEnabled(src != m_mainPageUrl);
}

QSize HelpWindow::sizeHint() const
{
    return QSize(600, 800);
}

//-----------------------------------------------------------------------------
