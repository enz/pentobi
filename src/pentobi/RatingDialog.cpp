//-----------------------------------------------------------------------------
/** @file pentobi/RatingDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatingDialog.h"

#include <boost/foreach.hpp>
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
    m_labelBestRating = new QLabel();
    layout->addWidget(m_labelBestRating);
    layout->addSpacing(layout->spacing());
    layout->addWidget(new QLabel(tr("Recent development:")));
    m_graph = new RatingGraph();
    layout->addWidget(m_graph, 1);
    m_list = new RatedGamesList();
    layout->addWidget(m_list, 1);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    layout->addWidget(buttonBox);
    updateLabels(Rating(0), 0, Rating(0));
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
    Rating bestRating;
    Util::getRating(variant, rating, nuGames, bestRating);

    // Older versions of Pentobi (up to version 3) did not save the all-time
    // best rating, so after an upgrade to a newer version of Pentobi, the
    // history of recent rated games can contain a higher rating than the
    // stored all-time best rating.
    // Fixing this here is a hack, but the code can be removed some time in
    // the future anyway when it is sufficiently unlikely that someone
    // still upgrades from an old version.
    BOOST_FOREACH(const RatingHistory::GameInfo& info, history.get())
        if (info.rating.get() > bestRating.get())
            bestRating = info.rating;

    updateLabels(rating, nuGames, bestRating);
    m_graph->setHistory(history);
    m_list->updateContent(variant, history);
}

void RatingDialog::updateLabels(Rating rating, unsigned int nuGames,
                                Rating bestRating)
{
    if (nuGames == 0)
        rating = Rating(0);
    m_labelRating->setText("<b>"
                           + tr("Your rating: %1")
                           .arg(rating.get(), 0, 'f', 0));
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
    m_labelNuGames->setText(tr("%n rated game(s)", "", nuGames));
    m_labelBestRating->setText(tr("Best previous rating: %1")
                               .arg(bestRating.get(), 0, 'f', 0));
}

//-----------------------------------------------------------------------------
