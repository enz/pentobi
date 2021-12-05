//-----------------------------------------------------------------------------
/** @file pentobi/GameModel.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "GameModel.h"

#include <cerrno>
#include <cmath>
#include <cstring>
#include <fstream>
#include <QClipboard>
#include <QGuiApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include "AndroidUtils.h"
#include "libboardgame_base/SgfUtil.h"
#include "libboardgame_base/StringUtil.h"
#include "libboardgame_base/TreeReader.h"
#include "libpentobi_base/MoveMarker.h"
#include "libpentobi_base/NodeUtil.h"
#include "libpentobi_base/PentobiTreeWriter.h"
#include "libpentobi_base/TreeUtil.h"

using namespace std;
using libboardgame_base::back_to_main_variation;
using libboardgame_base::beginning_of_branch;
using libboardgame_base::find_next_comment;
using libboardgame_base::get_last_node;
using libboardgame_base::get_letter_coord;
using libboardgame_base::has_comment;
using libboardgame_base::has_earlier_variation;
using libboardgame_base::is_main_variation;
using libboardgame_base::to_lower;
using libboardgame_base::ArrayList;
using libboardgame_base::SgfError;
using libboardgame_base::TreeReader;
using libpentobi_base::to_string_id;
using libpentobi_base::BoardType;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;
using libpentobi_base::ColorMove;
using libpentobi_base::MovePoints;
using libpentobi_base::PentobiTree;
using libpentobi_base::PentobiTreeWriter;
using libpentobi_base::Piece;
using libpentobi_base::PieceInfo;
using libpentobi_base::PiecePoints;
using libpentobi_base::PieceSet;
using libpentobi_base::Point;
using libpentobi_base::has_setup;
using libpentobi_base::get_move_number;
using libpentobi_base::get_moves_left;
using libpentobi_base::get_move_node;

//-----------------------------------------------------------------------------

namespace {

// Game coordinates are fractional because they refer to the center of a piece.
// This function is used to compare game coordinates of moves with the same
// piece, so we could even compare the rounded values (?), but comparing
// against epsilon is also safe.
bool compareGameCoord(const QPointF& p1, const QPointF& p2)
{
    return (p1 - p2).manhattanLength() < qreal(0.01);
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
        movePoints.push_back({geo.get_x(p), geo.get_y(p)});
    return PieceModel::findCenter(bd, movePoints, false);
}

/** Board uses 4 starting points per Color in GembloQ for technical reasons,
    GameModel only needs one for displaying the colored dot, shifted to the
    center of a square. */
QPointF getGembloQStartingPoint(const Board& bd, Color c)
{
    auto p = bd.get_starting_points(c)[0];
    auto& geo = bd.get_geometry();
    qreal x = geo.get_x(p);
    qreal y = geo.get_y(p);
    if (geo.get_x(p) % 2 == 0)
        x -= 0.5;
    else
        x += 0.5;
    if (geo.get_y(p) % 2 == 0)
        y += 0.5;
    else
        y -= 0.5;
    return {x, y};
}

/** Simple heuristic used for sorting the list used in GameModel::findMove().
    Prefers larger pieces, and moves of the same piece (in that order). */
float getHeuristic(const Board& bd, Move mv)
{
    auto piece = bd.get_move_piece(mv);
    auto points = bd.get_piece_info(piece).get_score_points();
    return Piece::max_pieces * points - piece.to_int();
}

/** Get the index of a variation.
    This ignores child nodes without moves so that the moves are still labeled
    1a, 1b, 1c, etc. even if this does not correspond to the child node
    index. (Note that this is a different convention from variation strings
    which does not use move number and child move index, but node depth and
    child node index) */
bool getVariationIndex(const PentobiTree& tree, const SgfNode& node,
                       unsigned& moveIndex)
{
    auto parent = node.get_parent_or_null();
    if (! parent || parent->has_single_child())
        return false;
    unsigned nuSiblingMoves = 0;
    moveIndex = 0;
    for (auto& i : parent->get_children())
    {
        if (! tree.has_move(i))
            continue;
        if (&i == &node)
            moveIndex = nuSiblingMoves;
        ++nuSiblingMoves;
    }
    return nuSiblingMoves != 1;
}

} //namespace

//-----------------------------------------------------------------------------

GameModel::GameModel(QObject* parent)
    : QObject(parent),
      m_game(getInitialGameVariant()),
      m_gameVariant(to_string_id(m_game.get_variant())),
      m_nuColors(getBoard().get_nu_colors()),
      m_nuPlayers(getBoard().get_nu_players())
{
    initGame(m_game.get_variant());
    createPieceModels();
    updateProperties();
}

GameModel::~GameModel() = default;

PieceModel* GameModel::addEmpty(const QPoint& pos)
{
    if (! checkSetupAllowed())
        return nullptr;
    auto move = getMoveAt(pos);
    if (move.is_null())
        return nullptr;
    auto c = move.color;
    auto mv = move.move;
    auto& bd = getBoard();
    LIBBOARDGAME_ASSERT(bd.get_setup().placements[c].contains(mv));
    auto gameCoord = getGameCoord(bd, mv);
    PieceModel* result = nullptr;
    for (auto& variant : as_const(m_pieceModels[c]))
    {
        auto pieceModel = qvariant_cast<PieceModel*>(variant);
        if (compareGameCoord(pieceModel->gameCoord(), gameCoord))
        {
            result = pieceModel;
            break;
        }
    }
    preparePositionChange();
    m_game.remove_setup(c, mv);
    setSetupPlayer();
    updateProperties();
    return result;
}

void GameModel::addSetup(PieceModel* pieceModel, QPointF coord)
{
    if (! checkSetupAllowed())
        return;
    Color c(static_cast<Color::IntType>(pieceModel->color()));
    Move mv;
    if (! findMove(*pieceModel, pieceModel->state(), coord, mv))
        return;
    preparePositionChange();
    preparePieceGameCoord(pieceModel, mv);
    pieceModel->setIsPlayed(true);
    preparePieceTransform(pieceModel, mv);
    try
    {
        m_game.add_setup(c, mv);
    }
    catch (const SgfError& error)
    {
        m_error = error.what();
        emit invalidSgfFile();
    }
    setSetupPlayer();
    updateProperties();
}

void GameModel::autoSave()
{
    auto& tree = m_game.get_tree();
    QSettings settings;
    settings.setValue(QStringLiteral("variant"),
                      to_string_id(m_game.get_variant()));
    if (! m_file.isEmpty() && ! m_isModified)
        settings.remove(QStringLiteral("autosave"));
    else
        settings.setValue(QStringLiteral("autosave"), getSgf());
    settings.setValue(QStringLiteral("file"), m_file);
    m_autosaveDate = QDateTime::currentDateTime();
    // Note: some Qt versions crash when adding invalid QDateTime to QSettings
    if (m_autosaveDate.isValid())
        settings.setValue(QStringLiteral("autosaveDate"), m_autosaveDate);
    else
        settings.remove(QStringLiteral("autosaveDate"));
    if (m_fileDate.isValid())
        settings.setValue(QStringLiteral("fileDate"), m_fileDate);
    else
        settings.remove(QStringLiteral("fileDate"));
    settings.setValue(QStringLiteral("isModified"), m_isModified);
    QVariantList location;
    uint depth = 0;
    auto node = &m_game.get_current();
    while (node != &tree.get_root())
    {
        auto& parent = node->get_parent();
        if (parent.get_nu_children() > 1)
            location.prepend(parent.get_child_index(*node));
        node = &parent;
        ++depth;
    }
    location.prepend(depth);
    settings.setValue(QStringLiteral("autosaveLocation"), location);
}

void GameModel::backToMainVar()
{
    gotoNode(back_to_main_variation(m_game.get_current()));
}

void GameModel::changeGameVariant(const QString& gameVariant)
{
    Variant variant;
    if (! parse_variant_id(gameVariant.toLocal8Bit().constData(), variant))
        return;
    initGameVariant(variant);
    setIsModified(false);
    clearFile();
}

bool GameModel::checkAutosaveModifiedOutside()
{
#ifdef Q_OS_ANDROID
    return false;
#else
    QSettings settings;
    auto autosaveDate =
            settings.value(QStringLiteral("autosaveDate")).toDateTime();
    return m_autosaveDate.isValid() && autosaveDate.isValid()
            && m_autosaveDate != autosaveDate
            && settings.value(QStringLiteral("isModified")).toBool()
            && settings.value(QStringLiteral("autosave")).toByteArray() != getSgf();
#endif
}

bool GameModel::checkFileExists(const QString& file)
{
    return QFileInfo::exists(file);
}

bool GameModel::checkFileModifiedOutside()
{
#ifdef Q_OS_ANDROID
    return false;
#else
    if (m_file.isEmpty() || ! m_fileDate.isValid())
        return false;
    QFileInfo fileInfo(m_file);
    if (! fileInfo.exists())
        return false;
    return fileInfo.lastModified() != m_fileDate;
#endif
}

/** Check if setup is allowed in the current position.
    Currently, we support setup mode only if no moves have been played. It
    should also work in inner nodes but this might be confusing for users and
    violate some assumptions in the user interface (e.g. node depth is equal to
    move number).*/
bool GameModel::checkSetupAllowed() const
{
    return ! m_canGoBackward && ! m_canGoForward && m_moveNumber == 0;
}

void GameModel::clearFile()
{
    if (m_file.isEmpty())
        return;
    m_file.clear();
    emit fileChanged();
}

bool GameModel::createFolder(const QUrl& folder)
{
    auto localFolder = folder.toLocalFile();
    if (! QDir().mkdir(localFolder)) {
        m_error = QString::fromLocal8Bit(strerror(errno));
        return false;
    }
    return true;
}

void GameModel::createPieceModels()
{
    createPieceModels(Color(0));
    createPieceModels(Color(1));
    if (m_nuColors > 2)
        createPieceModels(Color(2));
    else
        m_pieceModels[Color(2)].clear();
    if (m_nuColors > 3)
        createPieceModels(Color(3));
    else
        m_pieceModels[Color(3)].clear();
}

void GameModel::createPieceModels(Color c)
{
    auto& bd = getBoard();
    auto nuPieces = bd.get_nu_uniq_pieces();
    m_pieceModels[c].clear();
    m_pieceModels[c].reserve(nuPieces);
    for (Piece::IntType i = 0; i < nuPieces; ++i)
    {
        Piece piece(i);
        auto nuInstances = bd.get_piece_info(piece).get_nu_instances();
        for (unsigned j = 0; j < nuInstances; ++j)
        {
            auto variant =
                    QVariant::fromValue(new PieceModel(this, bd, piece, c));
            m_pieceModels[c].append(variant);
        }
    }
}

QString GameModel::decode(const string& s) const
{
    return QString::fromStdString(s);
}

void GameModel::deleteAllVar()
{
    if (! is_main_variation(m_game.get_current()))
        preparePositionChange();
    m_game.delete_all_variations();
    updateProperties();
}


QByteArray GameModel::encode(const QString& s) const
{
    return s.toUtf8();
}

GameMove* GameModel::findMoveNext()
{
    prepareFindMove();
    if (m_legalMoves->empty())
        return nullptr;
    auto i = m_legalMoveIndex >= m_legalMoves->size() ? 0 : m_legalMoveIndex;
    auto mv = (*m_legalMoves)[i];
    m_legalMoveIndex = i + 1;
    return new GameMove(this, ColorMove(getBoard().get_to_play(), mv));
}

GameMove* GameModel::findMovePrevious()
{
    prepareFindMove();
    if (m_legalMoves->empty())
        return nullptr;
    auto i = m_legalMoveIndex > 1 ? m_legalMoveIndex - 2
                                  : m_legalMoves->size() - 1;
    auto mv = (*m_legalMoves)[i];
    m_legalMoveIndex = i + 1;
    return new GameMove(this, ColorMove(getBoard().get_to_play(), mv));
}

bool GameModel::findMove(const PieceModel& pieceModel, const QString& state,
                         QPointF coord, Move& mv) const
{
    auto piece = pieceModel.getPiece();
    auto& bd = getBoard();
    if (piece.to_int() >= bd.get_nu_uniq_pieces())
        return false;
    auto transform = pieceModel.getTransform(state);
    if (! transform)
        return false;
    auto& info = bd.get_piece_info(piece);
    PiecePoints piecePoints = info.get_points();
    transform->transform(piecePoints.begin(), piecePoints.end());
    QPointF center(PieceModel::findCenter(bd, piecePoints, false));
    // Round y of center to a multiple of 0.5, works better in Trigon
    center.setY(round(2 * center.y()) / 2);
    auto pointType = transform->get_point_type();
    auto dx = coord.x() - center.x();
    auto dy = coord.y() - center.y();
    int offX;
    if (bd.get_piece_set() == PieceSet::gembloq)
    {
        // In GembloQ, every piece has at least one full square, so we can use
        // half the x resolution, which makes positioning easier for the user.
        if (pointType == 0 || pointType == 2)
            offX = static_cast<int>(round(dx * qreal(0.5))) * 2;
        else
            offX = static_cast<int>(round((dx - 1) * qreal(0.5))) * 2 + 1;
    }
    else
        offX = static_cast<int>(round(dx));
    auto offY = static_cast<int>(round(dy));
    auto& geo = bd.get_geometry();
    if (geo.get_point_type(offX, offY) != pointType)
        return false;
    MovePoints points;
    for (auto& p : piecePoints)
    {
        int x = p.x + offX;
        int y = p.y + offY;
        if (! geo.is_onboard(x, y))
            return false;
        points.push_back(geo.get_point(x, y));
    }
    return bd.find_move(points, piece, mv);
}

bool GameModel::findNextComment()
{
    auto node = find_next_comment(m_game.get_current());
    if (! node)
        return false;
    gotoNode(*node);
    return true;
}

bool GameModel::findNextCommentContinueFromRoot()
{
    auto node = &m_game.get_root();
    if (! has_comment(*node))
        node = find_next_comment(*node);
    if (! node)
        return false;
    gotoNode(*node);
    return true;
}

PieceModel* GameModel::findUnplayedPieceModel(Color c, Piece piece)
{
    for (auto& variant : as_const(m_pieceModels[c]))
    {
        auto pieceModel = qvariant_cast<PieceModel*>(variant);
        if (pieceModel->getPiece() == piece && ! pieceModel->isPlayed())
            return pieceModel;
    }
    return nullptr;
}

QVariantList GameModel::getPieceModels(int color)
{
    if (color >= 0 && color <= static_cast<int>(Color::range))
        return m_pieceModels[Color(static_cast<Color::IntType>(color))];
    return m_pieceModels[Color(0)];
}

Variant GameModel::getInitialGameVariant()
{
    QSettings settings;
    auto variantString = settings.value(QStringLiteral("variant")).toString();
    Variant variant;
    if (! parse_variant_id(variantString.toLocal8Bit().constData(), variant))
        variant = Variant::duo;
    return variant;
}

QString GameModel::getMoveAnnotation(int moveNumber)
{
    if (moveNumber <= 0)
        return {};
    auto node = get_move_node(m_game.get_tree(), m_game.get_current(),
                              static_cast<unsigned>(moveNumber));
    if (node == nullptr)
        return {};
    return getMoveAnnotationAtNode(*node);
}

QString GameModel::getMoveAnnotationAtNode(const SgfNode& node) const
{
    try
    {
        if (m_game.get_good_move(node) == 2)
            return QStringLiteral("‼");
        if (m_game.get_good_move(node) == 1)
            return QStringLiteral("!");
        if (m_game.is_interesting_move(node))
            return QStringLiteral("⁉");
        if (m_game.is_doubtful_move(node))
            return QStringLiteral("⁈");
        if (m_game.get_bad_move(node) == 1)
            return QStringLiteral("?");
        if (m_game.get_bad_move(node) == 2)
            return QStringLiteral("⁇");
    }
    catch (const SgfError&)
    {
        // Silently ignore GM, BM properties with invalid value
    }
    return {};
}

ColorMove GameModel::getMoveAt(const QPoint& pos) const
{
    auto& bd = getBoard();
    auto& geo = bd.get_geometry();
    if (pos.x() < 0 || pos.y() < 0)
        return ColorMove::null();
    if (! geo.is_onboard(pos.x(), pos.y()))
        return ColorMove::null();
    auto p = geo.get_point(static_cast<unsigned>(pos.x()),
                           static_cast<unsigned>(pos.y()));
    auto s = bd.get_point_state(p);
    if (s.is_empty())
        return ColorMove::null();
    auto c = s.to_color();
    auto mv = bd.get_move_at(p);
    return {c, mv};
}

int GameModel::getMoveNumberAt(const QPoint& pos)
{
    auto move = getMoveAt(pos);
    if (move.is_null())
        return -1;
    auto n = m_moveNumber;
    auto& tree = m_game.get_tree();
    auto node = &m_game.get_current();
    do
    {
        if (tree.has_move(*node))
        {
            if (tree.get_move(*node) == move)
                return n;
            --n;
        }
        node = node->get_parent_or_null();
    }
    while (node != nullptr);
    return -1;
}

QString GameModel::getResultMessage()
{
    auto& bd = getBoard();
    auto variant = bd.get_variant();
    if (variant == Variant::duo)
    {
        auto score = static_cast<double>(m_points0 - m_points1);
        if (score == 1)
            return tr("Purple wins with 1 point.");
        if (score > 0)
            return tr("Purple wins with %L1 points.").arg(score);
        if (score == -1)
            return tr("Orange wins with 1 point.");
        if (score < 0)
            return tr("Orange wins with %L1 points.").arg(-score);
        return tr("Game ends in a tie.");
    }
    if (variant == Variant::junior)
    {
        auto score = static_cast<double>(m_points0 - m_points1);
        if (score == 1)
            return tr("Green wins with 1 point.");
        if (score > 0)
            return tr("Green wins with %L1 points.").arg(score);
        if (score == -1)
            return tr("Orange wins with 1 point.");
        if (score < 0)
            return tr("Orange wins with %L1 points.").arg(-score);
        return tr("Game ends in a tie.");
    }
    bool breakTies = (bd.get_piece_set() == PieceSet::callisto);
    if (m_nuColors == 2)
    {
        auto score = static_cast<double>(m_points0 - m_points1);
        if (score == 1)
            return tr("Blue wins with 1 point.");
        if (score > 0)
            return tr("Blue wins with %L1 points.").arg(score);
        if (score == -1)
            return tr("Green wins with 1 point.");
        if (score < 0)
            return tr("Green wins with %L1 points.").arg(-score);
        if (breakTies)
            //: Game variant with tie-breaker rule made later player win.
            return tr("Green wins (tie resolved).");
        return tr("Game ends in a tie.");
    }
    if (m_nuColors == 4 && m_nuPlayers == 2)
    {
        auto score = static_cast<double>(m_points0 + m_points2
                                         - m_points1 - m_points3);
        if (score == 1)
            return tr("Blue/Red wins with 1 point.");
        if (score > 0)
            return tr("Blue/Red wins with %L1 points.").arg(score);
        if (score == -1)
            return tr("Yellow/Green wins with 1 point.");
        if (score < 0)
            return tr("Yellow/Green wins with %L1 points.").arg(-score);
        if (breakTies)
            //: Game variant with tie-breaker rule made later player win.
            return tr("Yellow/Green wins (tie resolved).");
        return tr("Game ends in a tie.");
    }
    if (m_nuPlayers == 3)
    {
        auto maxPoints = max({m_points0, m_points1, m_points2});
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
            //: Game variant with tie-breaker rule made later player win.
            return tr("Red wins (tie resolved).");
        if (m_points1 == maxPoints && breakTies)
            //: Game variant with tie-breaker rule made later player win.
            return tr("Yellow wins (tie resolved).");
        if (m_points0 == maxPoints && m_points1 == maxPoints && nuWinners == 2)
            return tr("Game ends in a tie between Blue and Yellow.");
        if (m_points0 == maxPoints && m_points2 == maxPoints && nuWinners == 2)
            return tr("Game ends in a tie between Blue and Red.");
        if (nuWinners == 2)
            return tr("Game ends in a tie between Yellow and Red.");
        return tr("Game ends in a tie between all players.");
    }
    auto maxPoints = max({m_points0, m_points1, m_points2, m_points3});
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
        //: Game variant with tie-breaker rule made later player win.
        return tr("Green wins (tie resolved).");
    if (m_points2 == maxPoints && breakTies)
        //: Game variant with tie-breaker rule made later player win.
        return tr("Red wins (tie resolved).");
    if (m_points1 == maxPoints && breakTies)
        //: Game variant with tie-breaker rule made later player win.
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

QByteArray GameModel::getSgf(int indent) const
{
    auto& tree = m_game.get_tree();
    ostringstream s;
    PentobiTreeWriter writer(s, tree);
    writer.set_indent(indent);
    writer.write();
    return QByteArray(s.str().c_str());
}

QString GameModel::getVariationInfo() const
{
    auto moveNumber = getBoard().get_nu_moves();
    QString s = QString::number(moveNumber);
    unsigned moveIndex;
    if (getVariationIndex(m_game.get_tree(), m_game.get_current(), moveIndex))
        s.append(get_letter_coord(moveIndex).c_str());
    return s;
}

void GameModel::goBackward()
{
    gotoNode(m_game.get_current().get_parent_or_null());
}

void GameModel::goBackward10()
{
    auto node = &m_game.get_current();
    for (unsigned i = 0; i < 10; ++i)
    {
        auto parent = node->get_parent_or_null();
        if (parent == nullptr)
            break;
        node = parent;
    }
    gotoNode(node);
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

void GameModel::goForward10()
{
    auto node = &m_game.get_current();
    for (unsigned i = 0; i < 10; ++i)
    {
        auto child = node->get_first_child_or_null();
        if (child == nullptr)
            break;
        node = child;
    }
    gotoNode(node);
}

void GameModel::goNextVar()
{
    gotoNode(m_game.get_current().get_sibling());
}

void GameModel::goPrevVar()
{
    gotoNode(m_game.get_current().get_previous_sibling());
}

void GameModel::gotoBeginningOfBranch()
{
    gotoNode(beginning_of_branch(m_game.get_current()));
}

void GameModel::gotoMove(int n)
{
    if (n == 0)
        goBeginning();
    else if (n > 0)
        gotoNode(get_move_node(m_game.get_tree(), m_game.get_current(),
                               static_cast<unsigned>(n)));
}

void GameModel::gotoNode(const SgfNode& node)
{
    if (&node == &m_game.get_current())
        return;
    preparePositionChange();
    try
    {
        m_game.goto_node(node);
    }
    catch (const SgfError& error)
    {
        m_error = error.what();
        emit invalidSgfFile();
    }
    updateProperties();
}

void GameModel::gotoNode(const SgfNode* node)
{
    if (node)
        gotoNode(*node);
}

void GameModel::initGame(Variant variant)
{
    m_game.init(variant);
#ifdef VERSION
    m_game.set_application("Pentobi", VERSION);
#else
    m_game.set_application("Pentobi");
#endif
    m_game.set_date_today();
    m_game.set_charset("UTF-8");
    updateGameInfo();
}

void GameModel::initGameVariant(Variant variant)
{
    if (m_game.get_variant() != variant)
        initGame(variant);
    auto& bd = getBoard();
    set(m_nuColors, static_cast<unsigned>(bd.get_nu_colors()),
        &GameModel::nuColorsChanged);
    set(m_nuPlayers, bd.get_nu_players(), &GameModel::nuPlayersChanged);
    m_lastMovePieceModel = nullptr;
    createPieceModels();
    m_gameVariant = to_string_id(variant);
    emit gameVariantChanged();
    updateProperties();
}

bool GameModel::isLegalPos(PieceModel* pieceModel, const QString& state,
                           QPointF coord) const
{
    Move mv;
    if (! findMove(*pieceModel, state, coord, mv))
        return false;
    Color c(static_cast<Color::IntType>(pieceModel->color()));
    return getBoard().is_legal(c, mv);
}

bool GameModel::isLegalSetupPos(PieceModel* pieceModel, const QString& state,
                                QPointF coord) const
{
    Move mv;
    if (! findMove(*pieceModel, state, coord, mv))
        return false;
    auto& bd = getBoard();
    for (auto p : bd.get_move_points(mv))
        if (! bd.get_point_state(p).is_empty())
            return false;
    return true;
}

void GameModel::keepOnlyPosition()
{
    m_game.keep_only_position();
    updateProperties();
}

void GameModel::keepOnlySubtree()
{
    m_game.keep_only_subtree();
    updateProperties();
}

bool GameModel::loadAutoSave()
{
    {
        QSettings settings;
        auto file = settings.value(QStringLiteral("file")).toString();
        auto isModified =
                settings.value(QStringLiteral("isModified")).toBool();
        if (! file.isEmpty() && ! isModified)
        {
            if (! openFile(file))
                return false;
            updateFileInfo(file);
            m_autosaveDate = m_fileDate;
            // Note: some Qt versions crash when adding invalid QDateTime to
            // QSettings
            if (m_autosaveDate.isValid())
                settings.setValue(
                            QStringLiteral("autosaveDate"), m_autosaveDate);
            else
                settings.remove(QStringLiteral("autosaveDate"));
        }
        else
        {
            if (! openByteArray(settings.value(
                                    QStringLiteral("autosave")).toByteArray()))
                return false;
            m_fileDate = settings.value(
                        QStringLiteral("fileDate")).toDateTime();
            m_autosaveDate = settings.value(
                        QStringLiteral("autosaveDate")).toDateTime();
            setFile(file);
        }
        // Sanitize isModified if value from settings is inconsistent
        if (file.isEmpty() && ! libboardgame_base::is_empty(m_game.get_tree()))
            isModified = true;
        setIsModified(isModified);
    }
    restoreAutoSaveLocation();
    updateProperties();
    return true;
}

bool GameModel::openByteArray(const QByteArray& byteArray)
{
    istringstream in(byteArray.constData());
    clearFile();
    if (! openStream(in))
        return false;
    goEnd();
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
    preparePositionChange();
    auto& bd = getBoard();
    m_game.set_to_play(bd.get_next(bd.get_to_play()));
    setSetupPlayer();
    updateProperties();
}

PieceModel* GameModel::nextPiece(PieceModel* currentPickedPiece)
{
    auto& bd = getBoard();
    auto c = bd.get_to_play();
    if (bd.get_pieces_left(c).empty())
        return nullptr;
    auto nuUniqPieces = bd.get_nu_uniq_pieces();
    Piece::IntType i;
    if (currentPickedPiece != nullptr)
        i = static_cast<Piece::IntType>(
                    currentPickedPiece->getPiece().to_int() + 1);
    else
        i = 0;
    while (true)
    {
        if (i >= nuUniqPieces)
            i = 0;
        if (bd.is_piece_left(c, Piece(i)))
            break;
        ++i;
    }
    return findUnplayedPieceModel(c, Piece(i));
}

void GameModel::newGame()
{
    preparePositionChange();
    initGame(m_game.get_variant());
    setIsModified(false);
    clearFile();
    for (Color c : Color::Range(Color::range))
        for (auto& variant : as_const(m_pieceModels[c]))
        {
            auto pieceModel = qvariant_cast<PieceModel*>(variant);
            pieceModel->setDefaultState();
        }
    updateProperties();
}

bool GameModel::openClipboard()
{
    auto text = QGuiApplication::clipboard()->text();
    if (text.isEmpty())
    {
        m_error = tr("Clipboard is empty.");
        return false;
    }
    istringstream in(text.toLocal8Bit().constData());
    bool result;
    if (openStream(in))
    {
        auto& root = m_game.get_root();
        if (! has_setup(root) && root.has_children())
            goEnd();
        result = true;
    }
    else
        result = false;
    clearFile();
    setIsModified(true);
    return result;
}

bool GameModel::openFile(const QString& file)
{
#ifdef Q_OS_ANDROID
    if (! QUrl(file).isRelative())
    {
        QByteArray sgf;
        if (! AndroidUtils::open(file, sgf))
        {
            m_error = AndroidUtils::getError();
            return false;
        }
        string s(sgf.constData(), sgf.size());
        istringstream in(s);
        if (! openStream(in))
        {
            clearFile();
            return false;
        }
        updateFileInfo(file);
    }
    else
#endif
    {
        auto canonicalFile = QFileInfo(file).absoluteFilePath();
        ifstream in(canonicalFile.toLocal8Bit().constData());
        if (! in)
        {
            m_error = QString::fromLocal8Bit(strerror(errno));
            return false;
        }
        if (! openStream(in))
        {
            clearFile();
            return false;
        }
        updateFileInfo(canonicalFile);
    }
    auto& root = m_game.get_root();
    // Show end of game position by default unless the root node has
    // setup stones or comments, because then it might be a puzzle and
    // we don't want to show the solution.
    if (! has_setup(root) && ! has_comment(root) && root.has_children())
        goEnd();
    return true;
}

bool GameModel::openStream(istream& in)
{
    bool result = true;
    try
    {
        preparePositionChange();
        TreeReader reader;
        reader.read(in);
        auto root = reader.get_tree_transfer_ownership();
        m_game.init(root);
    }
    catch (const runtime_error& e)
    {
        m_error =
                tr("Invalid Blokus SGF file. (%1)")
                .arg(QString::fromLocal8Bit(e.what()));
        result = false;
    }
    auto charSet = to_lower(m_game.get_charset());
    if (! charSet.empty() && charSet != "utf-8" && charSet != "utf8")
    {
        m_error = tr("Unsupported character set");
        result = false;
    }
    if (! result)
        m_game.init();
    auto variant = to_string_id(m_game.get_variant());
    if (variant != m_gameVariant)
        initGameVariant(m_game.get_variant());
    setIsModified(false);
    updateGameInfo();
    updateProperties();
    return result;
}

PieceModel* GameModel::pickNamedPiece(const QString& name,
                                      PieceModel* currentPickedPiece)
{
    string nameStr(name.toLocal8Bit().constData());
    auto& bd = getBoard();
    auto c = bd.get_to_play();
    Board::PiecesLeftList pieces;
    for (Piece::IntType i = 0; i < bd.get_nu_uniq_pieces(); ++i)
    {
        Piece piece(i);
        if (bd.is_piece_left(c, piece)
                && bd.get_piece_info(piece).get_name().find(nameStr) == 0)
            pieces.push_back(piece);
    }
    if (pieces.empty())
        return nullptr;
    Piece piece;
    if (currentPickedPiece == nullptr)
        piece = pieces[0];
    else
    {
        piece = currentPickedPiece->getPiece();
        auto pos = std::find(pieces.begin(), pieces.end(), piece);
        if (pos == pieces.end())
            piece = pieces[0];
        else
        {
            ++pos;
            if (pos == pieces.end())
                piece = pieces[0];
            else
                piece = *pos;
        }
    }
    return findUnplayedPieceModel(c, piece);
}

void GameModel::playMove(GameMove* move)
{
    auto mv = move->get();
    if (mv.is_null())
        return;
    preparePositionChange();
    m_game.play(mv, false);
    updateProperties();
}

void GameModel::playPiece(PieceModel* pieceModel, QPointF coord)
{
    Color c(static_cast<Color::IntType>(pieceModel->color()));
    Move mv;
    if (! findMove(*pieceModel, pieceModel->state(), coord, mv))
        return;
    preparePositionChange();
    preparePieceGameCoord(pieceModel, mv);
    pieceModel->setIsPlayed(true);
    preparePieceTransform(pieceModel, mv);
    m_game.play(c, mv, false);
    updateProperties();
}

void GameModel::prepareFindMove()
{
    auto& bd = getBoard();
    auto c = bd.get_to_play();
    if (! m_legalMoves)
        m_legalMoves = make_unique<MoveList>();
    if (m_legalMoves->empty())
    {
        if (! m_marker)
            m_marker = make_unique<MoveMarker>();
        bd.gen_moves(c, *m_marker, *m_legalMoves);
        m_marker->clear(*m_legalMoves);
        sort(m_legalMoves->begin(), m_legalMoves->end(),
             [&](Move mv1, Move mv2) {
                 return getHeuristic(bd, mv1) > getHeuristic(bd, mv2);
             });
        m_legalMoveIndex = 0;
    }
}

PieceModel* GameModel::preparePiece(GameMove* move)
{
    if (move == nullptr || move->get().is_null())
        return nullptr;
    auto c = move->get().color;
    auto mv = move->get().move;
    auto piece = getBoard().get_move_piece(mv);
    for (auto& variant : as_const(m_pieceModels[c]))
    {
        auto pieceModel = qvariant_cast<PieceModel*>(variant);
        if (pieceModel->getPiece() == piece && ! pieceModel->isPlayed())
        {
            preparePieceTransform(pieceModel, mv);
            preparePieceGameCoord(pieceModel, mv);
            return pieceModel;
        }
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

void GameModel::preparePositionChange()
{
    if (m_legalMoves)
    {
        m_legalMoves->clear();
        m_legalMoveIndex = 0;
    }
    emit positionAboutToChange();
}

PieceModel* GameModel::previousPiece(PieceModel* currentPickedPiece)
{
    auto& bd = getBoard();
    auto c = bd.get_to_play();
    if (bd.get_pieces_left(c).empty())
        return nullptr;
    auto nuUniqPieces = bd.get_nu_uniq_pieces();
    Piece::IntType i;
    if (currentPickedPiece != nullptr)
        i = static_cast<Piece::IntType>(currentPickedPiece->getPiece().to_int());
    else
        i = 0;
    while (true)
    {
        if (i == 0)
            i = static_cast<Piece::IntType>(nuUniqPieces - 1);
        else
            --i;
        if (bd.is_piece_left(c, Piece(i)))
            break;
    }
    return findUnplayedPieceModel(c, Piece(i));
}

void GameModel::restoreAutoSaveLocation()
{
    QSettings settings;
    auto location =
            settings.value(QStringLiteral("autosaveLocation")).value<QVariantList>();
    if (location.empty())
        return;
    int index = 0;
    bool ok;
    auto depth = location[index++].toUInt(&ok);
    if (! ok)
        return;
    auto node = &m_game.get_root();
    while (depth > 0)
    {
        auto nuChildren = node->get_nu_children();
        if (nuChildren == 0)
            break;
        if (nuChildren == 1)
            node = &node->get_first_child();
        else
        {
            if (index >= location.size())
                break;
            auto child = location[index++].toUInt(&ok);
            if (! ok || child >= nuChildren)
                break;
            node = &node->get_child(child);
        }
        --depth;
    }
    gotoNode(*node);
}

bool GameModel::save(const QString& file)
{
    auto sgf = getSgf(1);
#ifdef Q_OS_ANDROID
    if (! QUrl(file).isRelative())
    {
        if (! AndroidUtils::save(file, sgf))
        {
            m_error = AndroidUtils::getError();
            return false;
        }
    }
    else
#endif
    {
        ofstream out(file.toLocal8Bit().constData());
        out.write(sgf.constData(), sgf.size());
        if (! out)
        {
            m_error = QString::fromLocal8Bit(strerror(errno));
            return false;
        }
    }
    updateFileInfo(file);
    setIsModified(false);
    return true;
}

bool GameModel::saveAsciiArt(const QString& file)
{
#ifdef Q_OS_ANDROID
    ostringstream out;
    getBoard().write(out, false);
    string s(out.str());
    QByteArray array(&*s.begin(), s.size());
    if (! AndroidUtils::save(file, array))
    {
        m_error = AndroidUtils::getError();
        return false;
    }
    return true;
#else
    ofstream out(file.toLocal8Bit().constData());
    getBoard().write(out, false);
    if (! out)
    {
        m_error = QString::fromLocal8Bit(strerror(errno));
        return false;
    }
    return true;
#endif
}

template<typename T>
bool GameModel::set(T& target, const T& value,
                    void (GameModel::*changedSignal)())
{
    if (target != value)
    {
        target = value;
        emit (this->*changedSignal)();
        return true;
    }
    return false;
}

void GameModel::setComment(const QString& comment)
{
    if (comment == m_comment)
        return;
    m_game.set_comment(encode(comment).constData());
    m_comment = comment;
    emit commentChanged();
    updateIsModified();
}

void GameModel::setDate(const QString& date)
{
    if (date == m_date)
        return;
    m_date = date;
    m_game.set_date(encode(date).constData());
    emit dateChanged();
    updateIsModified();
}

void GameModel::setEvent(const QString& event)
{
    if (event == m_event)
        return;
    m_event = event;
    m_game.set_event(encode(event).constData());
    emit eventChanged();
    updateIsModified();
}

void GameModel::setFile(const QString& file)
{
    if (file == m_file)
        return;
    m_file = file;
    emit fileChanged();
}

void GameModel::setIsModified(bool isModified)
{
    m_game.set_modified(isModified);
    updateIsModified();
}

void GameModel::setMoveAnnotationAtNode(const SgfNode& node,
                                        const QString& annotation)
{
    m_game.remove_move_annotation(node);
    if (annotation == QStringLiteral("!"))
        m_game.set_good_move(node);
    else if (annotation == QStringLiteral("‼"))
        m_game.set_good_move(node, 2);
    else if (annotation == QStringLiteral("?"))
        m_game.set_bad_move(node);
    else if (annotation == QStringLiteral("⁇"))
        m_game.set_bad_move(node, 2);
    else if (annotation == QStringLiteral("⁉"))
        m_game.set_interesting_move(node);
    else if (annotation == QStringLiteral("⁈"))
        m_game.set_doubtful_move(node);
    updateIsModified();
    updatePositionInfo();
    updatePieces();
}

void GameModel::setMoveAnnotation(int moveNumber, const QString& annotation)
{
    if (moveNumber <= 0)
        return;
    auto node = get_move_node(m_game.get_tree(), m_game.get_current(),
                              static_cast<unsigned>(moveNumber));
    if (node == nullptr)
        return;
    setMoveAnnotationAtNode(*node, annotation);
}

void GameModel::setPlayerName0(const QString& name)
{
    if (name == m_playerName0)
        return;
    m_playerName0 = name;
    m_game.set_player_name(Color(0), encode(name).constData());
    emit playerName0Changed();
    updateIsModified();
}

void GameModel::setPlayerName1(const QString& name)
{
    if (name == m_playerName1)
        return;
    m_playerName1 = name;
    m_game.set_player_name(Color(1), encode(name).constData());
    emit playerName1Changed();
    updateIsModified();
}

void GameModel::setPlayerName2(const QString& name)
{
    if (name == m_playerName2)
        return;
    m_playerName2 = name;
    m_game.set_player_name(Color(2), encode(name).constData());
    emit playerName2Changed();
    updateIsModified();
}

void GameModel::setPlayerName3(const QString& name)
{
    if (name == m_playerName3)
        return;
    m_playerName3 = name;
    m_game.set_player_name(Color(3), encode(name).constData());
    emit playerName3Changed();
    updateIsModified();
}

void GameModel::setRound(const QString& round)
{
    if (round == m_round)
        return;
    m_round = round;
    m_game.set_round(encode(round).constData());
    emit roundChanged();
    updateIsModified();
}

void GameModel::setShowVariations(bool showVariations)
{
    if (set(m_showVariations, showVariations, &GameModel::showVariationsChanged))
        updatePieces();
}

void GameModel::setSetupPlayer()
{
    if (! m_game.has_setup())
        m_game.remove_player();
    else
        m_game.set_player(getBoard().get_to_play());
}

void GameModel::setTime(const QString& time)
{
    if (time == m_time)
        return;
    m_time = time;
    m_game.set_time(encode(time).constData());
    emit timeChanged();
    updateIsModified();
}

QString GameModel::suggestFileName(const QUrl& folder,
                                   const QString& fileEnding)
{
    QString suffix = QStringLiteral(".") + fileEnding;
    if (folder.isEmpty())
        return tr("Untitled") + suffix;
    auto localFolder = folder.toLocalFile();
    QString file = localFolder + '/' + tr("Untitled") + suffix;
    if (QFileInfo::exists(file))
        for (unsigned i = 1; ; ++i)
        {
            //: The argument is a number, which will be increased if a
            //: file with the same name already exists
            file = localFolder + '/' + tr("Untitled %1").arg(i)
                    + suffix;
            if (! QFileInfo::exists(file))
                break;
        }
    return QUrl::fromLocalFile(file).fileName();
}

QString GameModel::suggestGameFileName(const QUrl& folder)
{
#ifndef Q_OS_ANDROID
    if (! m_file.isEmpty())
        return QUrl::fromLocalFile(m_file).fileName();
#endif
    return suggestFileName(folder, QStringLiteral("blksgf"));
}

QString GameModel::suggestNewFolderName(const QUrl& folder)
{
    auto localFolder = folder.toLocalFile();
    QString file = localFolder;
    if (! file.endsWith('/'))
        file.append('/');
    file.append(tr("New Folder"));
    if (QFileInfo::exists(file))
        for (unsigned i = 1; ; ++i)
        {
            //: The argument is a number, which will be increased if a
            //: folder with the same name already exists
            file = localFolder + '/' + tr("New Folder %1").arg(i);
            if (! QFileInfo::exists(file))
                break;
        }
    return QUrl::fromLocalFile(file).fileName();
}

void GameModel::truncate()
{
    if (! m_game.get_current().has_parent())
        return;
    preparePositionChange();
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
    preparePositionChange();
    m_game.undo();
    updateProperties();
}

void GameModel::updateFileInfo(const QString& file)
{
    setFile(file);
    m_fileDate = QFileInfo(file).lastModified();
}

void GameModel::updateGameInfo()
{
    static_assert(Color::range == 4);
    setPlayerName0(decode(m_game.get_player_name(Color(0))));
    setPlayerName1(decode(m_game.get_player_name(Color(1))));
    if (m_nuPlayers > 2)
        setPlayerName2(decode(m_game.get_player_name(Color(2))));
    if (m_nuPlayers > 3)
        setPlayerName3(decode(m_game.get_player_name(Color(3))));
    setDate(decode(m_game.get_date()));
    setTime(decode(m_game.get_time()));
    setEvent(decode(m_game.get_event()));
    setRound(decode(m_game.get_round()));
}

void GameModel::updateIsModified()
{
    // Don't consider modified game tree as modified if it is empty and no
    // file is associated.
    bool isModified =
            m_game.is_modified()
            && (! libboardgame_base::is_empty(m_game.get_tree())
                || ! m_file.isEmpty());
    set(m_isModified, isModified, &GameModel::isModifiedChanged);
}

PieceModel* GameModel::updatePiece(Color c, Move mv,
                                   array<bool, Board::max_pieces>& isPlayed)
{
    auto& bd = getBoard();
    Piece piece = bd.get_move_piece(mv);
    auto& pieceInfo = bd.get_piece_info(piece);
    auto gameCoord = getGameCoord(bd, mv);
    auto transform = bd.find_transform(mv);
    auto& pieceModels = m_pieceModels[c];
    // Prefer piece models already played with the given gameCoord and
    // transform because class Board doesn't make a distinction between
    // instances of the same piece (in Junior) and we want to avoid
    // unwanted piece movement animations to switch instances.
    for (int i = 0; i < pieceModels.length(); ++i)
    {
        auto pieceModel = qvariant_cast<PieceModel*>(pieceModels[i]);
        if (pieceModel->getPiece() == piece
                && pieceModel->isPlayed()
                && compareGameCoord(pieceModel->gameCoord(), gameCoord)
                && compareTransform(pieceInfo, pieceModel->getTransform(),
                                    transform))
        {
            isPlayed[i] = true;
            return pieceModel;
        }
    }
    for (int i = 0; i < pieceModels.length(); ++i)
    {
        auto pieceModel = qvariant_cast<PieceModel*>(pieceModels[i]);
        if (pieceModel->getPiece() == piece && ! isPlayed[i])
        {
            isPlayed[i] = true;
            // Set PieceModel.isPlayed temporarily to false, such that there is
            // always a state transition animation (e.g. if the piece stays
            // on the board but changes its coordinates when navigating through
            // move variations).
            pieceModel->setIsPlayed(false);
            // Set gameCoord before isPlayed because the animation needs it.
            pieceModel->setGameCoord(gameCoord);
            pieceModel->setIsPlayed(true);
            pieceModel->setTransform(transform);
            return pieceModel;
        }
    }
    LIBBOARDGAME_ASSERT(false);
    return nullptr;
}

void GameModel::updatePieces()
{
    auto& bd = getBoard();
    ColorMap<array<bool, Board::max_pieces>> isPlayed;

    // Update pieces of setup
    for (Color c : bd.get_colors())
    {
        isPlayed[c].fill(false);
        for (Move mv : bd.get_setup().placements[c])
        {
            auto pieceModel = updatePiece(c, mv, isPlayed[c]);
            pieceModel->setMoveLabel(QString());
        }
    }

    // Update pieces of moves played after last setup or root
    auto& tree = m_game.get_tree();
    // We need to loop forward through the moves to ensure the persistence of
    // the GUI pieces, see comment in updatePiece()
    ArrayList<const SgfNode*, Board::max_moves> nodes;
    auto node = &m_game.get_current();
    do
    {
        if (tree.has_move(*node))
            nodes.push_back(node);
        if (has_setup(*node) || nodes.size() == decltype(nodes)::max_size)
            break;
        node = node->get_parent_or_null();
    }
    while (node);
    PieceModel* pieceModel = nullptr;
    int moveNumber = 1;
    for (auto i = nodes.size(); i > 0; --i)
    {
        node = nodes[i - 1];
        auto mv = tree.get_move(*node);
        auto c = mv.color;
        pieceModel = updatePiece(c, mv.move, isPlayed[c]);
        QString label = QString::number(moveNumber);
        ++moveNumber;
        unsigned moveIndex;
        if (m_showVariations && getVariationIndex(tree, *node, moveIndex))
            label.append(get_letter_coord(moveIndex).c_str());
        label.append(getMoveAnnotationAtNode(*node));
        pieceModel->setMoveLabel(label);
    }
    if (pieceModel != m_lastMovePieceModel)
    {
        if (m_lastMovePieceModel != nullptr)
            m_lastMovePieceModel->setIsLastMove(false);
        if (pieceModel != nullptr)
            pieceModel->setIsLastMove(true);
        m_lastMovePieceModel = pieceModel;
    }

    // Update pieces not on board
    for (Color c : bd.get_colors())
        for (int i = 0; i < m_pieceModels[c].length(); ++i)
        {
            auto pieceModel = qvariant_cast<PieceModel*>(m_pieceModels[c][i]);
            if (! isPlayed[c][i] && pieceModel->isPlayed())
            {
                pieceModel->setDefaultState();
                pieceModel->setIsPlayed(false);
                pieceModel->setMoveLabel(QString());
            }
        }
}

void GameModel::updatePositionInfo()
{
    auto& tree = m_game.get_tree();
    auto& current = m_game.get_current();
    auto& bd = m_game.get_board();
    auto move = get_move_number(tree, current);
    auto left = get_moves_left(tree, current);
    auto total = move + left;
    auto variation = get_variation_string(current);
    QString positionInfo = QString::number(move);
    if (left > 0 || move > 0)
        positionInfo.append(getMoveAnnotationAtNode(current));
    if (left > 0)
    {
        positionInfo.append('/');
        positionInfo.append(QString::number(total));
    }
    if (! variation.empty())
    {
        positionInfo.append(" (");
        positionInfo.append(QString::fromLocal8Bit(variation.c_str()));
        positionInfo.append(')');
    }
    auto positionInfoShort = positionInfo;
    if (positionInfo.isEmpty())
    {
        positionInfo = bd.has_setup() ? tr("(Setup)") : tr("(No moves)");
        positionInfoShort = bd.has_setup() ? tr("(Setup)") : QString();
    }
    else
    {
        //: The argument is the current move number.
        positionInfo = tr("Move %1").arg(positionInfo);
        if (bd.get_nu_moves() == 0 && bd.has_setup())
        {
            positionInfo.append(' ');
            positionInfo.append(tr("(Setup)"));
            positionInfoShort.append(' ');
            positionInfoShort.append(tr("(Setup)"));
        }
    }
    set(m_positionInfo, positionInfo, &GameModel::positionInfoChanged);
    set(m_positionInfoShort, positionInfoShort,
        &GameModel::positionInfoShortChanged);
}

/** Update all properties that might change when changing the current
    position in the game tree. */
void GameModel::updateProperties()
{
    auto& bd = getBoard();
    auto& geo = bd.get_geometry();
    auto& tree = m_game.get_tree();
    bool isGembloQ = (bd.get_piece_set() == PieceSet::gembloq);
    bool isTrigon = (bd.get_piece_set() == PieceSet::trigon);
    bool isNexos = (bd.get_board_type() == BoardType::nexos);
    set(m_points0, bd.get_points(Color(0)), &GameModel::points0Changed);
    set(m_points1, bd.get_points(Color(1)), &GameModel::points1Changed);
    set(m_bonus0, bd.get_bonus(Color(0)), &GameModel::bonus0Changed);
    set(m_bonus1, bd.get_bonus(Color(1)), &GameModel::bonus1Changed);
    set(m_hasMoves0, bd.has_moves(Color(0)), &GameModel::hasMoves0Changed);
    set(m_hasMoves1, bd.has_moves(Color(1)), &GameModel::hasMoves1Changed);
    bool isFirstPieceAny = false;
    if (m_nuColors > 2)
    {
        set(m_points2, bd.get_points(Color(2)), &GameModel::points2Changed);
        set(m_bonus2, bd.get_bonus(Color(2)), &GameModel::bonus2Changed);
        set(m_hasMoves2, bd.has_moves(Color(2)), &GameModel::hasMoves2Changed);
    }
    if (m_nuColors > 3)
    {
        set(m_points3, bd.get_points(Color(3)), &GameModel::points3Changed);
        set(m_bonus3, bd.get_bonus(Color(3)), &GameModel::bonus3Changed);
        set(m_hasMoves3, bd.has_moves(Color(3)), &GameModel::hasMoves3Changed);
    }
    m_tmpPoints.clear();
    if (bd.is_first_piece(Color(0)))
    {
        isFirstPieceAny = true;
        if (isNexos)
            m_tmpPoints.append(QPointF(4, 4));
        else if (isGembloQ)
            m_tmpPoints.append(getGembloQStartingPoint(bd, Color(0)));
        else if (! isTrigon)
            for (Point p : bd.get_starting_points(Color(0)))
                m_tmpPoints.append(QPointF(geo.get_x(p), geo.get_y(p)));
    }
    set(m_startingPoints0, m_tmpPoints, &GameModel::startingPoints0Changed);
    m_tmpPoints.clear();
    if (bd.is_first_piece(Color(1)))
    {
        isFirstPieceAny = true;
        if (isNexos)
            m_tmpPoints.append(QPointF(20, 4));
        else if (isGembloQ)
            m_tmpPoints.append(getGembloQStartingPoint(bd, Color(1)));
        else if (! isTrigon)
            for (Point p : bd.get_starting_points(Color(1)))
                m_tmpPoints.append(QPointF(geo.get_x(p), geo.get_y(p)));
    }
    set(m_startingPoints1, m_tmpPoints, &GameModel::startingPoints1Changed);
    m_tmpPoints.clear();
    if (m_nuColors > 2 && bd.is_first_piece(Color(2)))
    {
        isFirstPieceAny = true;
        if (isNexos)
            m_tmpPoints.append(QPointF(20, 20));
        else if (isGembloQ)
            m_tmpPoints.append(getGembloQStartingPoint(bd, Color(2)));
        else if (! isTrigon)
            for (Point p : bd.get_starting_points(Color(2)))
                m_tmpPoints.append(QPointF(geo.get_x(p), geo.get_y(p)));
    }
    set(m_startingPoints2, m_tmpPoints, &GameModel::startingPoints2Changed);
    m_tmpPoints.clear();
    if (m_nuColors > 3 && bd.is_first_piece(Color(3)))
    {
        isFirstPieceAny = true;
        if (isNexos)
            m_tmpPoints.append(QPointF(4, 20));
        else if (isGembloQ)
            m_tmpPoints.append(getGembloQStartingPoint(bd, Color(3)));
        else if (! isTrigon)
            for (Point p : bd.get_starting_points(Color(3)))
                m_tmpPoints.append(QPointF(geo.get_x(p), geo.get_y(p)));
    }
    set(m_startingPoints3, m_tmpPoints, &GameModel::startingPoints3Changed);
    m_tmpPoints.clear();
    if (isTrigon && isFirstPieceAny)
        for (Point p : bd.get_starting_points(Color(0)))
            m_tmpPoints.append(QPointF(geo.get_x(p), geo.get_y(p)));
    set(m_startingPointsAny, m_tmpPoints,
        &GameModel::startingPointsAnyChanged);
    auto& current = m_game.get_current();
    set(m_canUndo,
           ! current.has_children() && tree.has_move(current)
           && current.has_parent(),
           &GameModel::canUndoChanged);
    set(m_canGoForward, current.has_children(),
        &GameModel::canGoForwardChanged);
    set(m_canGoBackward, current.has_parent(),
        &GameModel::canGoBackwardChanged);
    set(m_hasPrevVar, (current.get_previous_sibling() != nullptr),
        &GameModel::hasPrevVarChanged);
    set(m_hasNextVar, (current.get_sibling() != nullptr),
        &GameModel::hasNextVarChanged);
    set(m_hasVariations, tree.has_variations(),
        &GameModel::hasVariationsChanged);
    set(m_hasEarlierVar, has_earlier_variation(current),
        &GameModel::hasEarlierVarChanged);
    set(m_isMainVar, is_main_variation(current), &GameModel::isMainVarChanged);
    set(m_moveNumber, static_cast<int>(get_move_number(tree, current)),
        &GameModel::moveNumberChanged);
    set(m_movesLeft, static_cast<int>(get_moves_left(tree, current)),
        &GameModel::movesLeftChanged);
    updatePositionInfo();
    bool isGameOver = true;
    for (Color c : bd.get_colors())
        if (bd.has_moves(c))
        {
            isGameOver = false;
            break;
        }
    set(m_isBoardEmpty, bd.get_nu_onboard_pieces() == 0,
        &GameModel::isBoardEmptyChanged);
    set(m_isGameOver, isGameOver, &GameModel::isGameOverChanged);
    updateIsModified();
    updatePieces();
    set(m_comment, decode(m_game.get_comment()), &GameModel::commentChanged);
    set(m_toPlay, m_isGameOver ? 0u : bd.get_effective_to_play().to_int(),
        &GameModel::toPlayChanged);
    set(m_altPlayer,
        bd.get_variant() == Variant::classic_3 ? bd.get_alt_player() : 0u,
        &GameModel::altPlayerChanged);

    emit positionChanged();
}

//-----------------------------------------------------------------------------
