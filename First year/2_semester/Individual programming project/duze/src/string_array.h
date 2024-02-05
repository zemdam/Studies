/** @file
* Interfejs klasy tablicy dynamicznej dla napisów.
*
* @author Adam Zembrzuski <az430277@students.mimuw.edu.pl>
* @copyright Uniwersytet Warszawski
* @date 2022
*/

#ifndef PHONE_NUMBERS_STRING_ARRAY_H
#define PHONE_NUMBERS_STRING_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

typedef struct StringArray StringArray; ///< Definicja typu StringArray.

/** @brief Tworzy i inicjuje nową tablicę napisów.
 * Tworzy nową pustą strukturę do przechowywania napisów. Zostanie ona zapisana
 * do @p toInit.
 * @param[out] toInit - podwójny wskaźnik na strukturę.
 * @return Wartość @p false jeśli udało się alokować potrzebną pamięć.
 *         Wartość @p true w przeciwnym przypadku.
 */
extern bool initStringArray(StringArray **toInit);

/** @brief Dodaje napis do tablicy.
 * Dodaje napis @p string do tablicy @p array.
 * @param[in] array  - wskaźnik na tablicę napisów;
 * @param[in] string - wskaźnik na napis.
 * @return Wartość @p true jeśli udało sie dodać.
 *         Wartość @p false w przeciwnym przypadku.
 */
extern bool addToStringArray(StringArray *array, char *string);

/** @brief Zwraca napis pod podanym indeksie w tablicy.
 * Jeśli to możliwe to zwraca napis znajdujący się pod indeksem @p idx
 * w tablicy napisów @p array.
 * @param[in] array - wskaźnik na tablicę napisów;
 * @param[in] idx   - wartość indeksu.
 * @return Wyznaczony napis lub @p NULL gdy został podany niepoprawny indeks.
 */
extern char const *atIndex(StringArray *array, size_t idx);

/** @brief Usuwa tablicę napisów.
 * Usuwa tablicę napisów @p array.
 * @param[in] array - wskaźnik na tablicę napisów.
 */
extern void arrayDelete(StringArray *array);

/** @brief Sortuje numery.
 * Sortuje numery zapisane w tablicy napisów @p array oraz usuwa powtórzenia.
 * @param[in] array - wskaźnik na tablicę napisów.
 */
extern void sortNumbers(StringArray *array);

#endif //PHONE_NUMBERS_STRING_ARRAY_H
