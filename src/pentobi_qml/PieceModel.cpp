//-----------------------------------------------------------------------------
/** @file pentobi_qml/PieceModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "PieceModel.h"

#include <limits>
#include <QDebug>

using namespace std;
using libboardgame_base::TransfIdentity;
using libboardgame_base::TransfRectRot90;
using libboardgame_base::TransfRectRot180;
using libboardgame_base::TransfRectRot270;
using libboardgame_base::TransfRectRefl;
using libboardgame_base::TransfRectRot90Refl;
using libboardgame_base::TransfRectRot180Refl;
using libboardgame_base::TransfRectRot270Refl;
using libboardgame_base::TransfTrigonIdentity;
using libboardgame_base::TransfTrigonRefl;
using libboardgame_base::TransfTrigonReflRot60;
using libboardgame_base::TransfTrigonReflRot120;
using libboardgame_base::TransfTrigonReflRot180;
using libboardgame_base::TransfTrigonReflRot240;
using libboardgame_base::TransfTrigonReflRot300;
using libboardgame_base::TransfTrigonRot60;
using libboardgame_base::TransfTrigonRot120;
using libboardgame_base::TransfTrigonRot180;
using libboardgame_base::TransfTrigonRot240;
using libboardgame_base::TransfTrigonRot300;
using libpentobi_base::BoardType;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

PieceModel::PieceModel(QObject* parent, const Board& bd,
                       libpentobi_base::Piece piece, Color c)
    : QObject(parent),
      m_bd(bd),
      m_color(c),
      m_piece(piece),
      m_isPlayed(false)
{
    auto& info = bd.get_piece_info(piece);
    for (auto& p : info.get_points())
        m_elements.append(QVariant(QPointF(p.x, p.y)));
    m_center = findCenter(bd, info.get_points(), false);
    updateTransformFromState();
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
    setTransform(m_bd.get_transforms().get_mirrored_vertically(m_transform));
}

void PieceModel::flipAcrossY()
{
    setTransform(m_bd.get_transforms().get_mirrored_horizontally(m_transform));
}

QPointF PieceModel::gameCoord() const
{
    return m_gameCoord;
}

QPointF PieceModel::findCenter(const Board& bd, const PiecePoints& points,
                               bool isOriginDownward)
{
    auto boardType = bd.get_board_type();
    auto& geo = bd.get_geometry();
    qreal sumX = 0;
    qreal sumY = 0;
    for (auto& p : points)
    {
        qreal centerX = p.x + 0.5;
        qreal centerY;
        if (boardType == BoardType::trigon || boardType == BoardType::trigon_3)
        {
            auto pointType = geo.get_point_type(p.x, p.y);
            bool isDownward = (pointType == (isOriginDownward ? 0 : 1));
            if (isDownward)
                centerY = p.y + 1.f / 3;
            else
                centerY = p.y + 2.f / 3;
        }
        else
            centerY = p.y + 0.5;
        sumX += centerX;
        sumY += centerY;
    }
    return QPointF(sumX / points.size(), sumY / points.size());
}

bool PieceModel::isPlayed() const
{
    return m_isPlayed;
}

void PieceModel::rotateLeft()
{
    setTransform(m_bd.get_transforms().get_rotated_anticlockwise(m_transform));
}

void PieceModel::rotateRight()
{
    setTransform(m_bd.get_transforms().get_rotated_clockwise(m_transform));
}

void PieceModel::setGameCoord(QPointF gameCoord)
{
    if (m_gameCoord == gameCoord)
        return;
    m_gameCoord = gameCoord;
    emit gameCoordChanged(gameCoord);
}

void PieceModel::setState(QString state)
{
    if (m_state == state)
        return;
    m_state = state;
    updateTransformFromState();
    emit stateChanged(state);
}

void PieceModel::setTransform(const Transform* transform)
{
    QString state;
    // libboardgame_base uses a different convention for the order of flipping
    // and rotation, so the names of the states and transform classes differ
    // for flipped states.
    if (dynamic_cast<const TransfIdentity*>(transform)
            || dynamic_cast<const TransfTrigonIdentity*>(transform))
        state = "";
    else if (dynamic_cast<const TransfTrigonRot60*>(transform))
        state = "rot60";
    else if (dynamic_cast<const TransfRectRot90*>(transform))
        state = "rot90";
    else if (dynamic_cast<const TransfTrigonRot120*>(transform))
        state = "rot120";
    else if (dynamic_cast<const TransfRectRot180*>(transform)
             || dynamic_cast<const TransfTrigonRot180*>(transform))
        state = "rot180";
    else if (dynamic_cast<const TransfTrigonRot240*>(transform))
        state = "rot240";
    else if (dynamic_cast<const TransfRectRot270*>(transform))
        state = "rot270";
    else if (dynamic_cast<const TransfTrigonRot300*>(transform))
        state = "rot300";
    else if (dynamic_cast<const TransfRectRot180Refl*>(transform)
             || dynamic_cast<const TransfTrigonReflRot180*>(transform))
        state = "flip";
    else if (dynamic_cast<const TransfTrigonReflRot120*>(transform))
        state = "rot60Flip";
    else if (dynamic_cast<const TransfRectRot90Refl*>(transform))
        state = "rot90Flip";
    else if (dynamic_cast<const TransfTrigonReflRot60*>(transform))
        state = "rot120Flip";
    else if (dynamic_cast<const TransfRectRefl*>(transform)
             || dynamic_cast<const TransfTrigonRefl*>(transform))
        state = "rot180Flip";
    else if (dynamic_cast<const TransfTrigonReflRot300*>(transform))
        state = "rot240Flip";
    else if (dynamic_cast<const TransfRectRot270Refl*>(transform))
        state = "rot270Flip";
    else if (dynamic_cast<const TransfTrigonReflRot240*>(transform))
        state = "rot300Flip";
    else
    {
        qWarning() << "Invalid Transform " << typeid(*transform).name();
        return;
    }
    setState(state);
}

void PieceModel::setIsPlayed(bool isPlayed)
{
    if (m_isPlayed == isPlayed)
        return;
    m_isPlayed = isPlayed;
    emit isPlayedChanged(isPlayed);
}

QString PieceModel::state() const
{
    return m_state;
}

void PieceModel::updateTransformFromState()
{
    auto variant = m_bd.get_variant();
    bool isTrigon = (variant == Variant::trigon || variant == Variant::trigon_2
                     || variant == Variant::trigon_3);
    auto& transforms = m_bd.get_transforms();
    // See comment in etTransform() about the mapping between states and
    // transform classes.
    if (m_state == "")
        m_transform = isTrigon ? transforms.find<TransfTrigonIdentity>()
                               : transforms.find<TransfIdentity>();
    else if (m_state == "rot60")
        m_transform = transforms.find<TransfTrigonRot60>();
    else if (m_state == "rot90")
        m_transform = transforms.find<TransfRectRot90>();
    else if (m_state == "rot120")
        m_transform = transforms.find<TransfTrigonRot120>();
    else if (m_state == "rot180")
        m_transform = isTrigon ? transforms.find<TransfTrigonRot180>()
                               : transforms.find<TransfRectRot180>();
    else if (m_state == "rot240")
        m_transform = transforms.find<TransfTrigonRot240>();
    else if (m_state == "rot270")
        m_transform = transforms.find<TransfRectRot270>();
    else if (m_state == "rot300")
        m_transform = transforms.find<TransfTrigonRot300>();
    else if (m_state == "flip")
        m_transform = isTrigon ? transforms.find<TransfTrigonReflRot180>()
                               : transforms.find<TransfRectRot180Refl>();
    else if (m_state == "rot60Flip")
        m_transform = transforms.find<TransfTrigonReflRot120>();
    else if (m_state == "rot90Flip")
        m_transform = transforms.find<TransfRectRot90Refl>();
    else if (m_state == "rot120Flip")
        m_transform = transforms.find<TransfTrigonReflRot60>();
    else if (m_state == "rot180Flip")
        m_transform = isTrigon ? transforms.find<TransfTrigonRefl>()
                               : transforms.find<TransfRectRefl>();
    else if (m_state == "rot240Flip")
        m_transform = transforms.find<TransfTrigonReflRot300>();
    else if (m_state == "rot270Flip")
        m_transform = transforms.find<TransfRectRot270Refl>();
    else if (m_state == "rot300Flip")
        m_transform = transforms.find<TransfTrigonReflRot240>();
    else
    {
        qWarning() << "PieceModel: unknown state " << m_state;
        m_transform = transforms.find<TransfIdentity>();
    }
}

//-----------------------------------------------------------------------------
