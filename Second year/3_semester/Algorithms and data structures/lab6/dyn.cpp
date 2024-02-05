#include<iostream>
#include<cstdio>
#include<sstream>
#include<algorithm>

int m;
int j, x, k;

using namespace std;
struct avl {
    int x;
    int k;
    int ls;
    int rs;
    int height;
    struct avl *l;
    struct avl *r;
}*r;
class avl_tree {
public:
    int height(avl *);
    int difference(avl *);
    avl *rr_rotat(avl *);
    avl *ll_rotat(avl *);
    avl *lr_rotat(avl*);
    avl *rl_rotat(avl *);
    avl * balance(avl *);
    avl * insert(avl*, int, int, int);
    void show(avl*, int);
    void inorder(avl *);
    void preorder(avl *);
    void postorder(avl*);
    int get(avl*, int);
    avl_tree() {
        r = NULL;
    }
};
int avl_tree::height(avl *t) {
    int h = 0;
    if (t != NULL) {
        h = t->height;
    }
    return h;
}
int avl_tree::difference(avl *t) {
    int l_height = height(t->l);
    int r_height = height(t->r);
    int b_factor = l_height - r_height;
    return b_factor;
}
avl *avl_tree::rr_rotat(avl *parent) {
    avl *t;
    t = parent->r;
    parent->r = t->l;
    parent->rs = t->ls;
    t->l = parent;
    t->ls = parent->ls + parent->rs + parent->k;
    t->height = max(height(t->l), height(t->r)) + 1;
    parent->height = max(height(parent->l), height(parent->r)) + 1;
    //cout<<"Right-Right Rotation" << endl;
    return t;
}
avl *avl_tree::ll_rotat(avl *parent) {
    avl *t;
    t = parent->l;
    parent->l = t->r;
    parent->ls = t->rs;
    t->r = parent;
    t->rs = parent->ls + parent->rs + parent->k;
    t->height = max(height(t->l), height(t->r)) + 1;
    parent->height = max(height(parent->l), height(parent->r)) + 1;
    //cout<<"Left-Left Rotation" << endl;
    return t;
}
avl *avl_tree::lr_rotat(avl *parent) {
    avl *t;
    t = parent->l;
    parent->l = rr_rotat(t);
    //cout<<"Left-Right Rotation" << endl;
    return ll_rotat(parent);
}
avl *avl_tree::rl_rotat(avl *parent) {
    avl *t;
    t = parent->r;
    parent->r = ll_rotat(t);
    //cout<<"Right-Left Rotation" << endl;
    return rr_rotat(parent);
}
avl *avl_tree::balance(avl *t) {
    int bal_factor = difference(t);
    if (bal_factor > 1) {
        if (difference(t->l) > 0)
            t = ll_rotat(t);
        else
            t = lr_rotat(t);
    } else if (bal_factor < -1) {
        if (difference(t->r) > 0)
            t = rl_rotat(t);
        else
            t = rr_rotat(t);
    }
    return t;
}
avl *avl_tree::insert(avl *r, int j, int x, int k) {
    if (r == NULL) {
        r = new avl;
        r->x = x;
        r->k = k;
        r->ls = 0;
        r->rs = 0;
        r->l = NULL;
        r->r = NULL;
        r->height = 0;
        return r;
    } else if (j <= r->ls) {
        r->ls += k;
        r->l = insert(r->l, j, x, k);
        r->height = max(height(r->l), height(r->r)) + 1;
        r = balance(r);
    } else if (j >= r->ls + r->k) {
        r->rs += k;
        r->r = insert(r->r, j - r->ls - r->k, x, k);
        r->height = max(height(r->l), height(r->r)) + 1;
        r = balance(r);
    } else if (r->x == x) {
        r->k += k;
    } else {
        int toAdd = r->k - j + r->ls;
        r->rs += k + toAdd;
        r->k = j - r->ls;
        r->r = r->r = insert(r->r, 0, r->x, toAdd);
        r->height = max(height(r->l), height(r->r)) + 1;
        r->r = r->r = insert(r->r, j - r->ls - r->k, x, k);
        r->height = max(height(r->l), height(r->r)) + 1;
        r = balance(r);
    }
    return r;
}
void avl_tree::show(avl *p, int l) {
    int i;
    if (p != NULL) {
        show(p->r, l+ 1);
        cout<<" ";
        if (p == r)
            cout << "Root -> ";
        for (i = 0; i < l&& p != r; i++)
            cout << " ";
        cout << p->x;
        show(p->l, l + 1);
    }
}
void avl_tree::inorder(avl *t) {
    if (t == NULL)
        return;
    inorder(t->l);
    for (int i = 0; i < t->k; i++) {
        cout << t->x << " ";
    }
    inorder(t->r);
}
void avl_tree::preorder(avl *t) {
    if (t == NULL)
        return;
    cout << t->x << " ";
    preorder(t->l);
    preorder(t->r);
}
void avl_tree::postorder(avl *t) {
    if (t == NULL)
        return;
    postorder(t ->l);
    postorder(t ->r);
    cout << t->x << " ";
}

int avl_tree::get(avl *r, int i) {
    if (i >= r->ls && i < r->ls + r->k) {
        return r->x;
    }

    if (i < r->ls) {
        return get(r->l, i);
    }

    return get(r->r, i - r->ls - r->k);
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    avl_tree avl;
    int n = 0;
    int w = 0;
    char c;
    cin >> m;
    for (int i = 0; i < m; i++) {
        cin >> c;
        if (c == 'g') {
            cin >> j;
            if (true) {
                j = (j + w) % n;
                w = avl.get(r, j);
                cout << w << endl;
            }
        } else if (c == 'i') {
            cin >> j >> x >> k;
            if (true) {
                j = (j + w) % (n + 1);
                r = avl.insert(r, j, x, k);
                n += k;
            } else {
                cout << "|j:" << j << "x:" << x << "k:" << k << "|";
            }
        }
    }
    return 0;
}
