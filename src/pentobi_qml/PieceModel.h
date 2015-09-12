//-----------------------------------------------------------------------------
/** @file pentobi_qml/PieceModel.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_QML_PIECE_MODEL_H
#define PENTOBI_QML_PIECE_MODEL_H

#include <QObject>
#include <QPointF>
#include <QVariant>
#include "libpentobi_base/Board.h"

using libboardgame_base::Transform;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::Piece;
using libpentobi_base::PiecePoints;

//-----------------------------------------------------------------------------

class PieceModel
    : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int color READ color CONSTANT)
    Q_PROPERTY(QVariantList elements READ elements CONSTANT)
    Q_PROPERTY(QPointF center READ center CONSTANT)
    Q_PROPERTY(QPointF labelPos READ labelPos CONSTANT)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(bool isPlayed READ isPlayed NOTIFY isPlayedChanged)
    Q_PROPERTY(QPointF gameCoord READ gameCoord NOTIFY gameCoordChanged)

public:
    static QPointF findCenter(const Board& bd, const PiecePoints& points,
                              bool isOriginDownward);

    PieceModel(QObject* parent, const Board& bd, libpentobi_base::Piece piece,
               Color c);

    int color();

    /** List of QPointF instances with coordinates of piece elements. */
    QVariantList elements();

    QPointF center() const;

    QPointF labelPos() const;

    QString state() const;

    bool isPlayed() const;

    QPointF gameCoord() const;

    Piece getPiece() const;

    const Transform* getTransform() const;

    void setState(QString state);

    void setTransform(const Transform* transform);

    void setIsPlayed(bool isPlayed);

    void setGameCoord(QPointF gameCoord);

    Q_INVOKABLE void rotateLeft();

    Q_INVOKABLE void rotateRight();

    Q_INVOKABLE void flipAcrossX();

    Q_INVOKABLE void flipAcrossY();

signals:
    void stateChanged(QString);

    void isPlayedChanged(bool);

    void gameCoordChanged(QPointF);

private:
    const Board& m_bd;

    Color m_color;

    Piece m_piece;

    bool m_isPlayed = false;

    QPointF m_gameCoord;

    QPointF m_center;

    QPointF m_labelPos;

    QVariantList m_elements;

    QString m_state;

    const Transform* m_transform;

    void updateTransformFromState();
};

inline libpentobi_base::Piece PieceModel::getPiece() const
{
    return m_piece;
}

inline const Transform* PieceModel::getTransform() const
{
    return m_transform;
}

//-----------------------------------------------------------------------------

#endif // PENTOBI_QML_PIECE_MODEL_H
