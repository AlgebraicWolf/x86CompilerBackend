//
// Created by alexey on 20.05.2020.
//

#ifndef X86COMPILERBACKEND_CIRCULARQUEUE_HPP
#define X86COMPILERBACKEND_CIRCULARQUEUE_HPP

#include "utilities.hpp"

class CircularQueue {
private:
    int *elements;
    int size;
    int capacity;
    int head;
    int tail;

public:
    explicit CircularQueue(int capacity);                                               // Queue constructor
    ~CircularQueue();                                                                   // Queue destructor
    CircularQueue(const CircularQueue& other) = delete;                                 // Prohibit copying
    CircularQueue& operator=(const CircularQueue& other) = delete;                      // Prohibit copying
    CircularQueue(CircularQueue&& other);                                               // Move constructor
    CircularQueue& operator=(CircularQueue&& other);                                    // Move assignment

    void push(int elem);                                                                // Add element to queue
    int pop();                                                                          // Get element from queue
    int getSize();                                                                      // Size getter
};

#endif //X86COMPILERBACKEND_CIRCULARQUEUE_HPP
