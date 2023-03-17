#include <iostream>
using namespace std;
int n;
int k;
int tree[65536];
int kon;
int pocz;
int tab[20000];
int tab2[20000];
int DZIEL = 1000000000;

void fill(int t[], int dl, int d) {
	for (int i = 0; i < dl; i++) {
		t[i] = d;
	}
}

int findPower2(int tmp) {
	int r = 1;
	while (r < tmp) {
		r *= 2;
	}
	return r;
}

int larger(int t[], int d, int p) {
	int akt = pocz + d - 1;
	int res = 0;
	while(akt != 0) {
		t[akt] += p;
		t[akt] %= DZIEL;
		if (akt % 2 == 0) {
			res += t[akt+1];
            res %= DZIEL;
		}
		akt /= 2;
	}
	return res;
}

int sum(int t[], int dl) {
	int res = 0;
	for (int i = 0; i < dl; i++) {
		res += t[i];
		res %= DZIEL;
	}
	return res;
}

int main() {
	cin >> n;
	cin >> k;
	fill(tab2, n, 1);
	pocz = findPower2(n);
	kon = 2*pocz;
	for (int i = 0; i < n; i++) {
		cin >> tab[i];
	}

	for (int i = 0; i < k-1; i++) {
		for (int j = 0; j < n; j++) {
			int tmp = tab[j];
			tab2[tmp-1] = larger(tree, tmp, tab2[tmp-1]);
		}
		fill(tree, kon, 0);
	}

	cout << sum(tab2, n) << "\n";
	return 0;
}
