#include "veb.h"
#include <vector>
#include <numeric>
#include <cassert>

using namespace std;

VEBTree::VEBTree(vector<int>& sorted_keys) {
    int n = (int)sorted_keys.size();
    height = 0;
    while ((1 << height) - 1 < n) {
        height++;
    }
    assert((1 << height) - 1 == n);

    arr.resize(n);

    vector<int> ranks(n);
    iota(ranks.begin(), ranks.end(), 0);

    vector<int> rank_of_pos(n);
    vector<int> pos_of_rank(n);
    int offset = 0;

    recursive_build(sorted_keys, ranks, height, offset, rank_of_pos);
    assert(offset == n);

    for (int i = 0; i < n; i++) {
        pos_of_rank[rank_of_pos[i]] = i;
    }

    link((n - 1) / 2, height, pos_of_rank);
}

void VEBTree::recursive_build(vector<int>& keys, vector<int>& ranks, int h, int& arr_offset, vector<int>& rank_of_pos) {
    assert((int)keys.size() == (1 << h) - 1);

    if (h == 1) {
        arr[arr_offset].key = keys[0];
        rank_of_pos[arr_offset] = ranks[0];
        arr_offset++;
        return;
    }

    int top_h = (h + 1) / 2; // ceil
    int bot_h = (h / 2);     // floor
    int top_size = (1 << top_h) - 1;
    int bot_size = (1 << bot_h) - 1;
    int n_bot_subtrees = 1 << top_h;


    vector<int> top_keys, top_ranks;
    top_keys.reserve(top_size);
    top_ranks.reserve(top_size);

    for (int i = 0; i < top_size; i++) {
        int idx = (i+1) * bot_size + i;
        top_keys.push_back(keys[idx]);
        top_ranks.push_back(ranks[idx]);
    }
    recursive_build(top_keys, top_ranks, top_h, arr_offset, rank_of_pos);


    for (int i = 0; i < n_bot_subtrees; i++) {
        int start = i * (bot_size + 1);

        vector<int> bot_keys(keys.begin() + start,
                                keys.begin() + start + bot_size);
        vector<int> bot_ranks(ranks.begin() + start,
                                ranks.begin() + start + bot_size);

        recursive_build(bot_keys, bot_ranks, bot_h, arr_offset, rank_of_pos);
    }

}


void VEBTree::link(int r, int h_v, vector<int>& pos_of_rank) {
    int my_flat = pos_of_rank[r];
    if (h_v == 1) {
        arr[my_flat].left = arr[my_flat].right = -1;
        return;
    }
    int delta = 1 << (h_v - 2);
    int lr = r - delta;
    int rr = r + delta;
    arr[my_flat].left = pos_of_rank[lr];
    arr[my_flat].right = pos_of_rank[rr];
    link(lr, h_v - 1, pos_of_rank);
    link(rr, h_v - 1, pos_of_rank);
}

int VEBTree::search(int key, LRUCache& cache) {
    int i = 0;
    while (i != -1) {
        cache.access(i);
        if (arr[i].key == key) return i;
        i = (key < arr[i].key) ? arr[i].left : arr[i].right;
    }
    return -1;
}

const vector<Node>& VEBTree::layout() const {
    return arr;
}