#include "veb.h"
#include "cache.h"

#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <chrono>

using namespace std;

int flat_search(vector<int>& arr, int key, LRUCache& cache) {
    int lo = 0, hi = (int)arr.size() - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        cache.access(mid);
        if (arr[mid] == key) return mid;
        if (key < arr[mid]) hi = mid - 1;
        else lo = mid + 1;
    }
    return -1;
}

int main() {
    int h = 20;
    int n = (1 << h) - 1;
    vector<int> sorted(n); iota(sorted.begin(), sorted.end(), 1);
    VEBTree t(sorted);

    vector<int> Bs = {4, 8, 16, 32, 64, 128, 256};
    vector<int> factors = {8, 8, 8, 8, 4, 4, 4};

    int x = 6;
    int q = 1;
    for (int i = 0; i < x; i++) q *= 10;
    cout << "queries=10^" << x << "\n";
    {
        for (int i = 0; i < (int)Bs.size(); i++) {
            int B = Bs[i];
            int M = B * factors[i];

            LRUCache cv(B, M);
            mt19937 rng_v(42);
            auto t0 = chrono::high_resolution_clock::now();
            for (int j = 0; j < q; j++) {
                int k = rng_v() % (2 * n);
                t.search(k, cv);
            }
            auto t1 = chrono::high_resolution_clock::now();

            LRUCache cf(B, M);
            mt19937 rng_f(42);
            auto t2 = chrono::high_resolution_clock::now();
            for (int j = 0; j < q; j++) {
                int k = rng_f() % (2 * n);
                flat_search(sorted, k, cf);
            }
            auto t3 = chrono::high_resolution_clock::now();

            double veb_ms = chrono::duration<double, milli>(t1 - t0).count();
            double flat_ms = chrono::duration<double, milli>(t3 - t2).count();

            cout << "  B=" << B << " M=" << factors[i] << "*B"
                 << " veb=" << cv.get_misses() << " (" << veb_ms << "ms)"
                 << " flat=" << cf.get_misses() << " (" << flat_ms << "ms)\n";
        }
    }
    return 0;
}
