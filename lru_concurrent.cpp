#include <iostream>
#include <unordered_map>
#include <string>
#include <mutex>      // Required for std::mutex and std::lock_guard
#include <thread>     // Required to spawn concurrent threads
#include <chrono>     // Required for std::this_thread::sleep_for

template <typename K, typename V>
class LRUCache {
private:
    struct Node {
        K key;
        V value;
        Node* prev;
        Node* next;

        Node() {
            prev = nullptr;
            next = nullptr;
        }

        Node(K k, V v) {
            key = k;
            value = v;
            prev = nullptr;
            next = nullptr;
        }
    };

    int capacity;
    Node* head;
    Node* tail;
    std::unordered_map<K, Node*> cache_map;
    
    // Mutex to synchronize all public accesses to the cache
    std::mutex cache_mutex;

    void addNode(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void moveToHead(Node* node) {
        removeNode(node);
        addNode(node);
    }

    Node* popTail() {
        Node* res = tail->prev;
        removeNode(res);
        return res;
    }

public:
    LRUCache(int cap) {
        capacity = cap;
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->prev = head;
    }

    ~LRUCache() {
        Node* current = head;
        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    // std::lock_guard locks the cache_mutex at the start and automatically unlocks it when the function returns
    bool get(K key, V& result) {
        std::lock_guard<std::mutex> lock(cache_mutex);
        if (cache_map.find(key) == cache_map.end()) {
            return false;
        }
        Node* node = cache_map[key];
        moveToHead(node);
        result = node->value;
        return true;
    }

    // std::lock_guard prevents writer thread and reader thread from editing cache pointers at the same time
    void put(K key, V value) {
        std::lock_guard<std::mutex> lock(cache_mutex);
        if (cache_map.find(key) != cache_map.end()) {
            Node* node = cache_map[key];
            node->value = value;
            moveToHead(node);
        } else {
            Node* newNode = new Node(key, value);
            cache_map[key] = newNode;
            addNode(newNode);
            if (cache_map.size() > capacity) {
                Node* evictedNode = popTail();
                cache_map.erase(evictedNode->key);
                delete evictedNode;
            }
        }
    }

    void print() {
        std::lock_guard<std::mutex> lock(cache_mutex);
        std::cout << "[Cache State]: ";
        Node* curr = head->next;
        if (curr == tail) {
            std::cout << "Empty\n";
            return;
        }
        while (curr != tail) {
            std::cout << "(" << curr->key << ":" << curr->value << ")";
            if (curr->next != tail) {
                std::cout << " <-> ";
            }
            curr = curr->next;
        }
        std::cout << "\n";
    }
};

// Console mutex: prevents threads from printing concurrently and garbling the terminal output
std::mutex cout_mutex;

// Writer Thread: Represents a background thread adding data to the cache (e.g. database updates)
void run_writer(LRUCache<std::string, int>& cache) {
    std::string keys[] = {"A", "B", "C", "D"};
    for (int i = 0; i < 4; ++i) {
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "[Writer Thread] Put: " << keys[i] << " = " << (i + 1) * 10 << "\n";
        }
        cache.put(keys[i], (i + 1) * 10);
        cache.print();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

// Reader Thread: Represents another parallel thread reading from the same cache (e.g. user requests)
void run_reader(LRUCache<std::string, int>& cache) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Start slightly after writer
    std::string keys[] = {"A", "C", "B", "E"};
    for (int i = 0; i < 4; ++i) {
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "[Reader Thread] Get: " << keys[i] << "\n";
        }
        int val;
        bool found = cache.get(keys[i], val);
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            if (found) {
                std::cout << "[Reader Thread] Found: " << keys[i] << " = " << val << "\n";
            } else {
                std::cout << "[Reader Thread] Missed: " << keys[i] << "\n";
            }
        }
        cache.print();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main() {
    std::cout << "=== Concurrent LRU Cache (lru_concurrent) ===\n\n";

    LRUCache<std::string, int> cache(3);

    // std::ref(cache) passes the cache object by reference.
    // std::thread copies arguments by default, but mutexes cannot be copied, so we MUST use std::ref() to avoid compilation error.
    std::thread t1(run_writer, std::ref(cache));
    std::thread t2(run_reader, std::ref(cache));

    // Wait for both threads to complete before exiting main
    t1.join();
    t2.join();

    return 0;
}
