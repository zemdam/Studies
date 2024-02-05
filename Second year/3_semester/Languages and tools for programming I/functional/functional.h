#ifndef ZADANIE7_FUNCTIONAL_H
#define ZADANIE7_FUNCTIONAL_H

#include <functional>

template<typename... Other>
inline auto compose(Other... args);

template<typename T, typename... Other>
inline auto compose(T fn, Other...other) {
    return [=](auto x) { return compose(other...)(fn(x)); };
}

template<>
inline auto compose() {
    return [](auto x) { return x; };
}

template<typename H, typename... Fs>
inline auto lift(H h, Fs...fs) {
    return [=](auto p) { return h(fs(p)...); };
}

#endif //ZADANIE7_FUNCTIONAL_H
