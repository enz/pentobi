//-----------------------------------------------------------------------------
/** @file pentobi/AboutDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "AboutDialog.h"

#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "ShowMessage.h"
#include "libboardgame_util/Assert.h"

//-----------------------------------------------------------------------------

AboutDialog::AboutDialog(QWidget* parent, const QString& version)
    : QDialog(parent)
{
    setWindowTitle(tr("About Pentobi"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    auto outerLayout = new QVBoxLayout();
    setLayout(outerLayout);
    auto innerLayout = new QHBoxLayout();
    outerLayout->addLayout(innerLayout);
    if (parent != nullptr)
    {
        QIcon icon = parent->windowIcon();
        if (! icon.isNull())
        {
            auto iconLabel = new QLabel();
            iconLabel->setPixmap(icon.pixmap(48));
            iconLabel->setAlignment(Qt::AlignTop);
            innerLayout->addWidget(iconLabel);
            innerLayout->addSpacing(8);
        }
    }
    auto textLabel =
        new QLabel("<style type=\"text/css\">"
                   ":link { text-decoration: none; }"
                   "</style>"
                   "<h2>" + tr("Pentobi") + "</h2>"
                   "<p>" + tr("Version %1").arg(version) + "</p>"
                   "<p>" +
                   tr("Pentobi is a computer program that plays the board game Blokus.")
                   + "</p><p>" +
                   tr("Copyright &copy; 2011&ndash;2013 Markus Enzenberger") +
                   + "<br>" +
                   "<a href=\"http://pentobi.sf.net\">http://pentobi.sf.net</a>"
                   "</p>");
    innerLayout->addWidget(textLabel);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    outerLayout->addWidget(buttonBox);
    m_donateButton =
        buttonBox->addButton(tr("Donate"), QDialogButtonBox::ActionRole);
    m_flattrButton =
        buttonBox->addButton(tr("Flattr this"), QDialogButtonBox::ActionRole);

    // QDesktopServices::openUrl() always return false on Windows 7
    // for unknown reasons. Disable the button that need openUrl()
    // for now
    m_donateButton->hide();
    m_flattrButton->hide();

    connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)),
            SLOT(buttonClicked(QAbstractButton*)));
}

void AboutDialog::buttonClicked(QAbstractButton* button)
{
    if (button == static_cast<QAbstractButton*>(m_donateButton))
    {
        openUrl("https://www.paypal.com/cgi-bin/webscr?item_name=Donation+to+Pentobi&cmd=_donations&business=enz%40users.sourceforge.net");
        accept();
    }
    else if (button == static_cast<QAbstractButton*>(m_flattrButton))
    {
        openUrl("http://flattr.com/thing/1133617/Pentobi");
        accept();
    }
}

void AboutDialog::openUrl(const QString& s)
{
    QUrl url = QUrl::fromEncoded(s.toUtf8());
    LIBBOARDGAME_ASSERT(url.isValid());
    if (! QDesktopServices::openUrl(url))
        showInfo(this, tr("Could not open URL %1").arg(s));
}

//-----------------------------------------------------------------------------
