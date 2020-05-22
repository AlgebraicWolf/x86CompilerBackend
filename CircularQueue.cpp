//
// Created by alexey on 20.05.2020.
//

#include "CircularQueue.hpp"

CircularQueue::CircularQueue(int capacity) : capacity(capacity), size(0), head(0), tail(0)  {
    elements = new int[capacity];
}

CircularQueue::~CircularQueue() {
    delete[] elements;
}

CircularQueue::CircularQueue(CircularQueue &&other) {
    swap(*this, other);
}

CircularQueue& CircularQueue::operator=(CircularQueue &&other) {
    swap(*this, other);
}

int CircularQueue::getSize() {
    return size;
}

void CircularQueue::push(int elem) {
    if(size == capacity)
        throw_exception("Queue overflow");

    size++;
    elements[head] = elem;
    head = (head + 1) % capacity;
}

int CircularQueue::pop() {
    if(!size)
        throw_exception("Queue underflow");

    size--;
    int elem = elements[tail];
    tail = (tail + 1) % capacity;

    return elem;
}



