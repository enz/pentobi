//-----------------------------------------------------------------------------
/** @file pentobi/RatedGamesList.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatedGamesList.h"

#include "libboardgame_util/Log.h"
#include "libpentobi_gui/Util.h"

using libboardgame_util::log;

//-----------------------------------------------------------------------------

RatedGamesList::RatedGamesList(QWidget* parent)
    : QTableView(parent)
{
    verticalHeader()->setVisible(false);
    setShowGrid(false);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setTabKeyNavigation(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAlternatingRowColors(true);
    m_model = new QStandardItemModel(this);
    setModel(m_model);
    connect(this, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(activateGame(const QModelIndex&)));
}

void RatedGamesList::activateGame(const QModelIndex& index)
{
    QStandardItem* item = m_model->item(index.row(), 0);
    if (item == 0)
        return;
    bool ok;
    unsigned int n = item->text().toUInt(&ok);
    if (ok)
        emit openRatedGame(n);
}

void RatedGamesList::focusInEvent(QFocusEvent* event)
{
    // Select current index if list has focus
    selectRow(currentIndex().row());
    QTableView::focusInEvent(event);
}

void RatedGamesList::focusOutEvent(QFocusEvent* event)
{
    // Show selection only if list has focus
    clearSelection();
    QTableView::focusOutEvent(event);
}

void RatedGamesList::keyPressEvent(QKeyEvent* event)
{
    if (event->type() == QEvent::KeyPress
        && static_cast<QKeyEvent*>(event)->key() == Qt::Key_Space)
    {
        QModelIndexList indexes =
            selectionModel()->selection().indexes();
        if (! indexes.isEmpty())
            activateGame(indexes[0]);
        return;
    }
    QTableView::keyPressEvent(event);
}

void RatedGamesList::updateContent(GameVariant variant,
                                   const RatingHistory& history)
{
    m_model->clear();
    QStringList headers;
    headers << tr("Game") << tr("Your Color") << tr("Level") << tr("Result")
            << tr("Date");
    m_model->setHorizontalHeaderLabels(headers);
    QHeaderView* header = horizontalHeader();
    header->setDefaultAlignment(Qt::AlignLeft);
    header->setHighlightSections(false);
    header->setResizeMode(0, QHeaderView::ResizeToContents);
    header->setResizeMode(1, QHeaderView::ResizeToContents);
    header->setResizeMode(2, QHeaderView::ResizeToContents);
    header->setResizeMode(3, QHeaderView::ResizeToContents);
    header->setResizeMode(4, QHeaderView::Stretch);
    int nuRows = history.get().size();
    m_model->setRowCount(nuRows);
    setSortingEnabled(false);
    for (int i = 0; i < nuRows; ++i)
    {
        const RatingHistory::GameInfo& info = history.get()[i];
        QStandardItem* number = new QStandardItem();
        number->setData(info.number, Qt::DisplayRole);
        QStandardItem* color = new QStandardItem();
        if (info.color.to_int() < get_nu_colors(variant))
            color->setText(Util::getPlayerString(variant, info.color));
        else
            log() << "Error: invalid color in rating history\n";
        QStandardItem* level = new QStandardItem();
        level->setData(info.level, Qt::DisplayRole);
        QString result;
        if (get_nu_players(variant) == 2)
        {
            if (info.place == 0 && ! info.is_place_shared)
                result = tr("Win");
            else if (info.place == 0 && info.is_place_shared)
                result = tr("Tie");
            else if (info.place == 1)
                result = tr("Loss");
        }
        else
        {
            if (info.place == 0)
                result = tr("1st place");
            else if (info.place == 1)
                result = tr("2nd place");
            else if (info.place == 2)
                result = tr("3rd place");
            else if (info.place == 3)
                result = tr("4th place");
            if (info.is_place_shared)
                result += " " + tr("(shared)");
        }
        QString date = QString::fromLocal8Bit(info.date.c_str());
        int row = nuRows - i - 1;
        m_model->setItem(row, 0, number);
        m_model->setItem(row, 1, color);
        m_model->setItem(row, 2, level);
        m_model->setItem(row, 3, new QStandardItem(result));
        m_model->setItem(row, 4, new QStandardItem(date));
    }
    setSortingEnabled(true);
    if (nuRows > 0)
        selectionModel()->setCurrentIndex(model()->index(0, 0),
                                          QItemSelectionModel::NoUpdate);
}

//-----------------------------------------------------------------------------
