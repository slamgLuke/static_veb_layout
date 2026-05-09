# Static vEB Layout — I/O Benchmark

**Student: Lucas Carranza Bueno**

## Project focus

Static implementation of the **van Emde Boas tree layout** — a cache-oblivious
layout for a complete binary search tree of `n = 2^h - 1` keys. The tree is
built once over a sorted input; nodes carry explicit `left` / `right` indices
into a flat node array, so traversal does not depend on heap-style implicit
indexing. We compare it against flat-array binary search on the same data.

## Testing model

- LRU cache simulator parameterized by block size `B` (elements) and
  capacity `M` (blocks); every node access goes through it.
- Metric is total **I/O operations** (cache misses) per configuration; we
  also report wall time.
- Cache is **shared across queries** within a configuration — warm-cache
  reuse counts.
- Both algorithms see the **same query stream** (`mt19937(42)` reseeded per
  `(x, B)`), so comparisons are head-to-head.
- `test.cpp` covers build correctness, search correctness, hand-traced miss
  counts, and randomised stress on `h ∈ [1, 10]`.

## Fixed parameters

| Parameter        | Value                                              |
| ---------------- | -------------------------------------------------- |
| Tree height `h`  | `20`                                               |
| Number of keys   | `n = 2^20 - 1 = 1,048,575`                         |
| Query count      | `q = 10^6`                                         |
| Query RNG        | `std::mt19937(42)`, reseeded per `B`               |
| Query domain     | `rng() % (2n)` (mix of hits and out-of-range)      |
| Block sizes `B`  | `{4, 8, 16, 32, 64, 128, 256}` (elements)          |
| Cache `M`        | `factor · B` blocks; `factor = 8` for `B ≤ 32`, `factor = 4` for `B ≥ 64` |

## Results

`q = 10^6` queries, `n = 2^20 - 1` keys.

| B   | M    | vEB I/O    | vEB ms   | flat I/O    | flat ms  | I/O ratio |
| --- | ---- | ---------- | -------- | ----------- | -------- | --------- |
| 4   | 8·B  | 6,269,076  | 842.35   | 11,935,363  | 1453.28  | 1.90      |
| 8   | 8·B  | 3,554,828  | 698.99   | 7,131,173   | 1324.96  | 2.01      |
| 16  | 8·B  | 2,393,108  | 753.53   | 5,651,777   | 1832.04  | 2.36      |
| 32  | 8·B  | 1,460,507  | 850.97   | 4,512,668   | 2403.38  | 3.09      |
| 64  | 4·B  | 1,166,295  | 973.25   | 3,937,063   | 2851.75  | 3.38      |
| 128 | 4·B  | 933,357    | 1465.64  | 2,731,526   | 3699.11  | 2.93      |
| 256 | 4·B  | 624,858    | 2073.74  | 1,434,030   | 3868.91  | 2.30      |

vEB beats flat-array binary search at every block size, in both I/O and
wall time. vEB I/O drops roughly geometrically with `B`, consistent with
the `O(log_B n)` bound.

## Hardware

| Component | Spec                                                |
| --------- | --------------------------------------------------- |
| CPU       | AMD Ryzen AI 7 350 (8 cores / 16 threads, 2.0 GHz)  |
| RAM       | 16 GB                                               |
| OS        | Windows 11                                          |
| Compiler  | g++ (GCC) 15.2.0, `-O2`                             |
| Clock     | `std::chrono::high_resolution_clock`                |

## Reproducing

```bash
g++ -O2 cache.cpp veb.cpp main.cpp -o bench
./bench

g++ -O2 cache.cpp veb.cpp test.cpp -o test
./test
```
