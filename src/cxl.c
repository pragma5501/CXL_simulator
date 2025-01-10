#include "mem.h"

int HDM_decoder__cxl_device (struct cxl_device* cxl_dev)
{
        u64 PFN = GET_PFN(mreq->addr) - NUM_LOCAL_DRAM_FRAME;
        PFN = PFN / NUM_CXL_DEVS;

        switch(mreq->rw) {
        case LOAD:
                cxl_dev->traff_load++;
                break;
        case STORE:
                cxl_dev->traff_store++;
                break;
        }
        return 0;
}

int HDM_decoder__cxl_switch ()
{
        u64 PFN = GET_PFN(mreq->addr) - NUM_LOCAL_DRAM_FRAME;
        u16 idx_cxl_dev = PFN2CXLDEV(PFN);
        HDM_decoder__cxl_device(my_cxl_switch->devs[idx_cxl_dev]);
        return 0;
}

int mode__cxl_switch ()
{
        HDM_decoder__cxl_switch();
        return 0;
}



