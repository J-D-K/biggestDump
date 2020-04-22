#include <string>
#include <cstdio>
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

void copyFile(const std::string& from, const std::string& to, console *c)
{
    FILE *f = fopen(from.c_str(), "rb");
    FILE *t = fopen(to.c_str(), "wb");

    if(f == NULL)
    {
        c->out(std::string("    *ERROR* Opening #" + from + "# for reading!"));
        c->nl();
        return;
    }
    else if(t == NULL)
    {
        c->out(std::string("    *ERROR* Opening #" + to + "# for writing!"));
        c->nl();
        return;
    }

    fseek(f, 0, SEEK_END);
    size_t fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char tmp[32];
    sprintf(tmp, "%luKB.", fileSize / 1024);
    c->out(" *Size*: " + std::string(tmp));

    uint8_t *buff = new uint8_t[0x80000];
    for(unsigned i = 0; i < fileSize; )
    {
        size_t tPos = ftell(t);

        size_t fcount = fread(buff, 1, 0x80000, f);
        fwrite(buff, 1, fcount, t);

        size_t writeSize = ftell(t) - tPos;

        if(writeSize < fcount)
        {
            char errorMess[128];
            sprintf(errorMess, "    *ERROR:* Read/Write mismatch. %lu/%lu.", fcount, writeSize);
            c->nl();
            c->out(errorMess);
            c->nl();
        }

        i += fcount;
    }

    delete[] buff;
    fclose(f);
    fclose(t);
}

void copyDirToDir(const std::string& from, const std::string& to, console *c)
{
    dirList list(from);
    char countStr[12];

    unsigned dirCount = 0;
    if(list.getCount() > 0)
        dirCount = list.getCount() - 1;

    sprintf(countStr, "%u", dirCount);
    c->out(std::string("#" + from + "# opened. ^" + countStr + "^ items found."));
    c->nl();

    for(unsigned i = 0; i < list.getCount(); i++)
    {
        if(list.isDir(i))
        {
            std::string newFrom = from + list.getItem(i) + "/";
            std::string newTo   = to + list.getItem(i);
            mkdir(newTo.c_str(), 0777);
            newTo += "/";

            copyDirToDir(newFrom, newTo, c);
            c->out("Finished with #" + from + list.getItem(i) + "#!");
            c->nl();
        }
        else
        {
            std::string fullFrom = from + list.getItem(i);
            std::string fullTo   = to   + list.getItem(i);
            char outLine[256];
            sprintf(outLine, "Copying %%%s%% (%03d/%03d)... ", list.getItem(i).c_str(), i, list.getCount() - 1);

            c->out(outLine);
            copyFile(fullFrom, fullTo, c);
            c->out("    ^Done^!");
            c->nl();
        }
    }
}

void delDir(const std::string& path, bool output, console *c)
{
    dirList list(path);
    for(unsigned i = 0; i < list.getCount(); i++)
    {
        if(list.isDir(i))
        {
            std::string newPath = path + list.getItem(i) + "/";
            delDir(newPath, false, c);

            std::string delPath = path + list.getItem(i);
            rmdir(delPath.c_str());
        }
        else
        {
            std::string delPath = path + list.getItem(i);
            if(output)
                c->out("Deleting %" + list.getItem(i) + "%...");
            if(std::remove(delPath.c_str()) == 0)
            {
                if(output)
                    c->out("    ^Done^!");
            }
            else
            {
                if(output)
                    c->out("    *Failed*.");
            }

            if(output)
                c->nl();
        }
    }

    rmdir(path.c_str());
}
