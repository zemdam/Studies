#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>

#define MAX 1000000
//#define MAX 5

using namespace std;

int n;
int m;
int k;
int p[MAX];
vector<int> wym[MAX];
pair<int,int> ile[MAX];
pair<int, int> minK[MAX];
int index;
int toAdd;

int znajdzMax(int ind) {
    if (minK[ind].first != 0) {
        return minK[ind].first;
    }

    int res = p[ind];
    for (auto iter : wym[ind]) {
        if (znajdzMax(iter) > res) {
            res = minK[iter].first;
        }
    }

    minK[ind].first = res;
    return res;
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    
    cin >> n >> m >> k;

    for (int i = 0; i < n; i++) {
        cin >> p[i];
    }

    for (int i = 0; i < m; i++) {
        cin >> index;
        index--;
        cin >> toAdd;
        toAdd--;
        wym[index].push_back(toAdd);
    }

    for (int i = 0; i < n; i++) {
        ile[i].first = wym[i].size();
        ile[i].second = i;
    }

    sort(ile, ile + n);
    for (int i = 0; i < n; i++) {
        index = ile[i].second;
        minK[index].first = znajdzMax(index);
        minK[index].second = index;
    }

    sort(minK, minK + n);

    cout << minK[k-1].first << endl;

    return 0;
}
