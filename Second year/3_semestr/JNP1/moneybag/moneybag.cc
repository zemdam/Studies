#include "moneybag.h"

static const int MAX_LENGTH = 40;

std::ostream &operator<<(std::ostream &os, const Moneybag &moneybag) {
    os << "(" << moneybag.livres << " livr";
    if (moneybag.livres != 1) {
        os << "es";
    }
    os << ", " << moneybag.soliduses << " solidus";
    if (moneybag.soliduses != 1) {
        os << "es";
    }
    os << ", " << moneybag.deniers << " denier";
    if (moneybag.deniers != 1) {
        os << "s";
    }
    os << ")";

    return os;
}

Value::operator std::string() const {
    value_number_t copy = deniersValue;
    size_t i = 0;
    char buffer[MAX_LENGTH];

    do {
        buffer[i] = (char) (copy % 10 + '0');
        copy /= 10;
        i++;
    } while (copy > 0);

    std::string string = std::string{buffer, i};
    std::reverse(string.begin(), string.end());

    return string;
}
