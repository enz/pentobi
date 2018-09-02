//-----------------------------------------------------------------------------
/** @file pentobi/PieceModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "PieceModel.h"

#include "libboardgame_base/RectTransform.h"
#include "libpentobi_base/Board.h"
#include "libpentobi_base/GembloQTransform.h"
#include "libpentobi_base/TrigonTransform.h"

using namespace std;
using libboardgame_base::CoordPoint;
using libboardgame_base::TransfIdentity;
using libboardgame_base::TransfRectRot90;
using libboardgame_base::TransfRectRot180;
using libboardgame_base::TransfRectRot270;
using libboardgame_base::TransfRectRefl;
using libboardgame_base::TransfRectRot90Refl;
using libboardgame_base::TransfRectRot180Refl;
using libboardgame_base::TransfRectRot270Refl;
using libboardgame_util::ArrayList;
using libpentobi_base::BoardType;
using libpentobi_base::GeometryType;
using libpentobi_base::PieceInfo;
using libpentobi_base::PieceSet;
using libpentobi_base::TransfGembloQIdentity;
using libpentobi_base::TransfGembloQRot90;
using libpentobi_base::TransfGembloQRot180;
using libpentobi_base::TransfGembloQRot270;
using libpentobi_base::TransfGembloQRefl;
using libpentobi_base::TransfGembloQRot90Refl;
using libpentobi_base::TransfGembloQRot180Refl;
using libpentobi_base::TransfGembloQRot270Refl;
using libpentobi_base::TransfTrigonIdentity;
using libpentobi_base::TransfTrigonRefl;
using libpentobi_base::TransfTrigonReflRot60;
using libpentobi_base::TransfTrigonReflRot120;
using libpentobi_base::TransfTrigonReflRot180;
using libpentobi_base::TransfTrigonReflRot240;
using libpentobi_base::TransfTrigonReflRot300;
using libpentobi_base::TransfTrigonRot60;
using libpentobi_base::TransfTrigonRot120;
using libpentobi_base::TransfTrigonRot180;
using libpentobi_base::TransfTrigonRot240;
using libpentobi_base::TransfTrigonRot300;

//-----------------------------------------------------------------------------

PieceModel::PieceModel(QObject* parent, const Board& bd, Piece piece, Color c)
    : QObject(parent),
      m_bd(bd),
      m_color(c),
      m_piece(piece)
{
    auto& geo = bd.get_geometry();
    bool isNexos = (bd.get_piece_set() == PieceSet::nexos);
    bool isCallisto = (bd.get_piece_set() == PieceSet::callisto);
    auto& info = bd.get_piece_info(piece);
    auto& points = info.get_points();
    m_elements.reserve(static_cast<int>(points.size()));
    for (auto& p : points)
    {
        if (isNexos && geo.get_point_type(p) == 0)
            continue;
        m_elements.append(QPointF(p.x, p.y));
    }
    if (isNexos)
    {
        ArrayList<CoordPoint, 2 * PieceInfo::max_scored_size, int> candidates;
        for (auto& p : points)
        {
            auto pointType = geo.get_point_type(p);
            if (pointType == 1)
            {
                candidates.include(CoordPoint(p.x - 1, p. y));
                candidates.include(CoordPoint(p.x + 1, p. y));
            }
            else if (pointType == 2)
            {
                candidates.include(CoordPoint(p.x, p. y - 1));
                candidates.include(CoordPoint(p.x, p. y + 1));
            }
        }
        m_junctions.reserve(candidates.size());
        m_junctionType.reserve(candidates.size());
        for (auto& p : candidates)
        {
            bool hasLeft = points.contains(CoordPoint(p.x - 1, p. y));
            bool hasRight = points.contains(CoordPoint(p.x + 1, p. y));
            bool hasUp = points.contains(CoordPoint(p.x, p. y - 1));
            bool hasDown = points.contains(CoordPoint(p.x, p. y + 1));
            int junctionType;
            if (hasLeft && hasRight && hasUp && hasDown)
                junctionType = 0;
            else if (hasRight && hasUp && hasDown)
                junctionType = 1;
            else if (hasLeft && hasUp && hasDown)
                junctionType = 2;
            else if (hasLeft && hasRight && hasDown)
                junctionType = 3;
            else if (hasLeft && hasRight && hasUp)
                junctionType = 4;
            else if (hasLeft && hasRight)
                junctionType = 5;
            else if (hasUp && hasDown)
                junctionType = 6;
            else if (hasLeft && hasUp)
                junctionType = 7;
            else if (hasLeft && hasDown)
                junctionType = 8;
            else if (hasRight && hasUp)
                junctionType = 9;
            else if (hasRight && hasDown)
                junctionType = 10;
            else
                continue;
            m_junctions.append(QPointF(p.x, p.y));
            m_junctionType.append(junctionType);
        }
    }
    if (isCallisto)
        for (auto& p : points)
        {
            bool hasRight = points.contains(CoordPoint(p.x + 1, p. y));
            bool hasDown = points.contains(CoordPoint(p.x, p.y + 1));
            int junctionType;
            if (hasRight && hasDown)
                junctionType = 0;
            else if (hasRight)
                junctionType = 1;
            else if (hasDown)
                junctionType = 2;
            else
                junctionType = 3;
            m_junctionType.append(junctionType);
        }
    m_center = findCenter(bd, points, true);
    m_labelPos = QPointF(info.get_label_pos().x, info.get_label_pos().y);
}

void PieceModel::flipAcrossX()
{
    setTransform(m_bd.get_transforms().get_mirrored_vertically(getTransform()));
}

void PieceModel::flipAcrossY()
{
    setTransform(m_bd.get_transforms().get_mirrored_horizontally(getTransform()));
}

const Transform* PieceModel::getTransform(const QString& state) const
{
    // See comment in getTransform() about the mapping between states and
    // transform classes.
    auto& transforms = m_bd.get_transforms();
    auto pieceSet = m_bd.get_piece_set();
    if (pieceSet == PieceSet::trigon)
    {
        if (state == QStringLiteral("rot60"))
            return transforms.find<TransfTrigonRot60>();
        if (state == QStringLiteral("rot120"))
            return transforms.find<TransfTrigonRot120>();
        if (state == QStringLiteral("rot180"))
            return transforms.find<TransfTrigonRot180>();
        if (state == QStringLiteral("rot240"))
            return transforms.find<TransfTrigonRot240>();
        if (state == QStringLiteral("rot300"))
            return transforms.find<TransfTrigonRot300>();
        if (state == QStringLiteral("flip"))
            return transforms.find<TransfTrigonReflRot180>();
        if (state == QStringLiteral("rot60Flip"))
            return transforms.find<TransfTrigonReflRot120>();
        if (state == QStringLiteral("rot120Flip"))
            return transforms.find<TransfTrigonReflRot60>();
        if (state == QStringLiteral("rot180Flip"))
            return transforms.find<TransfTrigonRefl>();
        if (state == QStringLiteral("rot240Flip"))
            return transforms.find<TransfTrigonReflRot300>();
        if (state == QStringLiteral("rot300Flip"))
            return transforms.find<TransfTrigonReflRot240>();
        return transforms.find<TransfTrigonIdentity>();
    }
    if (pieceSet == PieceSet::gembloq)
    {
        if (state == QStringLiteral("rot90"))
            return transforms.find<TransfGembloQRot90>();
        if (state == QStringLiteral("rot180"))
            return transforms.find<TransfGembloQRot180>();
        if (state == QStringLiteral("rot270"))
            return transforms.find<TransfGembloQRot270>();
        if (state == QStringLiteral("flip"))
            return transforms.find<TransfGembloQRot180Refl>();
        if (state == QStringLiteral("rot90Flip"))
            return transforms.find<TransfGembloQRot90Refl>();
        if (state == QStringLiteral("rot180Flip"))
            return transforms.find<TransfGembloQRefl>();
        if (state == QStringLiteral("rot270Flip"))
            return transforms.find<TransfGembloQRot270Refl>();
        return transforms.find<TransfGembloQIdentity>();
    }
    if (state == QStringLiteral("rot90"))
        return transforms.find<TransfRectRot90>();
    if (state == QStringLiteral("rot180"))
        return transforms.find<TransfRectRot180>();
    if (state == QStringLiteral("rot270"))
        return transforms.find<TransfRectRot270>();
    if (state == QStringLiteral("flip"))
        return transforms.find<TransfRectRot180Refl>();
    if (state == QStringLiteral("rot90Flip"))
        return transforms.find<TransfRectRot90Refl>();
    if (state == QStringLiteral("rot180Flip"))
        return transforms.find<TransfRectRefl>();
    if (state == QStringLiteral("rot270Flip"))
        return transforms.find<TransfRectRot270Refl>();
    return transforms.find<TransfIdentity>();
}

QPointF PieceModel::findCenter(const Board& bd, const PiecePoints& points,
                               bool usePieceInfoPointTypes)
{
    auto geoType = bd.get_geometry_type();
    bool isTrigon = (geoType == GeometryType::trigon);
    bool isGembloQ = (geoType == GeometryType::gembloq);
    bool isNexos = (geoType == GeometryType::nexos);
    bool isOriginDownward = (usePieceInfoPointTypes
                             && bd.get_board_type() == BoardType::trigon_3);
    auto& geo = bd.get_geometry();
    qreal sumX = 0;
    qreal sumY = 0;
    qreal n = 0;
    for (auto& p : points)
    {
        auto pointType = geo.get_point_type(p);
        if (isNexos && pointType == 0)
            continue;
        qreal centerX, centerY;
        if (isTrigon)
        {
            bool isDownward = (pointType == (isOriginDownward ? 0 : 1));
            centerX = 0.5;
            centerY = isDownward ?
                        static_cast<qreal>(1) / 3 : static_cast<qreal>(2) / 3;
        }
        else if (isGembloQ)
        {
            centerX = (pointType == 1 || pointType == 3) ?
                        static_cast<qreal>(1) / 3 : static_cast<qreal>(2) / 3;
            centerY = (pointType == 0 || pointType == 3) ?
                        static_cast<qreal>(1) / 3 : static_cast<qreal>(2) / 3;
        }
        else
        {
            centerX = 0.5;
            centerY = 0.5;
        }
        sumX += (p.x + centerX);
        sumY += (p.y + centerY);
        ++n;
    }
    return {sumX / n, sumY / n};
}

void PieceModel::nextOrientation()
{
    setTransform(m_bd.get_piece_info(m_piece).get_next_transform(getTransform()));
}

void PieceModel::previousOrientation()
{
    setTransform(m_bd.get_piece_info(m_piece).get_previous_transform(getTransform()));
}

void PieceModel::rotateLeft()
{
    setTransform(m_bd.get_transforms().get_rotated_anticlockwise(getTransform()));
}

void PieceModel::rotateRight()
{
    setTransform(m_bd.get_transforms().get_rotated_clockwise(getTransform()));
}

void PieceModel::setDefaultState()
{
    if (m_state.isEmpty())
        return;
    m_state.clear();
    emit stateChanged();
}

void PieceModel::setGameCoord(QPointF gameCoord)
{
    if (m_gameCoord == gameCoord)
        return;
    m_gameCoord = gameCoord;
    emit gameCoordChanged();
}

void PieceModel::setIsLastMove(bool isLastMove)
{
    if (m_isLastMove == isLastMove)
        return;
    m_isLastMove = isLastMove;
    emit isLastMoveChanged();
}

void PieceModel::setIsPlayed(bool isPlayed)
{
    if (m_isPlayed == isPlayed)
        return;
    m_isPlayed = isPlayed;
    emit isPlayedChanged();
}

void PieceModel::setMoveLabel(const QString& moveLabel)
{
    if (m_moveLabel == moveLabel)
        return;
    m_moveLabel = moveLabel;
    emit moveLabelChanged();
}

void PieceModel::setTransform(const Transform* transform)
{
    QString state;
    // libboardgame_base uses a different convention for the order of flipping
    // and rotation, so the names of the states and transform classes differ
    // for flipped states.
    auto pieceSet = m_bd.get_piece_set();
    if (pieceSet == PieceSet::trigon)
    {
        if (dynamic_cast<const TransfTrigonRot60*>(transform))
            state = QStringLiteral("rot60");
        else if (dynamic_cast<const TransfTrigonRot120*>(transform))
            state = QStringLiteral("rot120");
        else if (dynamic_cast<const TransfTrigonRot180*>(transform))
            state = QStringLiteral("rot180");
        else if (dynamic_cast<const TransfTrigonRot240*>(transform))
            state = QStringLiteral("rot240");
        else if (dynamic_cast<const TransfTrigonRot300*>(transform))
            state = QStringLiteral("rot300");
        else if (dynamic_cast<const TransfTrigonReflRot180*>(transform))
            state = QStringLiteral("flip");
        else if (dynamic_cast<const TransfTrigonReflRot120*>(transform))
            state = QStringLiteral("rot60Flip");
        else if (dynamic_cast<const TransfTrigonReflRot60*>(transform))
            state = QStringLiteral("rot120Flip");
        else if (dynamic_cast<const TransfTrigonRefl*>(transform))
            state = QStringLiteral("rot180Flip");
        else if (dynamic_cast<const TransfTrigonReflRot300*>(transform))
            state = QStringLiteral("rot240Flip");
        else if (dynamic_cast<const TransfTrigonReflRot240*>(transform))
            state = QStringLiteral("rot300Flip");
    }
    else if (pieceSet == PieceSet::gembloq)
    {
        if (dynamic_cast<const TransfGembloQRot90*>(transform))
            state = QStringLiteral("rot90");
        else if (dynamic_cast<const TransfGembloQRot180*>(transform))
            state = QStringLiteral("rot180");
        else if (dynamic_cast<const TransfGembloQRot270*>(transform))
            state = QStringLiteral("rot270");
        else if (dynamic_cast<const TransfGembloQRot180Refl*>(transform))
            state = QStringLiteral("flip");
        else if (dynamic_cast<const TransfGembloQRot90Refl*>(transform))
            state = QStringLiteral("rot90Flip");
        else if (dynamic_cast<const TransfGembloQRefl*>(transform))
            state = QStringLiteral("rot180Flip");
        else if (dynamic_cast<const TransfGembloQRot270Refl*>(transform))
            state = QStringLiteral("rot270Flip");
    }
    else
    {
        if (dynamic_cast<const TransfRectRot90*>(transform))
            state = QStringLiteral("rot90");
        else if (dynamic_cast<const TransfRectRot180*>(transform))
            state = QStringLiteral("rot180");
        else if (dynamic_cast<const TransfRectRot270*>(transform))
            state = QStringLiteral("rot270");
        else if (dynamic_cast<const TransfRectRot180Refl*>(transform))
            state = QStringLiteral("flip");
        else if (dynamic_cast<const TransfRectRot90Refl*>(transform))
            state = QStringLiteral("rot90Flip");
        else if (dynamic_cast<const TransfRectRefl*>(transform))
            state = QStringLiteral("rot180Flip");
        else if (dynamic_cast<const TransfRectRot270Refl*>(transform))
            state = QStringLiteral("rot270Flip");
    }
    if (m_state == state)
        return;
    m_state = state;
    emit stateChanged();
}

//-----------------------------------------------------------------------------
