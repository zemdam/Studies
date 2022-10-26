/** @file
* Implementacja klasy pomocnych funkcji dla napisów.
*
* @author Adam Zembrzuski <az430277@students.mimuw.edu.pl>
* @copyright Uniwersytet Warszawski
* @date 2022
*/

#include "string_tools.h"
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/** @brief Sprawdza przekroczenie zakresu.
 * Sprawdza czy suma @p number1 i @p number2 będzie większa bądź równa SIZE_MAX.
 * @param[in] number1 - zmienna typu size_t;
 * @param[in] number2 - zmienna typu size_t.
 * @return Wartość @p true gdy zakres zostanie przekroczony.
 *         Wartość @p false w przeciwnym przypadku.
 */
static bool checkOverflow(size_t number1, size_t number2) {
    if (number1 >= SIZE_MAX - number2) {
        return true;
    }

    return false;
}

/** @brief Sprawdza czy znak jest cyfrą.
 * Sprawdza czy znak @p c jest cyfrą rozszerzoną o * = 10 oraz # = 11.
 * @param[in] c - znak do sprawdzenia
 * @return @p True jeśli jest cyfrą.
 *         @p False w przeciwnym przypadku.
 */
static bool isDigit(char c) {
    return isdigit(c) || c == '*' || c == '#';
}

bool copyString(char **destination, char const *source, size_t length) {
    *destination = malloc(sizeof(char) * (length + 1));
    if (*destination == NULL) {
        return false;
    }

    strcpy(*destination, source);

    return true;
}

size_t stringNumberLength(char const *number) {
    size_t i = 0;
    while (number[i] != '\0') {
        if (!isDigit(number[i]) || i == SIZE_MAX - 1) {
            return 0;
        }
        i++;
    }

    return i;
}

int charToDecimal(char asciiCode) {
    switch (asciiCode) {
        case '*':
            return 10;
        case '#':
            return 11;
        default:
            return asciiCode - '0';
    }
}

char *changePrefix(char const *string, char const *prefix, size_t stringLength,
                   size_t prefixLength, size_t skipLength) {
    if (checkOverflow(stringLength - skipLength, prefixLength)) {
        return NULL;
    }

    size_t newLength = stringLength - skipLength + prefixLength;
    char *newString = malloc(sizeof(char) * (newLength + 1));
    if (newString == NULL) {
        return NULL;
    }

    if (prefix != NULL) {
        strcpy(newString, prefix);
    }

    for (size_t i = prefixLength; i < newLength; i++) {
        newString[i] = string[skipLength++];
    }

    newString[newLength] = '\0';

    return newString;
}

size_t strlenSafe(char const *string) {
    if (string == NULL) {
        return 0;
    }

    return strlen(string);
}