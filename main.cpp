#include "veb.h"
#include "cache.h"

#include <iostream>
#include <vector>
#include <numeric>
#include <random>

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
    VEB t(sorted);

    vector<int> Bs = {4, 8, 16, 32, 64, 128, 256};
    vector<int> factors = {8, 8, 8, 8, 4, 4, 4, 4};

    for (int x = 1; x <= 6; x++) {
        int q = 1;
        for (int i = 0; i < x; i++) q *= 10;
        cout << "queries=10^" << x << "\n";
        for (int i = 0; i < (int)Bs.size(); i++) {
            int B = Bs[i];
            int M = B * factors[i];
            mt19937 rng(42);
            LRUCache cv(B, M), cf(B, M);
            for (int j = 0; j < q; j++) {
                int k = rng() % (2 * n);
                t.search(k, cv);
                flat_search(sorted, k, cf);
            }
            cout << "  B=" << B << " M=" << factors[i] << "*B veb=" << cv.get_misses()
                 << " flat=" << cf.get_misses() << "\n";
        }
    }
    return 0;
}
