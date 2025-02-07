#pragma once

#include <functional>


template <typename K, typename T>
class AVLTree 
{
private:
    struct Node 
    {
    public:
        K key;
        T value;
        Node* left;
        Node* right;
        int height;
        
        Node(const K& k, const T& v) 
            : key(k), value(v), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root;

    int height(Node* node) const 
    {
        return node ? node->height : 0;
    }

    int getBalance(Node* node) const 
    {
        return node ? height(node->left) - height(node->right) : 0;
    }

    int max(const int a, const int b) const
    {
        return (a > b) ? a : b;
    }

    Node* rotateRight(Node* y) 
    {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;

        return x; 
    }

    Node* rotateLeft(Node* x) 
    {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;

        return y;
    }

    Node* insert(Node* node, const K& key, const T& value) 
    {
        if (!node)
        {
            return new Node(key, value);
        }

        if (key < node->key)
        {
            node->left = insert(node->left, key, value);
        }
        else if (key > node->key)
        {
            node->right = insert(node->right, key, value);
        }
        else 
        {
            node->value = value;
            return node;
        }

        node->height = 1 + max(height(node->left), height(node->right));

        int balance = getBalance(node);

        if (balance > 1 && key < node->left->key)
        {
            return rotateRight(node);
        }

        if (balance < -1 && key > node->right->key)
        {
            return rotateLeft(node);
        }

        if (balance > 1 && key > node->left->key) 
        {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1 && key < node->right->key) 
        {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    Node* minValueNode(Node* node) 
    {
        Node* current = node;
        while (current && current->left != nullptr)
        {
            current = current->left;
        }
        return current;
    }

    Node* remove(Node* node, const K& key, bool& removed) 
    {
        if (!node)
        {
            return node;
        }

        if (key < node->key)
        {
            node->left = remove(node->left, key, removed);
        }
        else if (key > node->key)
        {
            node->right = remove(node->right, key, removed);
        }
        else 
        {
            removed = true;
            if (!node->left || !node->right) 
            {
                Node* temp = node->left ? node->left : node->right;
                if (!temp) 
                {
                    temp = node;
                    node = nullptr;
                } 
                else 
                {
                    *node = *temp;
                }
                delete temp;
            } 
            else 
            {
                Node* temp = minValueNode(node->right);
                node->key = temp->key;
                node->value = temp->value;
                node->right = remove(node->right, temp->key, removed);
            }
        }

        if (!node)
        {
            return node;
        }

        node->height = 1 + max(height(node->left), height(node->right));

        int balance = getBalance(node);

        if (balance > 1 && getBalance(node->left) >= 0)
        {
            return rotateRight(node);
        }

        if (balance > 1 && getBalance(node->left) < 0) 
        {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1 && getBalance(node->right) <= 0)
        {
            return rotateLeft(node);
        }

        if (balance < -1 && getBalance(node->right) > 0) 
        {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    Node* find(Node* node, const K& key) const 
    {
        if (!node)
        {
            return nullptr;
        }
        if (key == node->key)
        {
            return node;
        }
        else if (key < node->key)
        {
            return find(node->left, key);
        }
        else
        {
            return find(node->right, key);
        }
    }

    void clear(Node* node) 
    {
        if (node) 
        {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    Node* clone(Node* node) const
    {
        if (!node)
        {
            return nullptr;
        }
        Node* newNode = new Node(node->key, node->value);
        newNode->height = node->height;
        newNode->left = clone(node->left);
        newNode->right = clone(node->right);
        return newNode;
    }

    void inorderHelper(Node* node, std::function<void(const K&, const T&)> func) const
    {
        if (node)
        {
            inorderHelper(node->left, func);   
            func(node->key, node->value);      
            inorderHelper(node->right, func);  
        }
    }

public:
    struct ReturnNode
    {
    public:
        K* key;
        T* value;
    };


    AVLTree() : root(nullptr) {}

    AVLTree(const AVLTree& other) : root(clone(other.root)) {}

    AVLTree& operator=(const AVLTree& other)
    {
        if (this != &other)
        {
            clear(root);
            root = clone(other.root);
        }
        return *this;
    }

    AVLTree(AVLTree&& other) noexcept : root(other.root)
    {
        other.root = nullptr;
    }

    AVLTree& operator=(AVLTree&& other) noexcept
    {
        if (this != &other)
        {
            clear(root);
            root = other.root;
            other.root = nullptr;
        }
        return *this;
    }

    ~AVLTree() 
    {
        clear(root);
    }

    void insert(const K& key, const T& value) 
    {
        root = insert(root, key, value);
    }

    bool remove(const K& key) 
    {
        bool removed = false;
        root = remove(root, key, removed);
        return removed;
    }

    ReturnNode find(const K& key) 
    {
        Node* node = find(root, key);
        ReturnNode result;
        result.key = node ? &node->key : nullptr;
        result.value = node ? &node->value : nullptr;
        return result;
    }

    void inorder(std::function<void(const K&, const T&)> func) const
    {
        inorderHelper(root, func);
    }
};