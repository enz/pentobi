//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_mcts/SearchTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libpentobi_mcts/Search.h"

#include "libboardgame_sgf/SgfUtil.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_test/Test.h"
#include "libboardgame_util/CpuTimeSource.h"
#include "libpentobi_base/BoardUpdater.h"
#include "libpentobi_base/PentobiTree.h"

using namespace std;
using namespace libpentobi_mcts;
using libboardgame_sgf::SgfNode;
using libboardgame_sgf::TreeReader;
using libboardgame_sgf::get_last_node;
using libboardgame_util::CpuTimeSource;
using libpentobi_base::BoardUpdater;
using libpentobi_base::PentobiTree;

//-----------------------------------------------------------------------------

/** Test that state generates a playout move even if no large pieces are
    playable early in the game.
    This tests for a bug that occurred in Pentobi 1.1 with game variant Trigon:
    Because moves that are below a certain piece size are not generated early
    in the game, it could happen in rare cases that no moves were generated
    at all. */
LIBBOARDGAME_TEST_CASE(pentobi_mcts_search_no_large_pieces)
{
    istringstream
        in(R"delim(
           (;GM[Blokus Trigon Two-Player];1[r4,r5,s5,r6,s6,r7]
           ;2[r12,q13,r13,q14,r14,r15];3[k11,l11,m11,n11,j12,k12]
           ;4[w7,x7,y7,z7,v8,w8];1[s8,t8,r9,s9,t9,u9]
           ;2[n12,o12,m13,n13,o13,o14];3[k13,k14,l14,l15,m15,n15]
           ;4[w9,t10,u10,v10,w10,x10];1[n10,o10,p10,q10,r10,r11]
           ;2[o15,k16,l16,m16,n16,o16];3[i15,j15,h16,i16,j16,j17]
           ;4[u11,s12,t12,u12,v12,v13];1[p4,m5,n5,o5,p5,m6]
           ;2[k17,i18,j18,k18,l18,m18];3[l17,m17,n17,o17,p17,o18]
           ;4[t14,u14,s15,t15,r16,s16];1[l8,m8,j9,k9,l9,m9])
           )delim");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto bd = make_unique<Board>(tree.get_variant());
    BoardUpdater updater;
    updater.update(*bd, tree, get_last_node(tree.get_root()));
    unsigned nu_threads = 1;
    size_t memory = 10000;
    auto search = make_unique<Search>(bd->get_variant(), nu_threads, memory);
    Float max_count = 1;
    size_t min_simulations = 1;
    double max_time = 0;
    CpuTimeSource time_source;
    Move mv;
    bool res = search->search(mv, *bd, Color(1), max_count, min_simulations,
                              max_time, time_source);
    LIBBOARDGAME_CHECK(res);
    LIBBOARDGAME_CHECK(! mv.is_null());
}

/** Test that useless one-piece moves are generated if no other moves exist.
    Useless one-piece moves (all neighbors occupied) are not needed during
    the search, but the search should still return one if no other legal
    moves exist. */
LIBBOARDGAME_TEST_CASE(pentobi_mcts_search_callisto_useless_one_piece)
{
    istringstream
        in(R"delim(
           (;GM[Callisto Two-Player];1[k10];2[k7];1[g6];2[g11]
           ;1[f7,g7,h7,f8,h8];2[d9,e9,e10,f10,f11];1[c8,d8,e8,c9]
           ;2[k8,l8,m8,l9,l10];1[j11,k11,i12,j12];2[h11,i11,h12,h13,i13]
           ;1[n9,m10,n10,l11,m11];2[j4,j5,j6,k6];1[j13,h14,i14,j14,j15]
           ;2[h3,g4,h4,i4,h5];1[n6,m7,n7,o7,n8];2[f13,g13,f14,g14]
           ;1[c10,d10,c11,d11];2[e5,f5,g5,f6];1[l5,m5,l6,m6];2[e6,c7,d7,e7]
           ;1[j3,k3,k4,k5];2[h1,i1,h2,i2];1[e11,e12,f12,e13];2[i8,h9,i9,h10]
           ;1[b7,a8,b8,a9];2[k12];1[g15,h15,i15,h16];2[l12,m12,k13,l13]
           ;1[j8,j9,j10];2[i5,h6,i6,i7];1[g8,g9,g10];2[g2,f3,g3];1[o9,p9,o10]
           ;2[d5,c6,d6];1[b9,b10];2[e4,f4];1[o8,p8])
           )delim");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto bd = make_unique<Board>(tree.get_variant());
    BoardUpdater updater;
    updater.update(*bd, tree, get_last_node(tree.get_root()));
    unsigned nu_threads = 1;
    size_t memory = 10000;
    auto search = make_unique<Search>(bd->get_variant(), nu_threads, memory);
    Float max_count = 1;
    size_t min_simulations = 1;
    double max_time = 0;
    CpuTimeSource time_source;
    Move mv;
    bool res = search->search(mv, *bd, Color(0), max_count, min_simulations,
                              max_time, time_source);
    LIBBOARDGAME_CHECK(res);
    LIBBOARDGAME_CHECK(! mv.is_null());
    LIBBOARDGAME_CHECK(bd->get_move_piece(mv) == bd->get_one_piece());
}

//-----------------------------------------------------------------------------
