#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

#define MAX 200000

using namespace std;

int n;
pair<int, int> x[MAX];
pair<int, int> y[MAX];
pair<int, int> loc[MAX];
int g[MAX];
int d[MAX];
int l[MAX];
int p[MAX];
int length[MAX];
int visited;
priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> q;

void initLength() {
    for (int i = 0; i < n; i++) {
        length[i] = -1;
    }
}

int findDistance(int a, int b) {
    return min(abs(loc[a].first - loc[b].first), abs(loc[a].second - loc[b].second));
}

void checkArray(int v, int *array) {
    if (array[v] != -1 && length[array[v]] == -1) {
        int newLength = length[v] + findDistance(v, array[v]);
        q.push({newLength, array[v]});
    }
}

void putOnQueue(int v) {
    checkArray(v, g);
    checkArray(v, d);
    checkArray(v, l);
    checkArray(v, p);
}

int popFromQueue() {
    pair<int,int> element;
    do {
        element = q.top();
        q.pop();
    } while(length[element.second] != -1 && !q.empty());

    if (length[element.second] == -1) {
        length[element.second] = element.first;
        visited++;
    }

    return element.second;
}

int main() {
    cin >> n;
    for (int i = 0; i < n; i++) {
        cin >> x[i].first;
        cin >> y[i].first;
        loc[i].first = x[i].first;
        loc[i].second = y[i].first;
        x[i].second = i;
        y[i].second = i;
    }

    sort(x, x + n);
    sort(y, y + n);

    p[x[0].second] = x[1].second;
    g[y[0].second] = y[1].second;
    l[x[0].second] = -1;
    d[y[0].second] = -1;

    for (int i = 1; i < n - 1; i++) {
        p[x[i].second] = x[i + 1].second;
        g[y[i].second] = y[i + 1].second;
        l[x[i].second] = x[i - 1].second;
        d[y[i].second] = y[i - 1].second;
    }

    p[x[n - 1].second] = -1;
    g[y[n - 1].second] = -1;
    l[x[n - 1].second] = x[n - 2].second;
    d[y[n - 1].second] = y[n - 2].second;

    initLength();

    length[0] = 0;
    visited++;
    
    int element = 0;

    while (visited < n) {
        putOnQueue(element);
        element = popFromQueue();
    }    

    cout << length[n-1] << endl;

    return 0;
}
