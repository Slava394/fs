#pragma once

#include <bitset>
#include <string>
#include <cstdlib>
#include <cstring>


constexpr size_t BLOCK_SIZE = 4096;              // Размер одного блока (например, 4К)
constexpr size_t TOTAL_SIZE = 1024ull * 1024 * 1024; // 1 ГБ памяти
constexpr size_t BLOCK_COUNT = TOTAL_SIZE / BLOCK_SIZE;

class PoolAllocator 
{
private:
    void* memory;
    std::bitset<BLOCK_COUNT> bitmap;
public:
    PoolAllocator() 
    {
        memory = std::aligned_alloc(BLOCK_SIZE, TOTAL_SIZE);
        if (!memory) 
        {
            throw std::bad_alloc();
        }
        std::memset(memory, 0, TOTAL_SIZE);
        bitmap.reset();
    }

    ~PoolAllocator() 
    {
        std::free(memory);
    }

    void* allocateBlock() 
    {
        for (size_t i = 0; i < BLOCK_COUNT; i++) 
        {
            if (!bitmap[i]) 
            {
                bitmap.set(i);
                return static_cast<char*>(memory) + (i * BLOCK_SIZE);
            }
        }
        return nullptr; 
    }

    void freeBlock(void* ptr) 
    {
        size_t index = (static_cast<char*>(ptr) - static_cast<char*>(memory)) / BLOCK_SIZE;
        if (index < BLOCK_COUNT) 
        {
            bitmap.reset(index);
        }
    }

    std::string getBitmapString() const 
    {
        return bitmap.to_string();
    }
};