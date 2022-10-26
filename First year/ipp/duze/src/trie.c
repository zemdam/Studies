/** @file
* Implementacja klasy drzew trie dla numerów telefonicznych.
*
* @author Adam Zembrzuski <az430277@students.mimuw.edu.pl>
* @copyright Uniwersytet Warszawski
* @date 2022
*/

#include "trie.h"
#include "macros.h"
#include "string_tools.h"
#include <stdlib.h>

/**
 * To jest struktura przechowująca zwykły numer.
 */
typedef struct NormalNumber {
    char *number;                  ///< napis z numerem;
    struct ReverseNumber *reverse; ///< wskaźnik na odwrotność numeru;
    struct TrieNode *
            reverseNode; ///< węzeł drzewa trie przechowujący odwrotność numeru.
} NormalNumber;

/**
 * To jest struktura przechowująca odwrotność zwykłego numeru.
 */
typedef struct ReverseNumber {
    char *number;                         ///< napis z numerem;
    struct ReverseNumber *nextNumber;     ///< następny odwrotny numer;
    struct ReverseNumber *previousNumber; ///< poprzedni odwrotny numer.
} ReverseNumber;

/**
 * To jest struktura reprezentująca węzeł drzewa trie.
 */
typedef struct TrieNode {
    struct TrieNode
            *nextDigits[AMOUNT_OF_DIGITS]; ///< tablica z następnymi węzłami;
    struct TrieNode *previousDigit;        ///< poprzedni węzeł;
    int representedDigit;                  ///< reprezentowana cyfra;
    size_t amountOfNumbers;                ///< liczba numerów w węźle;
    NormalNumber *normalNumber;            ///< zwykły numer;
    ReverseNumber *reverseNumber;          ///< odwrotny numer;
} TrieNode;

/**
 * To jest struktura przechowująca dwa drzewa trie.
 */
struct DoubleTrie {
    TrieNode *NormalTrie;  ///< drzewo trie ze zwykłymi numerami;
    TrieNode *ReverseTrie; ///< drzewo trie z odwrotnymi numerami.
};

/** @brief Inicjuje zwykły numer.
 * Inicjuje wszystkie pola @p toInit.
 * @param[in] toInit - wskaźnik na zwykły numer.
 */
static void initNormalNumber(NormalNumber *toInit) {
    toInit->number = NULL;
    toInit->reverse = NULL;
    toInit->reverseNode = NULL;
}

/** @brief Inicjuje odwrotny numer.
 * Tworzy atrapę odwrotnego numeru jeśli węzeł @p node nie przechowuje żadnego
 * odwrotnego numeru.
 * @param[in] node - wskaźnik na węzeł drzewa trie.
 * @return Wartość @p true jeśli udało się utworzyć atrapę lub węzeł @p node
 *         posiada już numery odwrotne.
 *         Wartość @p false jeśli nie udało się utworzyć atrapy.
 */
static bool initReverseNumber(TrieNode *node) {
    if (node->reverseNumber != NULL) {
        return true;
    }

    node->reverseNumber = malloc(sizeof(ReverseNumber));
    if (node->reverseNumber == NULL) {
        return false;
    }

    node->reverseNumber->nextNumber = NULL;
    node->reverseNumber->previousNumber = NULL;
    node->reverseNumber->number = NULL;

    return true;
}

/** @brief Inicjuje węzeł drzewa trie.
 * Ustawia pola @p toInit na początkowe wartości.
 * @param[in] toInit - wskaźnik na węzeł drzewa trie.
 */
static void initTrieNode(TrieNode *toInit) {
    for (size_t i = 0; i < AMOUNT_OF_DIGITS; i++) {
        toInit->nextDigits[i] = NULL;
    }

    toInit->normalNumber = NULL;
    toInit->reverseNumber = NULL;
    toInit->previousDigit = NULL;
    toInit->representedDigit = 0;
    toInit->amountOfNumbers = 0;
}

/** @brief Tworzy nowy węzeł drzewa trie.
 * Próbuje stworzyć nowy węzeł drzewa trie.
 * @return Wskaźnik na nowy węzeł drzewa trie jeśli się udało stworzyć.
 *         @p NULL jeśli się nie udało.
 */
static TrieNode *newTrie(void) {
    TrieNode *newTrieNode = malloc(sizeof(TrieNode));
    if (newTrieNode == NULL) {
        return NULL;
    }

    initTrieNode(newTrieNode);

    return newTrieNode;
}

/** @brief Usuwa odwrotny numer.
 * Usuwa odwrotny numer z listy jeśli to możliwe.
 * @param[in] toDelete
 */
static void deleteReverseNumber(ReverseNumber *toDelete) {
    if (toDelete == NULL) {
        return;
    }

    if (toDelete->previousNumber != NULL) {
        toDelete->previousNumber->nextNumber = toDelete->nextNumber;
    }

    if (toDelete->nextNumber != NULL) {
        toDelete->nextNumber->previousNumber = toDelete->previousNumber;
    }

    free(toDelete->number);
    toDelete->number = NULL;
    free(toDelete);
}

/** @brief Test na bycie liściem.
 * Sprawdza czy @p toCheck jest liściem drzewa trie.
 * @param[in] toCheck - wskaźnik na węzeł drzewa trie.
 * @return @p True jeśli jest liściem.
 *         @p False w przeciwnym przypadku.
 */
static bool isLeaf(TrieNode *toCheck) {
    for (size_t i = 0; i < AMOUNT_OF_DIGITS; i++) {
        if (toCheck->nextDigits[i] != NULL) {
            return false;
        }
    }

    return true;
}

/** @brief Próbuje usunąć pustą ścieżkę drzewa.
 * Usuwa puste liście idąc w górę drzewa zaczynając od węzła @p toTry.
 * @param[in] toTry - wskaźnik na węzeł drzewa trie.
 */
static void tryToDeleteNode(TrieNode *toTry) {
    TrieNode *tmp;

    while (toTry != NULL && toTry->amountOfNumbers == 0) {
        toTry->normalNumber = NULL;
        free(toTry->reverseNumber);
        toTry->reverseNumber = NULL;
        if (isLeaf(toTry)) {
            tmp = toTry->previousDigit;
            tmp->nextDigits[toTry->representedDigit] = NULL;
            free(toTry);
            toTry = tmp;
        } else {
            break;
        }
    }
}

/** @brief Usuwa zwykły numer z węzła drzewa.
 * Jeśli istnieje to usuwa z węzła @p node zwykły numer oraz numer do niego
 * odwrotny.
 * @param[in] node - wskaźnik na węzeł drzewa trie.
 */
static void deleteNormalNumber(TrieNode *node) {
    if (node->normalNumber == NULL) {
        return;
    }

    deleteReverseNumber(node->normalNumber->reverse);
    node->normalNumber->reverseNode->amountOfNumbers--;
    tryToDeleteNode(node->normalNumber->reverseNode);
    free(node->normalNumber->number);
    free(node->normalNumber);
    node->normalNumber = NULL;
    node->amountOfNumbers--;
}

/** @brief Usuwa drzewo trie.
 * Usuwa drzewo trie, którego korzeniem jest @p toDelete.
 * @param[in] toDelete - wskaźnik na węzeł drzewa trie.
 */
static void deleteTrie(TrieNode *toDelete) {
    if (toDelete != NULL) {
        if (toDelete->previousDigit != NULL) {
            toDelete->previousDigit->nextDigits[toDelete->representedDigit] =
                    NULL;
        }
        toDelete->previousDigit = NULL;
        toDelete->representedDigit = 0;
    }

    while (toDelete != NULL) {
        if (toDelete->representedDigit == AMOUNT_OF_DIGITS) {
            TrieNode *newNode = toDelete->previousDigit;
            deleteNormalNumber(toDelete);
            free(toDelete);
            toDelete = newNode;
            if (toDelete != NULL) {
                toDelete->representedDigit++;
            }
        } else if (toDelete->nextDigits[toDelete->representedDigit] != NULL) {
            toDelete = toDelete->nextDigits[toDelete->representedDigit];
            toDelete->representedDigit = 0;
        } else {
            toDelete->representedDigit++;
        }
    }
}

/** @brief Przechodzi drzewo trie.
 * Wyszukuje w drzewie trie zadanego przez korzeń @p start numer @p prefix.
 * Jeśli @p canExtend ma wartość @p true do drzewa zostanie dodany numer @p num
 * jeśli nie ma go w drzewie.
 * @param[in, out] start - podwójny wskaźnik na korzeń drzewa, przez który
 *                         później zostanie zwrócony węzeł odpowiadający @p num;
 * @param[in] prefix       - wskaźnik na napis reprezentujący numer;
 * @param[in] canExtend - bool odpowiadający za możliwość powiększenia drzewa.
 * @return Wartość @p true gdy udało się wyznaczyć szukany węzeł.
 *         Wartość @p false w przeciwnym przypadku.
 */
static bool goToPrefix(TrieNode **start, char const *prefix, bool canExtend) {
    size_t index = 0;
    int currentDigit = charToDecimal(prefix[index]);

    while (prefix[index] != '\0') {
        if (canExtend && (*start)->nextDigits[currentDigit] == NULL) {
            (*start)->nextDigits[currentDigit] = newTrie();
        }

        if ((*start)->nextDigits[currentDigit] == NULL) {
            return false;
        }

        (*start)->nextDigits[currentDigit]->previousDigit = *start;
        *start = (*start)->nextDigits[currentDigit];
        (*start)->representedDigit = currentDigit;
        currentDigit = charToDecimal(prefix[++index]);
    }

    return true;
}

/** @brief Dodaje zwykły numer do węzła drzewa trie.
 * Do węzła drzewa trie @p node dodaje zwykły numer reprezentowany przez napis
 * @p number. Jeśli węzeł, wcześniej posiadał zwykły numer to jest on nadpisywany.
 * @param[in] node   - wskaźnik na węzeł drzewa trie;
 * @param[in] number - wskaźnik na napis reprezentujący numer.
 * @return @p True jeśli udało się dodać.
 *         @p False w przeciwnym przypadku.
 */
static bool addNormalNumber(TrieNode *node, char *number) {
    deleteNormalNumber(node);
    node->normalNumber = malloc(sizeof(NormalNumber));
    if (node->normalNumber == NULL) {
        return false;
    }

    initNormalNumber(node->normalNumber);
    node->normalNumber->number = number;
    node->amountOfNumbers++;

    return true;
}

/** @brief Dodaje odwrotny numer do węzła drzewa trie.
 * Do numerów odwrotnych węzła @p node dopisuje nowy numer reprezentowany przez
 * napis @p number.
 * @param[in] node     - wskaźnik na węzeł drzewa trie;
 * @param[in] number   - wskaźnik na napis reprezentujący numer.
 * @param[out] reverse - podwójny wskaźnik pod który zostanie zapisana struktura
 *                       przechowująca dopisany numer.
 * @return @p True jeśli udało się dodać.
 *         @p False w przeciwnym przypadku.
 */
static bool addReverseNumber(TrieNode *node, char *number,
                             ReverseNumber **reverse) {
    ReverseNumber *newReverseNumber = malloc(sizeof(ReverseNumber));
    if (newReverseNumber == NULL || !initReverseNumber(node)) {
        free(newReverseNumber);
        return false;
    }

    if (node->reverseNumber->nextNumber != NULL) {
        node->reverseNumber->nextNumber->previousNumber = newReverseNumber;
    }

    newReverseNumber->number = number;
    newReverseNumber->previousNumber = node->reverseNumber;
    newReverseNumber->nextNumber = node->reverseNumber->nextNumber;
    node->reverseNumber->nextNumber = newReverseNumber;
    node->amountOfNumbers++;
    *reverse = newReverseNumber;

    return true;
}

/** @brief Dodaje do tablicy wszystkie numery odwrotne.
 * Dodaje do tablicy @p array wszystkie numery utworzone poprzez zamianę
 * prefiksu o długości @p skipLength numeru @p number na numery odwrotne trzymane
 * w węźle drzewa trie @p node.
 * @param[in] node         - wskaźnik na węzeł drzewa Trie;
 * @param[in] array        - wskaźnik na tablicę napisów;
 * @param[in] number       - wskaźnik na napis reprezentujący numer;
 * @param[in] numberLength - długość numeru;
 * @param[in] skipLength   - długość prefiksu.
 * @return @p True jeśli udało się dodać wszystkie numery.
 *         @p False w przeciwnym przypadku.
 */
static bool addAllReverse(TrieNode *node, StringArray *array,
                          char const *number, size_t numberLength,
                          size_t skipLength) {
    if (node->amountOfNumbers == 0) {
        return true;
    }

    ReverseNumber *tmp = node->reverseNumber->nextNumber;
    char *newNumber;
    size_t prefixLength;

    while (tmp != NULL) {
        prefixLength = strlenSafe(tmp->number);
        newNumber = changePrefix(number, tmp->number, numberLength,
                                 prefixLength, skipLength);
        if (newNumber == NULL || !addToStringArray(array, newNumber)) {
            free(newNumber);
            return false;
        }
        tmp = tmp->nextNumber;
    }

    return true;
}

bool addNumber(DoubleTrie *doubleTrie, char *number1, char *number2) {
    if (doubleTrie == NULL) {
        return false;
    }

    TrieNode *normalStart = doubleTrie->NormalTrie;
    TrieNode *reverseStart = doubleTrie->ReverseTrie;
    if (!goToPrefix(&normalStart, number1, true) ||
        !addNormalNumber(normalStart, number2)) {
        tryToDeleteNode(normalStart);
        return false;
    }

    if (!goToPrefix(&reverseStart, number2, true) ||
        !addReverseNumber(reverseStart, number1,
                          &normalStart->normalNumber->reverse)) {
        free(normalStart->normalNumber);
        normalStart->amountOfNumbers = 0;
        tryToDeleteNode(normalStart);
        tryToDeleteNode(reverseStart);
        return false;
    }

    normalStart->normalNumber->reverseNode = reverseStart;

    return true;
}

DoubleTrie *newDoubleTrie(void) {
    DoubleTrie *doubleTrie = malloc(sizeof(DoubleTrie));
    if (doubleTrie == NULL) {
        return NULL;
    }

    doubleTrie->NormalTrie = malloc(sizeof(TrieNode));
    doubleTrie->ReverseTrie = malloc(sizeof(TrieNode));
    if (doubleTrie->NormalTrie == NULL || doubleTrie->ReverseTrie == NULL) {
        free(doubleTrie->NormalTrie);
        free(doubleTrie->ReverseTrie);
        free(doubleTrie);
        return NULL;
    }

    initTrieNode(doubleTrie->NormalTrie);
    initTrieNode(doubleTrie->ReverseTrie);
    doubleTrie->NormalTrie->amountOfNumbers = 1;
    doubleTrie->ReverseTrie->amountOfNumbers = 1;

    return doubleTrie;
}

char *findLongest(DoubleTrie *doubleTrie, char const *num, size_t *length,
                  size_t *skipLength) {
    char *longest = NULL;
    TrieNode *node = doubleTrie->NormalTrie;
    size_t i = 0;
    *skipLength = i;
    for (; i < *length; i++) {
        int currentDigit = charToDecimal(num[i]);
        node = node->nextDigits[currentDigit];
        if (node == NULL) {
            *length = strlenSafe(longest);
            return longest;
        }
        if (node->normalNumber != NULL) {
            longest = node->normalNumber->number;
            *skipLength = i + 1;
        }
    }

    *length = strlenSafe(longest);

    return longest;
}

void trieRemove(DoubleTrie *doubleTrie, char const *num) {
    TrieNode *node = doubleTrie->NormalTrie;

    if (goToPrefix(&node, num, false)) {
        TrieNode *tmp = node->previousDigit;
        deleteTrie(node);
        tryToDeleteNode(tmp);
    }
}

StringArray *getReverse(DoubleTrie *doubleTrie, char *num, size_t numLength) {
    StringArray *reverseArray;
    TrieNode *node = doubleTrie->ReverseTrie;
    size_t index = 0;
    initStringArray(&reverseArray);

    if (reverseArray == NULL || !addToStringArray(reverseArray, num)) {
        arrayDelete(reverseArray);
        return NULL;
    }

    while (num[index] != '\0') {
        if (node->nextDigits[charToDecimal(num[index])] == NULL) {
            break;
        }

        node = node->nextDigits[charToDecimal(num[index])];
        index++;
        if (!addAllReverse(node, reverseArray, num, numLength, index)) {
            arrayDelete(reverseArray);
            return NULL;
        }
    }

    return reverseArray;
}

void deleteDoubleTrie(DoubleTrie *doubleTrie) {
    deleteTrie(doubleTrie->NormalTrie);
    free(doubleTrie->ReverseTrie);
    free(doubleTrie);
}