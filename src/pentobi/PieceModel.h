//-----------------------------------------------------------------------------
/** @file pentobi/PieceModel.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_PIECE_MODEL_H
#define PENTOBI_PIECE_MODEL_H

// Needed in the header because moc_*.cpp does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QObject>
#include <QPointF>
#include <QVariant>
#include <QVector>
#include "libpentobi_base/Color.h"
#include "libpentobi_base/Piece.h"
#include "libpentobi_base/PieceInfo.h"

namespace libboardgame_base { class Transform; }
namespace libpentobi_base { class Board; }

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

    /** List of QPointF instances with coordinates of piece elements. */
    Q_PROPERTY(QVariantList elements MEMBER m_elements CONSTANT)

    /** List of QPointF instances with coordinates of piece junctions.
        Only used in Nexos. */
    Q_PROPERTY(QVariantList junctions MEMBER m_junctions CONSTANT)

    /** List of integers determining the type of junctions.
        In Nexos, this is the type of junction in junction(). In Callisto, it
        is the information if the squares in elements() have a right and/or
        down neighbor. See implementation for the meaning of the numbers. */
    Q_PROPERTY(QVariantList junctionType MEMBER m_junctionType CONSTANT)

    Q_PROPERTY(QPointF center MEMBER m_center CONSTANT)
    Q_PROPERTY(QPointF labelPos MEMBER m_labelPos CONSTANT)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool isPlayed READ isPlayed NOTIFY isPlayedChanged)
    Q_PROPERTY(bool isLastMove READ isLastMove NOTIFY isLastMoveChanged)
    Q_PROPERTY(QString moveLabel READ moveLabel NOTIFY moveLabelChanged)
    Q_PROPERTY(QPointF gameCoord READ gameCoord NOTIFY gameCoordChanged)

public:
    static QPointF findCenter(const Board& bd, const PiecePoints& points,
                              bool isOriginDownward);

    PieceModel(QObject* parent, const Board& bd, Piece piece, Color c);

    int color() { return m_color.to_int(); }

    QString state() const { return m_state; }

    bool isPlayed() const { return m_isPlayed; }

    bool isLastMove() const { return m_isLastMove; }

    QString moveLabel() const { return m_moveLabel; }

    QPointF gameCoord() const { return m_gameCoord; }

    Piece getPiece() const { return m_piece; }

    const Transform* getTransform(const QString& state) const;

    const Transform* getTransform() const { return getTransform(m_state); }

    void setDefaultState();

    void setTransform(const Transform* transform);

    void setIsPlayed(bool isPlayed);

    void setIsLastMove(bool isLastMove);

    void setMoveLabel(const QString& moveLabel);

    void setGameCoord(QPointF gameCoord);

    Q_INVOKABLE void rotateLeft();

    Q_INVOKABLE void rotateRight();

    Q_INVOKABLE void flipAcrossX();

    Q_INVOKABLE void flipAcrossY();

    Q_INVOKABLE void nextOrientation();

    Q_INVOKABLE void previousOrientation();

signals:
    void stateChanged();

    void isPlayedChanged();

    void isLastMoveChanged();

    void gameCoordChanged();

    void moveLabelChanged();

private:
    const Board& m_bd;

    Color m_color;

    Piece m_piece;

    bool m_isPlayed = false;

    bool m_isLastMove = false;

    QPointF m_gameCoord;

    QPointF m_center;

    QPointF m_labelPos;

    QVariantList m_elements;

    QVariantList m_junctions;

    QVariantList m_junctionType;

    QString m_state;

    QString m_moveLabel;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_PIECE_MODEL_H
