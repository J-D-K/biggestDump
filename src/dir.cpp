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

    if(!f.is_open())
    {
        infoCons.out(sysFont, std::string("    *ERROR* Opening #" + from + "# for reading!"));
        infoCons.nl();
        return;
    }
    else if(!t.is_open())
    {
        infoCons.out(sysFont, std::string("    *ERROR* Opening #" + to + "# for writing!"));
        infoCons.nl();
        return;
    }

    f.seekg(0, f.end);
    size_t fileSize = f.tellg();
    f.seekg(0, f.beg);

    char tmp[32];
    sprintf(tmp, "%luKB.", fileSize / 1024);
    infoCons.out(sysFont, " *Size*: " + std::string(tmp));

    uint8_t *buff = new uint8_t[0x80000];
    for(unsigned i = 0; i < fileSize; )
    {
        size_t tPos = t.tellp();

        f.read((char *)buff, 0x80000);
        t.write((char *)buff, f.gcount());

        size_t writeSize = (size_t)t.tellp() - tPos;

        if(writeSize < (unsigned)f.gcount())
        {
            char errorMess[128];
            sprintf(errorMess, "    *ERROR:* Read/Write mismatch. %lu/%lu.", f.gcount(), writeSize);
            infoCons.nl();
            infoCons.out(sysFont, errorMess);
            infoCons.nl();
        }

        i += f.gcount();
    }

    delete[] buff;

    f.close();
    t.close();
}

void copyDirToDir(const std::string& from, const std::string& to)
{
    dirList list(from);
    char countStr[12];

    unsigned dirCount = 0;
    if(list.getCount() > 0)
        dirCount = list.getCount() - 1;

    sprintf(countStr, "%u", dirCount);
    infoCons.out(sysFont, std::string("#" + from + "# opened. ^" + countStr + "^ items found."));
    infoCons.nl();

    for(unsigned i = 0; i < list.getCount(); i++)
    {
        if(list.isDir(i))
        {
            std::string newFrom = from + list.getItem(i) + "/";
            std::string newTo   = to + list.getItem(i);
            mkdir(newTo.c_str(), 0777);
            newTo += "/";

            copyDirToDir(newFrom, newTo);
            infoCons.out(sysFont, "Finished with #" + from + list.getItem(i) + "#!");
            infoCons.nl();
        }
        else
        {
            std::string fullFrom = from + list.getItem(i);
            std::string fullTo   = to   + list.getItem(i);
            char outLine[256];
            sprintf(outLine, "Copying %%%s%% (%03d/%03d)... ", list.getItem(i).c_str(), i, list.getCount() - 1);

            infoCons.out(sysFont, outLine);
            copyFile(fullFrom, fullTo);
            infoCons.out(sysFont, "    ^Done^!");
            infoCons.nl();
        }
    }
}

void delDir(const std::string& path, bool output)
{
    dirList list(path);
    for(unsigned i = 0; i < list.getCount(); i++)
    {
        if(list.isDir(i))
        {
            std::string newPath = path + list.getItem(i) + "/";
            delDir(newPath, false);

            std::string delPath = path + list.getItem(i);
            rmdir(delPath.c_str());
        }
        else
        {
            std::string delPath = path + list.getItem(i);
            if(output)
                infoCons.out(sysFont, "Deleting %" + list.getItem(i) + "%...");
            if(std::remove(delPath.c_str()) == 0)
            {
                if(output)
                    infoCons.out(sysFont, "    ^Done^!");
            }
            else
            {
                if(output)
                    infoCons.out(sysFont, "    *Failed*.");
            }

            if(output)
                infoCons.nl();
        }
    }

    rmdir(path.c_str());
}
