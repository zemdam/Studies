/** @file
 * Interfejs klasy drzew trie dla numerów telefonicznych.
 *
 * @author Adam Zembrzuski <az430277@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
*/

#ifndef PHONE_NUMBERS_TRIE_H
#define PHONE_NUMBERS_TRIE_H

#include "string_array.h"

typedef struct DoubleTrie DoubleTrie; ///< Definicja typu DoubleTrie.

/** @brief Dodaje przekierowania do podwójnego drzewa.
 * Do drzewa ze zwykłymi numerami zawartego w @p doubleTrie dodaje przekierowanie
 * numeru @p number1 na @p number2. Natomiast do drzewa z odwrotnymi numerami
 * dodaje przekierowanie numeru @p number2 na @p number1.
 * @param[in] doubleTrie - wskaźnik na podwójne drzewo;
 * @param[in] number1    - wskaźnik na napis reprezentujący numer;
 * @param[in] number2    - wskaźnik na napis reprezentujący numer.
 * @return @p True jeśli udało się dodać przekierowania.
 *         @p False jeśli @p doubleTrie to NULL lub nie udało się dodać
 *         przekierowań.
 */
extern bool addNumber(DoubleTrie *doubleTrie, char *number1, char *number2);

/** @brief Tworzy nowe podwójne drzewo.
 * Tworzy i inicjuje strukturę przechowująca puste drzewo ze zwykłymi numerami
 * oraz z odwrotnymi.
 * @return Wskaźnik na utworzoną strukturę gdy udało się ją utworzyć.
 *         @p NULL w przeciwnym przypadku.
 */
extern DoubleTrie *newDoubleTrie(void);

/** @brief Usuwa podwójne drzewo.
 * Usuwa obydwa drzewa przechowywane w strukturze @p doubleTrie.
 * @param[in] doubleTrie - wskaźnik na strukturę przechowującą dwa drzewa trie.
 */
extern void deleteDoubleTrie(DoubleTrie *doubleTrie);

/** @brief Szuka najgłębszego przekierowania w drzewie trie.
 * Dla numeru @p num szuka w drzewie trie ze zwykłymi numerami
 * trzymanego w @p doubleTrie przekierowania który zastąpi jak największy prefiks.
 * @param[in] doubleTrie          - wskaźnik na podwójne drzewo;
 * @param[in] num         - wskaźnik na napis reprezentujący numer;
 * @param[in, out] length - wskaźnik na długość numeru @p num, przez który później
 *                          zostanie zwrócona długość @p num po przekierowaniu;
 * @param[out] skipLength - wskaźnik przez który zostanie zwrócona długość
 *                          przekierowanego prefiksu.
 * @return Wskaźnik na wyznaczone przekierowanie lub @p NULL gdy takie
 *         przekierowanie nie istnieje.
 */
extern char *findLongest(DoubleTrie *doubleTrie, char const *num,
                         size_t *length, size_t *skipLength);

/** @brief Usuwa przekierowanie zwykłego numeru.
 * Z drzewa trie ze zwykłymi numerami trzymanego w @p doubleTrie usuwa
 * przekierowanie numeru @p num.
 * @param[in] doubleTrie - wskaźnik na podwójne drzewo;
 * @param[in] num        - wskaźnik na napis reprezentujący numer.
 */
extern void trieRemove(DoubleTrie *doubleTrie, char const *num);

/** Wyznacza wszystkie numery z odwrotnym prefiksem.
 * Dla numeru @p num wyznacza wszystkie numery utworzone poprzez zamianę
 * prefiksu na numery odwrotne trzymane w drzewie z odwrotnymi numerami
 * zawartego w @p doubleTrie.
 * @param[in] doubleTrie - wskaźnik na podwójne drzewo;
 * @param[in] num        - wskaźnik na napis reprezentujący numer;
 * @param[in] numLength  - długość numeru.
 * @return Tablica z numerami jeśli nie zabrakło pamięci.
 *         @p NULL w przeciwnym przypadku.
 */
extern StringArray *getReverse(DoubleTrie *doubleTrie, char *num,
                               size_t numLength);

#endif //PHONE_NUMBERS_TRIE_H
