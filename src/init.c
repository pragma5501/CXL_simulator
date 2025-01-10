#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mem.h"

struct mem_req* mreq;
struct local_dram* my_local_dram;
struct cxl_switch* my_cxl_switch;

void* safe_malloc (size_t n)
{
        void *p = malloc(n);
        if (p == NULL) {
                fprintf(stderr, "[ERR] : malloc error");
                abort();
        }
        return p;
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