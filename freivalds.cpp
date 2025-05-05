#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>

using namespace std;
using namespace chrono;

using Matrix = vector<vector<int>>;

Matrix generate_matrix(int n, int mod = 1000) {
    Matrix mat(n, vector<int>(n));
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, mod - 1);

    for (auto& row : mat)
        for (auto& x : row)
            x = dist(gen);

    return mat;
}

bool freivalds_check(const Matrix& A, const Matrix& B, const Matrix& C, int trials = 10) {
    int n = A.size();
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> bit(0, 1);

    for (int t = 0; t < trials; ++t) {
        vector<int> r(n);
        for (int& val : r)
            val = bit(gen);

        vector<int> Br(n, 0), AB_r(n, 0), Cr(n, 0);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                Br[i] = (Br[i] + 1LL * B[i][j] * r[j]) % 1000000007;

        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                AB_r[i] = (AB_r[i] + 1LL * A[i][j] * Br[j]) % 1000000007;

        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                Cr[i] = (Cr[i] + 1LL * C[i][j] * r[j]) % 1000000007;

        if (AB_r != Cr)
            return false;
    }
    return true;
}

int main() {
    vector<int> sizes = {500, 1000, 2000, 4000, 8000};
    ofstream out_time("freivalds_timing.csv");
    ofstream out_error("freivalds_errors.csv");

    out_time << "n,time_sec\n";
    out_error << "n,false_accept_rate\n";

    for (int n : sizes) {
        int false_accepts = 0;

        auto t0 = high_resolution_clock::now();

        for (int i = 0; i < 1000; ++i) {
            Matrix A = generate_matrix(n);
            Matrix B = generate_matrix(n);
            Matrix C = generate_matrix(n);

            if (freivalds_check(A, B, C)) ++false_accepts;
        }

        auto t1 = high_resolution_clock::now();
        double elapsed = duration_cast<milliseconds>(t1 - t0).count() / 1000.0;

        double error_rate = false_accepts / 1000.0;

        cout << "n = " << n
             << " | time = " << elapsed << "s"
             << " | false accepts = " << error_rate << "\n";

        out_time << n << "," << elapsed << "\n";
        out_error << n << "," << error_rate << "\n";
    }

    out_time.close();
    out_error.close();
    return 0;
}
