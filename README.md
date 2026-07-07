# High-Performance LRU Cache (C++17)

A clean, standard implementation of a Least Recently Used (LRU) Cache in C++17. This project features two designs: a classic single-threaded cache and a thread-safe concurrent cache, structured to demonstrate C++ data structure fundamentals and multithreading synchronization.

---

## Folder Structure
```text
lru_systemDesign/
├── lru_classic.cpp      # Single-threaded implementation & detailed demonstration
├── lru_concurrent.cpp   # Multi-threaded implementation & concurrent demo
└── README.md            # Detailed project documentation
```

---

## 1. Core LRU Theory

An LRU Cache stores key-value pairs and automatically discards the **Least Recently Used** item when the cache exceeds its capacity limit. 

To achieve **$O(1)$ time complexity** for both lookups (`get`) and insertions (`put`), we pair two data structures:
1. **Hash Map (`std::unordered_map`):** Maps a key to the memory address of its corresponding Node (`Node*`). This allows us to search for any element in $O(1)$ time.
2. **Doubly Linked List:** Tracks the order in which items are used.
   - **Head (MRU - Most Recently Used):** The left-most element is the most recently accessed or inserted item.
   - **Tail (LRU - Least Recently Used):** The right-most element is the oldest accessed item.
   - **Sentinels:** The list uses dummy head and tail sentinel nodes. This removes the need for null-pointer checks during list insertions/deletions, keeping pointer manipulations clean and safe.

---

## 2. File 1: Single-Threaded Version (`lru_classic.cpp`)

This file contains the basic LRU cache implementation and a comprehensive console trace demonstration.

### Compilation & Run:
```powershell
g++ -std=c++17 lru_classic.cpp -o lru_classic.exe
.\lru_classic.exe
```

### Execution Output Trace:
```text
=== LRU Cache Demonstration (lru_classic) ===
Format: (Key : Value) | Left-most = Most Recently Used (MRU) | Right-most = Least Recently Used (LRU)

[Cache State]: Empty
--- Step 1: Adding 4 items to fill the cache (Capacity = 4) ---
[Cache State]: (D:40) <-> (C:30) <-> (B:20) <-> (A:10)
Explanation: 'D' is at the left (MRU) because it was added last.
             'A' is at the right (LRU) because it was added first.

--- Step 2: Accessing key 'A' (Cache Hit) ---
[Hit] Found A = 10
[Cache State]: (A:10) <-> (D:40) <-> (C:30) <-> (B:20)
Explanation: 'A' was at the far right (LRU), but accessing it
             moved it to the far left (MRU). 'B' is now the oldest (LRU).

--- Step 3: Accessing key 'C' (Cache Hit) ---
[Hit] Found C = 30
[Cache State]: (C:30) <-> (A:10) <-> (D:40) <-> (B:20)
Explanation: 'C' moves to the far left (MRU). 'B' remains the oldest (LRU).

--- Step 4: Adding 5th item 'E' = 50 (Triggers Eviction) ---
Since cache is full, the oldest item on the far right ('B') will be evicted.
[Cache State]: (E:50) <-> (C:30) <-> (A:10) <-> (D:40)

--- Step 5: Checking if 'B' was evicted successfully ---
[Miss] B not found (Evicted successfully!)

--- Step 6: Checking if 'A' is still safe in cache ---
[Hit] Found A = 10
[Cache State]: (A:10) <-> (E:50) <-> (C:30) <-> (D:40)
```

---

## 3. File 2: Concurrent Version (`lru_concurrent.cpp`)

This version implements thread safety to support parallel reads and writes.

### Compilation & Run:
```powershell
g++ -std=c++17 lru_concurrent.cpp -o lru_concurrent.exe -lpthread
.\lru_concurrent.exe
```
