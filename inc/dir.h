#pragma once

#include <string>
#include <vector>
#include <dirent.h>

#include "cons.h"

class dirList
{
    public:
        dirList(const std::string& _path);
        void reassign(const std::string& _path);
        void rescan();

        std::string getItem(int index);
        bool isDir(int index);
        unsigned getCount();

    private:
        DIR *d;
        struct dirent *ent;
        std::string path;
        std::vector<std::string> item;
};

void copyFile(const std::string& from, const std::string& to, console *c);
void copyDirToDir(const std::string& from, const std::string &to, console *c);
void delDir(const std::string& path, bool output, console *c);
