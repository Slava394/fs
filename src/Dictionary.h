#pragma once

#include <initializer_list>

#include "IDictionary.h"
#include "AVLTree.h"


template <typename K, typename T>
class Dictionary : public IDictionary<K, T>
{
private:
    AVLTree<K, T> tree;

public:
    Dictionary() {}

    Dictionary(std::initializer_list<std::pair<K, T>> initList)
    {
        for (const auto& elem : initList)
        {
            tree.insert(elem.first, elem.second);
        }
    }

    Dictionary(const Dictionary& other) : tree(other.tree) {}

    Dictionary& operator=(const Dictionary& other)
    {
        if (this != &other)
        {
            tree = other.tree;
        }
        return *this;
    }

    Dictionary(Dictionary&& other) noexcept : tree(std::move(other.tree)) {}

    Dictionary& operator=(Dictionary&& other) noexcept
    {
        if (this != &other)
        {
            tree = std::move(other.tree);
        }
        return *this;
    }

    void insert(const K& key, const T& value) override 
    {
        tree.insert(key, value);
    }

    bool remove(const K& key) override 
    {
        return tree.remove(key);
    }

    T* find(const K& key) override 
    {
        return tree.find(key).value;
    }

    void inorder(std::function<void(const K&, const T&)> func) const
    {
        tree.inorder(func);
    }
};