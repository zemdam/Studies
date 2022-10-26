/** @file
 * Implementacja klasy przechowującej przekierowania numerów telefonicznych
 *
 * @author Adam Zembrzuski <az430277@students.mimuw.edu.pl>
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include "phone_forward.h"
#include "string_array.h"
#include "string_tools.h"
#include "trie.h"
#include <stdlib.h>
#include <string.h>

/**
 * To jest struktura przechowująca przekierowania numerów telefonów w formie
 * drzewa trie.
 */
struct PhoneForward {
    DoubleTrie *doubleTrie; ///< Wskaźnik na podwójne drzewo.
};

/**
 * To jest struktura przechowująca ciąg numerów telefonów.
 */
struct PhoneNumbers {
    StringArray *numbers; ///< Wskaźnik na tablicę napisów.
};

/** @brief Tworzy i inicjuje nową strukturę.
 * Tworzy nową pustą strukturę do przechowywania numerów. Zostanie ona zapisana
 * do @p toInit.
 * @param[out] toInit - podwójny wskaźnik na strukturę.
 * @return Wartość @p false jeśli udało się alokować potrzebną pamięć.
 *         Wartość @p true w przeciwnym przypadku.
 */
static bool initPhoneNumbers(PhoneNumbers **toInit) {
    *toInit = malloc(sizeof(PhoneNumbers));
    if (*toInit == NULL || initStringArray(&(*toInit)->numbers)) {
        free(*toInit);
        *toInit = NULL;
        return true;
    }

    return false;
}

/** @brief Dodaje numer do struktury.
 * Dodaje napis reprezentujący numer @p number do struktury @p allNumbers.
 * @param[in] allNumbers - wskaźnik na węzeł drzewa trie;
 * @param[in] number     - wskaźnik na napis reprezentujący numer.
 * @return Wartość @p true jeśli udało sie dodać.
 *         Wartość @p false w przeciwnym przypadku.
 */
static bool addToPhoneNumbers(PhoneNumbers *allNumbers, char *number) {
    if (!addToStringArray(allNumbers->numbers, number)) {
        return false;
    }

    return true;
}

PhoneForward *phfwdNew(void) {
    PhoneForward *newPhoneForward = malloc(sizeof(PhoneForward));
    if (newPhoneForward == NULL) {
        return NULL;
    }

    newPhoneForward->doubleTrie = newDoubleTrie();
    if (newPhoneForward->doubleTrie == NULL) {
        free(newPhoneForward);
        return NULL;
    }

    return newPhoneForward;
}

void phfwdDelete(PhoneForward *pf) {
    if (pf == NULL) {
        return;
    }

    deleteDoubleTrie(pf->doubleTrie);
    free(pf);
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (pf == NULL || num1 == NULL || num2 == NULL) {
        return false;
    }

    size_t num1Length = stringNumberLength(num1);
    size_t num2Length = stringNumberLength(num2);
    bool areNumbersCorrect = num1Length && num2Length;
    if (!areNumbersCorrect || !strcmp(num1, num2)) {
        return false;
    }

    char *num1Copy = NULL;
    char *num2Copy = NULL;
    copyString(&num1Copy, num1, num1Length);
    copyString(&num2Copy, num2, num2Length);
    if (num1Copy == NULL || num2Copy == NULL ||
        !addNumber(pf->doubleTrie, num1Copy, num2Copy)) {
        free(num2Copy);
        free(num1Copy);
        return false;
    }

    return true;
}

void phfwdRemove(PhoneForward *pf, char const *num) {
    if (pf == NULL || num == NULL) {
        return;
    }

    size_t numLength = stringNumberLength(num);
    if (numLength) {
        trieRemove(pf->doubleTrie, num);
    }
}

PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num) {
    PhoneNumbers *phoneNumbers = NULL;
    if (pf == NULL || initPhoneNumbers(&phoneNumbers) || num == NULL) {
        return phoneNumbers;
    }

    size_t skipLength;
    size_t numberLength = stringNumberLength(num);
    size_t prefixLength = numberLength;
    if (numberLength == 0) {
        return phoneNumbers;
    }

    char *prefix = findLongest(pf->doubleTrie, num, &prefixLength, &skipLength);
    char *newNumber =
            changePrefix(num, prefix, numberLength, prefixLength, skipLength);
    if (newNumber == NULL || !addToPhoneNumbers(phoneNumbers, newNumber)) {
        phnumDelete(phoneNumbers);
        return NULL;
    }

    return phoneNumbers;
}

char const *phnumGet(PhoneNumbers const *pnum, size_t idx) {
    if (pnum == NULL) {
        return NULL;
    }

    return atIndex(pnum->numbers, idx);
}

void phnumDelete(PhoneNumbers *pnum) {
    if (pnum != NULL) {
        arrayDelete(pnum->numbers);
    }

    free(pnum);
}

PhoneNumbers *phfwdReverse(PhoneForward const *pf, char const *num) {
    PhoneNumbers *reverseNumbers = NULL;
    if (pf == NULL || initPhoneNumbers(&reverseNumbers) || num == NULL) {
        return reverseNumbers;
    }

    size_t numLength = stringNumberLength(num);
    char *numCopy = NULL;
    if (numLength == 0 || !copyString(&numCopy, num, numLength)) {
        return reverseNumbers;
    }

    StringArray *newArray = getReverse(pf->doubleTrie, numCopy, numLength);
    if (newArray != NULL) {
        sortNumbers(newArray);
        arrayDelete(reverseNumbers->numbers);
        reverseNumbers->numbers = newArray;
    }

    return reverseNumbers;
}