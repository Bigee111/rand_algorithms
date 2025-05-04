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
    Node* left;
    Node* right;
    Node(int k);
};

class RBST {

public:
    RBST();
    void insert(int key);
    int searchCount(int key);
private:
    Node* root;
    Node* insert(Node* node, int key);
    Node* insertRoot(Node* node, int key);
    Node* rotateLeft(Node* node);
    Node* rotateRight(Node* node);

};

Node::Node(int k) : key(k), left(nullptr), right(nullptr) {}

RBST::RBST() : root(nullptr) {}

Node* RBST::rotateLeft(Node* p) {
    Node* q = p->right;
    p->right = q->left;
    q->left = p;
    return q;
}

Node* RBST::rotateRight(Node* p) {
    Node* q = p->left;
    p->left = q->right;
    q->right = p;
    return q;
}

Node* RBST::insertRoot(Node* node, int key) {
    if (!node) return new Node(key);
    if (key < node->key) {
        node->left = insertRoot(node->left, key);
        return rotateRight(node);
    } else {
        node->right = insertRoot(node->right, key);
        return rotateLeft(node);
    }
}

Node* RBST::insert(Node* node, int key) {
    if (!node) return new Node(key);
    if (rand() % 2) return insertRoot(node, key);
    if (key < node->key)
        node->left = insert(node->left, key);
    else
        node->right = insert(node->right, key);
    return node;
}

void RBST::insert(int key) {
    root = insert(root, key);
}

int RBST::searchCount(int key) {
    Node* current = root;
    int count = 0;
    while (current) {
        count++;
        if (current->key == key) break;
        current = (key < current->key) ? current->left : current->right;
    }
    return count;
}

int main() {
    srand(time(nullptr));
    vector<int> sizes = {5000000, 10000000, 20000000, 50000000};
    ofstream out("rbst_results.csv");
    out << "n,RBST_exist,RBST_missing\n";

    random_device rd;
    mt19937 gen(rd());

    for (int n : sizes) {
        vector<int> data(n);
        for (int i = 0; i < n; i++) data[i] = i;
        shuffle(data.begin(), data.end(), gen);

        RBST rbst;
        for (int i = 0; i < n; i++) rbst.insert(data[i]);

        uniform_int_distribution<> dist_exist(0, n - 1);
        uniform_int_distribution<> dist_missing(n, 2 * n);

        int total_rbst_exist = 0, total_rbst_missing = 0;

        for (int i = 0; i < 500000; i++) {
            int key_exist = data[dist_exist(gen)];
            int key_missing = dist_missing(gen);

            total_rbst_exist += rbst.searchCount(key_exist);
            total_rbst_missing += rbst.searchCount(key_missing);
        }

        double avg_rbst_exist = total_rbst_exist / 500000.0;
        double avg_rbst_missing = total_rbst_missing / 500000.0;

        cout << "n=" << n
                  << " RBST: exist=" << avg_rbst_exist << ", missing=" << avg_rbst_missing << endl;

        out << n << ","
            << avg_rbst_exist << "," << avg_rbst_missing << "\n";
    }

    out.close();
    return 0;
}

