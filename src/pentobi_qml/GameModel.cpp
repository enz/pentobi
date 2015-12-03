//-----------------------------------------------------------------------------
/** @file pentobi_qml/GameModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "GameModel.h"

#include <QDebug>
#include <QSettings>
#include "libboardgame_util/Log.h"

using namespace std;
using libpentobi_base::to_string_id;
using libpentobi_base::BoardType;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;
using libpentobi_base::ColorMove;
using libpentobi_base::CoordPoint;
using libpentobi_base::MovePoints;
using libpentobi_base::Piece;
using libpentobi_base::PieceInfo;
using libpentobi_base::PiecePoints;
using libpentobi_base::Point;

//-----------------------------------------------------------------------------

namespace {

// Game coordinates are fractional because they refer to the center of a piece.
// This function is used to compare game coordinates of moves with the same
// piece, so we could even compare the rounded values (?), but comparing
// against epsilon is also safe.
bool compareGameCoord(const QPointF& p1, const QPointF& p2)
{
    return (p1 - p2).manhattanLength() < 0.01f;
}

bool compareTransform(const PieceInfo& pieceInfo, const Transform* t1,
                      const Transform* t2)
{
    return pieceInfo.get_equivalent_transform(t1) ==
            pieceInfo.get_equivalent_transform(t2);
}

QPointF getGameCoord(const Board& bd, Move mv)
{
    auto& geo = bd.get_geometry();
    auto moveInfo = bd.get_move_info(mv);
    PiecePoints movePoints;
    for (Point p : moveInfo)
        movePoints.push_back(CoordPoint(geo.get_x(p), geo.get_y(p)));
    return PieceModel::findCenter(bd, movePoints, false);
}

/** Set a variable to a value and return if it has changed. */
template<typename T>
bool set(T& target, const T& value)
{
    if (target == value)
        return false;
    target = value;
    return true;
}

} //namespace

//-----------------------------------------------------------------------------

GameModel::GameModel(QObject* parent)
    : QObject(parent),
      m_game(getInitialGameVariant()),
      m_gameVariant(to_string_id(m_game.get_variant())),
      m_nuColors(getBoard().get_nu_colors())
{
    createPieceModels();
    updateProperties();
}

void GameModel::autoSave()
{
    QString s;
    auto& bd = getBoard();
    if (bd.get_nu_moves() > 0)
    {
        s  = to_string_id(bd.get_variant());
        for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
        {
            ColorMove mv = bd.get_move(i);
            s.append(QString(";%1;%2")
                     .arg(mv.color.to_int())
                     .arg(bd.to_string(mv.move, false).c_str()));
        }
    }
    QSettings settings;
    settings.setValue("autosave", s);
}

void GameModel::clearAutoSave()
{
    QSettings settings;
    settings.remove("autosave");
}

void GameModel::createPieceModels()
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

void GameModel::createPieceModels(Color c, QList<PieceModel*>& pieceModels)
{
    pieceModels.clear();
    auto& bd = getBoard();
    for (unsigned i = 0; i < bd.get_nu_uniq_pieces(); ++i)
    {
        Piece piece(i);
        for (unsigned j = 0; j < bd.get_nu_piece_instances(); ++j)
            pieceModels.append(new PieceModel(this, bd, piece, c));
    }
}

bool GameModel::findMove(const PieceModel& piece, QString state,
                         QPointF coord, Move& mv) const
{
    auto& bd = getBoard();
    auto& info = bd.get_piece_info(piece.getPiece());
    auto transform = piece.getTransform(state);
    PiecePoints piecePoints = info.get_points();
    transform->transform(piecePoints.begin(), piecePoints.end());
    auto boardType = bd.get_board_type();
    auto newPointType = transform->get_new_point_type();
    bool pointTypeChanged =
            ((boardType == BoardType::trigon && newPointType == 1)
             || (boardType == BoardType::trigon_3 && newPointType == 0));
    QPointF center(PieceModel::findCenter(bd, piecePoints, false));
    // Round y of center to a multiple of 0.5, works better in Trigon
    center.setY(round(2 * center.y()) / 2);
    int offX = static_cast<int>(round(coord.x() - center.x()));
    int offY = static_cast<int>(round(coord.y() - center.y()));
    auto& geo = bd.get_geometry();
    MovePoints points;
    for (auto& p : piecePoints)
    {
        int x = p.x + offX;
        int y = p.y + offY;
        if (! geo.is_onboard(CoordPoint(x, y)))
            return false;
        auto pointType = geo.get_point_type(p.x, p.y);
        auto boardPointType = geo.get_point_type(x, y);
        if (! pointTypeChanged && pointType != boardPointType)
            return false;
        if (pointTypeChanged && pointType == boardPointType)
            return false;
        points.push_back(geo.get_point(x, y));
    }
    return bd.find_move(points, piece.getPiece(), mv);
}

Variant GameModel::getInitialGameVariant()
{
    QSettings settings;
    auto variantString = settings.value("variant", "").toString();
    Variant gameVariant;
    if (! parse_variant_id(variantString.toLocal8Bit().constData(),
                           gameVariant))
        gameVariant = Variant::duo;
    return gameVariant;
}

int GameModel::getLastMoveColor()
{
    auto& bd = getBoard();
    auto nuMoves = bd.get_nu_moves();
    if (nuMoves == 0)
        return 0;
    return bd.get_move(nuMoves - 1).color.to_int();
}

PieceModel* GameModel::getLastMovePieceModel()
{
    return m_lastMovePieceModel;
}

QList<PieceModel*>& GameModel::getPieceModels(Color c)
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

void GameModel::initGameVariant(QString gameVariant)
{
    if (m_gameVariant == gameVariant)
        return;
    if (gameVariant == "classic")
        m_game.init(Variant::classic);
    else if (gameVariant == "classic_2")
        m_game.init(Variant::classic_2);
    else if (gameVariant == "classic_3")
        m_game.init(Variant::classic_3);
    else if (gameVariant == "duo")
        m_game.init(Variant::duo);
    else if (gameVariant == "junior")
        m_game.init(Variant::junior);
    else if (gameVariant == "trigon")
        m_game.init(Variant::trigon);
    else if (gameVariant == "trigon_2")
        m_game.init(Variant::trigon_2);
    else if (gameVariant == "trigon_3")
        m_game.init(Variant::trigon_3);
    else
    {
        qWarning("GameModel: invalid game variant");
        return;
    }
    auto& bd = getBoard();
    if (set(m_nuColors, static_cast<int>(bd.get_nu_colors())))
        emit nuColorsChanged(m_nuColors);
    createPieceModels();
    m_gameVariant = gameVariant;
    emit gameVariantChanged(gameVariant);
    updateProperties();
    QSettings settings;
    settings.setValue("variant", gameVariant);
}

bool GameModel::isLegalPos(PieceModel* pieceModel, QString state,
                            QPointF coord) const
{
    Move mv;
    if (! findMove(*pieceModel, state, coord, mv))
        return false;
    Color c(pieceModel->color());
    bool result = getBoard().is_legal(c, mv);
    return result;
}

bool GameModel::loadAutoSave()
{
    QSettings settings;
    QString s = settings.value("autosave", "").toString();
    if (s.isEmpty())
        return false;
    QStringList l = s.split(';');
    if (l[0] != m_gameVariant)
    {
        qWarning("GameModel: autosave has wrong game variant");
        return false;
    }
    if (l.length() == 1)
    {
        qWarning("GameModel: autosave has no moves");
        return false;
    }
    m_game.init();
    auto& bd = getBoard();
    try
    {
        for (int i = 1; i < l.length(); i += 2)
        {
            unsigned colorInt = l[i].toUInt();
            if (colorInt >= bd.get_nu_colors())
                throw runtime_error("invalid color");
            Color c(colorInt);
            if (i + 1 >= l.length())
                throw runtime_error("color without move");
            Move mv = bd.from_string(l[i + 1].toLocal8Bit().constData());
            if (! bd.is_legal(c, mv))
                throw runtime_error("illegal move");
            m_game.play(c, mv, true);
        }
    }
    catch (const exception &e)
    {
        qWarning() << "GameModel: autosave has illegal move: " << e.what();
    }
    updateProperties();
    return true;
}

void GameModel::newGame()
{
    m_game.init();
    for (auto pieceModel : m_pieceModels0)
        pieceModel->setState("");
    for (auto pieceModel : m_pieceModels1)
        pieceModel->setState("");
    for (auto pieceModel : m_pieceModels2)
        pieceModel->setState("");
    for (auto pieceModel : m_pieceModels3)
        pieceModel->setState("");
    updateProperties();
}


QQmlListProperty<PieceModel> GameModel::pieceModels0()
{
    return QQmlListProperty<PieceModel>(this, m_pieceModels0);
}

QQmlListProperty<PieceModel> GameModel::pieceModels1()
{
    return QQmlListProperty<PieceModel>(this, m_pieceModels1);
}

QQmlListProperty<PieceModel> GameModel::pieceModels2()
{
    return QQmlListProperty<PieceModel>(this, m_pieceModels2);
}

QQmlListProperty<PieceModel> GameModel::pieceModels3()
{
    return QQmlListProperty<PieceModel>(this, m_pieceModels3);
}

void GameModel::play(PieceModel* pieceModel, QPointF coord)
{
    Color c(pieceModel->color());
    Move mv;
    if (! findMove(*pieceModel, pieceModel->state(), coord, mv))
    {
        qWarning("GameModel::play: illegal move");
        return;
    }
    preparePieceGameCoord(pieceModel, mv);
    pieceModel->setIsPlayed(true);
    preparePieceTransform(pieceModel, mv);
    m_game.play(c, mv, false);
    updateProperties();
}

void GameModel::playMove(int move)
{
    Color c = m_game.get_effective_to_play();
    Move mv(move);
    m_game.play(c, mv, false);
    updateProperties();
}

PieceModel* GameModel::preparePiece(int color, int move)
{
    Move mv(move);
    Piece piece = getBoard().get_move_info(mv).get_piece();
    for (auto pieceModel : getPieceModels(Color(color)))
        if (pieceModel->getPiece() == piece && ! pieceModel->isPlayed())
        {
            preparePieceTransform(pieceModel, mv);
            preparePieceGameCoord(pieceModel, mv);
            return pieceModel;
        }
    return nullptr;
}

void GameModel::preparePieceGameCoord(PieceModel* pieceModel, Move mv)
{
    pieceModel->setGameCoord(getGameCoord(getBoard(), mv));
}

void GameModel::preparePieceTransform(PieceModel* pieceModel, Move mv)
{
    auto& bd = getBoard();
    auto transform = bd.find_transform(mv);
    Piece piece = bd.get_move_info(mv).get_piece();
    auto& pieceInfo = bd.get_piece_info(piece);
    if (! compareTransform(pieceInfo, pieceModel->getTransform(), transform))
        pieceModel->setTransform(transform);
}

void GameModel::undo()
{
    if (getBoard().get_nu_moves() == 0)
        return;
    m_game.undo();
    updateProperties();
}

void GameModel::updateProperties()
{
    auto& bd = getBoard();
    if (set(m_points0, static_cast<int>(bd.get_points(Color(0)))))
        emit points0Changed(m_points0);
    if (set(m_points1, static_cast<int>(bd.get_points(Color(1)))))
        emit points1Changed(m_points1);
    if (set(m_bonus0, static_cast<int>(bd.get_bonus(Color(0)))))
        emit bonus0Changed(m_bonus0);
    if (set(m_bonus1, static_cast<int>(bd.get_bonus(Color(1)))))
        emit bonus1Changed(m_bonus1);
    if (set(m_hasMoves0, bd.has_moves(Color(0))))
        emit hasMoves0Changed(m_hasMoves0);
    if (set(m_hasMoves1, bd.has_moves(Color(1))))
        emit hasMoves1Changed(m_hasMoves1);
    if (m_nuColors > 2)
    {
        if (set(m_points2, static_cast<int>(bd.get_points(Color(2)))))
            emit points2Changed(m_points2);
        if (set(m_bonus2, static_cast<int>(bd.get_bonus(Color(2)))))
            emit bonus2Changed(m_bonus2);
        if (set(m_hasMoves2, bd.has_moves(Color(2))))
            emit hasMoves2Changed(m_hasMoves2);
    }
    if (m_nuColors > 3)
    {
        if (set(m_points3, static_cast<int>(bd.get_points(Color(3)))))
            emit points3Changed(m_points3);
        if (set(m_bonus3, static_cast<int>(bd.get_bonus(Color(3)))))
            emit bonus3Changed(m_bonus3);
        if (set(m_hasMoves3, bd.has_moves(Color(3))))
            emit hasMoves3Changed(m_hasMoves3);
    }
    if (set(m_canUndo, (bd.get_nu_moves() > 0)))
        emit canUndoChanged(m_canUndo);
    bool isGameOver = true;
    for (Color c : bd.get_colors())
        if (bd.has_moves(c))
        {
            isGameOver = false;
            break;
        }
    if (set(m_isGameOver, isGameOver))
        emit isGameOverChanged(m_isGameOver);
    if (set(m_isBoardEmpty, (bd.get_nu_onboard_pieces() == 0)))
        emit isBoardEmptyChanged(m_isBoardEmpty);

    ColorMap<array<bool, Board::max_pieces>> isPlayed;
    for (Color c : bd.get_colors())
        isPlayed[c].fill(false);
#if LIBBOARDGAME_DEBUG
    // Does not handle setup yet
    for (Color c : bd.get_colors())
        LIBBOARDGAME_ASSERT(bd.get_setup().placements[c].empty());
#endif
    m_lastMovePieceModel = nullptr;
    for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
    {
        auto mv = bd.get_move(i);
        Piece piece = bd.get_move_info(mv.move).get_piece();
        auto& pieceInfo = bd.get_piece_info(piece);
        auto gameCoord = getGameCoord(bd, mv.move);
        auto transform = bd.find_transform(mv.move);
        auto& pieceModels = getPieceModels(mv.color);
        PieceModel* pieceModel = nullptr;
        // Prefer piece models already played with the given gameCoord and
        // transform because class Board doesn't make a distinction between
        // instances of the same piece (in Junior) and we want to avoid
        // unwanted piece movement animations to switch instances.
        for (int j = 0; j < pieceModels.length(); ++j)
            if (pieceModels[j]->getPiece() == piece
                    && pieceModels[j]->isPlayed()
                    && compareGameCoord(pieceModels[j]->gameCoord(), gameCoord)
                    && compareTransform(pieceInfo,
                                        pieceModels[j]->getTransform(),
                                        transform))
            {
                pieceModel = pieceModels[j];
                isPlayed[mv.color][j] = true;
                break;
            }
        if (pieceModel == nullptr)
        {
            for (int j = 0; j < pieceModels.length(); ++j)
                if (pieceModels[j]->getPiece() == piece
                        && ! isPlayed[mv.color][j])
                {
                    pieceModel = pieceModels[j];
                    isPlayed[mv.color][j] = true;
                    break;
                }
            // Order is important: isPlayed will trigger an animation to move
            // the piece, so it needs to be set after gameCoord.
            pieceModel->setGameCoord(gameCoord);
            pieceModel->setIsPlayed(true);
            pieceModel->setTransform(transform);
        }
        if (i == bd.get_nu_moves() - 1)
            m_lastMovePieceModel = pieceModel;
    }
    for (Color c : bd.get_colors())
    {
        auto& pieceModels = getPieceModels(c);
        for (int i = 0; i < pieceModels.length(); ++i)
            if (! isPlayed[c][i])
                pieceModels[i]->setIsPlayed(false);
    }

    if (set(m_toPlay, m_isGameOver ? 0 : bd.get_effective_to_play().to_int()))
        emit toPlayChanged(m_toPlay);
    if (set(m_altPlayer, (bd.get_variant() == Variant::classic_3 ?
                          bd.get_alt_player() : 0)))
        emit altPlayerChanged(m_altPlayer);
}

//-----------------------------------------------------------------------------
