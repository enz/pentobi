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
using libboardgame_base::TransfIdentity;
using libboardgame_base::TransfRectRot90;
using libboardgame_base::TransfRectRot180;
using libboardgame_base::TransfRectRot270;
using libboardgame_base::TransfRectRefl;
using libboardgame_base::TransfRectRot90Refl;
using libboardgame_base::TransfRectRot180Refl;
using libboardgame_base::TransfRectRot270Refl;
using libpentobi_base::BoardType;
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

PieceModel::PieceModel(QObject* parent, const Board& bd,
                       libpentobi_base::Piece piece, Color c)
    : QObject(parent),
      m_bd(bd),
      m_color(c),
      m_piece(piece)
{
    auto& info = bd.get_piece_info(piece);
    for (auto& p : info.get_points())
        m_elements.append(QVariant(QPointF(p.x, p.y)));
    bool isOriginDownward = (m_bd.get_board_type() == BoardType::trigon_3);
    m_center = findCenter(bd, info.get_points(), isOriginDownward);
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

const Transform* PieceModel::getTransform(QString state) const
{
    auto variant = m_bd.get_variant();
    bool isTrigon = (variant == Variant::trigon || variant == Variant::trigon_2
                     || variant == Variant::trigon_3);
    auto& transforms = m_bd.get_transforms();
    // See comment in getTransform() about the mapping between states and
    // transform classes.
    if (state == "")
        return isTrigon ? transforms.find<TransfTrigonIdentity>()
                        : transforms.find<TransfIdentity>();
    if (state == "rot60")
        return transforms.find<TransfTrigonRot60>();
    if (state == "rot90")
        return transforms.find<TransfRectRot90>();
    if (state == "rot120")
        return transforms.find<TransfTrigonRot120>();
    if (state == "rot180")
        return isTrigon ? transforms.find<TransfTrigonRot180>()
                        : transforms.find<TransfRectRot180>();
    if (state == "rot240")
        return transforms.find<TransfTrigonRot240>();
    if (state == "rot270")
        return transforms.find<TransfRectRot270>();
    if (state == "rot300")
        return transforms.find<TransfTrigonRot300>();
    if (state == "flip")
        return isTrigon ? transforms.find<TransfTrigonReflRot180>()
                        : transforms.find<TransfRectRot180Refl>();
    if (state == "rot60Flip")
        return transforms.find<TransfTrigonReflRot120>();
    if (state == "rot90Flip")
        return transforms.find<TransfRectRot90Refl>();
    if (state == "rot120Flip")
        return transforms.find<TransfTrigonReflRot60>();
    if (state == "rot180Flip")
        return isTrigon ? transforms.find<TransfTrigonRefl>()
                        : transforms.find<TransfRectRefl>();
    if (state == "rot240Flip")
        return transforms.find<TransfTrigonReflRot300>();
    if (state == "rot270Flip")
        return transforms.find<TransfRectRot270Refl>();
    if (state == "rot300Flip")
        return transforms.find<TransfTrigonReflRot240>();
    qWarning() << "PieceModel: unknown state " << m_state;
    return transforms.find<TransfIdentity>();
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

void PieceModel::setState(QString state)
{
    if (m_state == state)
        return;
    m_state = state;
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

//-----------------------------------------------------------------------------
