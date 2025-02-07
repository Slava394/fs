#pragma once

#include <functional>


template <typename K>
class ISet
{
public:
    virtual void insert(const K& key) = 0;
    virtual bool remove(const K& key) = 0;
    virtual bool contains(const K& key) = 0;

    virtual void inorder(std::function<void(const K&, const std::nullptr_t&)> func) const = 0;
    
    virtual ~ISet() = default;
};