#include <iostream>
#include <set>
#include <vector>

using namespace std;


int n;
int m;
set<pair<int,int>> intervals;
pair<int, int> tmp;
char c;
int sum;

void insertC() {
    if (intervals.empty()) {
        return;
    }

    auto itr = intervals.lower_bound(tmp);
    if (itr != intervals.begin()) {
        itr--;
    }
    pair<int,int> tmp2;

    for (;itr != intervals.end();) {
        if (itr->first < tmp.first && itr->second <= tmp.second && itr->second >= tmp.first) {
            tmp2.first = itr->first;
            tmp2.second = tmp.first - 1;
            sum -= itr->second - itr->first + 1;
            sum += tmp2.second - tmp2.first + 1;
            intervals.erase(itr++);
            intervals.insert(tmp2);
        } else if (itr->first >= tmp.first && itr->second > tmp.second && itr->first <= tmp.second) {
            tmp2.first = tmp.second + 1;
            tmp2.second = itr->second;
            sum -= itr->second - itr->first + 1;
            sum += tmp2.second - tmp2.first + 1;
            intervals.erase(itr++);
            intervals.insert(tmp2);
        } else if (itr->first >= tmp.first && itr->second <= tmp.second) {
            sum -= itr->second - itr->first + 1;
            intervals.erase(itr++);
        } else if (itr->first < tmp.first && itr->second > tmp.second) {
            sum -= itr->second - itr->first + 1;
            tmp2.first = itr->first;
            tmp2.second = tmp.first - 1;
            sum += tmp2.second - tmp2.first + 1;
            intervals.insert(tmp2);
            tmp2.first = tmp.second + 1;
            tmp2.second = itr->second;
            sum += tmp2.second - tmp2.first + 1;
            intervals.insert(tmp2);
            intervals.erase(itr++);
        } else if (tmp.second < itr->first) {
            break;
        } else {
            itr++;
        }
    }
}

void insertB() {
    if (intervals.empty()) {
        intervals.insert(tmp);
        sum += tmp.second - tmp.first + 1;
        return;
    }

    auto itr = intervals.lower_bound(tmp);
    if (itr != intervals.begin()) {
        itr--;
    }

    for (;itr != intervals.end();) {
        if (itr->first <= tmp.first && itr->second >= tmp.first - 1) {
            tmp.first = itr->first;
            tmp.second = max(tmp.second, itr->second);
            sum -= itr->second - itr->first + 1;
            intervals.erase(itr++);
        } else if (itr->first - 1 <= tmp.second && itr->second >= tmp.second) {
            tmp.first = min(itr->first, tmp.first);
            tmp.second = itr->second;
            sum -= itr->second - itr->first + 1;
            intervals.erase(itr++);
        } else if(itr->first >= tmp.first && itr->second <= tmp.second) {
            sum -= itr->second - itr->first + 1;
            intervals.erase(itr++);
        } else if (tmp.second < itr->first - 1) {
            break;
        } else {
            itr++;
        }
    }

    sum += tmp.second - tmp.first + 1;
    intervals.insert(tmp);
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    cin >> n >> m;

    for (int i = 0; i < m; i++) {
        cin >> tmp.first >> tmp.second >> c;

        if (c == 'B') {
            insertB();
        } else {
            insertC();
        }

//        for (auto itr : intervals) {
//            cout << "(" << itr.first << "," << itr.second << ") ";
//        }
//        cout << endl;
        cout << sum << endl;
    }
    return 0;
}
