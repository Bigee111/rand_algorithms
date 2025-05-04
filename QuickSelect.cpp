#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>

int partition(std::vector<int>& arr, int left, int right) {
    int pivotIndex = left + rand() % (right - left + 1);
    int pivotValue = arr[pivotIndex];
    std::swap(arr[pivotIndex], arr[right]);
    int storeIndex = left;

    for (int i = left; i < right; ++i) {
        if (arr[i] < pivotValue) {
            std::swap(arr[i], arr[storeIndex]);
            ++storeIndex;
        }
    }
    std::swap(arr[storeIndex], arr[right]);
    return storeIndex;
}

int quickselect(std::vector<int>& arr, int k) {
    int left = 0, right = arr.size() - 1;
    k = k - 1;

    while (left <= right) {
        int pivotIndex = partition(arr, left, right);
        if (pivotIndex == k) return arr[pivotIndex];
        else if (pivotIndex < k) left = pivotIndex + 1;
        else right = pivotIndex - 1;
    }

    return -1;
}

void benchmark(int size, std::ofstream& outFile) {
    std::cout << "Generating array of size " << size << "...\n";
    std::vector<int> baseArray(size);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> distValue(1, 1e9);

    for (int& x : baseArray) x = distValue(rng);

    std::uniform_int_distribution<int> distK(1, size);

    std::cout << "Running 1000 Quickselects...\n";
    long long total_time_ns = 0;

    for (int i = 0; i < 1000; ++i) {
        int k = distK(rng);
        std::vector<int> tempArray = baseArray;

        auto start = std::chrono::high_resolution_clock::now();
        quickselect(tempArray, k);
        auto end = std::chrono::high_resolution_clock::now();

        total_time_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        if ((i + 1) % 100 == 0)
            std::cout << "  Completed " << (i + 1) << " queries\n";
    }

    double avg_ms = total_time_ns / 1000.0 / 1e6;
    std::cout << "Average time for size " << size << ": " << avg_ms << " ms\n\n";

    outFile << size << "," << avg_ms << "\n";
}

int main() {
    std::vector<int> sizes = {10'000'000, 20'000'000, 40'000'000, 80'000'000, 160'000'000};
    std::ofstream outFile("quickselect_results.csv");

    if (!outFile) {
        std::cerr << "Failed to open output file!\n";
        return 1;
    }

    outFile << "Size,AverageTimeMs\n";

    for (int size : sizes) {
        benchmark(size, outFile);
    }

    outFile.close();
    return 0;
}
