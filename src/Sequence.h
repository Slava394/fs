#pragma once

#include <initializer_list>

#include "ISequence.h"
#include "Vector.h"


template <class T>
class Sequence : public ISequence<T>
{
private:
    void swap(T& a, T& b)
    {
        T temp = a;
        a = b;
        b = temp;
    }

    size_t hoarePartition(size_t left, size_t right)
    {
        T pivot = get(left);
        size_t i = left;
        size_t j = right;
        while (true) 
        {
            while (get(j) > pivot) 
            {
                j--;
            }
            while (get(i) < pivot) 
            {
                i++;
            }
            if (i >= j) return j;
            swap(get(i), get(j));
            i++;
            j--;
        }
    }

    void quickSort(size_t left, size_t right) 
    {
        if (left < right) 
        {
            size_t pivotIndex = hoarePartition(left, right);
            quickSort(left, pivotIndex);       
            quickSort(pivotIndex + 1, right); 
        }
    }

    Vector<T> vector;
    
public:
    Sequence(size_t size = 0, T* dataPtr = nullptr) : vector(size, dataPtr) {}

    Sequence(std::initializer_list<T> initList) : vector(initList) {}

    Sequence(const Sequence<T>& sequence) : vector(sequence.vector) {}

    Sequence<T>& operator=(const Sequence<T>& other)
    {
        if (this != &other)
        {
            vector = other.vector;
        }
        return *this;
    }

    Sequence(Sequence<T>&& sequence) noexcept : vector(std::move(sequence.vector)) {}

    Sequence<T>& operator=(Sequence<T>&& sequence) noexcept
    {
        if (this != &sequence)
        {
            vector = std::move(sequence.vector);
        }
        return *this;
    }

    ~Sequence() override = default;

    T& operator[](size_t index) override 
    { 
        return vector[index]; 
    }

    const T& operator[](size_t index) const override 
    { 
        return vector[index]; 
    }
    
    T& getFirst() override 
    { 
        return vector.getFirst(); 
    }

    T& getLast() override 
    { 
        return vector.getLast(); 
    }

    T& get(size_t index) override 
    { 
        return vector.get(index); 
    }

    const T& getFirst() const override 
    { 
        return vector.getFirst(); 
    }

    const T& getLast() const override 
    { 
        return vector.getLast(); 
    }

    const T& get(size_t index) const override 
    { 
        return vector.get(index); 
    }

    size_t getSize() const override 
    { 
        return vector.getSize(); 
    }

    void set(const T& value, size_t index) override 
    { 
        vector.set(value, index); 
    }

    void prepend(const T& value) override 
    { 
        vector.prepend(value); 
    }

    void append(const T& value) override 
    { 
        vector.append(value); 
    }

    void insertAt(const T& value, size_t index) override 
    { 
        vector.insertAt(value, index); 
    }

    void removeFirst() override 
    { 
        vector.removeFirst(); 
    }

    void removeLast() override 
    { 
        vector.removeLast(); 
    }
    
    void removeAt(size_t index) override 
    { 
        vector.removeAt(index); 
    }

	size_t upperBound(const T& target) 
	{
		size_t left = 0;
		size_t right = getSize();
		while (left < right) 
		{
			size_t middle = left + (right - left) / 2;
			if (get(middle) <= target)
			{
				left = middle + 1;
			}
			else
			{
				right = middle;
			}
		}
		return right; 
	}

    void sort()
    {
        if (getSize() == 0) return;
		quickSort(0, getSize() - 1);
    }
};