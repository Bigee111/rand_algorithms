#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>

using namespace std;

struct Node {
    int val;
    vector<Node*> next;

    Node(int v, int lvl) : val(v), next(lvl + 1, nullptr) {}
};

class SkipList {
public:
    SkipList(int maxLvl, float p);
    ~SkipList();

    void insert(int val);
    bool search(int val, int& steps);

private:
    int maxLvl;
    float p;
    int currLvl;
    Node* head;

    int randLevel();
};

SkipList::SkipList(int maxLvl, float p)
    : maxLvl(maxLvl), p(p), currLvl(0) {
    head = new Node(INT_MIN, maxLvl);
    srand(time(0));
}

int SkipList::randLevel() {
    int lvl = 0;
    while ((float)rand() / RAND_MAX < p && lvl < maxLvl)
        ++lvl;
    return lvl;
}

void SkipList::insert(int val) {
    vector<Node*> prev(maxLvl + 1);
    Node* x = head;

    for (int i = currLvl; i >= 0; --i) {
        while (x->next[i] && x->next[i]->val < val)
            x = x->next[i];
        prev[i] = x;
    }

    x = x->next[0];

    if (!x || x->val != val) {
        int lvl = randLevel();

        if (lvl > currLvl) {
            for (int i = currLvl + 1; i <= lvl; ++i)
                prev[i] = head;
            currLvl = lvl;
        }

        Node* newNode = new Node(val, lvl);
        for (int i = 0; i <= lvl; ++i) {
            newNode->next[i] = prev[i]->next[i];
            prev[i]->next[i] = newNode;
        }
    }
}

bool SkipList::search(int val, int& steps) {
    Node* x = head;

    for (int i = currLvl; i >= 0; --i) {
        while (x->next[i] && x->next[i]->val < val) {
            x = x->next[i];
            ++steps;
        }
        ++steps;
    }

    x = x->next[0];
    if (x && x->val == val) {
        ++steps;
        return true;
    }
    return false;
}

SkipList::~SkipList() {
    Node* curr = head;
    while (curr) {
        Node* nxt = curr->next[0];
        delete curr;
        curr = nxt;
    }
}

int main() {
    srand(time(0));
    vector<int> sizes = {5'000'000, 10'000'000, 20'000'000, 50'000'000};

    ofstream out("skiplist_bench.csv");
    if (!out) {
        cerr << "File err.\n";
        return 1;
    }

    out << "n,avg_found,avg_not_found\n";

    random_device rd;
    mt19937 rng(rd());

    for (int n : sizes) {
        vector<int> data(n);
        for (int i = 0; i < n; ++i)
            data[i] = i;

        shuffle(data.begin(), data.end(), rng);

        SkipList sl(50, 0.25);
        for (int x : data)
            sl.insert(x);

        uniform_int_distribution<> pick_in(0, n - 1);
        uniform_int_distribution<> pick_out(n, 2 * n);

        int steps_in = 0, steps_out = 0;

        for (int i = 0; i < 500000; ++i) {
            int k_in = data[pick_in(rng)];
            int k_out = pick_out(rng);

            sl.search(k_in, steps_in);
            sl.search(k_out, steps_out);
        }

        double avg_in = steps_in / 500000.0;
        double avg_out = steps_out / 500000.0;

        cout << "n=" << n << " avg_in=" << avg_in << " avg_out=" << avg_out << "\n";
        out << n << "," << avg_in << "," << avg_out << "\n";
    }

    out.close();
    return 0;
}
