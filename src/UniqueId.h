#pragma once

#include <cstdint>


class UniqueIDGenerator 
{
private:
    static uint64_t nextID;
public:
    static uint64_t getID() 
    {
        return nextID++;
    }
};

uint64_t UniqueIDGenerator::nextID = 1;