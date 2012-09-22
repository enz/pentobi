//-----------------------------------------------------------------------------
/** @file libboardgame_base/PointTransform.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_POINT_TRANSFORM_H
#define LIBBOARDGAME_BASE_POINT_TRANSFORM_H

#include "Point.h"
#include "libboardgame_util/Unused.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

/** Transform a point.
    @tparam P An instance of class Point. */
template<class P>
class PointTransform
{
public:
    typedef P Point;

    virtual ~PointTransform();

    virtual Point get_transformed(const Point& p, unsigned width,
                                  unsigned height) const = 0;
};

template<class P>
PointTransform<P>::~PointTransform()
{
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfIdent
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const;
};

template<class P>
P PointTransfIdent<P>::get_transformed(const Point& p, unsigned width,
                                       unsigned height) const
{
    LIBBOARDGAME_UNUSED(width);
    LIBBOARDGAME_UNUSED(height);
    return p;
}

//-----------------------------------------------------------------------------

/** Rotate point by 180 degrees. */
template<class P>
class PointTransfRot180
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const;
};

template<class P>
P PointTransfRot180<P>::get_transformed(const Point& p, unsigned width,
                                        unsigned height) const
{
    unsigned x = width - p.get_x() - 1;
    unsigned y = height - p.get_y() - 1;
    return Point(x, y);
}

//-----------------------------------------------------------------------------

/** Rotate point by 270 degrees and reflect on y axis.
    This is equivalent to a relection on the y=-x line. */
template<class P>
class PointTransfRot270Refl
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const;
};

template<class P>
P PointTransfRot270Refl<P>::get_transformed(const Point& p, unsigned width,
                                            unsigned height) const
{
    unsigned x = height - p.get_y() - 1;
    unsigned y = width - p.get_x() - 1;
    return Point(x, y);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_POINT_TRANSFORM_H
