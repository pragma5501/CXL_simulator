#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "type.h"
#include "mem.h"


int init_env () 
{
        init_mreq();
        init_local_dram();
        init_cxl();
        return 0;
}

void destroy_env ()
{
        destroy_cxl();
        destroy_local_dram();
        destroy_mreq();
}

int parse (char* text) 
{
        char rw;
        char addr[50];
        u64 time;
        if (sscanf(text, "%c %s %lu", &rw, addr, &time) == 3) {
        }

        mreq->rw   = (rw == 'R') ? LOAD : STORE;
        mreq->addr = (u64)strtoul(addr, NULL, 0);
        mreq->time = (u64)time;
}


int mux_req ()
{
        u64 PFN = GET_PFN(mreq->addr);

        if (PFN < THRES_MUX_PFN) {
                mode__local_dram();
        } else {
                mode__cxl_switch();
        }

        return 0;
}

int read_request (FILE* fp) 
{
        char buf[1024];

        while (fgets(buf, sizeof(buf), fp)) {
                parse(buf);
                mux_req();
        }
}

void display_result()
{
        u32 access_time = 0;

        printf("__LOCAL_DRAM__\n");
        printf("LOAD  : %d\n", my_local_dram->traff_load);
        printf("STORE : %d\n", my_local_dram->traff_store);
        
        access_time += my_local_dram->traff_load * 105;
        access_time += my_local_dram->traff_store * 105;
        
        for (int i = 0; i < NUM_CXL_DEVS; i++) {
                printf("__CXL_DEVICE%d__\n", i);
                printf("LOAD  : %d\n", my_cxl_switch->devs[i]->traff_load);
                printf("STORE : %d\n", my_cxl_switch->devs[i]->traff_store);

                access_time += my_cxl_switch->devs[i]->traff_load * 250;
                access_time += my_cxl_switch->devs[i]->traff_store * 250;
        }

        
        printf("RESULT\n");
        printf("Total Access Time : %dns\n", access_time);
}

int main (int argv, char *args) 
{
        init_env();

        FILE *fp = fopen("./trace.txt", "r");
        if( fp == NULL ) {
                fprintf(stderr, "[ERR] : File not opened\n");
                return 0;
        }

        read_request(fp);
        fclose(fp);

        display_result();
        destroy_env();
        return 0;
}