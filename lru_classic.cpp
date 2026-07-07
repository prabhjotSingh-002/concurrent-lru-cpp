#include <iostream>
#include <unordered_map>
#include <string>

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

    bool get(K key, V& result) {
        if (cache_map.find(key) == cache_map.end()) {
            return false;
        }
        Node* node = cache_map[key];
        moveToHead(node);
        result = node->value;
        return true;
    }

    void put(K key, V value) {
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

int main() {
    std::cout << "=== LRU Cache Demonstration (lru_classic) ===\n";
    std::cout << "Format: (Key : Value) | Left-most = Most Recently Used (MRU) | Right-most = Least Recently Used (LRU)\n\n";

    LRUCache<std::string, int> cache(4);
    cache.print();

    // 1. Fill the cache
    std::cout << "--- Step 1: Adding 4 items to fill the cache (Capacity = 4) ---\n";
    cache.put("A", 10);
    cache.put("B", 20);
    cache.put("C", 30);
    cache.put("D", 40);
    cache.print();
    std::cout << "Explanation: 'D' is at the left (MRU) because it was added last.\n";
    std::cout << "             'A' is at the right (LRU) because it was added first.\n\n";

    // 2. Cache Hit (Accessing an item)
    std::cout << "--- Step 2: Accessing key 'A' (Cache Hit) ---\n";
    int val;
    if (cache.get("A", val)) {
        std::cout << "[Hit] Found A = " << val << "\n";
    }
    cache.print();
    std::cout << "Explanation: 'A' was at the far right (LRU), but accessing it\n";
    std::cout << "             moved it to the far left (MRU). 'B' is now the oldest (LRU).\n\n";

    // 3. Cache Hit on another item
    std::cout << "--- Step 3: Accessing key 'C' (Cache Hit) ---\n";
    if (cache.get("C", val)) {
        std::cout << "[Hit] Found C = " << val << "\n";
    }
    cache.print();
    std::cout << "Explanation: 'C' moves to the far left (MRU). 'B' remains the oldest (LRU).\n\n";

    // 4. Cache Insertion with Eviction
    std::cout << "--- Step 4: Adding 5th item 'E' = 50 (Triggers Eviction) ---\n";
    std::cout << "Since cache is full, the oldest item on the far right ('B') will be evicted.\n";
    cache.put("E", 50);
    cache.print();

    // 5. Verification
    std::cout << "\n--- Step 5: Checking if 'B' was evicted successfully ---\n";
    if (cache.get("B", val)) {
        std::cout << "Found B = " << val << "\n";
    } else {
        std::cout << "[Miss] B not found (Evicted successfully!)\n";
    }

    std::cout << "\n--- Step 6: Checking if 'A' is still safe in cache ---\n";
    if (cache.get("A", val)) {
        std::cout << "[Hit] Found A = " << val << "\n";
    } else {
        std::cout << "A not found\n";
    }
    cache.print();

    return 0;
}
