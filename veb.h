#pragma once

#include <vector>
#include "cache.h"

using namespace std;

struct Node {
    int key;
    int left, right;

    Node() : key(0), left(-1), right(-1) {}
};

class VEB {
    vector<Node> arr;
    int height;

    void recursive_build(vector<int>& keys, vector<int>& ranks, int h, int& arr_offset, vector<int>& rank_of_pos);
    void link(int r, int h_v, vector<int>& pos_of_rank);

   public:
    VEB(vector<int>& sorted_keys);
    int search(int key, LRUCache& cache);
    const vector<Node>& layout() const;
};