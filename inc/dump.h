#pragma once

#include "cons.h"

typedef struct
{
    console *c;
    bool *thFin;
} dumpArgs;

dumpArgs *dumpArgsCreate(console *c, bool *status);
void dumpArgsDestroy(dumpArgs *a);

void dumpThread(void *arg);
void delThread(void *arg);
