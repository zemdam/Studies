/** @file
* Implementacja klasy tablicy dynamicznej dla napisów.
*
* @author Adam Zembrzuski <az430277@students.mimuw.edu.pl>
* @copyright Uniwersytet Warszawski
* @date 2022
*/

#include "string_array.h"
#include "string_tools.h"
#include <stdlib.h>
#include <string.h>

/**
 * To jest struktura przechowująca tablicę napisów.
 */
struct StringArray {
    char **strings;   ///< tablica z numerami;
    size_t usedSize;  ///< użyta pamięć;
    size_t totalSize; ///< dostępna pamięć.
};

/** Porównuje dwa numery.
 * Funkcja porównująca leksykograficznie dwa numery @p string1 oraz @p string2.
 * Jest ona wykorzystywana podczas sortowania.
 * @param[in] string1 - napis reprezentujący numer;
 * @param[in] string2 - napis reprezentujący numer;
 * @return 0 jeśli podane numery są identyczne.
 *         Liczba mniejsza od zera jeśli @p string1 jest mniejszy niż @p string2.
 *         Liczba większa od zera jeśli @p string1 jest większy niż @p string2.
 */
static int compareNumbers(const void *string1, const void *string2) {
    size_t index = 0;
    char *left = *(char **) string1;
    char *right = *(char **) string2;
    while (left[index] != '\0' && right[index] != '\0') {
        if (charToDecimal(left[index]) != charToDecimal(right[index])) {
            return charToDecimal(left[index]) - charToDecimal(right[index]);
        }

        index++;
    }

    if (left[index] == '\0' && right[index] == '\0') {
        return 0;
    }

    if (left[index] == '\0') {
        return -1;
    }

    return 1;
}

/** @brief Usuwa powtórzenia.
 * W posortowanej tablicy napisów @p array usuwa powtórzenia nadpisując je NULL'em.
 * @param[in] array - wskaźnik na tablicę napisów.
 * @return Liczba powtórzeń.
 */
static size_t markSameStrings(StringArray *array) {
    size_t numberOfChanges = 0;
    for (size_t i = 0; i < array->usedSize - 1; i++) {
        if (strcmp(array->strings[i], array->strings[i + 1]) == 0) {
            free(array->strings[i]);
            array->strings[i] = NULL;
            numberOfChanges++;
        }
    }

    return numberOfChanges;
}

/** @brief Naprawia tablicę napisów.
 * Wszystkie wartości tablicy równe @p NULL przesuwa na koniec tablicy.
 * @param[in] array - wskaźnik na tablicę napisów.
 */
static void fixArray(StringArray *array) {
    size_t index1 = 0;
    size_t index2 = 0;
    while (index1 < array->usedSize && index2 < array->usedSize) {
        while (array->strings[index1] != NULL) {
            index1++;
            if (index1 == array->usedSize) {
                return;
            }
        }

        if (index1 > index2) {
            index2 = index1;
        }

        while (array->strings[index2] == NULL) {
            index2++;
            if (index1 == array->usedSize) {
                return;
            }
        }

        array->strings[index1] = array->strings[index2];
        array->strings[index2] = NULL;
        index1++;
        index2++;
    }
}

bool initStringArray(StringArray **toInit) {
    *toInit = malloc(sizeof(StringArray));
    if (*toInit == NULL) {
        return true;
    }

    (*toInit)->strings = malloc(sizeof(char *));
    if ((*toInit)->strings == NULL) {
        free(*toInit);
        *toInit = NULL;
        return true;
    }

    (*toInit)->usedSize = 0;
    (*toInit)->totalSize = 1;

    return false;
}

bool addToStringArray(StringArray *array, char *string) {
    size_t index = array->usedSize;
    size_t size = array->totalSize;
    if (index >= size) {
        char **newStrings = realloc(array->strings, size * 2 * sizeof(char *));
        if (newStrings == NULL) {
            return false;
        }
        array->strings = newStrings;
        array->totalSize = size * 2;
    }

    array->strings[index] = string;
    array->usedSize++;

    return true;
}

char const *atIndex(StringArray *array, size_t idx) {
    if (array == NULL || array->strings == NULL || idx >= array->usedSize) {
        return NULL;
    }

    return array->strings[idx];
}

void arrayDelete(StringArray *array) {
    if (array == NULL) {
        return;
    }

    for (size_t i = 0; i < array->usedSize; i++) {
        free(array->strings[i]);
    }

    free(array->strings);
    free(array);
}

void sortNumbers(StringArray *array) {
    qsort(array->strings, array->usedSize, sizeof(char *), compareNumbers);
    size_t fixSize = markSameStrings(array);
    fixArray(array);
    array->usedSize -= fixSize;
}