#include <iostream>

int a[1000];
int l[1000][1000];
int r[1000][1000];
int n;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
	std::cin >> n;

	for (int i = 0; i < n; i++) {
		std::cin >> a[i];
	}

	for (int  i = 0; i < n; i++) {
		l[0][i] = 1;
		r[0][i] = 0;
	}

	for (int i = 0; i < n-1; i++) {
		if (a[i] < a[i+1]) {
			l[1][i] = 1;
			r[1][i] = 1;
		}
	}

	for (int i = 2; i < n; i++) {
		for (int j = 0; j < n - i; j++) {
            l[i][j] = 0;
            r[i][j] = 0;
			if (a[j] < a[j+1]) {
				l[i][j] += l[i-1][j+1];
			}
            if (a[j] < a[j+i]) {
                l[i][j] += r[i-1][j+1];
                r[i][j] += l[i-1][j];
            }

			if (a[j+i-1] < a[j+i]) {
				r[i][j] += r[i-1][j];
			}
            l[i][j] %= 1000000000;
            r[i][j] %= 1000000000;
		}
	}

    int result = (l[n-1][0] + r[n-1][0]) % 1000000000;
	std::cout << result;
	return 0;
}
