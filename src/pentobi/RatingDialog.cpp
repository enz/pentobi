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
using libpentobi_base::variant_classic;
using libpentobi_base::variant_classic_2;
using libpentobi_base::variant_duo;
using libpentobi_base::variant_junior;
using libpentobi_base::variant_trigon;
using libpentobi_base::variant_trigon_2;
using libpentobi_base::variant_trigon_3;

//-----------------------------------------------------------------------------

RatingDialog::RatingDialog(QWidget* parent, RatingHistory& history)
    : QDialog(parent),
      m_history(history)
{
    setWindowTitle(tr("Rating"));
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
    QFrame* separator = new QFrame();
    separator->setFrameStyle(QFrame::HLine | QFrame::Plain);
    layout->addWidget(separator);
    layout->addWidget(new QLabel(tr("Recent development:")));
    m_graph = new RatingGraph();
    layout->addWidget(m_graph, 1);
    m_list = new RatedGamesList();
    layout->addWidget(m_list, 1);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);
    m_clearButton =
        buttonBox->addButton(tr("Clear"), QDialogButtonBox::ActionRole);
    buttonBox->button(QDialogButtonBox::Close)->setDefault(true);
    buttonBox->button(QDialogButtonBox::Close)->setAutoDefault(true);
    buttonBox->button(QDialogButtonBox::Close)->setFocus();
    updateContent();
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(buttonClicked(QAbstractButton*)));
    connect(m_list, SIGNAL(openRatedGame(unsigned int)),
            this, SLOT(activateGame(unsigned int)));
}

void RatingDialog::activateGame(unsigned int n)
{
    emit open(m_history.getFile(n).string().c_str());
}

void RatingDialog::buttonClicked(QAbstractButton* button)
{
    if (button != static_cast<QAbstractButton*>(m_clearButton))
        return;
    QMessageBox msgBox(QMessageBox::Warning, tr("Pentobi"),
                       tr("Clear rating and delete rating history?"),
                       QMessageBox::Cancel, this);
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
    unsigned int nuGames = m_history.getNuGames();
    Rating rating = m_history.getRating();
    Rating bestRating = m_history.getBestRating();
    if (nuGames == 0)
        rating = Rating(0);
    QString variantStr;
    switch (variant)
    {
    case variant_classic:
        variantStr = tr("Classic");
        break;
    case variant_classic_2:
        variantStr = tr("Classic Two-Player");
        break;
    case variant_duo:
        variantStr = tr("Duo");
        break;
    case variant_trigon:
        variantStr = tr("Trigon");
        break;
    case variant_trigon_2:
        variantStr = tr("Trigon Two-Player");
        break;
    case variant_trigon_3:
        variantStr = tr("Trigon Three-Player");
        break;
    case variant_junior:
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
