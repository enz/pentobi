//-----------------------------------------------------------------------------
/** @file pentobi_qml/PieceModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "PieceModel.h"

#include <QDebug>
#include "libboardgame_base/RectTransform.h"
#include "libpentobi_base/TrigonTransform.h"

using namespace std;
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
using libpentobi_base::PieceInfo;
using libpentobi_base::PieceSet;
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
using libpentobi_base::Variant;

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
    m_elements.reserve(points.size());
    for (auto& p : points)
    {
        if (isNexos && geo.get_point_type(p) == 0)
            continue;
        m_elements.append(QPointF(p.x, p.y));
    }
    if (isNexos)
    {
        ArrayList<CoordPoint, 2 * PieceInfo::max_scored_size> candidates;
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
            bool hasDown = points.contains(CoordPoint(p.x, p. y + 1));
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
    bool isOriginDownward = (m_bd.get_board_type() == BoardType::trigon_3);
    m_center = findCenter(bd, points, isOriginDownward);
    m_labelPos = QPointF(info.get_label_pos().x, info.get_label_pos().y);
}

QPointF PieceModel::center() const
{
    return m_center;
}

int PieceModel::color()
{
    return m_color.to_int();
}

QVariantList PieceModel::elements()
{
    return m_elements;
}

void PieceModel::flipAcrossX()
{
    setTransform(m_bd.get_transforms().get_mirrored_vertically(getTransform()));
}

void PieceModel::flipAcrossY()
{
    setTransform(m_bd.get_transforms().get_mirrored_horizontally(getTransform()));
}

QPointF PieceModel::gameCoord() const
{
    return m_gameCoord;
}

const Transform* PieceModel::getTransform(const QString& state) const
{
    auto variant = m_bd.get_variant();
    bool isTrigon = (variant == Variant::trigon || variant == Variant::trigon_2
                     || variant == Variant::trigon_3);
    auto& transforms = m_bd.get_transforms();
    // See comment in getTransform() about the mapping between states and
    // transform classes.
    if (state.isEmpty())
        return isTrigon ? transforms.find<TransfTrigonIdentity>()
                        : transforms.find<TransfIdentity>();
    if (state == QLatin1String("rot60"))
        return transforms.find<TransfTrigonRot60>();
    if (state == QLatin1String("rot90"))
        return transforms.find<TransfRectRot90>();
    if (state == QLatin1String("rot120"))
        return transforms.find<TransfTrigonRot120>();
    if (state == QLatin1String("rot180"))
        return isTrigon ? transforms.find<TransfTrigonRot180>()
                        : transforms.find<TransfRectRot180>();
    if (state == QLatin1String("rot240"))
        return transforms.find<TransfTrigonRot240>();
    if (state == QLatin1String("rot270"))
        return transforms.find<TransfRectRot270>();
    if (state == QLatin1String("rot300"))
        return transforms.find<TransfTrigonRot300>();
    if (state == QLatin1String("flip"))
        return isTrigon ? transforms.find<TransfTrigonReflRot180>()
                        : transforms.find<TransfRectRot180Refl>();
    if (state == QLatin1String("rot60Flip"))
        return transforms.find<TransfTrigonReflRot120>();
    if (state == QLatin1String("rot90Flip"))
        return transforms.find<TransfRectRot90Refl>();
    if (state == QLatin1String("rot120Flip"))
        return transforms.find<TransfTrigonReflRot60>();
    if (state == QLatin1String("rot180Flip"))
        return isTrigon ? transforms.find<TransfTrigonRefl>()
                        : transforms.find<TransfRectRefl>();
    if (state == QLatin1String("rot240Flip"))
        return transforms.find<TransfTrigonReflRot300>();
    if (state == QLatin1String("rot270Flip"))
        return transforms.find<TransfRectRot270Refl>();
    if (state == QLatin1String("rot300Flip"))
        return transforms.find<TransfTrigonReflRot240>();
    qWarning() << "PieceModel: unknown state " << m_state;
    return transforms.find<TransfIdentity>();
}

QPointF PieceModel::findCenter(const Board& bd, const PiecePoints& points,
                               bool isOriginDownward)
{
    auto pieceSet = bd.get_piece_set();
    bool isTrigon = (pieceSet == PieceSet::trigon);
    bool isNexos = (pieceSet == PieceSet::nexos);
    auto& geo = bd.get_geometry();
    qreal sumX = 0;
    qreal sumY = 0;
    qreal n = 0;
    for (auto& p : points)
    {
        if (isNexos && geo.get_point_type(p.x, p.y) == 0)
            continue;
        ++n;
        qreal centerX = p.x + 0.5;
        qreal centerY;
        if (isTrigon)
        {
            auto pointType = geo.get_point_type(p.x, p.y);
            bool isDownward = (pointType == (isOriginDownward ? 0 : 1));
            if (isDownward)
                centerY = static_cast<qreal>(p.y) + 1.f / 3;
            else
                centerY = static_cast<qreal>(p.y) + 2.f / 3;
        }
        else
            centerY = p.y + 0.5;
        sumX += centerX;
        sumY += centerY;
    }
    return QPointF(sumX / n, sumY / n);
}

bool PieceModel::isLastMove() const
{
    return m_isLastMove;
}

bool PieceModel::isPlayed() const
{
    return m_isPlayed;
}

QVariantList PieceModel::junctions()
{
    return m_junctions;
}

QVariantList PieceModel::junctionType()
{
    return m_junctionType;
}

QPointF PieceModel::labelPos() const
{
    return m_labelPos;
}

void PieceModel::rotateLeft()
{
    setTransform(m_bd.get_transforms().get_rotated_anticlockwise(getTransform()));
}

void PieceModel::rotateRight()
{
    setTransform(m_bd.get_transforms().get_rotated_clockwise(getTransform()));
}

void PieceModel::setGameCoord(QPointF gameCoord)
{
    if (m_gameCoord == gameCoord)
        return;
    m_gameCoord = gameCoord;
    emit gameCoordChanged(gameCoord);
}

void PieceModel::setIsLastMove(bool isLastMove)
{
    if (m_isLastMove == isLastMove)
        return;
    m_isLastMove = isLastMove;
    emit isLastMoveChanged(isLastMove);
}

void PieceModel::setIsPlayed(bool isPlayed)
{
    if (m_isPlayed == isPlayed)
        return;
    m_isPlayed = isPlayed;
    emit isPlayedChanged(isPlayed);
}

void PieceModel::setDefaultState()
{
    if (m_state.isEmpty())
        return;
    m_state.clear();
    emit stateChanged(m_state);
}

void PieceModel::setTransform(const Transform* transform)
{
    QLatin1String state;
    // libboardgame_base uses a different convention for the order of flipping
    // and rotation, so the names of the states and transform classes differ
    // for flipped states.
    if (dynamic_cast<const TransfIdentity*>(transform)
            || dynamic_cast<const TransfTrigonIdentity*>(transform))
        ;
    else if (dynamic_cast<const TransfTrigonRot60*>(transform))
        state = QLatin1String("rot60");
    else if (dynamic_cast<const TransfRectRot90*>(transform))
        state = QLatin1String("rot90");
    else if (dynamic_cast<const TransfTrigonRot120*>(transform))
        state = QLatin1String("rot120");
    else if (dynamic_cast<const TransfRectRot180*>(transform)
             || dynamic_cast<const TransfTrigonRot180*>(transform))
        state = QLatin1String("rot180");
    else if (dynamic_cast<const TransfTrigonRot240*>(transform))
        state = QLatin1String("rot240");
    else if (dynamic_cast<const TransfRectRot270*>(transform))
        state = QLatin1String("rot270");
    else if (dynamic_cast<const TransfTrigonRot300*>(transform))
        state = QLatin1String("rot300");
    else if (dynamic_cast<const TransfRectRot180Refl*>(transform)
             || dynamic_cast<const TransfTrigonReflRot180*>(transform))
        state = QLatin1String("flip");
    else if (dynamic_cast<const TransfTrigonReflRot120*>(transform))
        state = QLatin1String("rot60Flip");
    else if (dynamic_cast<const TransfRectRot90Refl*>(transform))
        state = QLatin1String("rot90Flip");
    else if (dynamic_cast<const TransfTrigonReflRot60*>(transform))
        state = QLatin1String("rot120Flip");
    else if (dynamic_cast<const TransfRectRefl*>(transform)
             || dynamic_cast<const TransfTrigonRefl*>(transform))
        state = QLatin1String("rot180Flip");
    else if (dynamic_cast<const TransfTrigonReflRot300*>(transform))
        state = QLatin1String("rot240Flip");
    else if (dynamic_cast<const TransfRectRot270Refl*>(transform))
        state = QLatin1String("rot270Flip");
    else if (dynamic_cast<const TransfTrigonReflRot240*>(transform))
        state = QLatin1String("rot300Flip");
    else
    {
        qWarning() << "Invalid Transform " << typeid(*transform).name();
        return;
    }
    if (m_state == state)
        return;
    m_state = state;
    emit stateChanged(m_state);
}

QString PieceModel::state() const
{
    return m_state;
}

//-----------------------------------------------------------------------------
