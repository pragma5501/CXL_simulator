#include "mem.h"

int mode__local_dram ()
{
        u64 PFN = GET_PFN(mreq->addr);
        switch(mreq->rw) {
        case LOAD:
                my_local_dram->traff_load++;
                break;
        case STORE:
                my_local_dram->traff_store++;
                break;
        }

        return 0;
}