#pragma once

#include <functional>


template <typename K, typename T>
class IDictionary 
{
public:
    virtual void insert(const K& key, const T& value) = 0;
    virtual bool remove(const K& key) = 0;
    virtual T* find(const K& key) = 0;

    virtual void inorder(std::function<void(const K&, const T&)> func) const = 0;
    
    virtual ~IDictionary() = default;
};