//-----------------------------------------------------------------------------
/** @file pentobi/RatingDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatingDialog.h"

#include "Util.h"

using namespace std;
using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;
using libpentobi_base::game_variant_junior;
using libpentobi_base::game_variant_trigon;
using libpentobi_base::game_variant_trigon_2;
using libpentobi_base::game_variant_trigon_3;

//-----------------------------------------------------------------------------

RatingDialog::RatingDialog(QWidget* parent)
    : QDialog(parent),
      m_variant(game_variant_classic)
{
    setWindowTitle(tr("Pentobi - Your Rating"));
    // Disable '?' button in title bar on Windows, we don't have
    // context help
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    QFormLayout* formLayout = new QFormLayout();
    layout->addLayout(formLayout);
    m_labelVariant = new QLabel();
    formLayout->addRow(tr("Game variant:"), m_labelVariant);
    m_labelRating = new QLabel();
    formLayout->addRow(tr("Your rating:"), m_labelRating);
    m_labelNuGames = new QLabel();
    formLayout->addRow(tr("Games played:"), m_labelNuGames);
    layout->addSpacing(layout->spacing());
    layout->addWidget(new QLabel(tr("Recent development:")));
    m_graph = new RatingGraph();
    layout->addWidget(m_graph, 1);
    m_list = new RatedGamesList();
    layout->addWidget(m_list, 1);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_list, SIGNAL(openRatedGame(unsigned int)),
            this, SLOT(activateGame(unsigned int)));
    buttonBox->setFocus();
}

void RatingDialog::activateGame(unsigned int n)
{
    emit openRatedGame(m_variant, n);
}

void RatingDialog::updateContent(GameVariant variant,
                                 const RatingHistory& history)
{
    m_variant = variant;
    QString variantStr;
    switch (variant)
    {
    case game_variant_classic:
        variantStr = tr("Classic");
        break;
    case game_variant_classic_2:
        variantStr = tr("Classic Two-Player");
        break;
    case game_variant_duo:
        variantStr = tr("Duo");
        break;
    case game_variant_trigon:
        variantStr = tr("Trigon");
        break;
    case game_variant_trigon_2:
        variantStr = tr("Trigon Two-Player");
        break;
    case game_variant_trigon_3:
        variantStr = tr("Trigon Three-Player");
        break;
    case game_variant_junior:
        variantStr = tr("Junior");
        break;
    }
    m_labelVariant->setText(variantStr);
    QSettings settings;
    Rating rating;
    unsigned int nuGames;
    Util::getRating(variant, rating, nuGames);
    QString nuGamesStr;
    nuGamesStr.setNum(nuGames);
    m_labelNuGames->setText(nuGamesStr);
    QString ratingStr;
    if (nuGames > 0)
        ratingStr.setNum(rating.get(), 'f', 0);
    m_labelRating->setText(ratingStr);
    m_graph->setHistory(history);
    m_list->updateContent(variant, history);
}

//-----------------------------------------------------------------------------
