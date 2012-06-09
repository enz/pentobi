//-----------------------------------------------------------------------------
/** @file pentobi/RatingDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatingDialog.h"

#include "Util.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using libboardgame_util::log;
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
    m_table = new QTableView();
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_model = new QStandardItemModel(m_table);
    m_table->setModel(m_model);
    m_model->setColumnCount(5);
    layout->addWidget(m_table, 1);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_table, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(gameClicked(const QModelIndex&)));
    buttonBox->setFocus();
}

void RatingDialog::gameClicked(const QModelIndex& index)
{
    QStandardItem* item = m_model->item(index.row(), 0);
    if (item == 0)
        return;
    bool ok;
    unsigned int n = item->text().toUInt(&ok);
    if (ok)
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
    m_model->clear();
    QStringList headers;
    headers << tr("Game") << tr("Your Color") << tr("Level") << tr("Result")
            << tr("Date");
    m_model->setHorizontalHeaderLabels(headers);
    QHeaderView* header = m_table->horizontalHeader();
    header->setDefaultAlignment(Qt::AlignLeft);
    header->setResizeMode(0, QHeaderView::ResizeToContents);
    header->setResizeMode(1, QHeaderView::ResizeToContents);
    header->setResizeMode(2, QHeaderView::ResizeToContents);
    header->setResizeMode(3, QHeaderView::ResizeToContents);
    header->setResizeMode(4, QHeaderView::Stretch);
    int nuRows = history.get().size();
    m_model->setRowCount(nuRows);
    m_table->setSortingEnabled(false);
    for (int i = 0; i < nuRows; ++i)
    {
        const RatingHistory::GameInfo& info = history.get()[i];
        QString number;
        number.setNum(info.number);
        QString date = QString::fromLocal8Bit(info.date.c_str());
        QString color;
        if (info.color.to_int() < get_nu_colors(variant))
            color = Util::getPlayerString(variant, info.color);
        else
            log() << "Error: invalid color in rating history\n";
        QString level;
        level.setNum(info.level);
        QString result;
        result.setNum(info.result, 'g', 2);
        m_model->setItem(i, 0, new QStandardItem(number));
        m_model->setItem(i, 1, new QStandardItem(color));
        m_model->setItem(i, 2, new QStandardItem(level));
        m_model->setItem(i, 3, new QStandardItem(result));
        m_model->setItem(i, 4, new QStandardItem(date));
    }
    m_table->setSortingEnabled(true);
    m_table->sortByColumn(0, Qt::DescendingOrder);
}

//-----------------------------------------------------------------------------
