//-----------------------------------------------------------------------------
/** @file libboardgame_base/PointTransform.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_POINT_TRANSFORM_H
#define LIBBOARDGAME_BASE_POINT_TRANSFORM_H

#include <cmath>
#include "Geometry.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

/** %Transform a point.
    @tparam P An instance of class Point. */
template<class P>
class PointTransform
{
public:
    using Point = P;

    virtual ~PointTransform() = default;

    virtual Point get_transformed(Point p, const Geometry<P>& geo) const = 0;
};


//-----------------------------------------------------------------------------

template<class P>
class PointTransfIdent
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfIdent<P>::get_transformed(
        Point p, [[maybe_unused]] const Geometry<P>& geo) const
{
    return p;
}

//-----------------------------------------------------------------------------

/** Rotate point by 90 degrees. */
template<class P>
class PointTransfRot90
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfRot90<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    unsigned x = geo.get_width() - geo.get_y(p) - 1;
    unsigned y = geo.get_x(p);
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

/** Rotate point by 180 degrees. */
template<class P>
class PointTransfRot180
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfRot180<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    unsigned x = geo.get_width() - geo.get_x(p) - 1;
    unsigned y = geo.get_height() - geo.get_y(p) - 1;
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

/** Rotate point by 270 degrees. */
template<class P>
class PointTransfRot270
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfRot270<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    unsigned x = geo.get_y(p);
    unsigned y = geo.get_height() - geo.get_x(p) - 1;
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

/** Rotate point by 270 degrees and reflect on y axis shifted to the center.
    This is equivalent to a reflection on the x=y line. */
template<class P>
class PointTransfRot270Refl
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfRot270Refl<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    return geo.get_point(geo.get_y(p), geo.get_x(p));
}

//-----------------------------------------------------------------------------

/** Rotate point by 90 degrees and reflect on y axis shifted to the center.
    This is equivalent to a reflection on the x=width-y line. */
template<class P>
class PointTransfRot90Refl
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfRot90Refl<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    unsigned x = geo.get_width() - geo.get_y(p) - 1;
    unsigned y = geo.get_height() - geo.get_x(p) - 1;
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

/** Mirror along x axis. */
template<class P>
class PointTransfRefl
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfRefl<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    unsigned x = geo.get_width() - geo.get_x(p) - 1;
    unsigned y = geo.get_y(p);
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

/** Mirror along y axis. */
template<class P>
class PointTransfReflRot180
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfReflRot180<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    unsigned x = geo.get_x(p);
    unsigned y = geo.get_height() - geo.get_y(p) - 1;
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonRot60
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfTrigonRot60<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    float cx = 0.5f * static_cast<float>(geo.get_width() - 1);
    float cy = 0.5f * static_cast<float>(geo.get_height() - 1);
    float px = static_cast<float>(geo.get_x(p)) - cx;
    float py = static_cast<float>(geo.get_y(p)) - cy;
    auto x = static_cast<unsigned>(round(cx + 0.5f * px + 1.5f * py));
    auto y = static_cast<unsigned>(round(cy - 0.5f * px + 0.5f * py));
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonRot120
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfTrigonRot120<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    float cx = 0.5f * static_cast<float>(geo.get_width() - 1);
    float cy = 0.5f * static_cast<float>(geo.get_height() - 1);
    float px = static_cast<float>(geo.get_x(p)) - cx;
    float py = static_cast<float>(geo.get_y(p)) - cy;
    auto x = static_cast<unsigned>(round(cx - 0.5f * px + 1.5f * py));
    auto y = static_cast<unsigned>(round(cy - 0.5f * px - 0.5f * py));
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonRot240
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfTrigonRot240<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    float cx = 0.5f * static_cast<float>(geo.get_width() - 1);
    float cy = 0.5f * static_cast<float>(geo.get_height() - 1);
    float px = static_cast<float>(geo.get_x(p)) - cx;
    float py = static_cast<float>(geo.get_y(p)) - cy;
    auto x = static_cast<unsigned>(round(cx - 0.5f * px - 1.5f * py));
    auto y = static_cast<unsigned>(round(cy + 0.5f * px - 0.5f * py));
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonRot300
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfTrigonRot300<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    float cx = 0.5f * static_cast<float>(geo.get_width() - 1);
    float cy = 0.5f * static_cast<float>(geo.get_height() - 1);
    float px = static_cast<float>(geo.get_x(p)) - cx;
    float py = static_cast<float>(geo.get_y(p)) - cy;
    auto x = static_cast<unsigned>(round(cx + 0.5f * px - 1.5f * py));
    auto y = static_cast<unsigned>(round(cy + 0.5f * px + 0.5f * py));
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonReflRot60
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfTrigonReflRot60<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    float cx = 0.5f * static_cast<float>(geo.get_width() - 1);
    float cy = 0.5f * static_cast<float>(geo.get_height() - 1);
    float px = static_cast<float>(geo.get_x(p)) - cx;
    float py = static_cast<float>(geo.get_y(p)) - cy;
    auto x = static_cast<unsigned>(round(cx + 0.5f * (-px) + 1.5f * py));
    auto y = static_cast<unsigned>(round(cy - 0.5f * (-px) + 0.5f * py));
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonReflRot120
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfTrigonReflRot120<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    float cx = 0.5f * static_cast<float>(geo.get_width() - 1);
    float cy = 0.5f * static_cast<float>(geo.get_height() - 1);
    float px = static_cast<float>(geo.get_x(p)) - cx;
    float py = static_cast<float>(geo.get_y(p)) - cy;
    auto x = static_cast<unsigned>(round(cx - 0.5f * (-px) + 1.5f * py));
    auto y = static_cast<unsigned>(round(cy - 0.5f * (-px) - 0.5f * py));
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonReflRot240
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfTrigonReflRot240<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    float cx = 0.5f * static_cast<float>(geo.get_width() - 1);
    float cy = 0.5f * static_cast<float>(geo.get_height() - 1);
    float px = static_cast<float>(geo.get_x(p)) - cx;
    float py = static_cast<float>(geo.get_y(p)) - cy;
    auto x = static_cast<unsigned>(round(cx - 0.5f * (-px) - 1.5f * py));
    auto y = static_cast<unsigned>(round(cy + 0.5f * (-px) - 0.5f * py));
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

template<class P>
class PointTransfTrigonReflRot300
    : public PointTransform<P>
{
public:
    using Point = P;

    Point get_transformed(Point p, const Geometry<P>& geo) const override;
};


template<class P>
P PointTransfTrigonReflRot300<P>::get_transformed(Point p, const Geometry<P>& geo) const
{
    float cx = 0.5f * static_cast<float>(geo.get_width() - 1);
    float cy = 0.5f * static_cast<float>(geo.get_height() - 1);
    float px = static_cast<float>(geo.get_x(p)) - cx;
    float py = static_cast<float>(geo.get_y(p)) - cy;
    auto x = static_cast<unsigned>(round(cx + 0.5f * (-px) - 1.5f * py));
    auto y = static_cast<unsigned>(round(cy + 0.5f * (-px) + 0.5f * py));
    return geo.get_point(x, y);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_POINT_TRANSFORM_H
