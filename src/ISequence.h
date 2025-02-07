#pragma once

#include <cstddef>


template <class T>
class ISequence
{
public:
    virtual T& operator[](size_t index) = 0;
    virtual const T& operator[](size_t index) const = 0;
    virtual T& getFirst() = 0;
    virtual T& getLast() = 0;
    virtual T& get(size_t index) = 0;
    virtual const T& getFirst() const = 0;
    virtual const T& getLast() const = 0;
    virtual const T& get(size_t index) const = 0;
    virtual size_t getSize() const = 0;

    virtual void set(const T& value, size_t index) = 0;

    virtual void prepend(const T& value) = 0;
    virtual void append(const T& value) = 0;
    virtual void insertAt(const T& value, size_t index) = 0;

    virtual void removeFirst() = 0;
    virtual void removeLast() = 0;
    virtual void removeAt(size_t index) = 0;

    virtual ~ISequence() = default;
};