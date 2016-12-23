//-----------------------------------------------------------------------------
/** @file pentobi_qml/RatingModel.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_QML_RATING_MODEL_H
#define PENTOBI_QML_PLAYER_MODEL_H

#include <QObject>

#include "libboardgame_base/Rating.h"

class GameModel;

using libboardgame_base::Rating;

//-----------------------------------------------------------------------------

class RatingModel
    : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float bestRating READ bestRating NOTIFY bestRatingChanged)
    Q_PROPERTY(QString gameVariant MEMBER m_gameVariant WRITE setGameVariant NOTIFY gameVariantChanged)
    Q_PROPERTY(int numberGames READ numberGames NOTIFY numberGamesChanged)
    Q_PROPERTY(float rating READ rating NOTIFY ratingChanged)

public:
    explicit RatingModel(QObject* parent = nullptr);


    Q_INVOKABLE void addResult(GameModel* gameModel, int level);

    Q_INVOKABLE void clearRating();

    Q_INVOKABLE int getNextHumanPlayer() const;

    Q_INVOKABLE int getNextLevel(int maxLevel) const;

    Q_INVOKABLE void setInitialRating(float rating);


    float bestRating() const { return m_bestRating.get(); }

    int numberGames() const { return m_numberGames; }

    float rating() const { return m_rating.get(); }

    void setGameVariant(const QString& gameVariant);

signals:
    void bestRatingChanged();

    void gameVariantChanged();

    void numberGamesChanged();

    void ratingChanged();

private:
    int m_numberGames = 0;

    Rating m_bestRating = Rating(1000.);

    Rating m_rating = Rating(1000.);

    QString m_gameVariant;


    void saveSettings();

    void setBestRating(float rating);

    void setRating(float rating);

    void setNumberGames(int numberGames);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_QML_PLAYER_MODEL_H
