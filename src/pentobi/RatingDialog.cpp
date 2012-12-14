//-----------------------------------------------------------------------------
/** @file pentobi/RatingDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatingDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include "Util.h"

using namespace std;

//-----------------------------------------------------------------------------

RatingDialog::RatingDialog(QWidget* parent, RatingHistory& history)
    : QDialog(parent),
      m_history(history)
{
    setWindowTitle(tr("Your Rating"));
    // Disable '?' button in title bar on Windows, we don't have
    // context help
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    QFormLayout* formLayout = new QFormLayout();
    layout->addLayout(formLayout);
    formLayout->setLabelAlignment(Qt::AlignLeft);
    m_labelRating = new QLabel();
    formLayout->addRow(tr("Your rating:"), m_labelRating);
    m_labelVariant = new QLabel();
    formLayout->addRow(tr("Game variant:"), m_labelVariant);
    m_labelNuGames = new QLabel();
    formLayout->addRow(tr("Number rated games:"), m_labelNuGames);
    m_labelBestRating = new QLabel();
    formLayout->addRow(tr("Best previous rating:"), m_labelBestRating);
    layout->addSpacing(layout->margin());
    layout->addWidget(new QLabel(tr("Recent development:")));
    m_graph = new RatingGraph();
    layout->addWidget(m_graph, 1);
    layout->addSpacing(layout->margin());
    layout->addWidget(new QLabel(tr("Recent games:")));
    m_list = new RatedGamesList();
    layout->addWidget(m_list, 1);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);
    m_clearButton =
        buttonBox->addButton(tr("&Clear"), QDialogButtonBox::ActionRole);
    buttonBox->button(QDialogButtonBox::Close)->setDefault(true);
    buttonBox->button(QDialogButtonBox::Close)->setAutoDefault(true);
    buttonBox->button(QDialogButtonBox::Close)->setFocus();
    updateContent();
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(buttonClicked(QAbstractButton*)));
    connect(m_list, SIGNAL(openRatedGame(unsigned)),
            this, SLOT(activateGame(unsigned)));
}

void RatingDialog::activateGame(unsigned n)
{
    emit open(m_history.getFile(n).string().c_str());
}

void RatingDialog::buttonClicked(QAbstractButton* button)
{
    if (button != static_cast<QAbstractButton*>(m_clearButton))
        return;
    QMessageBox msgBox(QMessageBox::Warning, "",
                       tr("Clear rating and delete rating history?"),
                       QMessageBox::Cancel, this);
    Util::setNoTitle(msgBox);
    QPushButton* clearButton =
        msgBox.addButton(tr("Clear rating"), QMessageBox::DestructiveRole);
    msgBox.setDefaultButton(clearButton);
    msgBox.exec();
    if (msgBox.clickedButton() != clearButton)
        return;
    m_history.clear();
    updateContent();
}

void RatingDialog::updateContent()
{
    Variant variant = m_history.getVariant();
    unsigned nuGames = m_history.getNuGames();
    Rating rating = m_history.getRating();
    Rating bestRating = m_history.getBestRating();
    if (nuGames == 0)
        rating = Rating(0);
    QString variantStr;
    switch (variant)
    {
    case Variant::classic:
        variantStr = tr("Classic (4 players)");
        break;
    case Variant::classic_2:
        variantStr = tr("Classic (2 players)");
        break;
    case Variant::duo:
        variantStr = tr("Duo");
        break;
    case Variant::trigon:
        variantStr = tr("Trigon (4 players)");
        break;
    case Variant::trigon_2:
        variantStr = tr("Trigon (2 players)");
        break;
    case Variant::trigon_3:
        variantStr = tr("Trigon (3 players)");
        break;
    case Variant::junior:
        variantStr = tr("Junior");
        break;
    }
    m_labelVariant->setText(variantStr);
        m_labelNuGames->setText(QString("%1").arg(nuGames));
    if (nuGames == 0)
    {
        m_labelRating->setText("");
        m_labelBestRating->setText("");
    }
    else
    {
        m_labelRating->setText(QString("<b>%1").arg(rating.toInt()));
        m_labelBestRating->setText(QString("%1").arg(bestRating.toInt()));
    }
    m_graph->updateContent(m_history);
    m_list->updateContent(variant, m_history);
    m_clearButton->setEnabled(nuGames > 0);
}

//-----------------------------------------------------------------------------
