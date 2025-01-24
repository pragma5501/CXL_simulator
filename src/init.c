#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mem.h"
#include "memtis.h"

struct mem_req* mreq;
struct local_dram* my_local_dram;
struct cxl_switch* my_cxl_switch;
struct memtis* my_memtis;
struct PTE* my_PTE;


void* safe_malloc (size_t n)
{
        void *p = malloc(n);
        if (p == NULL) {
                fprintf(stderr, "[ERR] : malloc error");
                abort();
        }
        return p;
}

void insert_free_list (struct free_list* fl, struct page* page)
{
        if (fl->head == NULL) {
                fl->head = page;
                fl->tail = page;
        }
        fl->tail->next = page;
        fl->tail = page;
}

struct page* pop_free_list (struct free_list* fl)
{
        if (fl->head == NULL) {
                fprintf(stderr, "Free list capacity error: pop\n");
                abort();
        }

        struct page* ret;
        ret = fl->head;

        fl->head  = fl->head->next;
        if (fl->head == NULL) fl->tail == NULL;

        ret->next = NULL;
}


static int init_free_list (struct free_list** fl, u64 start_PFN, u64 end_PFN)
{
        *fl = (struct free_list*)safe_malloc(sizeof(struct free_list));
        (*fl)->head = NULL;
        (*fl)->tail = NULL;

        struct page* page;
        for (int PFN = start_PFN; PFN <= end_PFN; PFN++) {
                page = (struct page*)safe_malloc(sizeof(struct page));
                page->PFN = PFN;
                insert_free_list(*fl, page);
        }
}

static int destroy_free_list (struct free_list* fl)
{
        struct page* page;
        while (fl->head != NULL) {
                page = fl->head;
                fl->head = fl->head->next;
                free(page);     
        }

        free(fl);
}



int init_local_dram ()
{
        my_local_dram        = (struct local_dram*)safe_malloc(sizeof(struct local_dram));
        my_local_dram->frame = (u16*)safe_malloc(sizeof(u16) * NUM_LOCAL_DRAM_FRAME);
        my_local_dram->traff_load  = 0;
        my_local_dram->traff_store = 0;

        return 0;
}


void destroy_local_dram () 
{
        free(my_local_dram->frame);
        free(my_local_dram);
}

int init_cxl ()
{
        my_cxl_switch       = (struct cxl_switch*)safe_malloc(sizeof(struct cxl_switch));
        my_cxl_switch->devs = (struct cxl_device**)safe_malloc(sizeof(struct cxl_device*) * NUM_CXL_DEVS);
        
        u16* tmp_frame;
        for (int i = 0; i < NUM_CXL_DEVS; i++) {
                my_cxl_switch->devs[i] = (struct cxl_device*)safe_malloc(sizeof(struct cxl_device));
                
                tmp_frame = (u16*)safe_malloc(sizeof(u16) * NUM_CXL_DEV_FRAME);
                my_cxl_switch->devs[i]->frame = tmp_frame;
                my_cxl_switch->devs[i]->traff_load  = 0;
                my_cxl_switch->devs[i]->traff_store = 0;
        }

        return 0;
}

void destroy_cxl ()
{
        for (int i = 0; i < NUM_CXL_DEVS; i++) {
                free(my_cxl_switch->devs[i]->frame);
                free(my_cxl_switch->devs[i]);
        }
        free(my_cxl_switch->devs);
        free(my_cxl_switch);
}

int init_mreq ()
{
        mreq = (struct mem_req*)safe_malloc(sizeof(struct mem_req));
        return 0;
}

void destroy_mreq ()
{
        free(mreq);
}

static int init_access_histo (struct access_histo **ah) 
{
        *ah = (struct access_histo*)safe_malloc(sizeof(struct access_histo));
        (*ah)->bin = (u16*)safe_malloc(sizeof(u16) * MAX_BIN);
        return 0;
}

static void destroy_access_histo (struct access_histo *ah)
{
        free(ah->bin);
        free(ah);
}

int init_PTE ()
{
        my_PTE        = (struct PTE*)safe_malloc(sizeof(struct PTE));
        my_PTE->pages = (struct page**)safe_malloc(sizeof(struct page*));
        for (int i = 0; i < NUM_TOTAL_MEM_FRAME; i++) {
                my_PTE->pages[i] = (struct page*)safe_malloc(sizeof(struct page));
        }


        u64 d_off = NUM_LOCAL_DRAM_FRAME;

        init_free_list(my_PTE->ldram_fl, 0, d_off - 1);
        init_free_list(my_PTE->cxl_fl, d_off, 
                        d_off + NUM_CXL_DEV_FRAME * NUM_CXL_DEVS);

}

void destroy_PTE ()
{
        for (int i = 0; i < NUM_TOTAL_MEM_FRAME; i++) {
                free(my_PTE->pages[i]);
        }
        destroy_free_list(my_PTE->ldram_fl);
        destroy_free_list(my_PTE->cxl_fl);
}

int init_memtis ()
{
        my_memtis = (struct memtis*)safe_malloc(sizeof(struct memtis));
        my_memtis->T_hot  = 0;
        my_memtis->T_warm = 0;
        my_memtis->T_cold = 0;

        init_access_histo(&my_memtis->PA_histo);
        init_access_histo(&my_memtis->EA_histo);

}

void destroy_memtis ()
{
        destroy_access_histo(my_memtis->PA_histo);
        destroy_access_histo(my_memtis->EA_histo);
        free(my_memtis);
}