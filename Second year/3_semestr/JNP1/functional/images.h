#ifndef ZADANIE7_IMAGES_H
#define ZADANIE7_IMAGES_H

#include "color.h"
#include "coordinate.h"
#include "functional.h"
#include <cmath>
#include <functional>

template<typename T>
using Base_image = std::function<T(const Point)>;

using Fraction = double;
using Region = Base_image<bool>;
using Image = Base_image<Color>;
using Blend = Base_image<Fraction>;

namespace Detail {
    const auto polarPoint = [](Point const &p) {
        return p.is_polar ? p : to_polar(p);
    };
    const auto notPolarPoint = [](Point const &p) {
        return p.is_polar ? from_polar(p) : p;
    };
    const auto createPoint = [](double const first, double const second) {
        return Point(first, second);
    };
    const auto movedPoint = [](Point const &p) {
        return Point(p.first, p.second);
    };
} // namespace Detail

template<typename T>
Base_image<T> constant(T const &t) {
    return [=](Point const &p) {
        (void) p;
        return t;
    };
}

template<typename T>
Base_image<T> rotate(Base_image<T> const &image, double phi) {
    const auto rotatedPoint = [=](Point const &p) {
        return Point(p.first, p.second - phi, true);
    };
    return [=](Point const &p) {
        return compose(Detail::polarPoint, rotatedPoint, image)(p);
    };
}

template<typename T>
Base_image<T> translate(Base_image<T> const &image, Vector v) {
    const auto moveX = [=](Point const &p) { return p.first - v.first; };
    const auto moveY = [=](Point const &p) { return p.second - v.second; };
    return [=](Point const &p) {
        return lift(image, lift(Detail::createPoint,
                                compose(Detail::notPolarPoint, moveX),
                                compose(Detail::notPolarPoint, moveY)))(p);
    };
}

template<typename T>
Base_image<T> vertical_stripe(double d, T const &this_way, T const &that_way) {
    return [=](Point const &p) {
        return std::abs(Detail::notPolarPoint(p).first) <= d / 2 ? this_way
                                                                 : that_way;
    };
}

template<typename T>
Base_image<T> scale(Base_image<T> const &image, double s) {
    return [=](Point const &p) {
        const auto scaledPoint = [=](Point p) {
            return Point(p.first / s, p.second / s);
        };
        return lift(image, compose(Detail::notPolarPoint, scaledPoint))(p);
    };
}

template<typename T>
Base_image<T> circle(Point const &q, double r, T const &inner, T const &outer) {
    const auto distanceFromQ = std::bind(distance, Detail::notPolarPoint(q),
                                         std::placeholders::_1);
    return [=](Point const &p) {
        return distanceFromQ(Detail::notPolarPoint(p)) <= r ? inner : outer;
    };
}

template<typename T>
Base_image<T> checker(double d, T const &this_way, T const &that_way) {
    const auto check = [=](Point const &p) {
        return (int) (floor(p.first / d) + floor(p.second / d)) % 2 == 0
                       ? this_way
                       : that_way;
    };
    return compose(Detail::notPolarPoint, check);
}

template<typename T>
Base_image<T> polar_checker(double d, int n, T const &this_way,
                            T const &that_way) {
    double part = (2 * M_PI) / n;
    const auto makePoint = [=](Point const &p) {
        return Point(p.first, p.second * d / part);
    };
    return [=](Point const &p) {
        return checker(d, this_way,
                       that_way)(compose(Detail::polarPoint, makePoint)(p));
    };
}

template<typename T>
Base_image<T> rings(Point const q, double d, T const this_way,
                    T const that_way) {
    return translate(polar_checker(d, 1, this_way, that_way),
                     Vector(q.first, q.second));
}

inline Image cond(Region const &region, Image const &this_way,
                  Image const &that_way) {
    return [=](Point const &p) {
        return region(p) ? this_way(p) : that_way(p);
    };
}

inline Image lerp(Blend const &blend, Image const &this_way,
                  Image const &that_way) {
    return [=](Point const &p) {
        return this_way(p).weighted_mean(that_way(p), blend(p));
    };
}

inline Image darken(Image const &image, Blend const &blend) {
    return [=](Point const &p) {
        return image(p).weighted_mean(Colors::black, blend(p));
    };
}

inline Image lighten(Image const &image, Blend const &blend) {
    return [=](Point const &p) {
        return image(p).weighted_mean(Colors::white, blend(p));
    };
}

#endif //ZADANIE7_IMAGES_H
