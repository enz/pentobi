//-----------------------------------------------------------------------------
/** @file pentobi/RatingModel.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_RATING_MODEL_H
#define PENTOBI_RATING_MODEL_H

#include <QAbstractTableModel>
#include "libboardgame_base/Rating.h"

class GameModel;

using libboardgame_base::Rating;

//-----------------------------------------------------------------------------

struct RatedGameInfo
{
    int number;

    int color;

    int level;

    double result;

    double rating;

    QString date;
};

//-----------------------------------------------------------------------------

class TableModel
    : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableModel(QObject* parent, const QList<RatedGameInfo>& history);

    int rowCount(const QModelIndex& parent) const override;

    int columnCount(const QModelIndex& parent) const override;

    QVariant data(const QModelIndex& index, int role) const override;

private:
    const QList<RatedGameInfo>& m_history;
};

//-----------------------------------------------------------------------------

class RatingModel
    : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double bestRating READ bestRating NOTIFY bestRatingChanged)
    Q_PROPERTY(QString gameVariant MEMBER m_gameVariant WRITE setGameVariant NOTIFY gameVariantChanged)
    Q_PROPERTY(TableModel* tableModel READ tableModel NOTIFY tableModelChanged)
    Q_PROPERTY(QList<double> ratingHistory READ ratingHistory NOTIFY ratingHistoryChanged)
    Q_PROPERTY(int numberGames READ numberGames NOTIFY numberGamesChanged)
    Q_PROPERTY(double rating READ rating NOTIFY ratingChanged)

public:
    RatingModel(QObject* parent = nullptr);

    Q_INVOKABLE void addResult(GameModel* gameModel, int level);

    Q_INVOKABLE void clearRating();

    Q_INVOKABLE int getNextHumanPlayer() const;

    Q_INVOKABLE int getNextLevel(int maxLevel) const;

    Q_INVOKABLE void setInitialRating(double rating);

    Q_INVOKABLE QString getFile(int gameNumber) const;

    Q_INVOKABLE int getGameNumber(int historyIndex) const;

    /** Get the game number corresponding to a file.
        @return The game number or 0 if file is not a rated game. */
    Q_INVOKABLE int getGameNumberOfFile(const QString& file) const;


    double bestRating() const { return m_bestRating.get(); }

    const QList<double>& ratingHistory();

    TableModel* tableModel() { return m_tableModel; }

    int numberGames() const { return m_numberGames; }

    double rating() const { return m_rating.get(); }

    void setGameVariant(const QString& gameVariant);

signals:
    void bestRatingChanged();

    void gameVariantChanged();

    void ratingHistoryChanged();

    void tableModelChanged();

    void numberGamesChanged();

    void ratingChanged();

private:
    int m_numberGames = 0;

    Rating m_bestRating = Rating(1000.);

    Rating m_rating = Rating(1000.);

    QString m_gameVariant;

    QString m_gameVariantName;

    QList<RatedGameInfo> m_history;

    QList<double> m_ratingHistory;

    TableModel* m_tableModel;


    QString getDir() const;

    void saveSettings();

    void setBestRating(double rating);

    void setRating(double rating);

    void setNumberGames(int numberGames);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATING_MODEL_H
