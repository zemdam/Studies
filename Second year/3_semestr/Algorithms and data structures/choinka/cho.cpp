#include <iostream>
#include <vector>

#define MAXN 200000
#define MAXTREE 524288

using namespace std;

using christmasNode = struct node;
using treeNode = struct node2;

struct node2 {
    int first;
    int second;
    bool areAllSame;
    bool isCorrect;
};

struct node {
    int son;
    int brother;
    int depth;
    int num;
    int end;
};

christmasNode christmasTree[MAXN + 1];

treeNode tree[MAXTREE];
int n, q, v, x, base;
char c;

void initTree() {
    for (auto & i : tree) {
        i.first = -2;
    }
}

void findBase() {
    base = 1;
    while (base < n) {
        base *= 2;
    }
}

int preorderBfs(int start, int actNum, int actDepth) {
    christmasTree[start].num = actNum;
    christmasTree[start].depth = actDepth;

    for (int actSon = christmasTree[start].son; actSon != 0; actSon = christmasTree[actSon].brother) {
        actNum = preorderBfs(actSon, actNum + 1, actDepth + 1);
    }

    christmasTree[start].end = actNum;
    return actNum;
}

void readTree() {
    int tmp1;
    for (int i = 2; i <= n; i++) {
        cin >> tmp1;
        christmasTree[i].brother = christmasTree[tmp1].son;
        christmasTree[tmp1].son = i;
    }

    preorderBfs(1, 0, 0);
}

treeNode check(treeNode left, treeNode right) {
    treeNode toReturn;
    toReturn.first = 0;
    toReturn.second = 0;
    toReturn.areAllSame = false;
    toReturn.isCorrect = false;

    if (left.first == -2) {
        return right;
    }

    if (right.first == -2) {
        return left;
    }

    if (left.first == -1) {
        return right;
    }

    if (!left.isCorrect || !right.isCorrect || (!left.areAllSame && !right.areAllSame)) {
        return toReturn;
    }
    
    
        if (right.second == -1 && left.second == -1) {
            toReturn.isCorrect = true;
            toReturn.first = left.first;
            toReturn.second = right.first;
            toReturn.areAllSame = right.first == left.first;
            return toReturn;
        }

        if (left.first == right.first || left.first == right.second) {
            toReturn.isCorrect = true;
            toReturn.first = left.first;
            toReturn.second = toReturn.first;
            toReturn.areAllSame = left.areAllSame && right.areAllSame;
            return toReturn;
        }

        if (left.second == right.first || left.second == right.second) {
            toReturn.isCorrect = true;
            toReturn.first = left.second;
            toReturn.second = toReturn.first;
            toReturn.areAllSame = left.areAllSame && right.areAllSame;
            return toReturn;
        }

        if (left.areAllSame && right.second == -1) {
            left.areAllSame = false;
            return left;
        }

        if (right.areAllSame && left.second == -1) {
            right.areAllSame = false;
            return right;
        }

    return toReturn;
}

treeNode findInterval(int start, int stop) {
    treeNode toReturn;
    toReturn.first = -1;
    start = start - 1 + base;
    stop = stop + 1 + base;

    while (start/2 != stop/2) {
        if (start%2 == 0) toReturn = check(toReturn, tree[start+1]);
        if (stop%2 == 1) toReturn = check(toReturn, tree[stop-1]);
        start /= 2;
        stop /= 2;
    }

    return toReturn;
}

void changeColor(int start, int color) {
    start = christmasTree[start].num + base;
    tree[start].first = color;
    tree[start].second = -1;
    tree[start].areAllSame = true;
    tree[start].isCorrect = true;
    start /= 2;

    while (start > 0) {
        tree[start] = check(tree[2*start], tree[2*start+1]);
        start /= 2;
    }
}

void readColors() {
    int color;
    for (int i = 1; i <= n; i++) {
        cin >> color;
        changeColor(i, color);
    }
}

bool checkColor(int start) {
    treeNode toCheck = findInterval(christmasTree[start].num, christmasTree[start].end);

    return toCheck.isCorrect;
}

void readData() {
    cin >> n >> q;
    findBase();
    readTree();
    readColors();
}

int main() {
    initTree();
    readData();

    for (int i = 0; i < q; i++) {
        cin >> c;
        switch (c) {
            case 'z':
                cin >> v >> x;
                changeColor(v, x);
                break;
            case '?':
                cin >> v;
                if (checkColor(v)) {
                    cout << "TAK\n";
                } else {
                    cout << "NIE\n";
                }
                break;
        }
    }

    return 0;
}
