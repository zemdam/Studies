#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Funckja: iton
 * -------------
 * Wstawia liczbe "x" do tablicy "*a" w reprezentacji
 * BBR-NAF i zapisuje dlugosc tablicy do "*n".
 *
 * x: zmienna typu int
 * a: adres do tablicy typu int
 * n: adres do zmiennej typu int
*/
void iton(int x, int **a, int *n) {
    int i = 0;
    int *wynik = NULL;
    int rozmiar = 0;
    if (x == INT_MAX) {
        wynik = realloc(wynik, (size_t) 2 * sizeof(int));
        wynik[0] = -1;
        wynik[1] = 31;
        rozmiar = 2;
    } else if (x == INT_MIN) {
        wynik = realloc(wynik, (size_t) sizeof(int));
        wynik[0] = -32;
        rozmiar = 1;
    } else {
        while (x != 0) {
            if (x % 2 == 0) {
                x = x / 2;
                i++;
            } else if ((x - 1) % 4 == 0) {
                rozmiar++;
                wynik = realloc(wynik, (size_t) rozmiar * sizeof(int));
                wynik[rozmiar - 1] = i;
                x = (x - 1) / 2;
                i++;
            } else {
                rozmiar++;
                wynik = realloc(wynik, (size_t) rozmiar * sizeof(int));
                wynik[rozmiar - 1] = -(i + 1);
                x = (x + 1) / 2;
                i++;
            }
        }
    }
    *a = wynik;
    *n = rozmiar;
}

/* Funckja: indeks
 * ---------------
 * Zwraca wykladnik ktory reprezentuje "liczba".
 *
 * liczba: zmienna typu int
 *
 * zwraca: zmienna typu int
*/
int indeks(int liczba) {
    if (liczba >= 0) return liczba;
    else if (liczba == -1)
        return 0;
    else
        return -(liczba + 1);
}


/* Funkcja: minindeks
 * ------------------
 * Zwraca -(wykladnik-1) ktory reprezentuje "liczba".
 *
 * liczba: zmienna typu int
 *
 * zwraca: zmienna typu int
*/
int minindeks(int liczba) {
    if (liczba < 0) return liczba;
    else
        return -(liczba + 1);
}


/* Funkcja: zamien
 * ---------------
 * Zamienia liczbe typu BBR-NAF zapisana w tablicy "c" o dlugosci
 * na "cn" na przeciwna.
 *
 * c: tablica typu int
 * cn: zmienna typu int wieksza badz rowna 0
*/
void zamien(int c[], int cn) {
    for (int i = 0; i < cn; i++) {
        if ((c)[i] >= 0) (c)[i] = minindeks((c)[i]);
        else
            (c)[i] = indeks((c)[i]);
    }
}


/* Funkcja: dodaj
 * --------------
 * Dodaje do liczby typu BBR-NAF zapisanej w tablicy
 * "*c" o dlugosci "*cn" liczbe "x" ktora przedstawia przynajmniej
 * tak samo znaczacy wykladnik co ostatni w tablicy.
 *
 * c: adres do tablicy typu int
 * cn: adres do zmiennej typu int wiekszej badz rownej 0
 * x: zmienna typu int
*/
void dodaj(int **c, int *cn, int x) {
    if (*cn == 0) {
        int *wynik = NULL;
        wynik = realloc(wynik, sizeof(int));
        wynik[0] = x;
        (*cn)++;
        *c = wynik;
    } else {
        int poprz = (*c)[*cn - 1];
        if (indeks(x) > indeks(poprz) + 1) {
            (*cn)++;
            *c = realloc(*c, (size_t) (*cn) * sizeof(int));
            (*c)[*cn - 1] = x;
        } else if (x >= 0) {
            if (poprz >= 0) {
                if (x == poprz) (*c)[*cn - 1]++;
                else {
                    (*cn)++;
                    *c = realloc(*c, (size_t) (*cn) * sizeof(int));
                    (*c)[*cn - 1] = x + 1;
                    (*c)[*cn - 2] = minindeks(poprz);
                }
            } else {
                if (indeks(x) == indeks(poprz)) {
                    (*cn)--;
                    *c = realloc(*c, (size_t) (*cn) * sizeof(int));
                } else {
                    (*c)[*cn - 1] = indeks(poprz);
                }
            }
        } else {
            if (poprz < 0) {
                if (x == poprz) (*c)[*cn - 1]--;
                else {
                    (*cn)++;
                    *c = realloc(*c, (size_t) (*cn) * sizeof(int));
                    (*c)[*cn - 1] = x - 1;
                    (*c)[*cn - 2] = indeks(poprz);
                }
            } else {
                if (indeks(x) == indeks(poprz)) {
                    (*cn)--;
                    *c = realloc(*c, (size_t) (*cn) * sizeof(int));
                } else {
                    (*c)[*cn - 1] = minindeks(poprz);
                }
            }
        }
    }
}

/* Funkcja: nadd
 * -------------
 * Dodaje liczby typu BBR-NAF w tablicy "a" o dlugosci "an" i
 * tablicy "b" o dlugosci "bn" do tablicy "*c" o dlugosci "*cn".
 *
 * a, b: tablica typu int
 * an, bn: zmienna typu int wieksza badz rowna 0
 * c: adres do tablicy typu int
 * cn: adres do zmiennej typu int
*/
void nadd(int *a, int an, int *b, int bn, int **c, int *cn) {
    int i = 0;
    int j = 0;
    int *wynik = NULL;
    int rozmiar = 0;
    while (i < an && j < bn) {
        if (indeks(a[i]) < indeks(b[j])) {
            dodaj(&wynik, &rozmiar, a[i]);
            i++;
        } else if (indeks(a[i]) > indeks(b[j])) {
            dodaj(&wynik, &rozmiar, b[j]);
            j++;
        } else {
            if (a[i] == b[j]) {
                if (a[i] >= 0)
                    dodaj(&wynik, &rozmiar, b[j] + 1);
                else
                    dodaj(&wynik, &rozmiar, b[j] - 1);
            }
            i++;
            j++;
        }
    }
    while (i < an) {
        dodaj(&wynik, &rozmiar, a[i]);
        i++;
    }
    while (j < bn) {
        dodaj(&wynik, &rozmiar, b[j]);
        j++;
    }
    *c = wynik;
    *cn = rozmiar;
}

/* Funkcja: skopiuj
 * ----------------
 * Kopiuje liczbe BBR-NAF z tablicy "z" o dlugosci "dlz" do
 * tablicy "*tu" o dlugosci "*dltu".
 *
 * z: tablica typu int
 * dlz: zmienna typu int wieksza badz rowna 0
 * tu: adres do tablicy typu int
 * dltu: adres do zmiennej typu int
*/
void skopiuj(int *z, int dlz, int **tu, int *dltu) {
    nadd(NULL, 0, z, dlz, tu, dltu);
}

/* Funkcja: sub
 * -------------
 * Odejmuje liczby typu BBR-NAF w tablicy "b" o dlugosci "bn" od
 * tablicy "a" o dlugosci "an" do tablicy "*c" o dlugosci "*cn".
 *
 * a, b: tablica typu int
 * an, bn: zmienna typu int wieksza badz rowna 0
 * c: adres do tablicy typu int
 * cn: adres do zmiennej typu int
*/
void nsub(int *a, int an, int *b, int bn, int **c, int *cn) {
    int *tym;
    int dltym = 0;
    skopiuj(b, bn, &tym, &dltym);
    zamien(tym, dltym);
    nadd(a, an, tym, dltym, c, cn);
    free(tym);
}

/* Funkcja: przesun
 * ----------------
 * Mnozy liczbe typu BBR-NAF w tablicy "a" o dlugosci "an"
 * przez 2^x.
 *
 * a: tablica typu int
 * an: zmienna typu int wieksza badz rowna 0
 * x: zmienna typu int wieksza od 0
*/
void przesun(int *a, int an, int x) {
    for (int i = 0; i < an; i++) {
        if (a[i] >= 0) a[i] = a[i] + x;
        else
            a[i] = a[i] - x;
    }
}

/* Funkcja: nmul
 * -------------
 * Mnozy liczby typu BBR-NAF w tablicy "a" o dlugosci "an" i
 * tablicy "b" o dlugosci "bn" do tablicy "*c" o dlugosci "*cn".
 *
 * a, b: tablica typu int
 * an, bn: zmienna typu int wieksza badz rowna 0
 * c: adres do tablicy typu int
 * cn: adres do zmiennej typu int
*/
void nmul(int *a, int an, int *b, int bn, int **c, int *cn) {
    int *wynik = NULL;
    int *tym = NULL;
    int *tym2 = NULL;
    int tym2dl;
    int tdl;
    int dl = 0;
    int akt;
    int przes = 0;
    skopiuj(b, bn, &tym2, &tym2dl);
    for (int i = 0; i < tym2dl; i++) {
        akt = indeks(tym2[i]);
        przesun(a, an, akt - przes);
        przes = akt;
        if (b[i] >= 0)
            nadd(wynik, dl, a, an, &tym, &tdl);
        else
            nsub(wynik, dl, a, an, &tym, &tdl);
        free(wynik);
        wynik = tym;
        tym = NULL;
        dl = tdl;
    }
    free(tym2);
    *c = wynik;
    *cn = dl;
    przesun(a, an, -przes);
}

/* Funkcja: nexp
 * -------------
 * Poteguje liczbe typu BBR-NAF w tablicy "a" o dlugosci "an" o wykladniku
 * w tablicy "b" o dlugosci "bn" do tablicy "*c" o dlugosci "*cn".
 *
 * a, b: tablica typu int
 * an, bn: zmienna typu int wieksza badz rowna 0
 * c: adres do tablicy typu int
 * cn: adres do zmiennej typu int
*/
void nexp(int *a, int an, int *b, int bn, int **c, int *cn) {
    int *iloczyn, *wyk, *tym, *wynik;
    int dltym, dlil, dlwyk, dlwynik;
    int akt = 0;
    int jed[] = {0};
    iton(1, &wynik, &dlwynik);
    skopiuj(b, bn, &wyk, &dlwyk);
    skopiuj(a, an, &iloczyn, &dlil);
    while (wyk != NULL) {
        int ile = indeks(wyk[0]);
        for (; akt < ile; akt++) {
            nmul(iloczyn, dlil, iloczyn, dlil, &tym, &dltym);
            free(iloczyn);
            iloczyn = tym;
            dlil = dltym;
            jed[0]++;
        }
        tym = NULL;
        dltym = 0;
        nmul(iloczyn, dlil, wynik, dlwynik, &tym, &dltym);
        free(wynik);
        wynik = tym;
        dlwynik = dltym;
        tym = NULL;
        dltym = 0;
        nsub(wyk, dlwyk, jed, 1, &tym, &dltym);
        free(wyk);
        wyk = tym;
        dlwyk = dltym;
        tym = NULL;
        dltym = 0;
    }
    free(iloczyn);
    free(wyk);
    *c = wynik;
    *cn = dlwynik;
}

/* Funkcja: czy_w_int
 * ------------------
 * Sprawdza czy liczba BBR-NAF zapisana w tablicy "a" o dlugosci "n"
 * miesci sie w zakresie int.
 *
 * a: tablica typu int
 * n: zmienna typu int wieksza badz rowna 0
 *
 * zwraca: 0 jesli sie nie miesci
 *         1 jesli sie miesci
*/
int czy_w_int(int *a, int n) {
    int *tym = NULL;
    int tymdl = 0;
    int *test = NULL;
    int dl = 0;
    int x;
    if (a[n - 1] >= 0) x = INT_MAX;
    else
        x = INT_MIN;
    iton(x, &test, &dl);
    nsub(a, n, test, dl, &tym, &tymdl);
    free(test);
    if (tymdl > 0) {
        if (tym[tymdl - 1] >= 0 && a[n - 1] >= 0) x = 0;
        else if (tym[tymdl - 1] < 0 && a[n - 1] < 0)
            x = 0;
        else
            x = 1;
    } else
        x = 1;
    free(tym);
    return x;
}

/* Funkcja: poteguj2
 * -----------------
 * Zwraca bie*2^("doc"-"akt").
 *
 * akt: zmienna typu int wieksza badz rowna 0
 * doc: zmienna typu int wieksza badz rowna akt
 *
 * zwraca: zmienna typu int
*/
int poteguj2(int akt, int doc, int bie) {
    for (int i = 0; i < doc - akt; i++) {
        bie = 2 * bie;
    }
    return bie;
}

/* Funkcja: znak
 * -------------
 * Zwraca 1 lub -1 w zaleznosci od znaku liczby "x".
 *
 * x: zmienna typu int
 *
 * zwraca: 1 jesli x wieksze badz rowne 0
 *        -1 jesli x mniejsze od 0
*/
int znak(int x) {
    if (x >= 0) return 1;
    else
        return -1;
}

/* Funkcja: ntoi
 * -------------
 * Jesli to mozliwe zamienia liczbe BBR-NAF zapisana w tablicy "a"
 * o dlugosci "n" na liczbe typu int jesli to mozliwe.
 *
 * a: tablica typu int
 * n: zmienna typu int wieksza badz rowna 0
 *
 * zwraca: 0 jesli liczba nie miesci w zakresie int
 *         liczbe w przecwinym przypadku
*/
int ntoi(int *a, int n) {
    if (n == 0) return 0;
    if (czy_w_int(a, n) == 0) return 0;

    if (n == 2) {
        if (a[n - 1] == 31 && a[n - 2] == -1) return INT_MAX;
    } else if (n == 1 && a[n - 1] == -32)
        return INT_MIN;
    int wynik = 1;
    int doc = indeks(a[n - 1]);
    int minus = znak(a[n - 1]);
    int akt;
    for (int i = n - 2; i >= 0; i--) {
        akt = indeks(a[i]);
        wynik = poteguj2(akt, doc, wynik);
        doc = akt;
        wynik = minus * (minus * wynik + znak(a[i]));
    }
    akt = 0;
    wynik = poteguj2(akt, doc, wynik);
    return minus * wynik;
}

/* Funkcja: nieujemne
 * ------------------
 * Sprawdza czy liczba w tablicy "a" o dlugosci "an" jest wieksza
 * badz rowna 0.
 *
 * a: tablica typu int
 * an: zmienna typu int wieksza badz rowna 0
 *
 * zwraca: "true" jesli liczba jest nieujemna
 *         "false" w przecwinym przypadku
*/
bool nieujemne(int *a, int an) {
    if (an == 0) return true;
    else if (a[an - 1] >= 0)
        return true;
    else
        return false;
}

/* Funkcja: co_dodac
 * -----------------
 * Zwraca liczbe ktora nalezy wstawic do tablicy dodawnej do reszty
 * podczas dzielenia.
 *
 * x: zmienna typu int
 *
 * zwraca: 0 jesli x wieksza badz rowne 0
 *        -1 jesli x mniejsze od 0
*/
int co_dodac(int x) {
    if (x >= 0) return 0;
    else
        return -1;
}

/* Funkcja: popraw_reszte
 * ----------------------
 * Poprawia reszta odejmujac liczbe BBR-NAF w tablicy "r" o dlugosci "rn"
 * od tablicy "b" o dlugosci "bn" do tablicy "*c" o dlugosci "*cn".
 *
 * r, b: tablica typu int
 * rn, bn: zmienna typu int wieksza badz rowna 0
 * c: adres do tablicy typu int
 * cn: adres do zmiennej typu int
*/
void popraw_reszte(int *r, int rn, int *b, int bn, int **c, int *cn) {
    int *wynik = NULL;
    int dl = 0;
    nsub(b, bn, r, rn, &wynik, &dl);
    *c = wynik;
    *cn = dl;
}

/* Funkcja: dzielenie
 * ------------------
 * Wykonuje dzialanie liczb BBR-NAF zapisanych w tablicach. Dzieli
 * tablice "a" o dlugosci "an" przez tablice "b" o dlugosci "bn"
 * wynik zapisuje do tablicy "*c" o dlugosci "*cn" a reszte do tablicy
 * "*q"o dlugosci "*qn". Tablice "a" i "b" musza reprezentowac liczby nieujemne.
 *
 * a, b: tablica typu int
 * an: zmienna typu int wieksza badz rowna 0
 * bn: zmienna typu int wieksza od 0
 * c, q: adres do tablicy typu int
 * cn, qn: adres do zmiennej typu int
*/
void dzielenie(int *a, int an, int *b, int bn, int **c, int *cn, int **q, int *qn) {
    int *wynik = NULL;
    int dl = 0;
    int jeden = 0;
    int *tym = NULL;
    int tymdl = 0;
    int *reszta = NULL;
    int rdl = 0;
    int i = a[an - 1];
    int j = an - 1;
    while (i >= 0) {
        przesun(wynik, dl, 1);
        przesun(reszta, rdl, 1);
        if (j >= 0 && indeks(a[j]) == i) {
            int x = co_dodac(a[j]);
            nadd(&x, 1, reszta, rdl, &tym, &tymdl);
            free(reszta);
            reszta = tym;
            rdl = tymdl;
            j--;
        }
        nsub(reszta, rdl, b, bn, &tym, &tymdl);
        if (nieujemne(tym, tymdl)) {
            free(reszta);
            reszta = tym;
            rdl = tymdl;
            nadd(&jeden, 1, wynik, dl, &tym, &tymdl);
            free(wynik);
            dl = tymdl;
            wynik = tym;
        } else
            free(tym);
        if (rdl > 0 && reszta[rdl - 1] < 0) {
            nsub(wynik, dl, &jeden, 1, &tym, &tymdl);
            free(wynik);
            dl = tymdl;
            wynik = tym;
            zamien(reszta, rdl);
            popraw_reszte(reszta, rdl, b, bn, &tym, &tymdl);
            free(reszta);
            reszta = tym;
            rdl = tymdl;
        }
        i--;
    }
    *c = wynik;
    *cn = dl;
    *q = reszta;
    *qn = rdl;
}

/* Funkcja: ndivmod
 * ------------------
 * Wykonuje dzialanie liczb BBR-NAF zapisanych w tablicach. Dzieli
 * tablice "a" o dlugosci "an" przez tablice "b" o dlugosci "bn"
 * wynik zapisuje do tablicy "*c" o dlugosci "*cn" a reszte do tablicy
 * "*q"o dlugosci "*qn".
 *
 * a, b: tablica typu int
 * an: zmienna typu int wieksza badz rowna 0
 * bn: zmienna typu int wieksza od 0
 * c, q: adres do tablicy typu int
 * cn, qn: adres do zmiennej typu int
*/
void ndivmod(int *a, int an, int *b, int bn, int **c, int *cn, int **q, int *qn) {
    int *wynik = NULL;
    int dl = 0;
    int *reszta = NULL;
    int rdl = 0;
    int *tym = NULL;
    int tymdl = 0;
    int x = 0;
    if (an != 0) {
        if (a[an - 1] >= 0) {
            if (b[bn - 1] >= 0) {
                dzielenie(a, an, b, bn, &wynik, &dl, &reszta, &rdl);
            } else {
                zamien(b, bn);
                dzielenie(a, an, b, bn, &wynik, &dl, &reszta, &rdl);
                zamien(wynik, dl);
                zamien(b, bn);
            }
        } else {
            zamien(a, an);
            if (b[bn - 1] >= 0) {
                dzielenie(a, an, b, bn, &wynik, &dl, &reszta, &rdl);
                popraw_reszte(reszta, rdl, b, bn, &tym, &tymdl);
                free(reszta);
                reszta = tym;
                rdl = tymdl;
                nadd(&x, 1, wynik, dl, &tym, &tymdl);
                zamien(tym, tymdl);
            } else {
                zamien(b, bn);
                dzielenie(a, an, b, bn, &wynik, &dl, &reszta, &rdl);
                popraw_reszte(reszta, rdl, b, bn, &tym, &tymdl);
                free(reszta);
                reszta = tym;
                rdl = tymdl;
                nadd(&x, 1, wynik, dl, &tym, &tymdl);
                zamien(b, bn);
            }
            free(wynik);
            wynik = tym;
            dl = tymdl;
            zamien(a, an);
        }
    }
    *c = wynik;
    *cn = dl;
    *q = reszta;
    *qn = rdl;
}
