//-----------------------------------------------------------------------------
/** @file pentobi/RatedGamesList.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RatedGamesList.h"

#include <QHeaderView>
#include <QKeyEvent>
#include <QStandardItemModel>
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
    scrollTo(currentIndex());
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

void RatedGamesList::updateContent(Variant variant,
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
        if (info.result == 1)
            result = tr("Win");
        else if (info.result == 0.5)
            result = tr("Tie");
        else if (info.result == 0)
            result = tr("Loss");
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
