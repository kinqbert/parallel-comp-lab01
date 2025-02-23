#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <chrono>

using namespace std;

void reflect_part(
    vector<vector<int>>& matrix,
    const int startRow,
    const int endRow,
    const int N
) {
    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < N; j++) {
            matrix[N - 1 - i][j] = matrix[i][j];
        }
    }
}

int main() {
    // ----------------------------
    // parameters
    // ----------------------------
    const int N = 10000;   // martrix size
    const int numThreads = 4;   // threads amount

    vector matrix(N, vector<int>(N));

    mt19937 rng{random_device{}()};
    uniform_int_distribution dist(0, 99);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = dist(rng);
        }
    }

    // ----------------------------
    // sequential
    // ----------------------------
    {
        auto matrixSequential = matrix;

        auto startSeq = chrono::high_resolution_clock::now();

        for (int i = 0; i < N / 2; i++) {
            for (int j = 0; j < N; j++) {
                matrixSequential[N - 1 - i][j] = matrixSequential[i][j];
            }
        }

        auto endSeq = chrono::high_resolution_clock::now();
        double elapsedSeq = chrono::duration<double>(endSeq - startSeq).count();

        cout << "[*] Sequential\n";
        cout << "Mirroring time: "
                  << elapsedSeq << " s" << endl << endl;
    }

    // ----------------------------
    // parallel
    // ----------------------------
    {
        auto matrixParallel = matrix;

        auto startPar = chrono::high_resolution_clock::now();

        int halfRows = N / 2;
        int chunkSize = halfRows / numThreads;

        vector<thread> threads;
        threads.reserve(numThreads);

        for (int t = 0; t < numThreads; t++) {
            int startRow = t * chunkSize;
            int endRow   = (t == numThreads - 1) ? halfRows : (t + 1) * chunkSize;

            threads.emplace_back(reflect_part,
                                 ref(matrixParallel),
                                 startRow,
                                 endRow,
                                 N);
        }

        for (auto& th : threads) {
            th.join();
        }

        auto endPar = chrono::high_resolution_clock::now();
        double elapsedPar = chrono::duration<double>(endPar - startPar).count();

        cout << "[*] Parallel \n";
        cout << "Threads amount: " << numThreads << endl;
        cout << "Mirroring time: "
                  << elapsedPar << " s" << endl << endl;
    }

    return 0;
}
