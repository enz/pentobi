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

RatingDialog::RatingDialog(QWidget* parent, GameVariant variant)
    : QDialog(parent)
{
    setWindowTitle(tr("Pentobi - Your Rating"));
    // Disable '?' button in title bar on Windows, we don't have
    // context help
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    QFormLayout* formLayout = new QFormLayout();
    layout->addLayout(formLayout);
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
    formLayout->addRow(tr("Game variant:"), new QLabel(variantStr));
    QSettings settings;
    Rating rating;
    unsigned int nuGames;
    Util::getRating(variant, rating, nuGames);
    QString nuGamesStr;
    nuGamesStr.setNum(nuGames);
    formLayout->addRow(tr("Games played:"), new QLabel(nuGamesStr));
    QString ratingStr;
    if (nuGames > 0)
        ratingStr.setNum(rating.get(), 'f', 0);
    formLayout->addRow(tr("Your rating:"), new QLabel(ratingStr));
    layout->addSpacing(layout->spacing());
    layout->addWidget(new QLabel(tr("Recent development")));
    m_graph = new RatingGraph();
    layout->addWidget(m_graph);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

//-----------------------------------------------------------------------------
