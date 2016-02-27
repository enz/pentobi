//-----------------------------------------------------------------------------
/** @file pentobi_qml/GameModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "GameModel.h"

#include <cerrno>
#include <cstring>
#include <fstream>
#include <QDebug>
#include <QSettings>
#include "libboardgame_sgf/SgfUtil.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_base/PentobiTreeWriter.h"
#include "libpentobi_base/TreeUtil.h"

using namespace std;
using libboardgame_sgf::InvalidTree;
using libboardgame_sgf::TreeReader;
using libboardgame_sgf::util::back_to_main_variation;
using libboardgame_sgf::util::get_last_node;
using libboardgame_sgf::util::is_main_variation;
using libpentobi_base::get_piece_set;
using libpentobi_base::to_string_id;
using libpentobi_base::BoardType;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;
using libpentobi_base::ColorMove;
using libpentobi_base::CoordPoint;
using libpentobi_base::MovePoints;
using libpentobi_base::PentobiTree;
using libpentobi_base::PentobiTreeWriter;
using libpentobi_base::Piece;
using libpentobi_base::PieceInfo;
using libpentobi_base::PiecePoints;
using libpentobi_base::PieceSet;
using libpentobi_base::Point;
using libpentobi_base::tree_util::get_position_info;

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
    PiecePoints movePoints;
    for (Point p : bd.get_move_points(mv))
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
    // Don't autosave if game was not modified because it could have been
    // loaded from a file, but autosave if not modified and empty to ensure
    // that we start with the same game variant next time.
    if (! m_game.is_modified()
            && ! libboardgame_sgf::util::is_empty(m_game.get_tree()))
        return;
    ostringstream s;
    PentobiTreeWriter writer(s, m_game.get_tree());
    writer.set_indent(-1);
    writer.write();
    QSettings settings;
    settings.setValue("variant", to_string_id(m_game.get_variant()));
    settings.setValue("autosave", s.str().c_str());
}

void GameModel::backToMainVar()
{
    gotoNode(back_to_main_variation(m_game.get_current()));
}

void GameModel::createPieceModels()
{
    createPieceModels(Color(0), m_pieceModels0);
    createPieceModels(Color(1), m_pieceModels1);
    if (m_nuColors > 2)
        createPieceModels(Color(2), m_pieceModels2);
    else
        m_pieceModels2.clear();
    if (m_nuColors > 3)
        createPieceModels(Color(3), m_pieceModels3);
    else
        m_pieceModels3.clear();
}

void GameModel::createPieceModels(Color c, QList<PieceModel*>& pieceModels)
{
    auto& bd = getBoard();
    auto nuPieces = bd.get_nu_uniq_pieces();
    pieceModels.clear();
    pieceModels.reserve(nuPieces);
    for (Piece::IntType i = 0; i < nuPieces; ++i)
    {
        Piece piece(i);
        for (unsigned j = 0; j < bd.get_piece_info(piece).get_nu_instances();
             ++j)
            pieceModels.append(new PieceModel(this, bd, piece, c));
    }
}

void GameModel::deleteAllVar()
{
    if (! is_main_variation(m_game.get_current()))
        emit positionAboutToChange();
    m_game.delete_all_variations();
    updateProperties();
}

bool GameModel::findMove(const PieceModel& pieceModel, const QString& state,
                         QPointF coord, Move& mv) const
{
    auto piece = pieceModel.getPiece();
    auto& bd = getBoard();
    if (piece.to_int() >= bd.get_nu_uniq_pieces())
    {
        qWarning("GameModel::findMove: pieceModel invalid in game variant");
        return false;
    }
    auto transform = pieceModel.getTransform(state);
    if (! transform)
    {
        qWarning("GameModel::findMove: transform not found");
        return false;
    }
    auto& info = bd.get_piece_info(piece);
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
    return bd.find_move(points, piece, mv);
}

QString GameModel::getResultMessage()
{
    auto& bd = getBoard();
    auto nuPlayers = bd.get_nu_players();
    bool breakTies = (bd.get_piece_set() == PieceSet::callisto);
    if (m_nuColors == 2)
    {
        auto score = m_points0 - m_points1;
        if (score == 1)
            return tr("Blue wins with 1 point.");
        if (score > 0)
            return tr("Blue wins with %1 points.").arg(score);
        if (score == -1)
            return tr("Green wins with 1 point.");
        if (score < 0)
            return tr("Green wins with %1 points.").arg(-score);
        if (breakTies)
            return tr("Green wins (tie resolved).");
        return tr("Game ends in a tie.");
    }
    if (m_nuColors == 4 && nuPlayers == 2)
    {
        auto score = m_points0 + m_points2 - m_points1 - m_points3;
        if (score == 1)
            return tr("Blue/Red wins with 1 point.");
        if (score > 0)
            return tr("Blue/Red wins with %1 points.").arg(score);
        if (score == -1)
            return tr("Yellow/Green wins with 1 point.");
        if (score < 0)
            return tr("Yellow/Green wins with %1 points.").arg(-score);
        if (breakTies)
            return tr("Yellow/Green wins (tie resolved).");
        return tr("Game ends in a tie.");
    }
    if (nuPlayers == 3)
    {
        auto maxPoints = max(max(m_points0, m_points1), m_points2);
        unsigned nuWinners = 0;
        if (m_points0 == maxPoints)
            ++nuWinners;
        if (m_points1 == maxPoints)
            ++nuWinners;
        if (m_points2 == maxPoints)
            ++nuWinners;
        if (m_points0 == maxPoints && nuWinners == 1)
            return tr("Blue wins.");
        if (m_points1 == maxPoints && nuWinners == 1)
            return tr("Yellow wins.");
        if (m_points2 == maxPoints && nuWinners == 1)
            return tr("Red wins.");
        if (m_points2 == maxPoints && breakTies)
            return tr("Red wins (tie resolved).");
        if (m_points1 == maxPoints && breakTies)
            return tr("Yellow wins (tie resolved).");
        if (m_points0 == maxPoints && m_points1 == maxPoints && nuWinners == 2)
            return tr("Game ends in a tie between Blue and Yellow.");
        if (m_points0 == maxPoints && m_points2 == maxPoints && nuWinners == 2)
            return tr("Game ends in a tie between Blue and Red.");
        if (nuWinners == 2)
            return tr("Game ends in a tie between Yellow and Red.");
        return tr("Game ends in a tie between all players.");
    }
    auto maxPoints = max(max(m_points0, m_points1), max(m_points2, m_points3));
    unsigned nuWinners = 0;
    if (m_points0 == maxPoints)
        ++nuWinners;
    if (m_points1 == maxPoints)
        ++nuWinners;
    if (m_points2 == maxPoints)
        ++nuWinners;
    if (m_points3 == maxPoints)
        ++nuWinners;
    if (m_points0 == maxPoints && nuWinners == 1)
        return tr("Blue wins.");
    if (m_points1 == maxPoints && nuWinners == 1)
        return tr("Yellow wins.");
    if (m_points2 == maxPoints && nuWinners == 1)
        return tr("Red wins.");
    if (m_points3 == maxPoints && nuWinners == 1)
        return tr("Green wins.");
    if (m_points3 == maxPoints && breakTies)
        return tr("Green wins (tie resolved).");
    if (m_points2 == maxPoints && breakTies)
        return tr("Red wins (tie resolved).");
    if (m_points1 == maxPoints && breakTies)
        return tr("Yellow wins (tie resolved).");
    if (m_points0 == maxPoints && m_points1 == maxPoints
            && m_points2 == maxPoints && nuWinners == 3)
        return tr("Game ends in a tie between Blue, Yellow and Red.");
    if (m_points0 == maxPoints && m_points1 == maxPoints
            && m_points3 == maxPoints && nuWinners == 3)
        return tr("Game ends in a tie between Blue, Yellow and Green.");
    if (m_points0 == maxPoints && m_points2 == maxPoints
            && m_points3 == maxPoints && nuWinners == 3)
        return tr("Game ends in a tie between Blue, Red and Green.");
    if (nuWinners == 3)
        return tr("Game ends in a tie between Yellow, Red and Green.");
    if (m_points0 == maxPoints && m_points1 == maxPoints && nuWinners == 2)
        return tr("Game ends in a tie between Blue and Yellow.");
    if (m_points0 == maxPoints && m_points2 == maxPoints && nuWinners == 2)
        return tr("Game ends in a tie between Blue and Red.");
    if (nuWinners == 2)
        return tr("Game ends in a tie between Yellow and Red.");
    return tr("Game ends in a tie between all players.");
}

Variant GameModel::getInitialGameVariant()
{
    QSettings settings;
    auto variantString = settings.value("variant", "").toString();
    Variant variant;
    if (! parse_variant_id(variantString.toLocal8Bit().constData(), variant))
        variant = Variant::duo;
    return variant;
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
        return m_pieceModels3;
}

void GameModel::goBackward()
{
    gotoNode(m_game.get_current().get_parent_or_null());
}

void GameModel::goBeginning()
{
    gotoNode(m_game.get_root());
}

void GameModel::goEnd()
{
    gotoNode(get_last_node(m_game.get_current()));
}

void GameModel::goForward()
{
    gotoNode(m_game.get_current().get_first_child_or_null());
}

void GameModel::goNextVar()
{
    gotoNode(m_game.get_current().get_sibling());
}

void GameModel::goPrevVar()
{
    gotoNode(m_game.get_current().get_previous_sibling());
}

void GameModel::gotoNode(const SgfNode& node)
{
    if (&node == &m_game.get_current())
        return;
    emit positionAboutToChange();
    try
    {
        m_game.goto_node(node);
    }
    catch (const InvalidTree&)
    {
    }
    updateProperties();
}

void GameModel::gotoNode(const SgfNode* node)
{
    if (node)
        gotoNode(*node);
}

void GameModel::initGameVariant(const QString& gameVariant)
{
    Variant variant;
    if (! parse_variant_id(gameVariant.toLocal8Bit().constData(), variant))
    {
        qWarning("GameModel: invalid game variant");
        return;
    }
    if (m_game.get_variant() != variant)
        m_game.init(variant);
    auto& bd = getBoard();
    if (set(m_nuColors, static_cast<int>(bd.get_nu_colors())))
        emit nuColorsChanged(m_nuColors);
    m_lastMovePieceModel = nullptr;
    createPieceModels();
    m_gameVariant = gameVariant;
    emit gameVariantChanged(gameVariant);
    updateProperties();
}

bool GameModel::isLegalPos(PieceModel* pieceModel, const QString& state,
                           QPointF coord) const
{
    Move mv;
    if (! findMove(*pieceModel, state, coord, mv))
        return false;
    Color c(static_cast<Color::IntType>(pieceModel->color()));
    bool result = getBoard().is_legal(c, mv);
    return result;
}

bool GameModel::loadAutoSave()
{
    QSettings settings;
    auto s = settings.value("autosave", "").toByteArray();
    istringstream in(s.constData());
    if (! open(in))
        return false;
    m_game.set_modified();
    return true;
}

void GameModel::makeMainVar()
{
    m_game.make_main_variation();
    updateProperties();
}

void GameModel::moveDownVar()
{
    m_game.move_down_variation();
    updateProperties();
}

void GameModel::moveUpVar()
{
    m_game.move_up_variation();
    updateProperties();
}

void GameModel::nextColor()
{
    emit positionAboutToChange();
    auto& bd = getBoard();
    m_game.set_to_play(bd.get_next(bd.get_to_play()));
    updateProperties();
}

void GameModel::newGame()
{
    emit positionAboutToChange();
    m_game.init();
    for (auto pieceModel : m_pieceModels0)
        pieceModel->setDefaultState();
    for (auto pieceModel : m_pieceModels1)
        pieceModel->setDefaultState();
    for (auto pieceModel : m_pieceModels2)
        pieceModel->setDefaultState();
    for (auto pieceModel : m_pieceModels3)
        pieceModel->setDefaultState();
    updateProperties();
}

bool GameModel::open(istream& in)
{
    try
    {
        TreeReader reader;
        reader.read(in);
        auto root = reader.get_tree_transfer_ownership();
        emit positionAboutToChange();
        m_game.init(root);
        auto variant = to_string_id(m_game.get_variant());
        if (variant != m_gameVariant)
            initGameVariant(variant);
        goEnd();
        updateProperties();
        QSettings settings;
        settings.remove("autosave");
    }
    catch (const runtime_error& e)
    {
        m_lastInputOutputError = QString::fromLocal8Bit(e.what());
        return false;
    }
    return true;
}

bool GameModel::open(const QString& file)
{
    ifstream in(file.toLocal8Bit().constData());
    if (! in)
    {
        m_lastInputOutputError = QString::fromLocal8Bit(strerror(errno));
        return false;
    }
    return open(in);
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

void GameModel::playMove(int move)
{
    Move mv(static_cast<Move::IntType>(move));
    if (mv.is_null())
        return;
    emit positionAboutToChange();
    m_game.play(m_game.get_to_play(), mv, false);
    updateProperties();
}

void GameModel::playPiece(PieceModel* pieceModel, QPointF coord)
{
    Color c(static_cast<Color::IntType>(pieceModel->color()));
    Move mv;
    if (! findMove(*pieceModel, pieceModel->state(), coord, mv))
    {
        qWarning("GameModel::play: illegal move");
        return;
    }
    emit positionAboutToChange();
    preparePieceGameCoord(pieceModel, mv);
    pieceModel->setIsPlayed(true);
    preparePieceTransform(pieceModel, mv);
    m_game.play(c, mv, false);
    updateProperties();
}

PieceModel* GameModel::preparePiece(int color, int move)
{
    Move mv(static_cast<Move::IntType>(move));
    Color c(static_cast<Color::IntType>(color));
    Piece piece = getBoard().get_move_piece(mv);
    for (auto pieceModel : getPieceModels(c))
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
    auto& pieceInfo = bd.get_piece_info(bd.get_move_piece(mv));
    if (! compareTransform(pieceInfo, pieceModel->getTransform(), transform))
        pieceModel->setTransform(transform);
}

bool GameModel::save(const QString& file)
{
    ofstream out(file.toLocal8Bit().constData());
    PentobiTreeWriter writer(out, m_game.get_tree());
    writer.set_indent(1);
    writer.write();
    if (! out)
    {
        m_lastInputOutputError = QString::fromLocal8Bit(strerror(errno));
        return false;
    }
    m_game.clear_modified();
    return true;
}

void GameModel::truncate()
{
    if (! m_game.get_current().has_parent())
        return;
    emit positionAboutToChange();
    m_game.truncate();
    updateProperties();
}

void GameModel::truncateChildren()
{
    m_game.truncate_children();
    updateProperties();
}

void GameModel::undo()
{
    if (! m_canUndo)
        return;
    emit positionAboutToChange();
    m_game.undo();
    updateProperties();
}

/** Helper function for updateProperties() */
PieceModel* GameModel::updatePiece(Color c, Move mv,
                                   array<bool, Board::max_pieces>& isPlayed)
{
    auto& bd = getBoard();
    Piece piece = bd.get_move_piece(mv);
    auto& pieceInfo = bd.get_piece_info(piece);
    auto gameCoord = getGameCoord(bd, mv);
    auto transform = bd.find_transform(mv);
    auto& pieceModels = getPieceModels(c);
    PieceModel* pieceModel = nullptr;
    // Prefer piece models already played with the given gameCoord and
    // transform because class Board doesn't make a distinction between
    // instances of the same piece (in Junior) and we want to avoid
    // unwanted piece movement animations to switch instances.
    for (int i = 0; i < pieceModels.length(); ++i)
        if (pieceModels[i]->getPiece() == piece
                && pieceModels[i]->isPlayed()
                && compareGameCoord(pieceModels[i]->gameCoord(), gameCoord)
                && compareTransform(pieceInfo, pieceModels[i]->getTransform(),
                                    transform))
        {
            pieceModel = pieceModels[i];
            isPlayed[i] = true;
            break;
        }
    if (pieceModel == nullptr)
    {
        for (int i = 0; i < pieceModels.length(); ++i)
            if (pieceModels[i]->getPiece() == piece && ! isPlayed[i])
            {
                pieceModel = pieceModels[i];
                isPlayed[i] = true;
                break;
            }
        // Order is important: isPlayed will trigger an animation to move
        // the piece, so it needs to be set after gameCoord.
        pieceModel->setGameCoord(gameCoord);
        pieceModel->setIsPlayed(true);
        pieceModel->setTransform(transform);
    }
    return pieceModel;
}

void GameModel::updateProperties()
{
    auto& bd = getBoard();
    auto& geo = bd.get_geometry();
    auto& tree = m_game.get_tree();
    bool isTrigon = (bd.get_piece_set() == PieceSet::trigon);
    if (set(m_points0, bd.get_points(Color(0))))
        emit points0Changed(m_points0);
    if (set(m_points1, bd.get_points(Color(1))))
        emit points1Changed(m_points1);
    if (set(m_bonus0, bd.get_bonus(Color(0))))
        emit bonus0Changed(m_bonus0);
    if (set(m_bonus1, bd.get_bonus(Color(1))))
        emit bonus1Changed(m_bonus1);
    if (set(m_hasMoves0, bd.has_moves(Color(0))))
        emit hasMoves0Changed(m_hasMoves0);
    if (set(m_hasMoves1, bd.has_moves(Color(1))))
        emit hasMoves1Changed(m_hasMoves1);
    bool isFirstPieceAny = false;
    if (m_nuColors > 2)
    {
        if (set(m_points2, bd.get_points(Color(2))))
            emit points2Changed(m_points2);
        if (set(m_bonus2, bd.get_bonus(Color(2))))
            emit bonus2Changed(m_bonus2);
        if (set(m_hasMoves2, bd.has_moves(Color(2))))
            emit hasMoves2Changed(m_hasMoves2);
    }
    if (m_nuColors > 3)
    {
        if (set(m_points3, bd.get_points(Color(3))))
            emit points3Changed(m_points3);
        if (set(m_bonus3, bd.get_bonus(Color(3))))
            emit bonus3Changed(m_bonus3);
        if (set(m_hasMoves3, bd.has_moves(Color(3))))
            emit hasMoves3Changed(m_hasMoves3);
    }
    m_tmpPoints.clear();
    if (bd.is_first_piece(Color(0)))
    {
        isFirstPieceAny = true;
        if (! isTrigon)
            for (Point p : bd.get_starting_points(Color(0)))
                m_tmpPoints.append(QPointF(geo.get_x(p), geo.get_y(p)));
    }
    if (set(m_startingPoints0, m_tmpPoints))
        emit startingPoints0Changed(m_startingPoints0);
    m_tmpPoints.clear();
    if (bd.is_first_piece(Color(1)))
    {
        isFirstPieceAny = true;
        if (! isTrigon)
            for (Point p : bd.get_starting_points(Color(1)))
                m_tmpPoints.append(QPointF(geo.get_x(p), geo.get_y(p)));
    }
    if (set(m_startingPoints1, m_tmpPoints))
        emit startingPoints1Changed(m_startingPoints1);
    m_tmpPoints.clear();
    if (m_nuColors > 2 && bd.is_first_piece(Color(2)))
    {
        isFirstPieceAny = true;
        if (! isTrigon)
            for (Point p : bd.get_starting_points(Color(2)))
                m_tmpPoints.append(QPointF(geo.get_x(p), geo.get_y(p)));
    }
    if (set(m_startingPoints2, m_tmpPoints))
        emit startingPoints2Changed(m_startingPoints2);
    m_tmpPoints.clear();
    if (m_nuColors > 3 && bd.is_first_piece(Color(3)))
    {
        isFirstPieceAny = true;
        if (! isTrigon)
            for (Point p : bd.get_starting_points(Color(3)))
                m_tmpPoints.append(QPointF(geo.get_x(p), geo.get_y(p)));
    }
    if (set(m_startingPoints3, m_tmpPoints))
        emit startingPoints3Changed(m_startingPoints3);
    m_tmpPoints.clear();
    if (isTrigon && isFirstPieceAny)
        for (Point p : bd.get_starting_points(Color(0)))
            m_tmpPoints.append(QPointF(geo.get_x(p), geo.get_y(p)));
    if (set(m_startingPointsAll, m_tmpPoints))
        emit startingPointsAllChanged(m_startingPointsAll);
    auto& current = m_game.get_current();
    if (set(m_canUndo,
            ! current.has_children() && tree.has_move_ignore_invalid(current)
            && current.has_parent()))
        emit canUndoChanged(m_canUndo);
    if (set(m_canGoForward, current.has_children()))
        emit canGoForwardChanged(m_canGoForward);
    if (set(m_canGoBackward, current.has_parent()))
        emit canGoBackwardChanged(m_canGoBackward);
    if (set(m_hasPrevVar, (current.get_previous_sibling() != nullptr)))
        emit hasPrevVarChanged(m_hasPrevVar);
    if (set(m_hasNextVar, (current.get_sibling() != nullptr)))
        emit hasNextVarChanged(m_hasNextVar);
    if (set(m_hasVariations, tree.has_variations()))
        emit hasVariationsChanged(m_hasVariations);
    if (set(m_isMainVar, is_main_variation(current)))
        emit isMainVarChanged(m_isMainVar);
    QString positionInfo
            = QString::fromLocal8Bit(get_position_info(tree, current).c_str());
    if (positionInfo.isEmpty())
        positionInfo = bd.has_setup() ? tr("(Setup)") : tr("(No moves)");
    else
    {
        positionInfo = tr("Move %1").arg(positionInfo);
        if (bd.get_nu_moves() == 0 && bd.has_setup())
        {
            positionInfo.append(' ');
            positionInfo.append(tr("(Setup)"));
        }
    }
    if (set(m_positionInfo, positionInfo))
        emit positionInfoChanged(m_positionInfo);
    bool isGameOver = true;
    for (Color c : bd.get_colors())
        if (bd.has_moves(c))
        {
            isGameOver = false;
            break;
        }
    if (set(m_isGameOver, isGameOver))
        emit isGameOverChanged(m_isGameOver);
    if (set(m_isGameEmpty, libboardgame_sgf::util::is_empty(tree)))
        emit isGameEmptyChanged(m_isGameEmpty);

    ColorMap<array<bool, Board::max_pieces>> isPlayed;
    for (Color c : bd.get_colors())
    {
        isPlayed[c].fill(false);
        for (Move mv : bd.get_setup().placements[c])
            updatePiece(c, mv, isPlayed[c]);
    }
    PieceModel* lastMovePieceModel = nullptr;
    for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
    {
        auto mv = bd.get_move(i);
        auto c = mv.color;
        lastMovePieceModel = updatePiece(c, mv.move, isPlayed[c]);
    }
    if (lastMovePieceModel != m_lastMovePieceModel)
    {
        if (m_lastMovePieceModel != nullptr)
            m_lastMovePieceModel->setIsLastMove(false);
        if (lastMovePieceModel != nullptr)
            lastMovePieceModel->setIsLastMove(true);
        m_lastMovePieceModel = lastMovePieceModel;
    }
    for (Color c : bd.get_colors())
    {
        auto& pieceModels = getPieceModels(c);
        for (int i = 0; i < pieceModels.length(); ++i)
            if (! isPlayed[c][i] && pieceModels[i]->isPlayed())
            {
                pieceModels[i]->setDefaultState();
                pieceModels[i]->setIsPlayed(false);
            }
    }

    if (set(m_toPlay, m_isGameOver ? 0 : bd.get_effective_to_play().to_int()))
        emit toPlayChanged(m_toPlay);
    if (set(m_altPlayer, (bd.get_variant() == Variant::classic_3 ?
                          bd.get_alt_player() : 0)))
        emit altPlayerChanged(m_altPlayer);

    emit positionChanged();
}

//-----------------------------------------------------------------------------
