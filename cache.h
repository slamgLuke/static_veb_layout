#pragma once

#include <unordered_map>

using namespace std;

class LRUCache {
    int B;
    int M;
    unordered_map<int, long long> last_used;
    long long timestamp = 0;
    long long misses = 0;

public:
    LRUCache(int B, int M);
    void access(int idx);
    long long get_misses() const;
    void reset();
};