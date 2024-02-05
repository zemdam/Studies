#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#ifndef POLA
#define POLA 5
#endif
#ifndef WIERSZE
#define WIERSZE 26
#endif
#ifndef KOLUMNY
#define KOLUMNY 26
#endif
#ifndef WYBOR
#define WYBOR 1000
#endif


typedef struct Ruchy {
    int wiersz;
    int kolumna;
    int roznica;
} Ruch;

/* Funkcja: wstaw_min
 * ------------------
 * Wstawia "min" oraz 1 na pierwsze miejsce zadanej tablicy.
 *
 * ruch_prawego: tablica typu Ruch o conajmniej dwóch elementach
 * min: zmienna typu int
*/
void wstaw_min(Ruch ruch_prawego[], int min) {
    ruch_prawego[0].wiersz = 1;
    ruch_prawego[0].roznica = min;
    ruch_prawego[0].kolumna = min;
}

/* Funkcja: wyzeruj
 * ----------------
 * Wstawia zera do tablicy dwuwymiarowej.
 *
 * plansza: dwuwymiarowa tablica typu int która rozmiar ma
 *          zadana kolejno stałymi WIERSZE i KOLUMNY
*/
void wyzeruj(int plansza[WIERSZE][KOLUMNY]) {
    for (int i = 0; i < WIERSZE; i++) {
        for (int j = 0; j < KOLUMNY; j++) {
            plansza[i][j] = 0;
        }
    }
}

/* Funkcja: czy_zero
 * -----------------
 * Sprawdza czy "suma" jest zerem.
 *
 * suma: zmienna typu print
 *
 * zwraca: "1" jeśli "suma" jest zerem
 *         "0" w przeciwnym wypadku
*/
int czy_zero(int suma) {
    if (suma == 0)
        return 1;
    else
        return 0;
}

/* Funkcja: odcinek_wiersza
 * ------------------------
 * Liczy sumę elementów od miejsca "poczatek" i długosci POLA
 * we wierszu "numer_wiersza" w "plansza".
 *
 * plansza: tablica dwuwymiarowa typu int
 * poczatek: zmienna typu int od 0 do KOLUMNY-1
 * numer_wiersza: zmienna typu int od 0 do WIERSZE-1
 *
 * zwraca: obliczona "suma"
*/
int odcinek_wiersza(int plansza[WIERSZE][KOLUMNY], int poczatek, int numer_wiersza) {
    int suma = 0;
    for (int i = poczatek; i < POLA + poczatek; i++)
        suma += plansza[numer_wiersza][i];
    return suma;
}

/* Funkcja: ocena_wiersza
 * ----------------------
 * Sprawdza ile jest odcinkow dlugości POLA o sumie "0" nie majacych zadnych
 * elementów wspólnych w "plansza" o numerze wiersza "numer_wiersza"
 *
 * plansza: tablica dwuwymiarowa typu int
 * numer_wiersza: zmienna typu int od 0 do WIERSZE-1
 *
 * zwraca: liczba odcinków które policzy funkcja
*/
int ocena_wiersza(int plansza[WIERSZE][KOLUMNY], int numer_wiersza) {
    int ocena = 0;
    int j = 0;
    int i = POLA - 1;
    int suma;
    while (i < KOLUMNY) {
        suma = odcinek_wiersza(plansza, j, numer_wiersza);
        ocena += czy_zero(suma);
        i++;
        while (i < KOLUMNY && suma != 0) {
            suma += plansza[numer_wiersza][i];
            suma -= plansza[numer_wiersza][i - POLA];
            i++;
            ocena += czy_zero(suma);
        }
        j = i;
        i = i + POLA - 1;
    }
    return ocena;
}

/* Funkcja: odcinek_kolumny
 * ------------------------
 * Liczy sumę elementów od miejsca "poczatek" i długosci POLA
 * w kolumnie "numer_kolumny w "plansza".
 *
 * plansza: tablica dwuwymiarowa typu int
 * poczatek: zmienna typu int od 0 do WIERSZE-1
 * numer_kolumny: zmienna typu int od 0 do KOLUMNY-1
 *
 * zwraca: obliczona "suma"
*/
int odcinek_kolumny(int plansza[WIERSZE][KOLUMNY], int poczatek, int numer_kolumny) {
    int suma = 0;
    for (int i = poczatek; i < POLA + poczatek; i++)
        suma += plansza[i][numer_kolumny];
    return suma;
}

/* Funkcja: ocena_kolumny
 * ----------------------
 * Sprawdza ile jest odcinkow dlugości POLA o sumie "0" nie majacych zadnych
 * elementów wspólnych w "plansza" o numerze kolumny "numer_kolumny"
 *
 * plansza: tablica dwuwymiarowa typu int
 * numer_kolumny: zmienna typu int od 0 do KOLUMNY-1
 *
 * zwraca: liczba odcinków które policzy funkcja
*/
int ocena_kolumny(int plansza[WIERSZE][KOLUMNY], int numer_kolumny) {
    int ocena = 0;
    int j = 0;
    int i = POLA - 1;
    int suma;
    while (i < WIERSZE) {
        suma = odcinek_kolumny(plansza, j, numer_kolumny);
        ocena += czy_zero(suma);
        i++;
        while (i < WIERSZE && suma != 0) {
            suma += plansza[i][numer_kolumny];
            suma -= plansza[i - POLA][numer_kolumny];
            i++;
            ocena += czy_zero(suma);
        }
        j = i;
        i = i + POLA - 1;
    }
    return ocena;
}

/* Funkcja: ocen_kolumny
 * ---------------------
 * Wywołuje funkcję "ocena_kolumny" dla wszystkich kolumn
 * w "plansza" i zapisuje ich wynik dla kazdej z nich w
 * tablicy "kolumny".
 *
 * plansza: tablica dwuwymiarowa typu int
 * kolumny: tablica typu int która ma KOLUMNY elementow
*/
void ocen_kolumny(int plansza[WIERSZE][KOLUMNY], int kolumny[]) {
    for (int i = 0; i < KOLUMNY; i++) {
        kolumny[i] = ocena_kolumny(plansza, i);
    }
}

/* Funkcja: ocen_wiersze
 * ---------------------
 * Wywołuje funkcję "ocena_wiersza" dla wszystkich wierszy
 * w "plansza" i zapisuje ich wynik dla kazdego z nich w
 * tablicy "wiersze".
 *
 * plansza: tablica dwuwymiarowa typu int
 * wiersze: tablica typu int która ma WIERSZE elementow
*/
void ocen_wiersze(int plansza[WIERSZE][KOLUMNY], int wiersze[]) {
    for (int i = 0; i < WIERSZE; i++) {
        wiersze[i] = ocena_wiersza(plansza, i);
    }
}

/* Funkcja: zamien_duza_litere
 * ---------------------------
 * Zamienia kod ASCII duzej litery na jej odpowiednik gdzie
 * A to 0, B to 1 i tak dalej
 *
 * n: zmienna typu int od 65 do 90
*/
int zamien_duza_litere(int n) {
    return n - 65;
}

/* Funkcja: zamien_mala_litere
 * ---------------------------
 * Zamienia kod ASCII malej litery na jej odpowiednik gdzie
 * a to 0, b to 1 i tak dalej
 *
 * n: zmienna typu int od 97 do 122
*/
int zamien_mala_litere(int n) {
    return n - 97;
}

/* Funkcja: zaznacz_prawy
 * ----------------------
 * Zamienia 0 na 1 lub 1 na 0 odcinka we wierszu o numerze "wiersz" w "plansza"
 * o długości POLA od kolumny z numerem "kolumna".
 *
 * plansza: tablica dwuwymiarowa typu int
 * wiersz: zmienna typu int od 0 do WIERSZE-1
 * kolumna: zmienna typu int od 0 do KOLUMNY-1
*/
void zaznacz_prawy(int plansza[WIERSZE][KOLUMNY], int wiersz, int kolumna) {
    for (int i = kolumna; i < kolumna + POLA; i++) {
        plansza[wiersz][i] = abs(plansza[wiersz][i] - 1);
    }
}

/* Funkcja: zaznacz_lewy
 * ----------------------
 * Zamienia 0 na 1 lub 1 na 0 odcinka w kolumnie o numerze "kolumna" w "plansza"
 * o długości POLA od wiersza z numerem "wiersz".
 *
 * plansza: tablica dwuwymiarowa typu int
 * wiersz: zmienna typu int od 0 do KOLUMNY-1
 * kolumna: zmienna typu int od 0 do WIERSZE-1
*/
void zaznacz_lewy(int plansza[WIERSZE][KOLUMNY], int wiersz, int kolumna) {
    for (int i = kolumna; i < kolumna + POLA; i++) {
        plansza[i][wiersz] = abs(plansza[i][wiersz] - 1);
    }
}

/* Funkcja wykonaj_ruch
 * --------------------
 * Zaznacza na planszy ruch lewego i aktualizuje ocenę kolumn i wierszy.
 *
 * plansza: tablica dwuwymiarowa typu int
 * wiersz: zmienna typu int od 65 do KOLUMNY + 65
 * kolumna: zmienna typu int od 97 do WIERSZE + 97
 * wiersze: tablica typu int o WIERSZE elementach
 * kolumny: tablica typu int o KOLUMNY elementach
*/
void wykonaj_ruch(int plansza[WIERSZE][KOLUMNY], int wiersz, int kolumna, int wiersze[], int kolumny[]) {
    zaznacz_lewy(plansza, zamien_duza_litere(wiersz), zamien_mala_litere(kolumna));
    kolumny[zamien_duza_litere(wiersz)] = ocena_kolumny(plansza, zamien_duza_litere(wiersz));
    for (int i = zamien_mala_litere(kolumna); i < zamien_mala_litere(kolumna) + POLA; i++) {
        wiersze[i] = ocena_wiersza(plansza, i);
    }
}

/* Funkcja: sumuj_tablice_bez_fragmentu
 * ------------------------------------
 * Sumuje tablice bez fragmentu o indekscie początkowym "poczatek" i końcowym
 * "koniec".
 *
 * tablica: tablica typu int o "dlugosc" elementow
 * dlugosc: zmienna typu int większa od 0
 * poczatek: zmienna typu int większa od -1 i mniejsza od "dlugosc"
 * koniec: zmienna typu int większa od "poczatek" - 1 i mniejsza od "dlugosc"
 *
 * zwraca: obliczoną sumę
*/
int sumuj_tablice_bez_fragmentu(int tablica[], int dlugosc, int poczatek, int koniec) {
    int suma = 0;
    for (int i = 0; i < poczatek; i++)
        suma += tablica[i];
    for (int i = koniec + 1; i < dlugosc; i++)
        suma += tablica[i];
    return suma;
}

/* Funkcja: wpisz_wybory
 * ---------------------
 * Wpisuje podany ruch do "ruch_prawego" w zaleznosci od jego oceny wyrazonej "roznica".
 *
 * ruch_prawego: tablica typu Ruch o WIERSZE*KOLUMNY elementach
 * wiersz: zmienna typu int od 0 do WIERSZE-1
 * kolumna: zmienna typu int od 0 do KOLUMNY-1
 * roznica: zmienna typu int
*/
void wpisz_wybory(Ruch ruch_prawego[], int wiersz, int kolumna, int roznica) {
    int i = ruch_prawego[0].wiersz;
    if (roznica == ruch_prawego[0].roznica) {
        ruch_prawego[i].roznica = roznica;
        ruch_prawego[i].kolumna = kolumna;
        ruch_prawego[i].wiersz = wiersz;
        ruch_prawego[0].wiersz++;
    } else if (roznica > ruch_prawego[0].roznica) {
        ruch_prawego[1].roznica = roznica;
        ruch_prawego[1].kolumna = kolumna;
        ruch_prawego[1].wiersz = wiersz;
        ruch_prawego[0].wiersz = 2;
        ruch_prawego[0].roznica = roznica;
    }
}

/* Funkcja: ocena
 * --------------
 * Liczy ocene dla danego ruchu i w zaleznosci od wyniku wpisuje ja do listy
 * mozliwych ruchow
 *
 * plansza: tablica dwuwymiarowa typu int
 * wiersz: zmienna typu int od 0 do WIERSZE-1
 * kolumna: zmienna typu int od 0 do KOLUMNY-1
 * ruch_prawego: tablica typu Ruch o WIERSZE*KOLUMNY elementach
 * wiersze: tablica typu int o WIERSZE elementach
 * kolumny: tablica typu int o KOLUMNY elementach
*/
void ocena(int plansza[WIERSZE][KOLUMNY], int wiersz, int kolumna, Ruch ruch_prawego[], int wiersze[], int kolumny[]) {
    zaznacz_prawy(plansza, wiersz, kolumna);
    //printf("wiersz: %d kolumna: %d\n", wiersz, kolumna);
    int suma_wierszy = sumuj_tablice_bez_fragmentu(wiersze, WIERSZE, wiersz, wiersz) + ocena_wiersza(plansza, wiersz);
    int suma_ocen_kolumn = 0;
    for (int i = kolumna; i < POLA + kolumna; i++)
        suma_ocen_kolumn += ocena_kolumny(plansza, i);
    int suma_kolumn = sumuj_tablice_bez_fragmentu(kolumny, KOLUMNY, kolumna, kolumna + POLA - 1) + suma_ocen_kolumn;
    int roznica = suma_wierszy - suma_kolumn;
    zaznacz_prawy(plansza, wiersz, kolumna);
    wpisz_wybory(ruch_prawego, wiersz, kolumna, roznica);
}

/* Funkcja: wyznacz_indeks
 * -----------------------
 * Liczy który ruch należy wykonać.
 *
 * n: zmienna typu int większa od 0
 *
 * zwraca: indeks ruchu
*/
int wyznacz_indeks(int n) {
    return WYBOR % n;
}

/* Funkcja: wybor
 * --------------
 * Wybiera ruch który wykonuje prawy gracz i wyświetla go na wyjściu.
 *
 * plansza: tablica dwuwymiarowa typu int
 * ruch_prawego: tablica typu Ruch o WIERSZE*KOLUMNY elementach
 * wiersze: tablica typu int o WIERSZE elementach
 * kolumny: tablica typu int o KOLUMNY elementach
*/
void wybor(int plansza[WIERSZE][KOLUMNY], Ruch ruch_prawego[], int wiersze[], int kolumny[]) {
    int indeks = wyznacz_indeks(ruch_prawego[0].wiersz - 1) + 1;
    int wiersz = ruch_prawego[indeks].wiersz;
    int kolumna = ruch_prawego[indeks].kolumna;
    printf("%c%c\n", wiersz + 97, kolumna + 65);
    zaznacz_prawy(plansza, wiersz, kolumna);
    wiersze[wiersz] = ocena_wiersza(plansza, wiersz);

    for (int i = kolumna; i < kolumna + POLA; i++) {
        kolumny[i] = ocena_kolumny(plansza, i);
    }
    wstaw_min(ruch_prawego, ruch_prawego[0].kolumna);
}

/* Funkcja mozliwe_ruchy_prawego
 * -----------------------------
 * Sprawdza wszystkie mozliwe ruchy prawego i wpisuje
 * ruchy o najwyższej ocenie do ruch_prawego.
 *
 * plansza: tablica dwuwymiarowa typu int
 * ruch_prawego: tablica typu Ruch o WIERSZE*KOLUMNY elementach
 * wiersze: tablica typu int o WIERSZE elementach
 * kolumny: tablica typu int o KOLUMNY elementach
 *
 * zwraca: true jeśli istnieją możliwe ruchy
 *         false w przeciwnym przypadku
*/
bool mozliwe_ruchy_prawego(int plansza[WIERSZE][KOLUMNY], Ruch ruch_prawego[], int wiersze[], int kolumny[]) {
    int wiersz = 0;
    int kolumna = 0;
    int suma;
    while (wiersz < WIERSZE) {
        kolumna = 0;
        if (POLA <= KOLUMNY) {
            suma = odcinek_wiersza(plansza, kolumna, wiersz);
            int i = POLA;
            if (suma == 0)
                ocena(plansza, wiersz, kolumna, ruch_prawego, wiersze, kolumny);
            while (i < KOLUMNY) {
                suma += plansza[wiersz][i];
                suma -= plansza[wiersz][kolumna];
                i++;
                kolumna++;
                if (suma == 0)
                    ocena(plansza, wiersz, kolumna, ruch_prawego, wiersze, kolumny);
            }
        }
        wiersz++;
    }
    if (ruch_prawego[0].wiersz == 1) {
        printf(".\n");
        return false;
    } else {
        wybor(plansza, ruch_prawego, wiersze, kolumny);
        return true;
    }
}

/* Funkcja: bez_znaku_nowej_lini
 * -----------------------------
 * Wczytuje znak z wejścia aż będzie on
 * różny od znaku nowej lini
 *
 * zwraca: znak różny od znaku nowej lini
*/
int bez_znaku_nowej_lini() {
    int liczba = getchar();
    while (liczba == '\n')
        liczba = getchar();
    return liczba;
}

/* Funkcja: wczytaj
 * ----------------
 * Wczytuje z wejścia ruch gracza lewego i go wykonuje.
 *
 * plansza: tablica dwuwymiarowa typu int
 * wiersze: tablica typu int o WIERSZE elementach
 * kolumny: tablica typu int o KOLUMNY elementach
 *
 * zwraca: true jeśli gracz podał pozycję
 *         false jeśli się poddał czyli podał '.'
*/
bool wczytaj(int plansza[WIERSZE][KOLUMNY], int wiersze[], int kolumny[]) {
    int pierwsza = bez_znaku_nowej_lini();
    if (pierwsza == '.') return false;
    int druga = bez_znaku_nowej_lini();
    wykonaj_ruch(plansza, pierwsza, druga, wiersze, kolumny);
    return true;
}

/* Funkcja: petla
 * --------------
 * Wczytuje ruchy lewego i podaje ruchy prawego dopóki
 * gracz lewy się nie podda lub gracz prawy dalej będzie miał
 * możliwe ruchy.
 *
 * plansza: tablica dwuwymiarowa typu int
 * ruch_prawego: tablica typu Ruch o WIERSZE*KOLUMNY elementach
 * wiersze: tablica typu int o WIERSZE elementach
 * kolumny: tablica typu int o KOLUMNY elementach
*/
void petla(int plansza[WIERSZE][KOLUMNY], Ruch ruch_prawego[], int wiersze[], int kolumny[]) {
    bool czy_dalej;
    int pierwsza = bez_znaku_nowej_lini();
    if (pierwsza == '.') return;
    ocen_kolumny(plansza, kolumny);
    ocen_wiersze(plansza, wiersze);
    int min = sumuj_tablice_bez_fragmentu(kolumny, KOLUMNY, -1, -1);
    wstaw_min(ruch_prawego, -min);
    if (pierwsza == '-')
        czy_dalej = mozliwe_ruchy_prawego(plansza, ruch_prawego, wiersze, kolumny);
    else {
        int druga = bez_znaku_nowej_lini();
        wykonaj_ruch(plansza, pierwsza, druga, wiersze, kolumny);
        czy_dalej = mozliwe_ruchy_prawego(plansza, ruch_prawego, wiersze, kolumny);
    }
    while (czy_dalej && wczytaj(plansza, wiersze, kolumny)) {
        czy_dalej = mozliwe_ruchy_prawego(plansza, ruch_prawego, wiersze, kolumny);
    }
}

int main() {
    int plansza[WIERSZE][KOLUMNY];                 //deklaruje plansze do gry
    wyzeruj(plansza);                              //zeruje wszystkie miejsca
    Ruch ruch_prawego[WIERSZE * KOLUMNY + 1];      //deklaruje tablice na mozliwe ruchy prawego
    int kolumny[KOLUMNY];                          //deklaruje tablice na ocene kolumn
    int wiersze[WIERSZE];                          //deklaruje tablice na ocene wierszy
    petla(plansza, ruch_prawego, wiersze, kolumny);//rozpoczynam gre
    return 0;
}
