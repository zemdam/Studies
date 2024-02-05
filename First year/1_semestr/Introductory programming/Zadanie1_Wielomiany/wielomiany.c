#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Funkcja: absu
 * ------------------
 * Zwraca wartosc bezwgledna z "x". Rowniez z INT_MIN.
 *
 * x: zmienna typu int
 * returns: zmienna typu unsigned int
*/
unsigned int absu(int x) {
    if (x == INT_MIN)
        return (unsigned int) INT_MAX + 1;
    else
        return (unsigned int) abs(x);
}

/* Funkcja: zerowanie
 * ------------------
 * Wstawia zera w pierwsze "n-1" elementow tablicy.
 *
 * akumulator: tablica typu int o 11 elementach
 * n: zmienna typu int wieksza od 0 i mniejsza badz rowna 11
*/
void zerowanie(int akumulator[], int n) {
    for (int i = 0; i < n; i++)
        akumulator[i] = 0;
}

/* Funkcja: dodawana_liczba
 * ------------------------
 * Zwraca wspolczynnik w zaleznosci o "znaku" i "tmp".
 *
 * znak: zmienna typu int równa 1 lub -1
 * tmp: zmienna typu unsigned int mniejsza badz rowna INT_MAX + 1
 * returns: zmienna typu int
*/
int dodawana_liczba(int znak, unsigned int tmp) {
    if (tmp == 0)
        return znak * 1;
    else if (tmp == (unsigned int) INT_MAX + 1)
        return INT_MIN;
    else
        return znak * (int) tmp;
}

/* Funkcja: czy_liczba
 * -------------------
 * Zwraca true or false w zaleznosci od tego czy "aktualna"
 * wyraza liczbe w ASCII.
 *
 * aktualna: zmienna typu int przyjmujaca wartosci od 0 do 127
 * returns: true lub false
*/
bool czy_liczba(int aktualna) {
    if (aktualna >= '0' && aktualna <= '9')
        return true;
    else
        return false;
}

/* Funkcja: nastepny_bez_spacji
 * ----------------------------
 * Zwraca kod ASCII znaku roznego od spacji.
 *
 * returns: zmienna typu int z zakresu ACII bez 32
*/
int nastepny_bez_spacji() {
    int aktualna;
    do {
        aktualna = getchar();
    } while (aktualna == ' ');
    return aktualna;
}

/* Funkcja: dodawanie
 * ------------------
 * Dodaje do tablicy "akumulator" wprowadzony na wejscie wielomian.
 *
 * akumulator: tablica typu int o 11 miejscach
*/
void dodawanie(int akumulator[]) {
    int aktualna = getchar();
    int znak = 1;
    int znak_x = 1;
    bool czy_byl_x = false;
    bool czy_byl_daszek = false;
    unsigned int tmp = 0;

    while (aktualna != '\n') {
        if (czy_liczba(aktualna)) {
            if (czy_byl_daszek) {
                switch (aktualna) {
                    case '1'://jesli napotkam 1 po ^ to napewno bedzie x^10
                        akumulator[10] += dodawana_liczba(znak, tmp);
                        aktualna = getchar();//ide na nastepny element aby ominac 0
                        break;

                    default:
                        akumulator[aktualna - '0'] += dodawana_liczba(znak, tmp);
                        break;
                }
                aktualna = nastepny_bez_spacji();
                tmp = 0;
                czy_byl_x = false;
                czy_byl_daszek = false;
            } else if (czy_byl_x) {
                akumulator[1] += dodawana_liczba(znak_x, tmp);
                tmp = 0;
                czy_byl_x = false;
            } else {
                while (czy_liczba(aktualna)) {
                    tmp = 10 * tmp;
                    tmp += (unsigned int) (aktualna - '0');
                    aktualna = getchar();
                }
                if (aktualna == ' ')
                    aktualna = nastepny_bez_spacji();
            }
        } else {
            switch (aktualna) {
                case '+':
                    znak = 1;
                    break;

                case '-':
                    znak = -1;
                    break;

                case 'x':
                    czy_byl_x = true;
                    znak_x = znak;
                    break;

                case '^':
                    czy_byl_daszek = true;
                    break;
            }
            aktualna = nastepny_bez_spacji();
        }
    }
    if (czy_byl_x)
        akumulator[1] += dodawana_liczba(znak, tmp);
    else if (tmp >= 1)
        akumulator[0] += znak * (int) (tmp - 1) + znak;
}

/* Funkcja: mnozenie
 * -----------------
 * Do tabicy "akumulator" zapisuje wynik mnozenia tablic "akumulator"
 * i "skladnik".
 *
 * akumulator: tablica typu int o 11 elementach
 * skladnik: tablica typu int o 11 elementach
*/
void mnozenie(int akumulator[], int skladnik[]) {
    int pomocnicza[11];
    zerowanie(pomocnicza, 11);

    for (int i = 0; i < 11; i++) {
        for (int j = 0; j + i < 11; j++) {
            pomocnicza[i + j] += skladnik[i] * akumulator[j];
        }
    }
    for (int i = 0; i < 11; i++)
        akumulator[i] = pomocnicza[i];
}

/* Funkcja: wyswietl_wspolczynik
 * -----------------------------
 * Wyswietla na wyjsciu "wspolczynnik"
 *
 * wspolczynnik: zmienna typu unsigned int mniejsza badz rowna INT_MAX + 1
*/
void wyswietl_wspolczynik(unsigned int wspolczynnik) {
    if (wspolczynnik > 1)
        printf("%u", wspolczynnik);
}

/* Funkcja: wyswietl_element
 * -------------------------
 * Wyswietla na wyjsciu element wielomianu o stopniu == "i" oraz
 * wspolczynniku == "wspolczynnik".
 *
 * i: zmienna typu int od 0 do 10
 * wspolczynnik: zmienna typu int
*/
void wyswietl_element(int i, int wspolczynnik) {
    unsigned int abs_wspolczynnik = absu(wspolczynnik);
    switch (i) {
        case 1:
            wyswietl_wspolczynik(abs_wspolczynnik);
            printf("x");
            break;

        case 0:
            printf("%u", abs_wspolczynnik);
            break;

        default:
            wyswietl_wspolczynik(abs_wspolczynnik);
            printf("x^%d", i);
            break;
    }
}

/* Funkcja: przerwa
 * ----------------
 * W zaleznosci od znaku "wspolczynnik" wyswietla " - " lub " + ".
 *
 * wspolczynnik: zmienna typu int
*/
void przerwa(int wspolczynnik) {
    if (wspolczynnik < 0)
        printf(" - ");
    else
        printf(" + ");
}

/* Funkcja: wyswietl_wielomian
 * ---------------------------
 * Wyswietla na wyjsciu wielomian zapisany w "akumulator".
 *
 * akumulator: tablica typu int o 11 miejscach
*/
void wyswietl_wielomian(int akumulator[]) {
    int i = 10;

    while (i > 0 && akumulator[i] == 0)
        i--;
    if (akumulator[i] == 0) {
        printf("0");
    } else if (akumulator[i] < 0) {
        printf("-");
        wyswietl_element(i, akumulator[i]);
    } else
        wyswietl_element(i, akumulator[i]);
    i--;

    for (; i >= 0; i--) {
        if (akumulator[i] != 0) {
            przerwa(akumulator[i]);
            wyswietl_element(i, akumulator[i]);
        }
    }

    printf("\n");
}

/* Funkcja: decyzja
 * ----------------
 * Jesli aktualna == "+" lub "-" wykonuje dodawanie lub mnozenie wielomianu z
 * wejscia i "akumulator". A wynik zapisuje do "akumulator" i zwraca true.
 * Dla pozostalych "aktualna" w tym "." nic nie robi i zwraca false.
 *
 * aktualna: zmienna typu int z zakresu ASCII
 * akumulator: tablica typu int o 11 elementach
*/
bool decyzja(int aktualna, int akumulator[]) {
    if (aktualna == '+') {
        dodawanie(akumulator);
        wyswietl_wielomian(akumulator);
        return true;
    } else if (aktualna == '*') {
        int skladnik[11];
        zerowanie(skladnik, 11);
        dodawanie(skladnik);
        mnozenie(akumulator, skladnik);
        wyswietl_wielomian(akumulator);
        return true;
    } else if (aktualna == '.') {
        return false;
    }
    return false;
}

int main() {
    int akumulator[11];       //deklaruje akumulator
    zerowanie(akumulator, 11);//zeruje akumulator
    while (decyzja(getchar(), akumulator))
        ;//wykonuje decyzja az pojawi sie "."
    return 0;
}
