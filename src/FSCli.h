#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "FSTree.h"


class FileSystemCLI 
{
private:
    FileSystemTree fs;
    void printHelp() 
    {
        std::cout << "Available commands:\n"
                  << "  ls                     : List contents\n"
                  << "  pwd                    : Print current path\n"
                  << "  cd <dir>               : Change directory (use '..' to go up)\n"
                  << "  mkdir <dir>            : Create a directory\n"
                  << "  touch <file>           : Create a file\n"
                  << "  rm <name>              : Remove file or directory\n"
                  << "  write <file> <content> : Overwrite file content\n"
                  << "  append <file> <text>   : Append text to file\n"
                  << "  cat <file>             : Display file content\n"
                  << "  save <filename>        : Save file system state to file\n"
                  << "  exit                   : Exit the CLI\n";
    }
public:
    void run() 
    {
        std::string line;
        std::cout << "Type 'help' for available commands.\n";
        while (true) 
        {
            std::cout << fs.getCurrentPath() << " $ ";
            if (!std::getline(std::cin, line))
                break;
            if (line.empty())
                continue;
            std::istringstream iss(line);
            std::string command;
            iss >> command;
            if (command == "exit")
                break;
            else if (command == "help")
                printHelp();
            else if (command == "ls")
                fs.listContents();
            else if (command == "pwd")
                std::cout << fs.getCurrentPath() << std::endl;
            else if (command == "cd") {
                std::string dir;
                iss >> dir;
                if (dir.empty())
                    std::cout << "Usage: cd <directory>\n";
                else
                    fs.changeDirectory(dir);
            }
            else if (command == "mkdir") 
            {
                std::string dirname;
                iss >> dirname;
                if (dirname.empty())
                    std::cout << "Usage: mkdir <directory>\n";
                else
                    fs.addDirectory(dirname);
            }
            else if (command == "touch") 
            {
                std::string filename;
                iss >> filename;
                if (filename.empty())
                    std::cout << "Usage: touch <filename>\n";
                else
                    fs.addFile(filename);
            }
            else if (command == "rm") 
            {
                std::string name;
                iss >> name;
                if (name.empty())
                    std::cout << "Usage: rm <name>\n";
                else
                    fs.deleteEntry(name);
            }
            else if (command == "write") 
            {
                std::string filename;
                iss >> filename;
                if (filename.empty()) 
                {
                    std::cout << "Usage: write <filename> <content>\n";
                    continue;
                }
                std::string content;
                std::getline(iss, content);
                if (!content.empty() && content[0] == ' ')
                    content.erase(0, 1);
                fs.writeFile(filename, content);
            }
            else if (command == "append") 
            {
                std::string filename;
                iss >> filename;
                if (filename.empty()) 
                {
                    std::cout << "Usage: append <filename> <text>\n";
                    continue;
                }
                std::string text;
                std::getline(iss, text);
                if (!text.empty() && text[0] == ' ')
                    text.erase(0, 1);
                fs.appendToFile(filename, text);
            }
            else if (command == "cat") 
            {
                std::string filename;
                iss >> filename;
                if (filename.empty())
                    std::cout << "Usage: cat <filename>\n";
                else
                    fs.readFile(filename);
            }
            else if (command == "save") 
            {
                std::string filename;
                iss >> filename;
                if (filename.empty())
                    std::cout << "Usage: save <filename>\n";
                else
                    fs.saveToFile(filename);
            }
            else
                std::cout << "Unknown command: " << command << "\n";
        }
        std::cout << "Exiting FS CLI.\n";
    }
};