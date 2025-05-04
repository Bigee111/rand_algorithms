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
    vector<Node*> forward;
    Node(int key, int level);
};

class SkipList {
public:
    SkipList(int maxLevel);
    void insert(int key);
    int searchCount(int key);
private:
    int maxLevel;
    Node* head;
    int randomLevel();
};

Node::Node(int k, int level) : key(k), forward(level + 1, nullptr) {}

SkipList::SkipList(int maxLevel) : maxLevel(maxLevel) {
    head = new Node(-1, maxLevel);
}

int SkipList::randomLevel() {
    int lvl = 0;
    while ((rand() % 2) && lvl < maxLevel) ++lvl;
    return lvl;
}

void SkipList::insert(int key) {
    vector<Node*> update(maxLevel + 1);
    Node* current = head;
    for (int i = maxLevel; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < key)
            current = current->forward[i];
        update[i] = current;
    }

    int lvl = randomLevel();
    Node* newNode = new Node(key, lvl);
    for (int i = 0; i <= lvl; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
}

int SkipList::searchCount(int key) {
    Node* current = head;
    int count = 0;
    for (int i = maxLevel; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
            count++;
        }
        //count++;
    }
    return count;
}


int main() {
    srand(time(nullptr));
    vector<int> sizes = {5000000, 10000000, 20000000, 50000000};
    ofstream out("skip_list_results.csv");
    out << "n,exist,missing\n";

    random_device rd;
    mt19937 gen(rd());

    for (int n : sizes) {
        vector<int> data(n);
        for (int i = 0; i < n; i++) data[i] = i;
        shuffle(data.begin(), data.end(), gen);

        SkipList sl(25);
        for (int i = 0; i < n; i++) sl.insert(data[i]);

        uniform_int_distribution<> dist_exist(0, n - 1);
        uniform_int_distribution<> dist_missing(n, 2 * n);

        int total_skip_exist = 0, total_skip_missing = 0;

        for (int i = 0; i < 500000; i++) {
            int key_exist = data[dist_exist(gen)];
            int key_missing = dist_missing(gen);

            total_skip_exist += sl.searchCount(key_exist);
            total_skip_missing += sl.searchCount(key_missing);
        }

        double avg_skip_exist = total_skip_exist / 500000.0;
        double avg_skip_missing = total_skip_missing / 500000.0;

        cout << "n=" << n << " exist=" << avg_skip_exist << ", missing=" << avg_skip_missing
                  << endl;

        out << n << ","
            << avg_skip_exist << "," << avg_skip_missing << "\n";
    }
    out.close();

    return 0;
}
