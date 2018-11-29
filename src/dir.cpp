#include <string>
#include <fstream>
#include <vector>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "dir.h"
#include "cons.h"

extern console infoCons;
extern font *sysFont;

dirList::dirList(const std::string& _path)
{
    path = _path;
    d = opendir(path.c_str());

    while((ent = readdir(d)))
        item.push_back(ent->d_name);

    closedir(d);
}

void dirList::reassign(const std::string& _path)
{
    path = _path;

    d = opendir(path.c_str());

    item.clear();

    while((ent = readdir(d)))
        item.push_back(ent->d_name);

    closedir(d);
}

void dirList::rescan()
{
    item.clear();
    d = opendir(path.c_str());

    while((ent = readdir(d)))
        item.push_back(ent->d_name);

    closedir(d);
}

std::string dirList::getItem(int index)
{
    return item[index];
}

bool dirList::isDir(int index)
{
    std::string fullPath = path + item[index];
    struct stat s;
    return stat(fullPath.c_str(), &s) == 0 && S_ISDIR(s.st_mode);
}

unsigned dirList::getCount()
{
    return item.size();
}

void copyFile(const std::string& from, const std::string& to)
{
    std::fstream f(from, std::ios::in | std::ios::binary);
    std::fstream t(to, std::ios::out | std::ios::binary);

    if(!f.is_open() || !t.is_open())
    {
        f.close();
        t.close();
        return;
    }

    f.seekg(0, f.end);
    size_t fileSize = f.tellg();
    f.seekg(0, f.beg);

    uint8_t *buff = new uint8_t[0x80000];
    for(unsigned i = 0; i < fileSize; )
    {
        f.read((char *)buff, 0x80000);
        t.write((char *)buff, f.gcount());

        i += f.gcount();
    }

    delete[] buff;

    f.close();
    t.close();
}

void copyDirToDir(const std::string& from, const std::string& to)
{
    dirList list(from);
    char countStr[8];
    sprintf(countStr, "%u", list.getCount());
    infoCons.out(sysFont, std::string("\"" + from + "\" opened. " + countStr + " items found."));

    for(unsigned i = 0; i < list.getCount(); i++)
    {
        if(list.isDir(i))
        {
            std::string newFrom = from + list.getItem(i) + "/";
            std::string newTo   = to + list.getItem(i);
            mkdir(newTo.c_str(), 0777);
            newTo += "/";

            copyDirToDir(newFrom, newTo);
            infoCons.out(sysFont, "Finished with \"" + list.getItem(i) + "\"!");
        }
        else
        {
            std::string fullFrom = from + list.getItem(i);
            std::string fullTo   = to   + list.getItem(i);
            infoCons.out(sysFont, "Copying \"" + list.getItem(i) + "\"...");
            copyFile(fullFrom, fullTo);
        }
    }
}

void delDir(const std::string& path)
{
    dirList list(path);
    for(unsigned i = 0; i < list.getCount(); i++)
    {
        if(list.isDir(i))
        {
            std::string newPath = path + list.getItem(i) + "/";
            delDir(newPath);

            std::string delPath = path + list.getItem(i);
            rmdir(delPath.c_str());
        }
        else
        {
            std::string delPath = path + list.getItem(i);
            std::remove(delPath.c_str());
        }
    }

    rmdir(path.c_str());
}
