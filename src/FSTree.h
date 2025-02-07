#pragma once

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "PoolAllocator.h"
#include "UniqueId.h"
#include "Sequence.h"
#include "Dictionary.h"


class Node 
{
public:
    uint64_t id;
    std::string name;
    bool isDirectory;
    size_t fileSize;
    Node* parent;
    Dictionary<std::string, Node*> children;
    Node(const std::string& name, bool isDirectory, Node* parent = nullptr)
        : id(UniqueIDGenerator::getID()), name(name), isDirectory(isDirectory), fileSize(0), parent(parent) {}

    ~Node() 
    {
        children.inorder([](const std::string& key, Node* child) {
            delete child;
        });
    }

    void listContents() 
    {
        children.inorder([](const std::string& key, Node* child) {
            std::cout << (child->isDirectory ? "[D] " : "[F] ") << key << " (ID: " << child->id << ")\n";
        });
    }
};

class FileSystemTree 
{
private:
    Node* root;
    Node* currentDirectory;
    PoolAllocator pool;
    Dictionary<uint64_t, Sequence<void*>> fileIndex;

    void freeFileBlocks(uint64_t fileId) 
    {
        Sequence<void*>* blocks = fileIndex.find(fileId);
        if (blocks) 
        {
            for (size_t i = 0; i < blocks->getSize(); ++i)
                pool.freeBlock((*blocks)[i]);
            fileIndex.remove(fileId);
        }
    }

    void deleteNodeRecursive(Node* node) 
    {
        node->children.inorder([&](const std::string& key, Node* child) {
            deleteNodeRecursive(child);
        });
        if (!node->isDirectory)
            freeFileBlocks(node->id);
        delete node;
    }

    void saveNode(std::ostream& out, Node* node, int level) 
    {
        std::string indent(level * 2, ' ');
        out << indent << (node->isDirectory ? "[D] " : "[F] ") << node->name << " (ID: " << node->id;
        if (!node->isDirectory)
            out << ", fileSize: " << node->fileSize;
        out << ")\n";
        node->children.inorder([&](const std::string& key, Node* child) {
            saveNode(out, child, level + 1);
        });
    }

    static Node* findNodeById(Node* node, uint64_t id) 
    {
        if (node->id == id)
            return node;
        Node* found = nullptr;
        node->children.inorder([&](const std::string& key, Node* child) {
            if (!found)
                found = findNodeById(child, id);
        });
        return found;
    }

public:
    FileSystemTree() 
    {
        root = new Node("root", true);
        currentDirectory = root;
    }

    void listContents() 
    {
        std::cout << "Contents of " << currentDirectory->name << ":\n";
        currentDirectory->listContents();
    }

    void addFile(const std::string& name) 
    {
        if (currentDirectory->children.find(name) != nullptr) 
        {
            std::cout << "Error: File or directory already exists.\n";
            return;
        }
        Node* newNode = new Node(name, false, currentDirectory);
        currentDirectory->children.insert(name, newNode);
        void* fileBlock = pool.allocateBlock();
        if (!fileBlock) 
        {
            std::cout << "Error: Memory allocation failed.\n";
            delete newNode;
            currentDirectory->children.remove(name);
            return;
        }
        std::memset(fileBlock, 0, BLOCK_SIZE);
        Sequence<void*> blockSequence;
        blockSequence.append(fileBlock);
        fileIndex.insert(newNode->id, blockSequence);
        newNode->fileSize = 0;
    }

    void addDirectory(const std::string& name) 
    {
        if (currentDirectory->children.find(name) != nullptr) 
        {
            std::cout << "Error: File or directory already exists.\n";
            return;
        }
        Node* newNode = new Node(name, true, currentDirectory);
        currentDirectory->children.insert(name, newNode);
    }

    void changeDirectory(const std::string& name) 
    {
        if (name == "..") 
        {
            if (currentDirectory->parent)
                currentDirectory = currentDirectory->parent;
        } 
        else 
        {
            Node** targetPtr = currentDirectory->children.find(name);
            Node* target = (targetPtr ? *targetPtr : nullptr);
            if (target != nullptr && target->isDirectory)
                currentDirectory = target;
            else
                std::cout << "Error: Directory not found.\n";
        }
    }

    void printCurrentPath() 
    {
        Sequence<std::string> path;
        Node* temp = currentDirectory;
        while (temp) 
        {
            path.append(temp->name);
            temp = temp->parent;
        }
        for (int i = path.getSize() - 1; i >= 0; --i) 
        {
            std::cout << path[i];
            if (i > 0)
                std::cout << "/";
        }
        std::cout << std::endl;
    }

    const std::string getCurrentPath() 
    {
        Sequence<std::string> path;
        Node* temp = currentDirectory;
        while (temp) 
        {
            path.append(temp->name);
            temp = temp->parent;
        }
        std::string strPath;
        for (int i = path.getSize() - 1; i >= 0; --i) 
        {
            strPath += path[i];
            if (i > 0)
                strPath += "/";
        }
        return strPath;
    }

    void writeFile(const std::string& name, const std::string& newContent) 
    {
        Node** nodePtr = currentDirectory->children.find(name);
        Node* node = (nodePtr ? *nodePtr : nullptr);
        if (!node) 
        {
            std::cout << "Error: File " << name << " not found.\n";
            return;
        }
        if (node->isDirectory) 
        {
            std::cout << "Error: " << name << " is a directory.\n";
            return;
        }
        freeFileBlocks(node->id);
        size_t contentLength = newContent.size();
        size_t blocksNeeded = (contentLength + BLOCK_SIZE - 1) / BLOCK_SIZE;
        if (blocksNeeded == 0)
            blocksNeeded = 1;
        size_t offset = 0;
        Sequence<void*> newBlockSequence;
        for (size_t i = 0; i < blocksNeeded; ++i) 
        {
            void* block = pool.allocateBlock();
            if (!block) 
            {
                std::cout << "Error: Memory allocation failed during write.\n";
                return;
            }
            std::memset(block, 0, BLOCK_SIZE);
            size_t copySize = std::min(BLOCK_SIZE, contentLength - offset);
            std::memcpy(block, newContent.data() + offset, copySize);
            offset += copySize;
            newBlockSequence.append(block);
        }
        fileIndex.insert(node->id, newBlockSequence);
        node->fileSize = contentLength;
    }

    void appendToFile(const std::string& name, const std::string& textToAppend) 
    {
        Node** nodePtr = currentDirectory->children.find(name);
        Node* node = (nodePtr ? *nodePtr : nullptr);
        if (!node) 
        {
            std::cout << "Error: File " << name << " not found.\n";
            return;
        }
        if (node->isDirectory) 
        {
            std::cout << "Error: " << name << " is a directory.\n";
            return;
        }
        size_t appendLength = textToAppend.size();
        size_t currentSize = node->fileSize;
        size_t lastBlockUsedBytes = currentSize % BLOCK_SIZE;
        if (lastBlockUsedBytes == 0 && currentSize > 0)
            lastBlockUsedBytes = BLOCK_SIZE;
        Sequence<void*>* blockSequence = fileIndex.find(node->id);
        if (!blockSequence) 
        {
            std::cout << "Error: File representation not found.\n";
            return;
        }
        size_t offset = 0;
        if (blockSequence->getSize() > 0) 
        {
            void* lastBlock = (*blockSequence)[blockSequence->getSize() - 1];
            size_t freeSpace = (lastBlockUsedBytes < BLOCK_SIZE) ? (BLOCK_SIZE - lastBlockUsedBytes) : 0;
            if (freeSpace > 0) 
            {
                size_t writeSize = std::min(freeSpace, appendLength);
                std::memcpy(static_cast<char*>(lastBlock) + lastBlockUsedBytes, textToAppend.data(), writeSize);
                offset += writeSize;
            }
        }
        while (offset < appendLength) 
        {
            void* block = pool.allocateBlock();
            if (!block) 
            {
                std::cout << "Error: Memory allocation failed during append.\n";
                return;
            }
            std::memset(block, 0, BLOCK_SIZE);
            size_t writeSize = std::min(BLOCK_SIZE, appendLength - offset);
            std::memcpy(block, textToAppend.data() + offset, writeSize);
            offset += writeSize;
            blockSequence->append(block);
        }
        node->fileSize += appendLength;
    }

    void readFile(const std::string& name) 
    {
        Node** nodePtr = currentDirectory->children.find(name);
        Node* node = (nodePtr ? *nodePtr : nullptr);
        if (!node) 
        {
            std::cout << "Error: File " << name << " not found.\n";
            return;
        }
        if (node->isDirectory) 
        {
            std::cout << "Error: " << name << " is a directory.\n";
            return;
        }
        Sequence<void*>* blockSequence = fileIndex.find(node->id);
        if (!blockSequence) 
        {
            std::cout << "Error: File representation not found.\n";
            return;
        }
        std::string result;
        result.reserve(node->fileSize);
        size_t remaining = node->fileSize;
        for (size_t i = 0; i < blockSequence->getSize() && remaining > 0; ++i) 
        {
            char* blockData = static_cast<char*>((*blockSequence)[i]);
            size_t toRead = std::min(remaining, BLOCK_SIZE);
            result.append(blockData, toRead);
            remaining -= toRead;
        }
        std::cout << "Content of " << name << " (" << node->fileSize << " bytes):\n" << result << std::endl;
    }

    void deleteEntry(const std::string& name) 
    {
        Node** nodePtr = currentDirectory->children.find(name);
        Node* node = (nodePtr ? *nodePtr : nullptr);
        if (node) 
        {
            deleteNodeRecursive(node);
            currentDirectory->children.remove(name);
        } 
        else 
        {
            std::cout << "Error: No such file or directory.\n";
        }
    }

    void saveToFile(const std::string& filename) 
    {
        std::ofstream out(filename, std::ios::binary);
        if (!out) 
        {
            std::cerr << "Error: Could not open file " << filename << " for writing.\n";
            return;
        }
        const char header[] = "MYFS_V1";
        out.write(header, sizeof(header));
        std::ostringstream treeStream;
        treeStream << "FileSystemTree Structure:\n";
        saveNode(treeStream, root, 0);
        std::string treeStr = treeStream.str();
        size_t treeStrLength = treeStr.size();
        out.write(reinterpret_cast<const char*>(&treeStrLength), sizeof(treeStrLength));
        out.write(treeStr.c_str(), treeStrLength);
        Sequence<std::pair<uint64_t, Sequence<void*>>> fileEntries;
        fileIndex.inorder([&fileEntries](const uint64_t& key, const Sequence<void*>& seq) 
        {
            fileEntries.append(std::make_pair(key, seq));
        });
        size_t numFiles = fileEntries.getSize();
        out.write(reinterpret_cast<const char*>(&numFiles), sizeof(numFiles));
        for (size_t i = 0; i < fileEntries.getSize(); ++i) 
        {
            uint64_t fileId = fileEntries[i].first;
            out.write(reinterpret_cast<const char*>(&fileId), sizeof(fileId));
            Node* node = findNodeById(root, fileId);
            size_t fileSize = node ? node->fileSize : 0;
            out.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
            const Sequence<void*>& blocks = fileEntries[i].second;
            size_t numBlocks = blocks.getSize();
            out.write(reinterpret_cast<const char*>(&numBlocks), sizeof(numBlocks));
            for (size_t j = 0; j < numBlocks; ++j) 
            {
                void* blockPtr = blocks[j];
                out.write(reinterpret_cast<const char*>(blockPtr), BLOCK_SIZE);
            }
        }
        std::string bitmapStr = pool.getBitmapString();
        size_t bitmapLength = bitmapStr.size();
        out.write(reinterpret_cast<const char*>(&bitmapLength), sizeof(bitmapLength));
        out.write(bitmapStr.c_str(), bitmapLength);
        out.close();
        std::cout << "File system state saved to " << filename << "\n";
    }

    ~FileSystemTree() 
    {
        deleteNodeRecursive(root);
    }
};
