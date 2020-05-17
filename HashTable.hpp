//
// Created by alexey on 17.05.2020.
//

#ifndef X86COMPILERBACKEND_HASHTABLE_HPP
#define X86COMPILERBACKEND_HASHTABLE_HPP

#include <cstring>
#include <immintrin.h>

template<typename T>
class List {
private:
    size_t capacity;
    size_t size;

    size_t head;
    size_t tail;
    size_t freeHead;

    T *values;
    size_t *nexts;
public:
//    List();                                       // Default constructor
    explicit List(size_t capacity = 32);            // Constructor according to desired capacity
    List(const List &lst);                          // Copy constructor
    List(List &&lst) noexcept;                      // Move constructor
    ~List() noexcept;                               // Destructor
    List &operator=(const List &lst);               // Copy assignment
    List &operator=(List &&lst) noexcept;           // Move assignment

    size_t Head();                                  // Get head
    size_t Size();                                  // Get size
    void Reserve(size_t n);                         // Allocate space in order to ensure n elements could be placed
    void Insert(size_t pos, const T &value);        // Insert values after element at pos
    void PushBack(const T &value);                  // Insert values at the end of the list
    T Get(size_t pos);                              // Get element
    T *GetValuesArray();                            // Array of values
    size_t *GetNextsArray();                        // Array of next positions
};


template<typename FunctorObject, int BucketSize>
class HashTable {
private:
    struct KeyValuePair {
        const char *key;
        int value;

        KeyValuePair() = default;

        KeyValuePair(const char *key, int value);
    };

    size_t capacity;                                // Hash table capacity
    List<KeyValuePair> *table;                      // Hash table itself
    FunctorObject hash;

    void releaseMemory();                           // Function to release memory
public:
//    HashTable();                                    // Default constructor
    explicit HashTable(
            size_t n = 997);                      // Constructor that ensures n different hash values could be stored
    ~HashTable() noexcept;                          // Destructor
    HashTable(const HashTable &other);              // Copy constructor
    HashTable(HashTable &&other) noexcept;                   // Move constructor
    HashTable &operator=(const HashTable &other);   // Copy assignment
    HashTable &operator=(HashTable &&other);        // Move assignment

    void Insert(const char *key, int value);        // Insertion method
    int Get(const char *key);                       // Get values by key
//    void Delete(const char *key);                   // Delete values by key
};

struct CRC32CFunctor {
    unsigned int operator()(const char *key);
};

template<typename T>
size_t List<T>::Head() {
    return head;
}

template<typename T>
void List<T>::Reserve(size_t n) {
    if (capacity < n) {
        size_t *newNexts = new size_t[n];
        T *newValues = new T[n];

        memcpy(newNexts, nexts, sizeof(size_t) * capacity);
        memcpy(newValues, values, sizeof(T) * capacity);

        for (int i = capacity; i < n; i++) {
            newNexts[i] = i + 1;
        }

        delete[] values;
        delete[] nexts;

        values = newValues;
        nexts = newNexts;
    }
}

template<typename T>
List<T>::List(size_t capacity) : capacity(capacity), size(0), head(0), tail(0), freeHead(0) {
    values = new T[capacity];
    nexts = new size_t[capacity];

    for (int i = 0; i < capacity; i++) {
        nexts[i] = i + 1;                           // Set addresses of next free positions
    }
}

template<typename T>
List<T>::List(List &&lst) noexcept: capacity(lst.capacity), size(lst.size), head(lst.head), tail(lst.tail),
                                    freeHead(lst.freeHead) {
    values = lst.values;
    nexts = lst.nexts;

    lst.capacity = 0;
    lst.size = 0;
    lst.head = 0;
    lst.tail = 0;
    lst.freeHead = 0;

    lst.nexts = nullptr;
    lst.values = nullptr;
}

template<typename T>
List<T> &List<T>::operator=(const List &lst) {
    delete[] values;
    delete[] nexts;

    capacity = lst.capacity;
    size = lst.size;
    head = lst.head;
    tail = lst.tail;
    freeHead = lst.freeHead;

    values = new T[capacity];
    nexts = new size_t[capacity];

    memcpy(values, lst.values, sizeof(T) * capacity);
    memcpy(nexts, lst.nexts, sizeof(size_t) * capacity);

    return *this;
}

template<typename T>
List<T> &List<T>::operator=(List &&lst) noexcept {
    delete[] values;
    delete[] nexts;

    capacity = lst.capacity;
    size = lst.size;
    head = lst.head;
    tail = lst.tail;
    freeHead = lst.freeHead;

    values = lst.values;
    nexts = lst.nexts;

    lst.capacity = 0;
    lst.size = 0;
    lst.head = 0;
    lst.tail = 0;
    lst.freeHead = 0;

    lst.nexts = nullptr;
    lst.values = nullptr;

    return *this;
}

template<typename T>
List<T>::~List() noexcept {
    delete[] values;
    delete[] nexts;
}

template<typename T>
void List<T>::PushBack(const T &val) {
    if (size == capacity) {
        Reserve(2 * capacity);
        capacity *= 2;
    }

    values[freeHead] = val;

    if (!size) {
        head = freeHead;
        tail = freeHead;
    } else {
        nexts[tail] = freeHead;
        tail = freeHead;
    }

    freeHead = nexts[freeHead];
    size++;

}

template<typename T>
T *List<T>::GetValuesArray() {
    return values;
}

template<typename T>
size_t *List<T>::GetNextsArray() {
    return nexts;
}

template<typename T>
size_t List<T>::Size() {
    return size;
}

template<typename T>
T List<T>::Get(size_t pos) {
    if (pos < size) {
        int cur = head;
        for (int i = 0; i < pos; i++)
            cur = nexts[cur];

        return values[cur];
    }
    return static_cast<T>(0);
}

template<typename T>
void List<T>::Insert(size_t pos, const T &val) {
    if (size == capacity) {
        Reserve(2 * capacity);
        capacity *= 2;
    }

    int cur = head;
    for (int i = 0; i < pos; i++) {
        cur = nexts[cur];
    }

    int newPos = freeHead;
    freeHead = nexts[freeHead];

    values[newPos] = val;
    nexts[newPos] = nexts[cur];
    nexts[cur] = newPos;
    size++;
    if (tail == cur)
        tail = newPos;

}

template<typename T>
List<T>::List(const List<T> &lst): capacity(lst.capacity), size(lst.size), head(lst.head), tail(lst.tail),
                                   freeHead(lst.freeHead) {
    values = new T[capacity];
    nexts = new size_t[capacity];

    memcpy(values, lst.values, sizeof(T) * capacity);
    memcpy(nexts, lst.nexts, sizeof(size_t) * capacity);
}

template<typename FunctorObject, int BucketSize>
HashTable<FunctorObject, BucketSize>::KeyValuePair::KeyValuePair(const char *key, int value): value(value) {
    this->key = key;
}

template<typename FunctorObject, int BucketSize>
void HashTable<FunctorObject, BucketSize>::releaseMemory() {
    delete[] table;
}

template<typename FunctorObject, int BucketSize>
HashTable<FunctorObject, BucketSize>::HashTable(size_t n): capacity(n), hash() {
    table = new List<KeyValuePair>[capacity];
    for (int i = 0; i < capacity; i++) {
        table[i] = List<KeyValuePair>(BucketSize);
    }
}

template<typename FunctorObject, int BucketSize>
HashTable<FunctorObject, BucketSize>::~HashTable() noexcept {
    releaseMemory();
}

template<typename FunctorObject, int BucketSize>
HashTable<FunctorObject, BucketSize>::HashTable(const HashTable<FunctorObject, BucketSize> &other): capacity(
        other.capacity), hash() {
    table = new List<KeyValuePair>[capacity];

    for (int i = 0; i < capacity; i++) {
        table[i] = other.table[i];
    }
}

template<typename FunctorObject, int BucketSize>
HashTable<FunctorObject, BucketSize>::HashTable(HashTable<FunctorObject, BucketSize> &&other) noexcept: capacity(
        other.capacity) {
    table = other.table;

    other.capacity = 0;
    other.table = nullptr;
}

template<typename FunctorObject, int BucketSize>
HashTable<FunctorObject, BucketSize> &
HashTable<FunctorObject, BucketSize>::operator=(const HashTable<FunctorObject, BucketSize> &other) {
    capacity = other.capacity;

    releaseMemory();

    table = new List<KeyValuePair>[capacity];
    for (int i = 0; i < capacity; i++) {
        table[i] = other.table[i];
    }

    return *this;
}

template<typename FunctorObject, int BucketSize>
HashTable<FunctorObject, BucketSize> &HashTable<FunctorObject, BucketSize>::operator=(HashTable &&other) {
    releaseMemory();

    table = other.table;
    capacity = other.capacity;

    other.capacity = 0;
    other.table = nullptr;

    return *this;
}

template<typename FunctorObject, int BucketSize>
void HashTable<FunctorObject, BucketSize>::Insert(const char *key, int value) {
    unsigned int pos = hash(key) % capacity;
    List<KeyValuePair> &bucket = table[pos];

    KeyValuePair *bucketData = bucket.GetValuesArray();
    size_t *nexts = bucket.GetNextsArray();

    int cur = bucket.Head();

    for (int i = 0; i < bucket.Size(); i++) {
        if (!strcmp(bucketData[cur].key, key)) {
            bucketData[cur].value = value;
            return;
        }
        cur = nexts[cur];
    }
    bucket.PushBack(KeyValuePair(key, value));
}

template<typename FunctorObject, int BucketSize>
int HashTable<FunctorObject, BucketSize>::Get(const char *key) {
    unsigned int bucketNum = hash(key) % capacity;
    List<KeyValuePair> &bucket = table[bucketNum];

    size_t cur = bucket.Head();
    size_t *nexts = bucket.GetNextsArray();
    KeyValuePair *elems = bucket.GetValuesArray();

    for (int i = 0; i < bucket.Size(); i++) {
        if (!strcmp(elems[cur].key, key))
            return elems[cur].value;
        cur = nexts[cur];
    }

    return -1;
}

unsigned int CRC32CFunctor::operator()(const char *key) {
    unsigned int hash = 0;

    while (*key) {
        hash = _mm_crc32_u8(hash, *key);
        key++;
    }

    return hash;
}

#endif //X86COMPILERBACKEND_HASHTABLE_HPP
