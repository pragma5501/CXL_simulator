#ifndef __MEM_H__
#define __MEM_H__

#include "type.h"

#define PAGE_SIZE 4 * (KB)
#define HUGE_PAGE_SIZE 2 * (MB)

#define NUM_CXL_DEVS 4
#define NUM_CXL_DEV_FRAME ((u64)5 * (GB)) / (PAGE_SIZE)

#define NUM_LOCAL_DRAM_FRAME ((u64)2 * (GB)) / (PAGE_SIZE) 
#define THRES_MUX_PFN ((u64)2 * (GB)) / (PAGE_SIZE)

#define NUM_TOTAL_MEM_FRAME (NUM_LOCAL_DRAM_FRAME) + ((NUM_CXL_DEV_FRAME) * 4)

struct mem_req {
    char rw;
    u64 addr;
    u64 time;
};

struct local_dram {
    u16* frame;
    u16 traff_load;
    u16 traff_store;
};

struct cxl_device {
    u16* frame;
    u16 traff_load;
    u16 traff_store;
};

struct cxl_switch {
    struct cxl_device** devs;
    
};

// Page Entry
struct page {
    u64 PFN;
    u32 access_count;
    u32 hotness_factor;
    struct page* next;
};

struct free_list {
    struct page* head;
    struct page* tail;
};

struct PTE {
    struct page** pages;
    struct free_list* ldram_fl;
    struct free_list* cxl_fl;
};

#define GET_PFN(addr)    (addr >> 12)
#define GET_OFFSET(addr) (addr & 0xfff)
#define PFN2CXLDEV(PFN)  (PFN % NUM_CXL_DEVS)

extern struct mem_req* mreq;
extern struct local_dram* my_local_dram;
extern struct cxl_switch* my_cxl_switch;
extern struct memtis* my_memtis;
extern struct PTE* my_PTE;


// init.c
void* safe_malloc (size_t n);
int init_local_dram ();
void destroy_local_dram ();
int init_cxl ();
void destroy_cxl ();
int init_mreq ();
void destroy_mreq ();

// cxl.c
int mode__cxl_switch();

// dram.c
int mode__local_dram();

#endif