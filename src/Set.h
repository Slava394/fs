#pragma once

#include <initializer_list>

#include "ISet.h"
#include "AVLTree.h"


template <typename K>
class Set : public ISet<K>
{
private:
    AVLTree<K, std::nullptr_t> tree;

public:
    Set() {}

    Set(std::initializer_list<K> initList)
    {
        for (const auto& elem : initList)
        {
            insert(elem);
        }
    }

    Set(const Set& other) : tree(other.tree) {}

    Set& operator=(const Set& other)
    {
        if (this != &other)
        {
            tree = other.tree;
        }
        return *this;
    }

    Set(Set&& other) noexcept : tree(std::move(other.tree)) {}

    Set& operator=(Set&& other) noexcept
    {
        if (this != &other)
        {
            tree = std::move(other.tree);
        }
        return *this;
    }

    void insert(const K& key) override 
    {
        tree.insert(key, nullptr);
    }

    bool remove(const K& key) override 
    {
        return tree.remove(key);
    }

    bool contains(const K& key) override 
    {
        if (tree.find(key).key)
        {
            return true;
        }
        return false;
    }

    void inorder(std::function<void(const K&, const std::nullptr_t&)> func) const
    {
        tree.inorder(func);
    }
};