//-----------------------------------------------------------------------------
/** @file pentobi_qml/BoardModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "BoardModel.h"

#include <limits>
#include <QDebug>
#include <QSettings>
#include "libboardgame_util/Log.h"

using namespace std;
using libboardgame_util::log;
using libboardgame_util::Exception;
using libpentobi_base::to_string_id;
using libpentobi_base::BoardType;
using libpentobi_base::Color;
using libpentobi_base::ColorIterator;
using libpentobi_base::ColorMap;
using libpentobi_base::ColorMove;
using libpentobi_base::CoordPoint;
using libpentobi_base::MovePoints;
using libpentobi_base::Piece;
using libpentobi_base::PiecePoints;
using libpentobi_base::Point;

//-----------------------------------------------------------------------------

namespace {

int getNuPiecesLeft(const Board& bd, Color c)
{
    unsigned n = 0;
    for (auto piece : bd.get_pieces_left(c))
        n += bd.get_nu_left_piece(c, piece);
    return static_cast<int>(n);
}

} //namespace

//-----------------------------------------------------------------------------

BoardModel::BoardModel(QObject* parent)
    : QObject(parent),
      m_bd(getInitialGameVariant()),
      m_gameVariant(to_string_id(m_bd.get_variant())),
      m_nuColors(m_bd.get_nu_colors()),
      m_toPlay(0),
      m_points0(0),
      m_points1(0),
      m_points2(0),
      m_points3(0),
      m_nuPiecesLeft0(0),
      m_nuPiecesLeft1(0),
      m_nuPiecesLeft2(0),
      m_nuPiecesLeft3(0),
      m_hasMoves0(true),
      m_hasMoves1(true),
      m_hasMoves2(true),
      m_hasMoves3(true),
      m_isGameOver(false),
      m_canUndo(false)
{
    createPieceModels();
    updateProperties();
}

void BoardModel::autoSave()
{
    QString s;
    if (m_bd.get_nu_moves() > 0)
    {
        s  = to_string_id(m_bd.get_variant());
        for (unsigned i = 0; i < m_bd.get_nu_moves(); ++i)
        {
            ColorMove mv = m_bd.get_move(i);
            s.append(QString(";%1;%2")
                     .arg(mv.color.to_int())
                     .arg(m_bd.to_string(mv.move, false).c_str()));
        }
    }
    QSettings settings;
    settings.setValue("autosave", s);
}

void BoardModel::createPieceModels()
{
    m_pieceModels0.clear();
    m_pieceModels1.clear();
    m_pieceModels2.clear();
    m_pieceModels3.clear();
    createPieceModels(Color(0), m_pieceModels0);
    createPieceModels(Color(1), m_pieceModels1);
    if (m_nuColors > 2)
        createPieceModels(Color(2), m_pieceModels2);
    if (m_nuColors > 3)
        createPieceModels(Color(3), m_pieceModels3);
}

void BoardModel::createPieceModels(Color c, QList<PieceModel*>& pieceModels)
{
    pieceModels.clear();
    for (unsigned i = 0; i < m_bd.get_nu_uniq_pieces(); ++i)
    {
        Piece piece(i);
        for (unsigned j = 0; j < m_bd.get_nu_piece_instances(); ++j)
            pieceModels.append(new PieceModel(this, m_bd, piece, c));
    }
}

bool BoardModel::findMove(const PieceModel& piece, QPointF coord,
                          Move& mv) const
{
    auto& info = m_bd.get_piece_info(piece.getPiece());
    auto transform = piece.getTransform();
    PiecePoints piecePoints = info.get_points();
    transform->transform(piecePoints.begin(), piecePoints.end());
    auto boardType = m_bd.get_board_type();
    auto newPointType = transform->get_new_point_type();
    bool pointTypeChanged =
            ((boardType == BoardType::trigon && newPointType == 0)
             || (boardType == BoardType::trigon_3 && newPointType == 1));
    QPointF center(PieceModel::findCenter(m_bd, piecePoints, false));
    auto& geo = m_bd.get_geometry();
    MovePoints points;
    for (auto& p : piecePoints)
    {
        qreal x = static_cast<int>(round(p.x - center.x() + coord.x()));
        qreal y = static_cast<int>(round(p.y - center.y() + coord.y()));
        if (! geo.is_onboard(CoordPoint(x, y)))
            return false;
        auto pointType = geo.get_point_type(p.x, p.y);
        auto boardPointType = geo.get_point_type(x, y);
        if (! pointTypeChanged && pointType != boardPointType)
            return false;
        if (pointTypeChanged && pointType == boardPointType)
            return false;
        points.push_back(Point(static_cast<unsigned>(x),
                               static_cast<unsigned>(y),
                               geo.get_width()));
    }
    return m_bd.find_move(points, mv);
}

Variant BoardModel::getInitialGameVariant()
{
    QSettings settings;
    auto variantString = settings.value("variant", "").toString();
    Variant gameVariant;
    if (! parse_variant_id(variantString.toLocal8Bit().constData(),
                           gameVariant))
        // Use Duo on the first invocation because it is a better
        // default for mobile devices than Classic.
        gameVariant = Variant::duo;
    return gameVariant;
}

QList<PieceModel*>& BoardModel::getPieceModels(Color c)
{
    if (c == Color(0))
        return m_pieceModels0;
    else if (c == Color(1))
        return m_pieceModels1;
    else if (c == Color(2))
        return m_pieceModels2;
    else
        return  m_pieceModels3;
}

void BoardModel::initGameVariant(QString gameVariant)
{
    if (m_gameVariant == gameVariant)
        return;
    if (gameVariant == "classic")
        m_bd.init(Variant::classic);
    else if (gameVariant == "classic_2")
        m_bd.init(Variant::classic_2);
    else if (gameVariant == "duo")
        m_bd.init(Variant::duo);
    else if (gameVariant == "junior")
        m_bd.init(Variant::junior);
    else if (gameVariant == "trigon")
        m_bd.init(Variant::trigon);
    else if (gameVariant == "trigon_2")
        m_bd.init(Variant::trigon_2);
    else if (gameVariant == "trigon_3")
        m_bd.init(Variant::trigon_3);
    else
    {
        qWarning("BoardModel: invalid game variant");
        return;
    }
    int nuColors = m_bd.get_nu_colors();
    if (nuColors != m_nuColors)
    {
        m_nuColors = nuColors;
        emit nuColorsChanged(nuColors);
    }
    createPieceModels();
    m_gameVariant = gameVariant;
    emit gameVariantChanged(gameVariant);
    updateProperties();
    QSettings settings;
    settings.setValue("variant", gameVariant);
}

bool BoardModel::isLegalPos(PieceModel* pieceModel, QPointF coord) const
{
    Move mv;
    if (! findMove(*pieceModel, coord, mv))
        return false;
    Color c(pieceModel->color());
    bool result = m_bd.is_legal(c, mv);
    return result;
}

bool BoardModel::loadAutoSave()
{
    QSettings settings;
    QString s = settings.value("autosave", "").toString();
    if (s.isEmpty())
        return false;
    QStringList l = s.split(';');
    if (l[0] != to_string_id(m_bd.get_variant()))
    {
        qWarning("BoardModel: autosave has wrong game variant");
        return false;
    }
    if (l.length() == 1)
    {
        qWarning("BoardModel: autosave has no moves");
        return false;
    }
    m_bd.init();
    try
    {
        for (int i = 1; i < l.length(); i += 2)
        {
            unsigned colorInt = l[i].toUInt();
            if (colorInt >= m_bd.get_nu_colors())
                throw Exception("invalid color");
            Color c(colorInt);
            if (i + 1 >= l.length())
                throw Exception("color without move");
            Move mv = m_bd.from_string(l[i + 1].toLocal8Bit().constData());
            if (! m_bd.is_legal(c, mv))
                throw Exception("illegal move");
            m_bd.play(c, mv);
        }
    }
    catch (const Exception &e)
    {
        qWarning() << "BoardModel: autosave has illegal move: " << e.what();
    }
    updateProperties();
    return true;
}

void BoardModel::newGame()
{
    m_bd.init();
    updateProperties();
}


QQmlListProperty<PieceModel> BoardModel::pieceModels0()
{
    return QQmlListProperty<PieceModel>(this, m_pieceModels0);
}

QQmlListProperty<PieceModel> BoardModel::pieceModels1()
{
    return QQmlListProperty<PieceModel>(this, m_pieceModels1);
}

QQmlListProperty<PieceModel> BoardModel::pieceModels2()
{
    return QQmlListProperty<PieceModel>(this, m_pieceModels2);
}

QQmlListProperty<PieceModel> BoardModel::pieceModels3()
{
    return QQmlListProperty<PieceModel>(this, m_pieceModels3);
}

void BoardModel::play(PieceModel* pieceModel, QPointF coord)
{
    Color c(pieceModel->color());
    Move mv;
    if (! findMove(*pieceModel, coord, mv))
    {
        qWarning("BoardModel::play: illegal move");
        return;
    }
    m_bd.play(c, mv);
    updateProperties();
}

void BoardModel::playMove(int move)
{
    Color c = m_bd.get_effective_to_play();
    Move mv(move);
    m_bd.play(c, mv);
    updateProperties();
}

PieceModel* BoardModel::preparePiece(int color, int move)
{
    Move mv(move);
    Piece piece = m_bd.get_move_info(mv).get_piece();
    for (auto pieceModel : getPieceModels(Color(color)))
        if (pieceModel->getPiece() == piece && ! pieceModel->isPlayed())
        {
            preparePiece(pieceModel, mv);
            return pieceModel;
        }
    return nullptr;
}

void BoardModel::preparePiece(PieceModel* pieceModel, Move mv)
{
    auto& geo = m_bd.get_geometry();
    auto width = geo.get_width();
    auto moveInfo = m_bd.get_move_info(mv);
    PiecePoints movePoints;
    for (Point p : moveInfo)
        movePoints.push_back(CoordPoint(p.get_x(width), p.get_y(width)));
    auto& pieceInfo = m_bd.get_piece_info(moveInfo.get_piece());
    auto transform = pieceInfo.find_transform(geo, movePoints);
    auto oldTransform = pieceModel->getTransform();
    if (transform != pieceInfo.get_equivalent_transform(oldTransform))
        pieceModel->setTransform(transform);
    QPointF center = PieceModel::findCenter(m_bd, movePoints, false);
    pieceModel->setGameCoord(center);
}

void BoardModel::undo()
{
    if (m_bd.get_nu_moves() == 0)
        return;
    m_bd.undo();
    updateProperties();
}

void BoardModel::updateProperties()
{
    int points0 = m_bd.get_points(Color(0));
    if (m_points0 != points0)
    {
        m_points0 = points0;
        emit points0Changed(points0);
    }

    int points1 = m_bd.get_points(Color(1));
    if (m_points1 != points1)
    {
        m_points1 = points1;
        emit points1Changed(points1);
    }

    int nuPiecesLeft0 = getNuPiecesLeft(m_bd, Color(0));
    if (m_nuPiecesLeft0 != nuPiecesLeft0)
    {
        m_nuPiecesLeft0 = nuPiecesLeft0;
        emit nuPiecesLeft0Changed(nuPiecesLeft0);
    }

    int nuPiecesLeft1 = getNuPiecesLeft(m_bd, Color(1));
    if (m_nuPiecesLeft1 != nuPiecesLeft1)
    {
        m_nuPiecesLeft1 = nuPiecesLeft1;
        emit nuPiecesLeft1Changed(nuPiecesLeft1);
    }

    bool hasMoves0 = m_bd.has_moves(Color(0));
    if (m_hasMoves0 != hasMoves0)
    {
        m_hasMoves0 = hasMoves0;
        emit hasMoves0Changed(hasMoves0);
    }

    bool hasMoves1 = m_bd.has_moves(Color(1));
    if (m_hasMoves1 != hasMoves1)
    {
        m_hasMoves1 = hasMoves1;
        emit hasMoves1Changed(hasMoves1);
    }

    if (m_nuColors > 2)
    {
        int points2 = m_bd.get_points(Color(2));
        if (m_points2 != points2)
        {
            m_points2 = points2;
            emit points2Changed(points2);
        }

        bool hasMoves2 = m_bd.has_moves(Color(2));
        if (m_hasMoves2 != hasMoves2)
        {
            m_hasMoves2 = hasMoves2;
            emit hasMoves2Changed(hasMoves2);
        }

        int nuPiecesLeft2 = getNuPiecesLeft(m_bd, Color(2));
        if (m_nuPiecesLeft2 != nuPiecesLeft2)
        {
            m_nuPiecesLeft2 = nuPiecesLeft2;
            emit nuPiecesLeft2Changed(nuPiecesLeft2);
        }
    }

    if (m_nuColors > 3)
    {
        int points3 = m_bd.get_points(Color(3));
        if (m_points3 != points3)
        {
            m_points3 = points3;
            emit points3Changed(points3);
        }

        bool hasMoves3 = m_bd.has_moves(Color(3));
        if (m_hasMoves3 != hasMoves3)
        {
            m_hasMoves3 = hasMoves3;
            emit hasMoves3Changed(hasMoves3);
        }

        int nuPiecesLeft3 = getNuPiecesLeft(m_bd, Color(3));
        if (m_nuPiecesLeft3 != nuPiecesLeft3)
        {
            m_nuPiecesLeft3 = nuPiecesLeft3;
            emit nuPiecesLeft3Changed(nuPiecesLeft3);
        }
    }

    bool canUndo = (m_bd.get_nu_moves() > 0);
    if (m_canUndo != canUndo)
    {
        m_canUndo = canUndo;
        emit canUndoChanged(canUndo);
    }

    bool isGameOver = true;
    for (ColorIterator i(m_nuColors); i; ++i)
        if (m_bd.has_moves(*i))
            isGameOver = false;
    if (m_isGameOver != isGameOver)
    {
        m_isGameOver = isGameOver;
        emit isGameOverChanged(isGameOver);
    }

    ColorMap<array<bool, Board::max_pieces>> isPlayed;
    for (ColorIterator i(m_nuColors); i; ++i)
        isPlayed[*i].fill(false);
    // Does not handle setup yet
    for (ColorIterator i(m_nuColors); i; ++i)
        LIBBOARDGAME_ASSERT(m_bd.get_setup().placements[*i].empty());
    for (unsigned i = 0; i < m_bd.get_nu_moves(); ++i)
    {
        auto mv = m_bd.get_move(i);
        if (mv.is_pass())
            continue;
        Piece piece = m_bd.get_move_info(mv.move).get_piece();
        auto& pieceModels = getPieceModels(mv.color);
        PieceModel* pieceModel = nullptr;
        for (int j = 0; j < pieceModels.length(); ++j)
            if (pieceModels[j]->getPiece() == piece && ! isPlayed[mv.color][j])
            {
                pieceModel = pieceModels[j];
                isPlayed[mv.color][j] = true;
                break;
            }
        preparePiece(pieceModel, mv.move);
    }
    for (ColorIterator i(m_nuColors); i; ++i)
    {
        auto& pieceModels = getPieceModels(*i);
        for (int j = 0; j < pieceModels.length(); ++j)
            pieceModels[j]->setIsPlayed(isPlayed[*i][j]);
    }

    int toPlay = m_bd.get_effective_to_play().to_int();
    if (m_toPlay != toPlay)
    {
        m_toPlay = toPlay;
        emit toPlayChanged(toPlay);
    }
}

//-----------------------------------------------------------------------------
