//-----------------------------------------------------------------------------
/** @file learn_tool/Main.cpp
    Learn the parameters used in libpentobi_mcts/PriorKnowledge from existing
    games with softmax training.

    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include <fstream>
#include <random>
#include "libboardgame_base/FmtSaver.h"
#include "libboardgame_base/Log.h"
#include "libboardgame_base/Options.h"
#include "libboardgame_base/TreeReader.h"
#include "libpentobi_base/Game.h"
#include "libpentobi_base/MoveMarker.h"
#include "libpentobi_mcts/LocalPoints.h"

using namespace std;
using libboardgame_base::split;
using libboardgame_base::FmtSaver;
using libboardgame_base::Options;
using libboardgame_base::TreeReader;
using libpentobi_base::Board;
using libpentobi_base::BoardConst;
using libpentobi_base::Color;
using libpentobi_base::Game;
using libpentobi_base::Geometry;
using libpentobi_base::GridExt;
using libpentobi_base::Move;
using libpentobi_base::MoveList;
using libpentobi_base::MoveMarker;
using libpentobi_base::PointList;
using libpentobi_base::Variant;
using libpentobi_mcts::LocalPoints;

//-----------------------------------------------------------------------------

namespace {

/** Features, see PriorKnowledge::m_gamma_... */
enum {
    point_other,
    point_opp_attach_or_nb,
    point_second_color_attach,
    adj_connect,
    adj_occupied_other,
    adj_forbidden_other,
    adj_own_attach,
    adj_nonforbidden,
    attach_to_play,
    attach_forbidden_other,
    attach_nonforbidden_0,
    attach_nonforbidden_1,
    attach_nonforbidden_2,
    attach_nonforbidden_3,
    attach_nonforbidden_4,
    attach_nonforbidden_5,
    attach_nonforbidden_6,
    attach_second_color,
    local_move,
    piece_score_0,
    piece_score_1,
    piece_score_2,
    piece_score_3,
    piece_score_4,
    piece_score_5,
    piece_score_6,
    _nu_features
};

struct Features
{
    using IntType = uint_least8_t;


    array<IntType, _nu_features> feature;


    Features() { feature.fill(0); }

    void operator+=(const Features& f)
    {
        for (unsigned i = 0; i < _nu_features; ++i)
             feature[i] = static_cast<IntType>(feature[i] + f.feature[i]);
    }

    void operator|=(const Features& f)
    {
        for (unsigned i = 0; i < _nu_features; ++i)
             feature[i] = feature[i] | f.feature[i];
    }
};

struct Sample
{
    unsigned played_move;

    vector<Features> features;
};


using Float = double;

const Float step_size = 0.05;

MoveMarker marker;

MoveList moves;

MoveList tmp_moves;

long nu_games;

long nu_positions;

long nu_moves;

random_device rand_dev;

mt19937 rand_gen(rand_dev());

vector<Float> probs;

array<Float, _nu_features> weights;

array<Float, _nu_features> grad_weights;

GridExt<Features> feature_grid_point;

GridExt<Features> feature_grid_adj;

GridExt<Features> feature_grid_attach;

vector<Sample> samples;

LocalPoints local_points;

Features feature_occured_globally;


/** This function mirrors what happens in PriorKnowledge::gen_children, but
    produces feature vectors instead of a gamma value for each move. */
template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH, bool IS_CALLISTO>
void add_sample(const Board& bd, Color to_play, Move played_mv)
{
    marker.clear();
    bd.gen_moves(to_play, marker, moves);
    nu_moves += moves.size();

    local_points.init<MAX_SIZE, MAX_ADJ_ATTACH>(bd);
    auto& geo = bd.get_geometry();
    auto variant = bd.get_variant();
    auto& is_forbidden = bd.is_forbidden(to_play);
    Color second_color;
    Color connect_color;
    if (variant == Variant::classic_3 && to_play.to_int() == 3)
    {
        second_color = Color(bd.get_alt_player());
        connect_color = to_play;
    }
    else
    {
        second_color = bd.get_second_color(to_play);
        connect_color = second_color;
    }
    for (auto p : geo)
    {
        feature_grid_point[p] = Features();
        feature_grid_adj[p] = Features();
        feature_grid_attach[p] = Features();
    }
    for (auto p : geo)
    {
        auto& feature_point = feature_grid_point[p].feature;
        auto& feature_adj = feature_grid_adj[p].feature;
        auto& feature_attach = feature_grid_attach[p].feature;
        auto s = bd.get_point_state(p);
        if (is_forbidden[p])
        {
            if (s != to_play)
                feature_attach[attach_forbidden_other] = 1;
            else
                feature_attach[attach_to_play] = 1;
            if (s == connect_color)
                feature_adj[adj_connect] = 1;
            else if (! s.is_empty())
                feature_adj[adj_occupied_other] = 1;
            else
                feature_adj[adj_forbidden_other] = 1;
        }
        else
        {
            feature_point[point_other] = 1;
            if (bd.is_attach_point(p, to_play))
                feature_adj[adj_own_attach] = 1;
            else
                feature_adj[adj_nonforbidden] = 1;
            unsigned n = 0;
            if (MAX_SIZE == 7 || IS_CALLISTO)
            {
                LIBBOARDGAME_ASSERT(geo.get_adj(p).empty());
                for (auto pa : geo.get_diag(p))
                    n += 1u - static_cast<unsigned>(is_forbidden[pa]);
            }
            else
                for (auto pa : geo.get_adj(p))
                    n += 1u - static_cast<unsigned>(is_forbidden[pa]);
            switch (n)
            {
            case 0: feature_attach[attach_nonforbidden_0] = 1; break;
            case 1: feature_attach[attach_nonforbidden_1] = 1; break;
            case 2: feature_attach[attach_nonforbidden_2] = 1; break;
            case 3: feature_attach[attach_nonforbidden_3] = 1; break;
            case 4: feature_attach[attach_nonforbidden_4] = 1; break;
            case 5: feature_attach[attach_nonforbidden_5] = 1; break;
            default: feature_attach[attach_nonforbidden_6] = 1; break;
            }
        }
    }
    for (Color c : bd.get_colors())
    {
        if (c == to_play || c == second_color)
            continue;
        auto& is_forbidden = bd.is_forbidden(c);
        for (auto p : bd.get_attach_points(c))
            if (! is_forbidden[p])
            {
                feature_grid_point[p].feature[point_other] = 0;
                feature_grid_point[p].feature[point_opp_attach_or_nb] = 1;
                for (auto j : geo.get_adj(p))
                    if (! is_forbidden[j])
                    {
                        feature_grid_point[j].feature[point_other] = 0;
                        feature_grid_point[j].feature[point_opp_attach_or_nb] = 1;
                    }
            }
    }
    if (second_color != to_play)
    {
        auto& is_forbidden_second_color = bd.is_forbidden(second_color);
        for (auto p : bd.get_attach_points(second_color))
            if (! is_forbidden_second_color[p])
            {
                feature_grid_point[p].feature[point_second_color_attach] = 1;
                if (! is_forbidden[p])
                    feature_grid_attach[p].feature[attach_second_color] = 1;
            }
    }

    Sample sample;
    sample.played_move = moves.size() + 1;
    sample.features.reserve(moves.size());
    auto& bc = bd.get_board_const();
    auto move_info_array = bc.get_move_info_array();
    auto move_info_ext_array = bc.get_move_info_ext_array();
    for (unsigned i = 0; i < moves.size(); ++i)
    {
        auto mv = moves[i];
        if (mv == played_mv)
            sample.played_move = i;
        auto& info_ext = BoardConst::get_move_info_ext<MAX_ADJ_ATTACH>(
                    mv, move_info_ext_array);
        auto& info = BoardConst::get_move_info<MAX_SIZE>(mv, move_info_array);
        auto j = info.begin();
        Features features = feature_grid_point[*j];
        bool local = local_points.contains(*j);
        for (unsigned k = 1; k < MAX_SIZE; ++k)
        {
            ++j;
            features += feature_grid_point[*j];
            local |= local_points.contains(*j);
        }
        if (local)
            features.feature[local_move] = 1;
        if (MAX_SIZE == 7 || IS_CALLISTO)
        {
            j = info_ext.begin_attach();
            auto end = info_ext.end_attach();
            features += feature_grid_attach[*j];
            while (++j != end)
            {
                features += feature_grid_adj[*j];
                features += feature_grid_attach[*j];
            }
        }
        else
        {
            j = info_ext.begin_attach();
            auto end = info_ext.end_attach();
            features += feature_grid_attach[*j];
            while (++j != end)
                features += feature_grid_attach[*j];
            j = info_ext.begin_adj();
            end = info_ext.end_adj();
            for ( ; j != end; ++j)
                features += feature_grid_adj[*j];
        }
        switch (static_cast<unsigned>(bd.get_piece_info(info.get_piece()).get_score_points()))
        {
        case 0: features.feature[piece_score_0] = 1; break;
        case 1: features.feature[piece_score_1] = 1; break;
        case 2: features.feature[piece_score_2] = 1; break;
        case 3: features.feature[piece_score_3] = 1; break;
        case 4: features.feature[piece_score_4] = 1; break;
        case 5: features.feature[piece_score_5] = 1; break;
        default: features.feature[piece_score_6] = 1; break;
        }
        sample.features.push_back(features);
        feature_occured_globally |= features;
    }
    if (sample.played_move == moves.size() + 1)
        throw runtime_error("game contains illegal move");
    samples.push_back(sample);
}

void gen_train_data(const string& file, Variant& variant)
{
    ifstream in(file);
    if (! in)
        throw runtime_error("could not open " + file);
    Game game(variant);
    auto& bd = game.get_board();
    TreeReader reader;
    bool has_more;
    do
    {
        has_more = reader.read(in, false);
        auto tree = reader.get_tree_transfer_ownership();
        game.init(tree);
        if (nu_games > 0 && game.get_variant() != variant)
            throw runtime_error("Files have inconsistent game variants");
        ++nu_games;
        variant = game.get_variant();
        auto max_piece_size = bd.get_board_const().get_max_piece_size();
        auto node = &game.get_root();
        do
        {
            auto mv = game.get_tree().get_move(*node);
            if (! mv.is_null() && node->has_parent())
            {
                ++nu_positions;
                game.goto_node(node->get_parent());
                game.set_to_play(mv.color);
                if (max_piece_size == 5 && bd.is_callisto())
                    add_sample<5, 16, true>(bd, mv.color, mv.move);
                else if (max_piece_size == 5)
                    add_sample<5, 16, false>(bd, mv.color, mv.move);
                else if (max_piece_size == 6)
                    add_sample<6, 22, false>(bd, mv.color, mv.move);
                else if (max_piece_size == 7)
                    add_sample<7, 12, false>(bd, mv.color, mv.move);
                else
                    add_sample<22, 44, false>(bd, mv.color, mv.move);
            }
            node = node->get_first_child_or_null();
        }
        while (node != nullptr);
        cerr << '.';
        if (nu_games % 79 == 0)
            cerr << '\n';
    }
    while (has_more);
}

void print_weight(unsigned i, const char* name, bool is_member = true)
{
    if (is_member)
        cout << "m_";
    cout << "gamma_" << name << " = ";
    if (feature_occured_globally.feature[i] == 0u)
        cout << "1; // unused\n";
    else
        cout << "exp(" << weights[i] << "f / temperature);\n";
}

void print_weights()
{
    FmtSaver saver(cout);
    cout << std::fixed << setprecision(3);
    print_weight(point_other, "point_other");
    print_weight(point_opp_attach_or_nb, "point_opp_attach_or_nb");
    print_weight(point_second_color_attach, "point_second_color_attach");
    print_weight(adj_connect, "adj_connect");
    print_weight(adj_occupied_other, "adj_occupied_other");
    print_weight(adj_forbidden_other, "adj_forbidden_other");
    print_weight(adj_own_attach, "adj_own_attach");
    print_weight(adj_nonforbidden, "adj_nonforbidden");
    print_weight(attach_to_play, "attach_to_play");
    print_weight(attach_forbidden_other, "attach_forbidden_other");
    print_weight(attach_nonforbidden_0, "attach_nonforbidden[0]");
    print_weight(attach_nonforbidden_1, "attach_nonforbidden[1]");
    print_weight(attach_nonforbidden_2, "attach_nonforbidden[2]");
    print_weight(attach_nonforbidden_3, "attach_nonforbidden[3]");
    print_weight(attach_nonforbidden_4, "attach_nonforbidden[4]");
    print_weight(attach_nonforbidden_5, "attach_nonforbidden[5]");
    print_weight(attach_nonforbidden_6, "attach_nonforbidden[6]");
    print_weight(attach_second_color, "attach_second_color");
    print_weight(local_move, "local");
    print_weight(piece_score_0, "piece_score_0", false);
    print_weight(piece_score_1, "piece_score_1", false);
    print_weight(piece_score_2, "piece_score_2", false);
    print_weight(piece_score_3, "piece_score_3", false);
    print_weight(piece_score_4, "piece_score_4", false);
    print_weight(piece_score_5, "piece_score_5", false);
    print_weight(piece_score_6, "piece_score_6", false);
}

void init_weights()
{
    normal_distribution<Float> distribution(0, 0.01);
    for (auto& w : weights)
        w = distribution(rand_gen);
}

/** Gradient descent step using softmax training. */
void train_step(unsigned step, bool print)
{
    for (auto& w : grad_weights)
        w = 0;

    Float cost = 0;
    for (auto& sample : samples)
    {
        auto nu_moves = sample.features.size();
        probs.resize(nu_moves);
        Float sum = 0;
        for (size_t i = 0; i < nu_moves; ++i)
        {
            auto& feature = sample.features[i].feature;
            probs[i] = 0;
            for (unsigned j = 0; j < _nu_features; ++j)
                probs[i] += weights[j] * feature[j];
            probs[i] = exp(probs[i]);
            sum += probs[i];
        }
        for (size_t i = 0; i < nu_moves; ++i)
            probs[i] /= sum;
        for (size_t i = 0; i < nu_moves; ++i)
        {
            auto p = probs[i];
            auto& feature = sample.features[i].feature;
            if (i == sample.played_move)
            {
                for (unsigned j = 0; j < _nu_features; ++j)
                    grad_weights[j] -= (1 - p) * feature[j];
            }
            else
            {
                for (unsigned j = 0; j < _nu_features; ++j)
                    grad_weights[j] -= (-p) * feature[j];
            }
        }
        cost += -log(probs[sample.played_move]);
    }

    auto nu_samples = static_cast<Float>(samples.size());
    Float decay = 1e-3;
    for (unsigned i = 0; i < _nu_features; ++i)
    {
        auto& w = weights[i];
        auto dw = grad_weights[i] / nu_samples + decay * w;
        w += -step_size * dw;
    }

    cost /= nu_samples;

    if (print)
    {
        LIBBOARDGAME_LOG("Step ", step);
        LIBBOARDGAME_LOG("Cost ", cost);
        print_weights();
    }
}

void train(const string& file_list, unsigned steps)
{
    nu_games = 0;
    nu_positions = 0;
    nu_moves = 0;
        auto files = split(file_list, ',');
    Variant variant = Variant::classic_2;
    for (auto& file : files)
        gen_train_data(file, variant);
    cerr << '\n';
    LIBBOARDGAME_LOG("Files: ", file_list);
    LIBBOARDGAME_LOG(nu_games, " games");
    LIBBOARDGAME_LOG(nu_positions, " positions");
    if (nu_positions == 0)
        return;
    LIBBOARDGAME_LOG(double(nu_moves) / double(nu_positions), " moves/pos");
    init_weights();
    for (unsigned i = 1; i <= steps; ++i)
        train_step(i, i % 100 == 0 || i == steps);
}

} // namespace

//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    libboardgame_base::LogInitializer log_initializer;
    try
    {
        vector<string> specs = {
            "sgffiles:",
            "steps:"
        };
        Options opt(argc, argv, specs);
        train(opt.get("sgffiles"), opt.get<unsigned>("steps", 3000));
    }
    catch (const exception& e)
    {
        LIBBOARDGAME_LOG("Error: ", e.what());
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
