#include "cache.h"

LRUCache::LRUCache(int B, int M) : B(B), M(M) {}

void LRUCache::access(int idx) {
    int block = idx / B;
    timestamp++;

    if (last_used.find(block) != last_used.end()) {
        last_used[block] = timestamp;
        return;
    }

    misses++;
    if ((int)last_used.size() >= M) {
        auto lru = last_used.begin();
        for (auto it = last_used.begin(); it != last_used.end(); it++) {
            if (it->second < lru->second) lru = it;
        }
        last_used.erase(lru);
    }
    last_used[block] = timestamp;


}

long long LRUCache::get_misses() const { return misses; }

void LRUCache::reset() {
    last_used.clear();
    timestamp = 0;
    misses = 0;
}