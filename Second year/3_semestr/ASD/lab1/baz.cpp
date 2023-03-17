#include <iostream>

int n;
int m;
int tab[1000000];
int nMIN[1000000];
int pMIN[1000000];
int nMAX[1000000];
int pMAX[1000000];
long long sum[1000000];
int tmp;
long long tmpSum;
long long tmpSum1;
long long tmpSum2;
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    std::cin >> n;
    for (int i = 0; i < n; i++) {
        std::cin >> tab[i];
    }
    std::cin >> m;

    if (tab[0] % 2 == 0) {
        pMAX[0] = tab[0];
        nMAX[0] = 0;
    } else {
        pMAX[0] = 0;
        nMAX[0] = tab[0];
    }

    if (tab[n-1] % 2 == 0) {
        pMIN[0] = tab[n-1];
        nMIN[0] = 0;
    } else {
        pMIN[0] = 0;
        nMIN[0] = tab[n-1];
    }

    sum[0] = tab[n-1];

    for (int i = 1; i < n; i++) {
        if (tab[i] % 2 == 0) {
            pMAX[i] = tab[i];
            nMAX[i] = nMAX[i-1];
        } else {
            nMAX[i] = tab[i];
            pMAX[i] = pMAX[i-1];
        }

        if (tab[n-i-1] % 2 == 0) {
            pMIN[i] = tab[n-i-1];
            nMIN[i] = nMIN[i-1];
        } else {
            nMIN[i] = tab[n-i-1];
            pMIN[i] = pMIN[i-1];
        }

        sum[i] += sum[i-1] + tab[n-i-1];
    }

    for (int i = 0; i < m; i++) {
        std::cin >> tmp;
        tmpSum = sum[tmp-1];
        if (tmpSum % 2 != 0) {
            std::cout << sum[tmp-1] << "\n";
        } else if (tmp == n) {
            std::cout << -1 << "\n";
        } else {
            tmpSum1 = 0;
            tmpSum2 = 0;
            if (pMAX[n - tmp - 1] != 0 && nMIN[tmp - 1] != 0) {
                tmpSum1 = tmpSum;
                tmpSum1 -= nMIN[tmp - 1];
                tmpSum1 += pMAX[n - tmp - 1];
            }
            if (nMAX[n - tmp - 1] != 0 && pMIN[tmp - 1] != 0) {
                tmpSum2 = tmpSum;
                tmpSum2 -= pMIN[tmp - 1];
                tmpSum2 += nMAX[n - tmp - 1];
            }
            tmpSum = std::max(tmpSum1, tmpSum2);
            if (tmpSum == 0) {
                std::cout << -1 << "\n";
            } else {
                std::cout << tmpSum << "\n";
            }
        }
    }
    return 0;
}
