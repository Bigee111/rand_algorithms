#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>

using namespace std;

struct Node {
    int key;
    int priority;
    Node* left;
    Node* right;

    Node(int k, int p) {
        key = k;
        priority = p;
        left = right = nullptr;
    }
};

class RBST {
public:
    RBST();
    void insert(int key);
    int searchCount(int key);

private:
    Node* root;
    mt19937 rng;

    Node* insert(Node* n, int key, int priority);
    Node* rotateL(Node* n);
    Node* rotateR(Node* n);
};

RBST::RBST() : root(nullptr) {
    random_device rd;
    rng.seed(rd());
}

Node* RBST::rotateL(Node* p) {
    Node* q = p->right;
    if (!q) return p;
    p->right = q->left;
    q->left = p;
    return q;
}

Node* RBST::rotateR(Node* p) {
    Node* q = p->left;
    if (!q) return p;
    p->left = q->right;
    q->right = p;
    return q;
}

Node* RBST::insert(Node* n, int key, int priority) {
    if (!n) return new Node(key, priority);

    if (key < n->key) {
        n->left = insert(n->left, key, priority);
        if (n->left->priority > n->priority)
            n = rotateR(n);
    } else {
        n->right = insert(n->right, key, priority);
        if (n->right->priority > n->priority)
            n = rotateL(n);
    }
    return n;
}

void RBST::insert(int key) {
    int priority = uniform_int_distribution<>(1, INT32_MAX)(rng);
    root = insert(root, key, priority);
}

int RBST::searchCount(int key) {
    Node* curr = root;
    int steps = 0;
    while (curr) {
        steps++;
        if (curr->key == key) break;
        curr = (key < curr->key) ? curr->left : curr->right;
    }
    return steps;
}

int main() {
    vector<int> sizes = {5000000, 10000000, 20000000, 50000000};

    ofstream fout("treap_results.csv");
    if (!fout.is_open()) {
        cerr << "File err.\n";
        return 1;
    }

    fout << "n,existing,missing\n";

    random_device rd;
    mt19937 rng(rd());

    for (int n : sizes) {
        cout << "Working on size: " << n << endl;

        vector<int> values(n);
        for (int i = 0; i < n; ++i)
            values[i] = i;

        shuffle(values.begin(), values.end(), rng);

        RBST tree;
        for (int x : values)
            tree.insert(x);

        uniform_int_distribution<> pickExist(0, n - 1);
        uniform_int_distribution<> pickMissing(n, 2 * n);

        int steps_exist = 0, steps_miss = 0;

        for (int i = 0; i < 500000; ++i) {
            int foundKey = values[pickExist(rng)];
            int notFoundKey = pickMissing(rng);

            steps_exist += tree.searchCount(foundKey);
            steps_miss += tree.searchCount(notFoundKey);
        }

        double avg_exist = steps_exist / 500000.0;
        double avg_miss = steps_miss / 500000.0;

        cout << "n=" << n << " --> found=" << avg_exist
             << ", not found=" << avg_miss << endl;

        fout << n << "," << avg_exist << "," << avg_miss << "\n";
    }

    fout.close();
    return 0;
}
