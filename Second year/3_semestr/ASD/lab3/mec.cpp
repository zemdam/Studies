#include <iostream>
#include <unordered_set>

using namespace std;

int n, m, tmp, s;
int64_t key[40000];

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    cin >> n;
    cin >> m;
    s = n/2;
    unordered_set<int64_t> mySet;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            cin >> tmp;
            key[tmp - 1] += j < s;
            key[tmp - 1] *= 2;
        }
    }

    int old;
    for (int i = 0; i < n; i++) {
        old = mySet.size();
        mySet.insert(key[i]);
        if (old == mySet.size()) {
            cout << "NIE" << "\n";
            return 0;
        }
    }

    cout << "TAK" << "\n";
    return 0;
}
