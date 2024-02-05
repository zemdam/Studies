#ifndef MONEYBAG_H
#define MONEYBAG_H

#include <compare>
#include <cstdint>
#include <iostream>
#include <stdexcept>

class Moneybag {
public:
    using coin_number_t = uint64_t;

private:
    static const coin_number_t COIN_NUMBER_MAX = UINT64_MAX;
    coin_number_t livres;
    coin_number_t soliduses;
    coin_number_t deniers;

    constexpr static bool checkValues(coin_number_t first,
                                      coin_number_t second) {
        return first > COIN_NUMBER_MAX - second;
    }

    [[nodiscard]] constexpr bool isOutOfRange(const Moneybag &toAdd) const {
        return (checkValues(livres, toAdd.livres) ||
                checkValues(soliduses, toAdd.soliduses) ||
                checkValues(deniers, toAdd.deniers));
    }

public:
    constexpr Moneybag(coin_number_t livres, coin_number_t soliduses,
                       coin_number_t deniers)
        : livres(livres), soliduses(soliduses), deniers(deniers) {
    }

    constexpr Moneybag() = delete;
    constexpr Moneybag(const Moneybag &) = default;
    constexpr Moneybag(Moneybag &&) = default;
    constexpr Moneybag &operator=(const Moneybag &) = default;
    constexpr Moneybag &operator=(Moneybag &&) = default;
    constexpr ~Moneybag() = default;


    [[nodiscard]] constexpr coin_number_t livre_number() const {
        return livres;
    }
    [[nodiscard]] constexpr coin_number_t solidus_number() const {
        return soliduses;
    }
    [[nodiscard]] constexpr coin_number_t denier_number() const {
        return deniers;
    }

    constexpr Moneybag &operator+=(const Moneybag &toAdd) {
        if (isOutOfRange(toAdd)) {
            throw std::out_of_range("Too big.");
        }

        livres += toAdd.livres;
        deniers += toAdd.deniers;
        soliduses += toAdd.soliduses;

        return *this;
    }

    constexpr Moneybag operator+(const Moneybag &toAdd) const {
        return Moneybag(*this) += toAdd;
    }

    constexpr Moneybag &operator-=(const Moneybag &toSub) {
        if (livres < toSub.livres || deniers < toSub.deniers ||
            soliduses < toSub.soliduses) {
            throw std::out_of_range("Too low.");
        }

        livres -= toSub.livres;
        deniers -= toSub.deniers;
        soliduses -= toSub.soliduses;

        return *this;
    }

    constexpr Moneybag operator-(const Moneybag &toAdd) const {
        return Moneybag(*this) -= toAdd;
    }

    constexpr Moneybag &operator*=(coin_number_t multiplayer) {
        if (livres > COIN_NUMBER_MAX / multiplayer ||
            deniers > COIN_NUMBER_MAX / multiplayer ||
            soliduses > COIN_NUMBER_MAX / multiplayer) {
            throw std::out_of_range("Too big.");
        }

        livres *= multiplayer;
        deniers *= multiplayer;
        soliduses *= multiplayer;

        return *this;
    }

    constexpr Moneybag operator*(coin_number_t multiplayer) const {
        return Moneybag(*this) *= multiplayer;
    }

    constexpr friend Moneybag operator*(coin_number_t multiplayer,
                                        const Moneybag &moneybag) {
        return Moneybag(moneybag) *= multiplayer;
    }

    constexpr std::partial_ordering
    operator<=>(const Moneybag &moneybag) const {
        if (livres == moneybag.livres && deniers == moneybag.deniers &&
            soliduses == moneybag.soliduses) {
            return std::partial_ordering::equivalent;
        }

        if (livres >= moneybag.livres && deniers >= moneybag.deniers &&
            soliduses >= moneybag.soliduses) {
            return std::partial_ordering::greater;
        }

        if (livres <= moneybag.livres && deniers <= moneybag.deniers &&
            soliduses <= moneybag.soliduses) {
            return std::partial_ordering::less;
        }

        return std::partial_ordering::unordered;
    }

    constexpr bool operator==(const Moneybag &) const = default;

    constexpr explicit operator bool() const {
        return livres || deniers || soliduses;
    }

    friend std::ostream &operator<<(std::ostream &os, const Moneybag &moneybag);
};

inline constexpr Moneybag Livre = Moneybag(1, 0, 0);
inline constexpr Moneybag Solidus = Moneybag(0, 1, 0);
inline constexpr Moneybag Denier = Moneybag(0, 0, 1);

class Value {
private:
    using value_number_t = __uint128_t;

    value_number_t deniersValue;

public:
    constexpr explicit Value(const Moneybag &moneybag)
        : deniersValue(240 * (value_number_t) moneybag.livre_number() +
                       12 * (value_number_t) moneybag.solidus_number() +
                       moneybag.denier_number()) {
    }

    constexpr explicit Value(Moneybag::coin_number_t deniers)
        : deniersValue(deniers) {
    }

    constexpr Value() = default;
    constexpr Value(const Value &) = default;
    constexpr Value(Value &&) = default;
    constexpr Value &operator=(const Value &) = default;
    constexpr Value &operator=(Value &&) = default;
    constexpr ~Value() = default;
    constexpr auto operator<=>(const Value &value) const = default;
    constexpr bool operator==(const Value &value) const = default;

    constexpr auto operator<=>(Moneybag::coin_number_t _deniersValue) const {
        return deniersValue <=> _deniersValue;
    }

    constexpr bool operator==(Moneybag::coin_number_t _deniersValue) const {
        return deniersValue == _deniersValue;
    }

    explicit operator std::string() const;
};

#endif //MONEYBAG_H
