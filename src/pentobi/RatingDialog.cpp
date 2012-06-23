//-----------------------------------------------------------------------------
/** @file pentobi/RatingDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatingDialog.h"

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

RatingDialog::RatingDialog(QWidget* parent)
    : QDialog(parent),
      m_variant(variant_classic)
{
    setWindowTitle(tr("Rating"));
    // Disable '?' button in title bar on Windows, we don't have
    // context help
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    m_labelRating = new QLabel();
    layout->addWidget(m_labelRating);
    m_labelVariant = new QLabel();
    layout->addWidget(m_labelVariant);
    m_labelNuGames = new QLabel();
    layout->addWidget(m_labelNuGames);
    layout->addSpacing(layout->spacing());
    layout->addWidget(new QLabel(tr("Recent development:")));
    m_graph = new RatingGraph();
    layout->addWidget(m_graph, 1);
    m_list = new RatedGamesList();
    layout->addWidget(m_list, 1);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);
    updateLabels(Rating(0), 0);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_list, SIGNAL(openRatedGame(unsigned int)),
            this, SLOT(activateGame(unsigned int)));
    buttonBox->setFocus();
}

void RatingDialog::activateGame(unsigned int n)
{
    emit openRatedGame(m_variant, n);
}

void RatingDialog::updateContent(Variant variant,
                                 const RatingHistory& history)
{
    m_variant = variant;
    QSettings settings;
    Rating rating;
    unsigned int nuGames;
    Util::getRating(variant, rating, nuGames);
    updateLabels(rating, nuGames);
    m_graph->setHistory(history);
    m_list->updateContent(variant, history);
}

void RatingDialog::updateLabels(Rating rating, unsigned int nuGames)
{
    if (nuGames == 0)
        rating = Rating(0);
    m_labelRating->setText("<b>"
                           + tr("Your rating: %1").arg(rating.get(),
                                                       0, 'f', 0));
    QString variantStr;
    switch (m_variant)
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
    m_labelVariant->setText(tr("Game variant %1").arg(variantStr));
    m_labelNuGames->setText(tr("%n game(s) played", "", nuGames));
}

//-----------------------------------------------------------------------------
