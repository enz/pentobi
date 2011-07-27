//-----------------------------------------------------------------------------
/** @file libpentobi_gui/HelpWindow.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "HelpWindow.h"

//-----------------------------------------------------------------------------

HelpWindow::HelpWindow(QWidget* parent, const QString& mainPage)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Pentobi Help"));
    QTextBrowser* browser = new QTextBrowser(this);
    setCentralWidget(browser);
    browser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    browser->setSource(QUrl::fromLocalFile(mainPage));
    QAction* actionBack = new QAction(tr("Back"), this);
    actionBack->setIcon(QIcon(":/go-previous.png"));
    connect(actionBack, SIGNAL(triggered()), browser, SLOT(backward()));
    QAction* actionForward = new QAction(tr("Forward"), this);
    actionForward->setIcon(QIcon(":/go-next.png"));
    connect(actionForward, SIGNAL(triggered()), browser, SLOT(forward()));
    QAction* actionHome = new QAction(tr("Home"), this);
    actionHome->setIcon(QIcon(":/go-home.png"));
    connect(actionHome, SIGNAL(triggered()), browser, SLOT(home()));
    QToolBar* toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->addAction(actionBack);
    toolBar->addAction(actionForward);
    toolBar->addAction(actionHome);
    addToolBar(toolBar);
    resize(480, 600);
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

//-----------------------------------------------------------------------------
