#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>

int doPartition(std::vector<int>& data, int l, int r) {
    int pIdx = l + std::rand() % (r - l + 1);
    int pivot = data[pIdx];
    std::swap(data[pIdx], data[r]);

    int sIdx = l;
    for (int i = l; i < r; ++i) {
        if (data[i] < pivot) {
            std::swap(data[i], data[sIdx]);
            ++sIdx;
        }
    }
    std::swap(data[sIdx], data[r]);
    return sIdx;
}

int quickSelect(std::vector<int>& data, int k) {
    int lo = 0, hi = data.size() - 1;

    while (lo <= hi) {
        int idx = doPartition(data, lo, hi);
        if (idx == k) return data[idx];
        else if (idx < k) lo = idx + 1;
        else hi = idx - 1;
    }

    return -1;
}

void runBenchmark(int N, std::ofstream& fout) {
    std::cout << "Preparing " << N << " elements...\n";
    std::vector<int> values(N);

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, 1000000000); //1e9

    for (int i = 0; i < N; ++i)
        values[i] = dist(gen);

    std::uniform_int_distribution<int> kDist(1, N);
    long long total_ns = 0;

    std::cout << "Running quickselects...\n";
    for (int i = 1; i <= 1000; ++i) {
        int k = kDist(gen);

        std::vector<int> tmp(values);

        auto t0 = std::chrono::high_resolution_clock::now();
        quickSelect(tmp, k);
        auto t1 = std::chrono::high_resolution_clock::now();

        total_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

        if (i % 100 == 0)
            std::cout << "   [" << i << "/1000] done\n";
    }

    double avg_ms = total_ns / 1e6 / 1000.0;
    std::cout << "Avg time for " << N << ": " << avg_ms << " ms\n\n";

    fout << N << "," << avg_ms << "\n";
}

int main() {
    std::vector<int> testSizes = {10000000, 20000000, 40000000, 80000000, 160000000};

    std::ofstream fout("quickselect_results.csv");
    if (!fout.is_open()) {
        std::cerr << "Couldn't open output file\n";
        return 1;
    }

    fout << "Size,AvgTimeMs\n";

    for (int n : testSizes) {
        runBenchmark(n, fout);
    }

    fout.close();
    return 0;
}
