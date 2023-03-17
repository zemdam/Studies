#include <iostream>
#include <algorithm>

using namespace std;

static const int MAX = 500000;
static const int MOD = 1000000000 + 7;

int n, k, l;
int a[MAX];
int indexy[MAX + 1];
int ostatniZ[MAX*2 + 1];
int lew[MAX + 1];
int praw[MAX + 1];
pair<int, int> sumB[MAX];
pair<int, int> b[MAX+1];
pair<int, int> cale;

void wczytaj() {
    cin >> n;
    cin >> k;
    cin >> l;
    for (int i = 0; i < n; i++) {
        cin >> a[i];
        indexy[i] = i;
    }
    cale = make_pair(n+1,0);
    indexy[n] = n;
    indexy[n+1] = n+1;
}

void dodajDoCale(pair<int, int> para) {
    if (para.first < cale.first && para.second > 0) {
        cale = para;
        cale.second %= MOD;
        return;
    }
    if (para.first == cale.first) {
        cale.second += para.second;
        cale.second %= MOD;
    }
}

pair<int, int> wyznaczPrzedzial(int pocz, int kon) {
    if (kon == n+1) {
        return make_pair(1,1);
    }

    if (pocz >= kon) {
        return make_pair(sumB[pocz-1].first + 1, 0);
    }

    int index = max(pocz, ostatniZ[b[kon - 1].first]);

    pair<int, int> wynik = sumB[index];
    if (wynik.first == sumB[kon].first) {
        sumB[kon].second %= MOD;
        wynik.second += MOD;
        wynik.second -= sumB[kon].second;
        wynik.second %= MOD;
    }
    wynik.first++;
    return wynik;
}

bool porownajUpper(const int &t1, const int &t2) {
    if (praw[t1] + 1 < lew[t2]) {
        return true;
    }

    return false;
}

void wyznaczZasieg() {
    for (int i = 0; i < n; i++) {
        lew[i] = (int)(upper_bound(a, a + i, a[i] - k - 1) - a);
        praw[i] = (int)(upper_bound(a + i, a + n, a[i] + k) - a) - 1;
    }
    praw[n] = praw[n-1];
    lew[n] = n;
    b[n] = make_pair(1,1);
}

int wyznaczKoniec(int index) {
    return *upper_bound(indexy + index, indexy + n + 1, index, porownajUpper);
}

bool porownajLower(const int &t1, const int &t2) {
    if (a[t1] - a[t2] < l) {
        return true;
    }

    return false;
}

int wyznaczPoczatek(int index) {
    return *lower_bound(indexy + index, indexy + n, index, porownajLower);
}

void dodajDoTab(int index, pair<int, int> ile) {
    if (lew[index] == 0) {
        dodajDoCale(ile);
    }
    b[index] = ile;
    ostatniZ[ile.first] = index;
    if (index == n-1) {
        sumB[index] = ile;
        return;
    }

    if (ile.first == sumB[index + 1].first) {
        sumB[index] = sumB[index + 1];
        ile.second %= MOD;
        sumB[index].second += ile.second;
        sumB[index].second %= MOD;
        return;
    }

    sumB[index] = ile;
}

void wyznacz() {
    int pocz, kon;
    for (int i = n - 1; i >= 0; i--) {
        pocz = wyznaczPoczatek(i);
        kon = wyznaczKoniec(i);

        dodajDoTab(i, wyznaczPrzedzial(pocz, kon));

    }
}

int main() {
    wczytaj();
    sort(a, a + n);
    wyznaczZasieg();
    wyznacz();

    for (int i = 0; i < 1; i++) {
        cout << cale.first << " " << cale.second << endl;
    }



    return 0;
}
