/** @file
* Interfejs klasy pomocnych funkcji dla napisów.
*
* @author Adam Zembrzuski <az430277@students.mimuw.edu.pl>
* @copyright Uniwersytet Warszawski
* @date 2022
*/

#ifndef PHONE_NUMBERS_STRING_TOOLS_H
#define PHONE_NUMBERS_STRING_TOOLS_H

#include <stdbool.h>
#include <stddef.h>

/** @brief Kopiuje napis.
 * Zwalnia, a następnie alokuje pamięć w @p destination i kopiuje napis
 * @p source do @p destination.
 * @param[in, out] destination – podwójny wskaźnik na napis;
 * @param[in] source           – wskaźnik na napis;
 * @param[in] length           – długość napisu reprezentowanego przez @p source.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         alokować pamięci.
 */
extern bool copyString(char **destination, char const *source, size_t length);

/** @brief Wyznacz długość numeru.
 * Wyznacza długość napisu reprezentującego numer @p number.
 * @param[in] number - wskaźnik na napis reprezentujący numer.
 * @return Długość numeru gdy numer jest poprawny.
 *         Wartość @p 0 gdy numer nie jest poprawny czyli nie jest zakończony
 *         @p \0 lub jest zbyt długi.
 */
extern size_t stringNumberLength(char const *number);

/** @brief Zwraca cyfrę.
 * Zamienia kod ASCII reprezentowany przez @p asciiCode na odpowiadająca mu cyfrę
 * rozszerzoną o * = 10 oraz # = 11.
 * @param[in] asciiCode - zmienna typu char odpowiadająca cyfrze.
 * @return Wyznaczona cyfra.
 */
extern int charToDecimal(char asciiCode);

/** Zamienia prefiks napisu.
 * Zmienia prefiks o długości @p skipLength napisu @p string na @p prefix.
 * @param[in] string       - wskaźnik na napis;
 * @param[in] prefix       - wskaźnik na napis;
 * @param[in] stringLength - długość @p string;
 * @param[in] prefixLength - długość @p prefix;
 * @param[in] skipLength   - długość prefiksu @p string.
 * @return Napis z zamienionym prefiksem lub @p NULL jeśli zabrakło pamięci lub
 *         nowy napis byłby zbyt długi.
 */
extern char *changePrefix(char const *string, char const *prefix,
                          size_t stringLength, size_t prefixLength,
                          size_t skipLength);

/** @brief Sprawdza długość napisu.
 * Sprawdza czy @p string nie jest @p NULL'em, a następnie wyznacza długość
 * napisu.
 * @param[in] string - wskaźnik na napis.
 * @return Długość napisu lub 0 kiedy @p string równa się @p NULL.
 */
extern size_t strlenSafe(char const *string);

#endif //PHONE_NUMBERS_STRING_TOOLS_H
