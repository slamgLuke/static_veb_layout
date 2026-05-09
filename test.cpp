#include "veb.h"
#include "cache.h"

#include <iostream>
#include <vector>
#include <numeric>
#include <cassert>
#include <set>
#include <random>

using namespace std;

void test_build_correctness() {
    cout << "=== test_build_correctness ===\n";

    // h=2: input [1,2,3], expected layout [2,1,3]
    {
        vector<int> in = {1, 2, 3};
        VEBTree t(in);
        vector<int> got;
        for (auto& n : t.layout()) got.push_back(n.key);
        vector<int> expected = {2, 1, 3};
        assert(got == expected);
        cout << "  h=2 OK\n";
    }

    // h=3: input [1..7], expected [4,2,6,1,3,5,7]
    {
        vector<int> in(7); iota(in.begin(), in.end(), 1);
        VEBTree t(in);
        vector<int> got;
        for (auto& n : t.layout()) got.push_back(n.key);
        vector<int> expected = {4, 2, 6, 1, 3, 5, 7};
        assert(got == expected);
        cout << "  h=3 OK\n";
    }

    // h=4: input [1..15], expected [8,4,12,2,1,3,6,5,7,10,9,11,14,13,15]
    {
        vector<int> in(15); iota(in.begin(), in.end(), 1);
        VEBTree t(in);
        vector<int> got;
        for (auto& n : t.layout()) got.push_back(n.key);
        vector<int> expected = {8,4,12,2,1,3,6,5,7,10,9,11,14,13,15};
        assert(got == expected);
        cout << "  h=4 OK\n";
    }

    cout << "  PASS\n\n";
}

void test_search_correctness() {
    cout << "=== test_search_correctness ===\n";

    int h = 4, n = (1 << h) - 1;  // 15
    vector<int> in(n); iota(in.begin(), in.end(), 1);
    VEBTree t(in);
    LRUCache dummy(/*B=*/16, /*M=*/100);

    // Every present key should be found.
    for (int k = 1; k <= n; k++) {
        int idx = t.search(k, dummy);
        assert(idx != -1);
        assert(t.layout()[idx].key == k);
    }
    cout << "  all present keys found\n";

    // Absent keys should return -1.
    for (int k : {0, -5, 16, 100}) {
        int idx = t.search(k, dummy);
        assert(idx == -1);
    }
    cout << "  absent keys correctly missing\n";

    cout << "  PASS\n\n";
}

void test_io_counting() {
    cout << "=== test_io_counting ===\n";

    int n = 15;
    vector<int> in(n); iota(in.begin(), in.end(), 1);
    VEBTree t(in);

    // Search for 7 with B=4, M=2 should yield 3 misses (hand-traced).
    {
        LRUCache cache(4, 4*2);
        int idx = t.search(7, cache);
        assert(idx != -1);
        long long misses = cache.get_misses();
        cout << "  search(7) with B=4, M=4*2: " << misses << " misses (expected 3)\n";
        assert(misses == 3);
    }

    // Search for the root (8) should yield exactly 1 miss.
    {
        LRUCache cache(4, 4*2);
        t.search(8, cache);
        assert(cache.get_misses() == 1);
        cout << "  search(8) with B=4, M=4*2: 1 miss (expected 1) OK\n";
    }

    // Searching for the same key twice (with cache not reset) should yield
    // exactly the misses of the first search — second search is all hits.
    {
        LRUCache cache(4, 400);  // big M so nothing gets evicted
        t.search(7, cache);
        long long m1 = cache.get_misses();
        t.search(7, cache);
        long long m2 = cache.get_misses();
        cout << "  search(7) twice: first " << m1 << " misses, total " << m2 << " (second should add 0)\n";
        assert(m1 == m2);
    }

    cout << "  PASS\n\n";
}

void test_stress_correctness() {
    cout << "=== test_stress_correctness ===\n";

    mt19937 rng(42);
    for (int h = 1; h <= 10; h++) {
        int n = (1 << h) - 1;
        vector<int> in(n);
        iota(in.begin(), in.end(), 1);

        // Optional: shuffle the *values* but keep them sorted in the input.
        // To get distinct sorted keys not just 1..n:
        set<int> picked;
        while ((int)picked.size() < n) picked.insert(rng() % (10 * n));
        in.assign(picked.begin(), picked.end());

        VEBTree t(in);
        LRUCache dummy(16, 1000);

        // Every key must be findable.
        for (int k : in) {
            assert(t.search(k, dummy) != -1);
        }

        // Random absent keys.
        for (int trial = 0; trial < 100; trial++) {
            int k = rng() % (10 * n);
            bool present = picked.count(k);
            int idx = t.search(k, dummy);
            assert((idx != -1) == present);
        }
    }
    cout << "  PASS\n\n";
}

int main() {
    test_build_correctness();
    test_search_correctness();
    test_io_counting();
    test_stress_correctness();


    return 0;

}

