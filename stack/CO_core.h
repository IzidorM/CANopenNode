#ifndef CO_CORE_H
#define CO_CORE_H

#include "CO_NMT.h"
#include "CO_SDO.h"
#include "CO_TPDO.h"
#include "CO_RPDO.h"

struct CO_core {
        void *CAN_DRIVER;
        void *OD;
        CO_NMT_t NMT;
        CO_SDO_t SDO;
        CO_TPDO_t TPDO;
        CO_RPDO_t RPDO;
};


struct CO_core *CO_init(uint32_t node_id,
                        void *can_driver,
                        void (*nmt_state_changed_callback)
                        (CO_NMT_internalState_t previous_state,
                         CO_NMT_internalState_t requested_state));
int32_t CO_process(void);
#endif
