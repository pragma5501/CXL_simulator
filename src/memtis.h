#ifndef __MEMTIS_H__
#define __MEMTIS_H__


#include "mem.h"


#define MAX_BIN 16
#define MAX_BIN_IDX ((MAX_BIN) - 1)

struct access_histo {
    u16* bin;
    
};

struct memtis {
    u8 T_hot;
    u8 T_warm;
    u8 T_cold;

    struct access_histo* PA_histo; // Page Access Histogram
    struct access_histo* EA_histo; // Emulated base page Histogram
};


#endif