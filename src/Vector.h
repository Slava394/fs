#pragma once

#include <initializer_list>
#include <stdexcept>


template <class T>
class Vector
{
private:
	void checkSize() const
	{
		if (size_ == 0)
		{
			throw std::logic_error("Vector has zero size.");
		}
	}

	void checkIndex(size_t index = 0, bool isInsert = false)
	{
		if (index >= size_ && !isInsert)
		{
			throw std::out_of_range("Index out of range.");
		}
		if (index > size_ && isInsert)
		{
			throw std::out_of_range("Index out of range.");
		}
	}

	void reserve(size_t allocatedMemory)
	{
		if (allocatedMemory < size_)
		{
			throw std::invalid_argument("Cannot reduce reserved memory.");
		}
		if (allocatedMemory > allocatedMemory_)
		{
			T* dataPtr = new T[allocatedMemory];
			for (size_t i = 0; i < size_; ++i)
			{
				dataPtr[i] = dataPtr_[i];
			}
			delete[] dataPtr_;
			dataPtr_ = dataPtr;
			allocatedMemory_ = allocatedMemory;
		}
	}

	void grow()
	{
		if (allocatedMemory_ == 0)
		{
			reserve(2);
		}
		else
		{
			reserve(allocatedMemory_ * 2);
		}
	}

	size_t size_;
	size_t allocatedMemory_;
	T* dataPtr_;

public:
	Vector(size_t size = 0, T* dataPtr = nullptr)
	{
		size_ = size;
		allocatedMemory_ = size * 2;
		dataPtr_ = nullptr;
		if (allocatedMemory_ > 0)
		{
			dataPtr_ = new T[allocatedMemory_];
			if (dataPtr != nullptr)
			{
				for (size_t i = 0; i < size_; ++i)
				{
					dataPtr_[i] = dataPtr[i];
				}
			}
		}
	}

	Vector(std::initializer_list<T> initList)
    {
        size_ = initList.size();
        allocatedMemory_ = size_ * 2;
        dataPtr_ = new T[allocatedMemory_];
        size_t i = 0;
        for (const T& item : initList)
        {
            dataPtr_[i++] = item;
        }
    }

	Vector(const Vector<T>& other)
	{
		size_ = other.size_;
		allocatedMemory_ = other.allocatedMemory_;
		dataPtr_ = nullptr;
		if (allocatedMemory_ > 0)
		{
			dataPtr_ = new T[allocatedMemory_];
			for (size_t i = 0; i < size_; ++i)
			{
				dataPtr_[i] = other.dataPtr_[i];
			}
		}
	}

	Vector<T>& operator=(const Vector<T>& other)
	{
		if (this != &other)
		{
			clear();
			size_ = other.size_;
			allocatedMemory_ = other.allocatedMemory_;
			if (allocatedMemory_ > 0)
			{
				dataPtr_ = new T[allocatedMemory_];
				for (size_t i = 0; i < size_; ++i)
				{
					dataPtr_[i] = other.dataPtr_[i];
				}
			}
		}
		return *this;
	}

	Vector(Vector<T>&& other) noexcept
	{
		size_ = other.size_;
		allocatedMemory_ = other.allocatedMemory_;
		dataPtr_ = other.dataPtr_;
		other.size_ = other.allocatedMemory_ = 0;
		other.dataPtr_ = nullptr;
	}

	Vector<T>& operator=(Vector<T>&& other) noexcept
	{
		if (this != &other)
		{
			clear();
			size_ = other.size_;
			allocatedMemory_ = other.allocatedMemory_;
			dataPtr_ = other.dataPtr_;
			other.size_ = other.allocatedMemory_ = 0;
			other.dataPtr_ = nullptr;
		}
		return *this;
	}

	~Vector()
	{
		clear();
	}

	T& operator[](size_t index)
	{
		return get(index);
	}

	const T& operator[](size_t index) const
	{
		return get(index);
	}

	T& get(size_t index) 
	{
		checkSize();
		return dataPtr_[index];
	}

	T& getFirst() 
	{
		checkSize();
		return dataPtr_[0];
	}

	T& getLast() 
	{
		checkSize();
		return dataPtr_[size_ - 1];
	}

	const T& get(size_t index) const
	{
		checkSize();
		return dataPtr_[index];
	}

	const T& getFirst() const
	{
		checkSize();
		return dataPtr_[0];
	}

	const T& getLast() const
	{
		checkSize();
		return dataPtr_[size_ - 1];
	}

	size_t getSize() const
	{
		return size_;
	}

	size_t getCapacity() const
	{
		return allocatedMemory_;
	}

	void set(const T& value, size_t index)
	{
		checkIndex(index);
		dataPtr_[index] = value;
	}

	void insertAt(const T& value, size_t index)
	{
		checkIndex(index, true);
		if (size_ == allocatedMemory_)
		{
			grow();
		}
		for (size_t backIndex = size_; index < backIndex; --backIndex)
		{
			dataPtr_[backIndex] = dataPtr_[backIndex - 1];
		}
		dataPtr_[index] = value;
		size_++;
	}

	void prepend(const T& value)
	{
		insertAt(value, 0);
	}

	void append(const T& value)
	{
		insertAt(value, size_);
	}

	void removeAt(size_t index)
	{
		checkIndex(index);
		for (size_t newIndex = index; newIndex + 1 < size_; ++newIndex)
		{
			dataPtr_[newIndex] = dataPtr_[newIndex + 1];
		}
		size_--;
	}

	void removeFirst()
	{
		checkSize();
		removeAt(0);
	}

	void removeLast()
	{
		checkSize();
		removeAt(size_ - 1);
	}

	void clear()
	{
		size_ = allocatedMemory_ = 0;
		delete[] dataPtr_;
		dataPtr_ = nullptr;
	}
};