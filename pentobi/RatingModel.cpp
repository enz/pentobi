//-----------------------------------------------------------------------------
/** @file pentobi/RatingModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "RatingModel.h"

#include <limits>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include "GameModel.h"
#include "libpentobi_mcts/Player.h"

using namespace Qt::StringLiterals;
using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

namespace {

const qsizetype maxSavedGames = 50;

} // namespace

//-----------------------------------------------------------------------------

TableModel::TableModel(QObject* parent, const QList<RatedGameInfo>& history)
    : QAbstractTableModel(parent),
      m_history(history)
{
}

int TableModel::rowCount([[maybe_unused]] const QModelIndex& parent) const
{
    qsizetype length = min(m_history.length(), maxSavedGames);
    static_assert(maxSavedGames < numeric_limits<int>::max());
    return static_cast<int>(length + 1);
}

int TableModel::columnCount([[maybe_unused]] const QModelIndex& parent) const
{
    return 5;
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if (role != Qt::DisplayRole || row < 0 || row >= m_history.length() + 1)
        return {};
    if (row == 0)
    {
        // We currently put the table headers in row 0 because Qt 5.12 does
        // not support table headers.
        switch (index.column())
        {
        case 0:
            //: Table header for game number in rating dialog
            return tr("Game");
        case 1:
            //: Table header for game result in rating dialog
            return tr("Result");
        case 2:
            //: Table header for level in rating dialog
            return tr("Level");
        case 3:
            //: Table header for player color(s) in rating dialog
            return tr("Your Color");
        case 4:
            //: Table header for game date in rating dialog
            return tr("Date");
        default:
            return {};
        }
    }
    auto& info = m_history[row - 1];
    switch (index.column())
    {
    case 0:
        return info.number;
    case 1:
        if (info.result == 1)
            //: Result of rated game is a win
            return tr("Win");
        else if (info.result == 0)
            //: Result of rated game is a loss
            return tr("Loss");
        else
            //: Result of rated game is a tie. Abbreviate long translations
            //: to ensure that all columns of rated games list are visible
            //: on mobile devices with small screens.
            return tr("Tie");
    case 2:
        return info.level;
    case 3:
        return info.color;
    case 4:
        return info.date;
    default:
        return {};
    }
}

//-----------------------------------------------------------------------------

RatingModel::RatingModel(QObject* parent)
    : QObject(parent)
{
    m_tableModel = new TableModel(this, m_history);
}

void RatingModel::addResult(GameModel* gameModel, int level)
{
    Variant variant;
    if (! parse_variant_id(m_gameVariant.toLocal8Bit().constData(), variant))
        return;
    unsigned place;
    bool isPlaceShared;
    auto color = getNextHumanPlayer();
    gameModel->getBoard().get_place(Color(static_cast<Color::IntType>(color)),
                                    place, isPlaceShared);
    double gameResult;
    if (place == 0 && ! isPlaceShared)
        gameResult = 1;
    else if (place == 0)
        gameResult = 0.5;
    else
        gameResult = 0;
    auto nuOpponents = static_cast<unsigned>(get_nu_players(variant) - 1);
    Rating opponentRating =
            Player::get_rating(variant, static_cast<unsigned>(level));
    double kValue = (m_numberGames < 30 ? 40 : 20);
    Rating rating = m_rating;
    rating.update(gameResult, opponentRating, kValue, nuOpponents);
    setRating(rating.get());
    auto numberGames = m_numberGames + 1;
    if (numberGames == 1 || rating.get() > m_bestRating.get())
        setBestRating(rating.get());
    auto date = QDate::currentDate().toString("yyyy-MM-dd"_L1);
    m_history.prepend({numberGames, color, level, gameResult, m_rating.get(),
                       date});
    auto file = getFile(numberGames);
    QFileInfo(file).dir().mkpath("."_L1);
    gameModel->save(file);
    emit ratingHistoryChanged();
    emit tableModelChanged();
    setNumberGames(numberGames);
    saveSettings();
}

void RatingModel::clearRating()
{
    if (! m_history.isEmpty())
    {
        m_history.clear();
        emit ratingHistoryChanged();
        emit tableModelChanged();
    }
    QDir(getDir()).removeRecursively();
    setRating(1000);
    setBestRating(1000);
    setNumberGames(0);
}

QString RatingModel::getDir() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
           + QDir::separator() + "Rated Games"_L1 + QDir::separator()
           + m_gameVariantName;
}

QString RatingModel::getFile(int gameNumber) const
{
    return getDir() + QDir::separator() + m_gameVariantName + " "_L1
           + QString::number(gameNumber) + ".blksgf"_L1;
}

int RatingModel::getGameNumber(int historyIndex) const
{
    if (historyIndex < 0 || historyIndex >= m_history.length())
        return -1;
    return m_history[historyIndex].number;
}

int RatingModel::getGameNumberOfFile(const QString& file) const
{
    QString left = getDir() + QDir::separator() + m_gameVariantName + " "_L1;
    if (! file.startsWith(left))
        return 0;
    QString right = ".blksgf"_L1;
    if (! file.endsWith(right))
        return 0;
    auto leftLen = left.length();
    auto rightLen = right.length();
    QStringView view(file);
    bool ok;
    int n = view.sliced(leftLen, file.length() - leftLen - rightLen).toInt(&ok);
    return ok && n >= 1 && n <= m_numberGames ? n : 0;
}

int RatingModel::getNextHumanPlayer() const
{
    Variant variant;
    if (! parse_variant_id(m_gameVariant.toLocal8Bit().constData(), variant))
        return 0;
    mt19937 generator;
    generator.discard(static_cast<unsigned>(m_numberGames));
    uniform_int_distribution<> distribution(0, get_nu_players(variant) - 1);
    return distribution(generator);
}

int RatingModel::getNextLevel(int maxLevel) const
{
    Variant variant;
    if (! parse_variant_id(m_gameVariant.toLocal8Bit().constData(), variant))
        return 1;
    int level = 1; // Initialize to avoid compiler warning
    double minDiff = 0; // Initialize to avoid compiler warning
    for (int i = 1; i <= maxLevel; ++i)
    {
        auto diff =
                abs(m_rating.get()
                    - Player::get_rating(variant, static_cast<unsigned>(i)).get());
        if (i == 1 || diff < minDiff)
        {
            minDiff = diff;
            level = i;
        }
    }
    return level;
}

const QList<double>& RatingModel::ratingHistory()
{
    m_ratingHistory.clear();
    m_ratingHistory.reserve(m_history.length());
    for (auto i = m_history.rbegin(); i != m_history.rend(); ++i)
        m_ratingHistory.push_back(i->rating);
    return m_ratingHistory;
}

void RatingModel::saveSettings()
{
    QSettings settings("%1/%2.ini"_L1.arg(getDir(), m_gameVariantName),
                       QSettings::IniFormat);
    if (m_numberGames == 0)
    {
        settings.remove("rated_games"_L1);
        settings.remove("rating"_L1);
        settings.remove("best_rating"_L1);
    }
    else
    {
        settings.setValue("rated_games"_L1, m_numberGames);
        settings.setValue("rating"_L1, m_rating.get());
        settings.setValue("best_rating"_L1, round(m_bestRating.get()));
    }
    QList<RatedGameInfo> newHistory;
    newHistory.reserve(m_history.size());
    for (auto& info : m_history)
    {
        if (info.number <= m_numberGames - maxSavedGames)
            QFile::remove(getFile(info.number));
        else
            newHistory.append(info);
    }
    if (newHistory.size() != m_history.size())
    {
        m_history = newHistory;
        emit ratingHistoryChanged();
        emit tableModelChanged();
    }
    settings.remove("rated_game_info"_L1);
    if (m_numberGames > 0)
    {
        settings.beginWriteArray("rated_game_info"_L1);
        int n = 0;
        for (auto& info : m_history)
        {
            if (info.number <= m_numberGames - maxSavedGames)
                continue;
            settings.setArrayIndex(n++);
            settings.setValue("number"_L1, info.number);
            settings.setValue("color"_L1, info.color);
            settings.setValue("result"_L1, info.result);
            settings.setValue("date"_L1, info.date);
            settings.setValue("level"_L1, info.level);
            settings.setValue("rating"_L1, round(info.rating));
        }
        settings.endArray();
    }
}

void RatingModel::setBestRating(double rating)
{
    m_bestRating = Rating(rating);
    emit bestRatingChanged();
}

void RatingModel::setGameVariant(const QString& gameVariant)
{
    if (m_gameVariant == gameVariant)
        return;
    Variant variant;
    if (! libpentobi_base::parse_variant_id(
                gameVariant.toLocal8Bit().constData(), variant))
        return;
    m_gameVariant = gameVariant;
    m_gameVariantName =
            QString::fromLocal8Bit(libpentobi_base::to_string(variant));
    QSettings settings("%1/%2.ini"_L1.arg(getDir(), m_gameVariantName),
                       QSettings::IniFormat);
    auto currentRating = settings.value("rating"_L1, 1000).toDouble();
    auto bestRating = settings.value("best_rating"_L1, 0).toDouble();
    setRating(currentRating);
    setBestRating(bestRating);
    m_history.clear();
    auto size = settings.beginReadArray("rated_game_info"_L1);
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        auto number = settings.value("number"_L1).toInt();
        auto color = settings.value("color"_L1).toInt();
        auto result = settings.value("result"_L1).toDouble();
        auto date = settings.value("date"_L1).toString();
        auto level = settings.value("level"_L1).toInt();
        auto rating = settings.value("rating"_L1).toDouble();
        if (i < maxSavedGames)
            m_history.append({number, color, level, result, rating, date});
        else
            QFile::remove(getFile(number));
    }
    settings.endArray();
    emit ratingHistoryChanged();
    emit tableModelChanged();
    setNumberGames(settings.value("rated_games"_L1, 0).toInt());
    emit gameVariantChanged();
}

void RatingModel::setInitialRating(double rating)
{
    setRating(rating);
    setBestRating(rating);
    saveSettings();
}

void RatingModel::setNumberGames(int numberGames)
{
    if (numberGames < 0)
        return;
    if (m_numberGames == numberGames)
        return;
    m_numberGames = numberGames;
    emit numberGamesChanged();
}

void RatingModel::setRating(double rating)
{
    m_rating = Rating(rating);
    emit ratingChanged();
}

//-----------------------------------------------------------------------------
