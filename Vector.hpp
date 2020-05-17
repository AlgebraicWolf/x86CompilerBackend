//
// Created by alexey on 17.05.2020.
//

#ifndef X86COMPILERBACKEND_VECTOR_HPP
#define X86COMPILERBACKEND_VECTOR_HPP

#include <cstddef>
#include <utility>

template <typename T>
class vector {
private:
    size_t size;
    size_t capacity;
    T* elems;
public:
    explicit vector<T>(size_t capacity = 1);                    // Default constructor
    vector<T>(vector<T>&& other);                               // Move constructor
    vector<T> &operator=(vector<T>&& other);                    // Move assignment
    vector<T>(const vector<T>& other) = delete;                 // Prohibit copy constructor
    vector<T> &operator=(const vector<T>& other) = delete;      // Prohibit copy assignment
    ~vector<T>();                                               // Destructor

    T& operator[](size_t pos);                                  // Access operator
    void reserve(size_t newSize);                               // Vector resize
    void push_back(T&& elem);                                   // Append rvalue to back
    void push_back(const T& elem);                              // Append lvalue to back

    size_t getSize();
};

template<typename T>
vector<T>::vector(size_t capacity) : capacity(capacity), size(0) {
    elems = new T[capacity];
}

template<typename T>
vector<T>::vector(vector<T> &&other) {
    swap(*this, other);
}

template<typename T>
vector<T>& vector<T>::operator=(vector<T> &&other) {
    swap(*this, other);
    return *this;
}

template<typename T>
T& vector<T>::operator[](size_t pos) {
    return elems[pos];
}

template<typename T>
void vector<T>::reserve(size_t newSize) {
    if(newSize <= capacity)
        return;

    T* newElems = new T[newSize];

    for (int i = 0; i < size; i++) {
        new(newElems + i) T(std::move(elems[i]));
    }

    delete[] reinterpret_cast<const char *>(elems);

    elems = newElems;
    capacity = newSize;
}

template<typename T>
void vector<T>::push_back(T&& elem) {
    if(size == capacity)
        reserve(2 * capacity);

    new(elems + size) T(elem);
    size++;
}

template<typename T>
void vector<T>::push_back(const T& elem) {
    if(size == capacity)
        reserve(2 * capacity);

    new(elems + size) T(elem);
    size++;
}

template<typename T>
vector<T>::~vector() {
    delete[] elems;
}

template<typename T>
size_t vector<T>::getSize() {
    return size;
}

#endif //X86COMPILERBACKEND_VECTOR_HPP
