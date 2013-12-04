//-----------------------------------------------------------------------------
/** @file libboardgame_base/PointTransform.h
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_POINT_TRANSFORM_H
#define LIBBOARDGAME_BASE_POINT_TRANSFORM_H

#include <cmath>
#include "Point.h"
#include "libboardgame_util/Unused.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

/** %Transform a point.
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
                          unsigned height) const override;
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
                          unsigned height) const override;
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
    This is equivalent to a reflection on the y=-x line. */
template<class P>
class PointTransfRot270Refl
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const override;
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

/** Mirror along x axis. */
template<class P>
class PointTransfRefl
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const override;
};

template<class P>
P PointTransfRefl<P>::get_transformed(const Point& p, unsigned width,
                                      unsigned height) const
{
    LIBBOARDGAME_UNUSED(height);
    unsigned x = width - p.get_x() - 1;
    unsigned y = p.get_y();
    return Point(x, y);
}

//-----------------------------------------------------------------------------

/** Mirror along y axis. */
template<class P>
class PointTransfReflRot180
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const override;
};

template<class P>
P PointTransfReflRot180<P>::get_transformed(const Point& p, unsigned width,
                                            unsigned height) const
{
    LIBBOARDGAME_UNUSED(width);
    unsigned x = p.get_x();
    unsigned y = height - p.get_y() - 1;
    return Point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonRot60
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const override;
};

template<class P>
P PointTransfTrigonRot60<P>::get_transformed(const Point& p, unsigned width,
                                             unsigned height) const
{
    float cx = 0.5f * static_cast<float>(width - 1);
    float cy = 0.5f * static_cast<float>(height - 1);
    float px = static_cast<float>(p.get_x()) - cx;
    float py = static_cast<float>(p.get_y()) - cy;
    unsigned x = static_cast<unsigned>(round(cx + 0.5f * px + 1.5f * py));
    unsigned y = static_cast<unsigned>(round(cy - 0.5f * px + 0.5f * py));
    return Point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonRot120
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const override;
};

template<class P>
P PointTransfTrigonRot120<P>::get_transformed(const Point& p, unsigned width,
                                              unsigned height) const
{
    float cx = 0.5f * static_cast<float>(width - 1);
    float cy = 0.5f * static_cast<float>(height - 1);
    float px = static_cast<float>(p.get_x()) - cx;
    float py = static_cast<float>(p.get_y()) - cy;
    unsigned x = static_cast<unsigned>(round(cx - 0.5f * px + 1.5f * py));
    unsigned y = static_cast<unsigned>(round(cy - 0.5f * px - 0.5f * py));
    return Point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonRot240
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const override;
};

template<class P>
P PointTransfTrigonRot240<P>::get_transformed(const Point& p, unsigned width,
                                              unsigned height) const
{
    float cx = 0.5f * static_cast<float>(width - 1);
    float cy = 0.5f * static_cast<float>(height - 1);
    float px = static_cast<float>(p.get_x()) - cx;
    float py = static_cast<float>(p.get_y()) - cy;
    unsigned x = static_cast<unsigned>(round(cx - 0.5f * px - 1.5f * py));
    unsigned y = static_cast<unsigned>(round(cy + 0.5f * px - 0.5f * py));
    return Point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonRot300
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const override;
};

template<class P>
P PointTransfTrigonRot300<P>::get_transformed(const Point& p, unsigned width,
                                              unsigned height) const
{
    float cx = 0.5f * static_cast<float>(width - 1);
    float cy = 0.5f * static_cast<float>(height - 1);
    float px = static_cast<float>(p.get_x()) - cx;
    float py = static_cast<float>(p.get_y()) - cy;
    unsigned x = static_cast<unsigned>(round(cx + 0.5f * px - 1.5f * py));
    unsigned y = static_cast<unsigned>(round(cy + 0.5f * px + 0.5f * py));
    return Point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonReflRot60
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const override;
};

template<class P>
P PointTransfTrigonReflRot60<P>::get_transformed(const Point& p,
                                                 unsigned width,
                                                 unsigned height) const
{
    float cx = 0.5f * static_cast<float>(width - 1);
    float cy = 0.5f * static_cast<float>(height - 1);
    float px = static_cast<float>(p.get_x()) - cx;
    float py = static_cast<float>(p.get_y()) - cy;
    unsigned x = static_cast<unsigned>(round(cx + 0.5f * (-px) + 1.5f * py));
    unsigned y = static_cast<unsigned>(round(cy - 0.5f * (-px) + 0.5f * py));
    return Point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonReflRot120
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const override;
};

template<class P>
P PointTransfTrigonReflRot120<P>::get_transformed(const Point& p,
                                                  unsigned width,
                                                  unsigned height) const
{
    float cx = 0.5f * static_cast<float>(width - 1);
    float cy = 0.5f * static_cast<float>(height - 1);
    float px = static_cast<float>(p.get_x()) - cx;
    float py = static_cast<float>(p.get_y()) - cy;
    unsigned x = static_cast<unsigned>(round(cx - 0.5f * (-px) + 1.5f * py));
    unsigned y = static_cast<unsigned>(round(cy - 0.5f * (-px) - 0.5f * py));
    return Point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonReflRot240
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const override;
};

template<class P>
P PointTransfTrigonReflRot240<P>::get_transformed(const Point& p,
                                                  unsigned width,
                                                  unsigned height) const
{
    float cx = 0.5f * static_cast<float>(width - 1);
    float cy = 0.5f * static_cast<float>(height - 1);
    float px = static_cast<float>(p.get_x()) - cx;
    float py = static_cast<float>(p.get_y()) - cy;
    unsigned x = static_cast<unsigned>(round(cx - 0.5f * (-px) - 1.5f * py));
    unsigned y = static_cast<unsigned>(round(cy + 0.5f * (-px) - 0.5f * py));
    return Point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonReflRot300
    : public PointTransform<P>
{
public:
    typedef P Point;

    Point get_transformed(const Point& p, unsigned width,
                          unsigned height) const override;
};

template<class P>
P PointTransfTrigonReflRot300<P>::get_transformed(const Point& p,
                                                  unsigned width,
                                                  unsigned height) const
{
    float cx = 0.5f * static_cast<float>(width - 1);
    float cy = 0.5f * static_cast<float>(height - 1);
    float px = static_cast<float>(p.get_x()) - cx;
    float py = static_cast<float>(p.get_y()) - cy;
    unsigned x = static_cast<unsigned>(round(cx + 0.5f * (-px) - 1.5f * py));
    unsigned y = static_cast<unsigned>(round(cy + 0.5f * (-px) + 0.5f * py));
    return Point(x, y);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_POINT_TRANSFORM_H
