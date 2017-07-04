#ifndef CO_CORE_H
#define CO_CORE_H

#include "CO_NMT.h"

struct CO_core;

struct CO_core *CO_init(uint32_t node_id,
                        void *can_driver,
                        void (*nmt_state_changed_callback)
                        (CO_NMT_internalState_t previous_state,
                         CO_NMT_internalState_t requested_state));
int32_t CO_process(void);
#endif
