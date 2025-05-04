#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <climits>
#include <fstream>
#include <random>
#include <algorithm>

using namespace std;

class SkipListNode {
public:
    int value;
    vector<SkipListNode*> forward;

    SkipListNode(int val, int level) : value(val), forward(level + 1, nullptr) {}
};

class SkipList {
    int maxLevel;
    float probability;
    int currentLevel;
    SkipListNode* head;

    int randomLevel() {
        int level = 0;
        while ((float)rand() / RAND_MAX < probability && level < maxLevel)
            level++;
        return level;
    }

public:
    SkipList(int maxLevel, float probability)
        : maxLevel(maxLevel), probability(probability), currentLevel(0) {
        head = new SkipListNode(INT_MIN, maxLevel);
        srand(time(nullptr));
    }

    void insert(int value) {
        vector<SkipListNode*> update(maxLevel + 1);
        SkipListNode* current = head;

        for (int i = currentLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->value < value)
                current = current->forward[i];
            update[i] = current;
        }

        current = current->forward[0];

        if (!current || current->value != value) {
            int rlevel = randomLevel();

            if (rlevel > currentLevel) {
                for (int i = currentLevel + 1; i <= rlevel; i++)
                    update[i] = head;
                currentLevel = rlevel;
            }

            SkipListNode* newNode = new SkipListNode(value, rlevel);
            for (int i = 0; i <= rlevel; i++) {
                newNode->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = newNode;
            }
        }
    }

    bool search(int value, int& nodesVisited) {
        SkipListNode* current = head;
        //nodesVisited = 0;

        for (int i = currentLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
                nodesVisited++;
            }
            nodesVisited++;
        }

        current = current->forward[0];
        if (current && current->value == value) {
            nodesVisited++;
            return true;
        }
        return false;
    }

    ~SkipList() {
        SkipListNode* current = head;
        while (current) {
            SkipListNode* next = current->forward[0];
            delete current;
            current = next;
        }
    }
};



int main() {
    srand(time(nullptr));
    vector<int> sizes = {5000000, 10000000, 20000000, 50000000};
    ofstream out("skip_list_results2.csv");
    out << "n,exist,missing\n";

    random_device rd;
    mt19937 gen(rd());

    for (int n : sizes) {
        vector<int> data(n);
        for (int i = 0; i < n; i++) data[i] = i;
        shuffle(data.begin(), data.end(), gen);

        SkipList sl(25, 0.25);
        for (int i = 0; i < n; i++) sl.insert(data[i]);

        uniform_int_distribution<> dist_exist(0, n - 1);
        uniform_int_distribution<> dist_missing(n, 2 * n);

        int total_skip_exist = 0, total_skip_missing = 0;

        for (int i = 0; i < 500000; i++) {
            int key_exist = data[dist_exist(gen)];
            int key_missing = dist_missing(gen);

            sl.search(key_exist, total_skip_exist);
            sl.search(key_missing, total_skip_missing);
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
