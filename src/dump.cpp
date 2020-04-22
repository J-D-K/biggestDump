#include <switch.h>
#include <sys/stat.h>

#include "dump.h"
#include "cons.h"
#include "dir.h"

dumpArgs *dumpArgsCreate(console *c, bool *status)
{
    dumpArgs *ret = new dumpArgs;
    ret->c = c;
    ret->thFin = status;
    return ret;
}

void dumpArgsDestroy(dumpArgs *a)
{
    delete a;
}

void dumpThread(void *arg)
{
    dumpArgs *a = (dumpArgs *)arg;
    console *c = a->c;

    FsFileSystem sys;
    if(R_SUCCEEDED(fsOpenBisFileSystem(&sys, FsBisPartitionId_System, "")))
    {
        fsdevMountDevice("sys", sys);
        c->out("Beginning Update + Firmware dump.");
        c->nl();

        //Del first for safety
        delDir("sdmc:/Update/", false, c);
        mkdir("sdmc:/Update", 777);

        //Copy whole contents folder
        copyDirToDir("sys:/Contents/", "sdmc:/Update/", c);

        fsdevUnmountDevice("sys");
        c->out("Firmware Update dump finished.");
        c->nl();
    }
    else
    {
        c->out("*FAILED TO OPEN SYSTEM PARTITION!*");
        c->nl();
    }

    *a->thFin = true;
}

void delThread(void *arg)
{
    dumpArgs *a = (dumpArgs *)arg;
    console *c = a->c;

    FsFileSystem sys;
    if(R_SUCCEEDED(fsOpenBisFileSystem(&sys, FsBisPartitionId_System, "")))
    {
        fsdevMountDevice("sys", sys);
        c->out("Deleting update data from NAND.");
        c->nl();

        delDir("sys:/Contents/placehld/", true, c);
        fsdevUnmountDevice("sys");
        c->out("Update deleted from NAND!");
        c->nl();
    }
    *a->thFin = true;
}
