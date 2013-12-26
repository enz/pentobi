//-----------------------------------------------------------------------------
/** @file libboardgame_base/PointList.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_POINT_LIST_H
#define LIBBOARDGAME_BASE_POINT_LIST_H

#include <sstream>
#include <string>
#include "Point.h"
#include "libboardgame_util/ArrayList.h"

namespace libboardgame_base {

using namespace std;
using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

/** Array list that can contain all points on the board.
    @tparam P An instantiation of libboardgame_base::Point */
template<class P>
class PointList
    : public ArrayList<P, P::max_onboard>
{
public:
    typedef P Point;

    PointList() = default;

    explicit PointList(const P& p);

    explicit PointList(const string& s);
};

template<class P>
inline PointList<P>::PointList(const P& p)
    : ArrayList<P, P::max_onboard>(p)
{
}

template<class P>
PointList<P>::PointList(const string& s)
{
    istringstream in(s);
    string t;
    while (in >> t)
        ArrayList<P,P::max_onboard>::push_back(P::from_string(t));
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_POINT_LIST_H
