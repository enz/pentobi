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

using namespace Qt::StringLiterals;
using libboardgame_base::ArrayList;
using libboardgame_base::CoordPoint;
using libboardgame_base::TransfIdentity;
using libboardgame_base::TransfRectRot90;
using libboardgame_base::TransfRectRot180;
using libboardgame_base::TransfRectRot270;
using libboardgame_base::TransfRectRefl;
using libboardgame_base::TransfRectRot90Refl;
using libboardgame_base::TransfRectRot180Refl;
using libboardgame_base::TransfRectRot270Refl;
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
    auto geoType = bd.get_geometry_type();
    bool isCallisto = (geoType == GeometryType::callisto);
    bool isGembloQ = (geoType == GeometryType::gembloq);
    bool isNexos = (geoType == GeometryType::nexos);
    bool isTrigon = (geoType == GeometryType::trigon);
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
                candidates.include({p.x - 1, p. y});
                candidates.include({p.x + 1, p. y});
            }
            else if (pointType == 2)
            {
                candidates.include({p.x, p. y - 1});
                candidates.include({p.x, p. y + 1});
            }
        }
        m_junctions.reserve(candidates.size());
        m_junctionType.reserve(candidates.size());
        for (auto& p : candidates)
        {
            bool hasLeft = points.contains({p.x - 1, p. y});
            bool hasRight = points.contains({p.x + 1, p. y});
            bool hasUp = points.contains({p.x, p. y - 1});
            bool hasDown = points.contains({p.x, p. y + 1});
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
    else if (isCallisto)
        for (auto& p : points)
        {
            bool hasRight = points.contains({p.x + 1, p. y});
            bool hasDown = points.contains({p.x, p.y + 1});
            bool hasRightDown = points.contains({p.x + 1, p.y + 1});
            int junctionType = 0;
            if (hasRight)
                junctionType |= 1;
            if (hasDown)
                junctionType |= 2;
            if (hasRightDown)
                junctionType |= 4;
            m_junctionType.append(junctionType);
        }
    m_center = findCenter(bd, points, true);
    auto& labelPos = info.get_label_pos();
    qreal labelX = labelPos.x - m_center.x();
    qreal labelY = labelPos.y - m_center.y();
    if (isGembloQ)
    {
        if (labelPos.x % 2 != 0)
            labelX += 1;
        if (labelPos.y % 2 != 0)
            labelY += 1;
    }
    else if (isTrigon)
    {
        labelX += 0.5;
        if ((labelPos.x % 2 == 0) != (labelPos.y % 2 == 0))
            // Downward
            labelY += 1. / 3;
        else
            labelY += 2. / 3;
    }
    else
    {
        labelX += 0.5;
        labelY += 0.5;
    }
    m_labelPos = QPointF(labelX, labelY);
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
        if (state == "60"_L1)
            return transforms.find<TransfTrigonRot60>();
        if (state == "120"_L1)
            return transforms.find<TransfTrigonRot120>();
        if (state == "180"_L1)
            return transforms.find<TransfTrigonRot180>();
        if (state == "240"_L1)
            return transforms.find<TransfTrigonRot240>();
        if (state == "300"_L1)
            return transforms.find<TransfTrigonRot300>();
        if (state == "flip"_L1)
            return transforms.find<TransfTrigonReflRot180>();
        if (state == "60flip"_L1)
            return transforms.find<TransfTrigonReflRot120>();
        if (state == "120flip"_L1)
            return transforms.find<TransfTrigonReflRot60>();
        if (state == "180flip"_L1)
            return transforms.find<TransfTrigonRefl>();
        if (state == "240flip"_L1)
            return transforms.find<TransfTrigonReflRot300>();
        if (state == "300flip"_L1)
            return transforms.find<TransfTrigonReflRot240>();
        return transforms.find<TransfTrigonIdentity>();
    }
    if (pieceSet == PieceSet::gembloq)
    {
        if (state == "90"_L1)
            return transforms.find<TransfGembloQRot90>();
        if (state == "180"_L1)
            return transforms.find<TransfGembloQRot180>();
        if (state == "270"_L1)
            return transforms.find<TransfGembloQRot270>();
        if (state == "flip"_L1)
            return transforms.find<TransfGembloQRot180Refl>();
        if (state == "90flip"_L1)
            return transforms.find<TransfGembloQRot90Refl>();
        if (state == "180flip"_L1)
            return transforms.find<TransfGembloQRefl>();
        if (state == "270flip"_L1)
            return transforms.find<TransfGembloQRot270Refl>();
        return transforms.find<TransfGembloQIdentity>();
    }
    if (state == "90"_L1)
        return transforms.find<TransfRectRot90>();
    if (state == "180"_L1)
        return transforms.find<TransfRectRot180>();
    if (state == "270"_L1)
        return transforms.find<TransfRectRot270>();
    if (state == "flip"_L1)
        return transforms.find<TransfRectRot180Refl>();
    if (state == "90flip"_L1)
        return transforms.find<TransfRectRot90Refl>();
    if (state == "180flip"_L1)
        return transforms.find<TransfRectRefl>();
    if (state == "270flip"_L1)
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
        qreal centerX;
        qreal centerY;
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
            state = "60"_L1;
        else if (dynamic_cast<const TransfTrigonRot120*>(transform))
            state = "120"_L1;
        else if (dynamic_cast<const TransfTrigonRot180*>(transform))
            state = "180"_L1;
        else if (dynamic_cast<const TransfTrigonRot240*>(transform))
            state = "240"_L1;
        else if (dynamic_cast<const TransfTrigonRot300*>(transform))
            state = "300"_L1;
        else if (dynamic_cast<const TransfTrigonReflRot180*>(transform))
            state = "flip"_L1;
        else if (dynamic_cast<const TransfTrigonReflRot120*>(transform))
            state = "60flip"_L1;
        else if (dynamic_cast<const TransfTrigonReflRot60*>(transform))
            state = "120flip"_L1;
        else if (dynamic_cast<const TransfTrigonRefl*>(transform))
            state = "180flip"_L1;
        else if (dynamic_cast<const TransfTrigonReflRot300*>(transform))
            state = "240flip"_L1;
        else if (dynamic_cast<const TransfTrigonReflRot240*>(transform))
            state = "300flip"_L1;
    }
    else if (pieceSet == PieceSet::gembloq)
    {
        if (dynamic_cast<const TransfGembloQRot90*>(transform))
            state = "90"_L1;
        else if (dynamic_cast<const TransfGembloQRot180*>(transform))
            state = "180"_L1;
        else if (dynamic_cast<const TransfGembloQRot270*>(transform))
            state = "270"_L1;
        else if (dynamic_cast<const TransfGembloQRot180Refl*>(transform))
            state = "flip"_L1;
        else if (dynamic_cast<const TransfGembloQRot90Refl*>(transform))
            state = "90flip"_L1;
        else if (dynamic_cast<const TransfGembloQRefl*>(transform))
            state = "180flip"_L1;
        else if (dynamic_cast<const TransfGembloQRot270Refl*>(transform))
            state = "270flip"_L1;
    }
    else
    {
        if (dynamic_cast<const TransfRectRot90*>(transform))
            state = "90"_L1;
        else if (dynamic_cast<const TransfRectRot180*>(transform))
            state = "180"_L1;
        else if (dynamic_cast<const TransfRectRot270*>(transform))
            state = "270"_L1;
        else if (dynamic_cast<const TransfRectRot180Refl*>(transform))
            state = "flip"_L1;
        else if (dynamic_cast<const TransfRectRot90Refl*>(transform))
            state = "90flip"_L1;
        else if (dynamic_cast<const TransfRectRefl*>(transform))
            state = "180flip"_L1;
        else if (dynamic_cast<const TransfRectRot270Refl*>(transform))
            state = "270flip"_L1;
    }
    if (m_state == state)
        return;
    m_state = state;
    emit stateChanged();
}

//-----------------------------------------------------------------------------
