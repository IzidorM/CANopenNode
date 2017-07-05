#include <inttypes.h>
#include <stdlib.h>


#include "CO_Config.h"
#include "CO_OD_interface.h"
#include "CO_driver.h"

#include "CO_NMT.h"
#include "CO_RPDO.h"

#define MALLOC malloc

uint32_t     COB_IDUsedByRPDO = CO_CAN_ID_TPDO_1;
uint8_t      transmissionType = 0x1;
uint16_t     inhibitTime = 0x0102;
uint8_t      compatibilityEntry = 0x3;
uint16_t     eventTimer = 0x0405;
uint8_t      SYNCStartValue = 0x6;
uint32_t     mappedObjects[8];

struct CO_core {
        void *CAN_DRIVER;
        void *OD;
        CO_NMT_t NMT;
        CO_RPDO_t RPDO;
};

struct CO_core CO_core;

void *make_rpdo_od_entries(void *OD)
{
        int32_t err;

        struct con_od_record_entry *OD_1400_subelements;
        OD_1400_subelements = MALLOC(sizeof(struct con_od_record_entry) * 2);
        
        INIT_RECORD_SUBENTRY(&OD_1400_subelements[0],
                             OD_TYPE_UINT32,
                             0x8D, //CO_ODA_WRITEABLE | CO_ODA_READABLE,
                             &COB_IDUsedByRPDO, "", "");

        INIT_RECORD_SUBENTRY(&OD_1400_subelements[1],
                             OD_TYPE_UINT32,
                             0x0D, //CO_ODA_WRITEABLE | CO_ODA_READABLE,
                             &transmissionType, "", "");

        struct con_od_list_node_record *OD_1400;
        OD_1400 = MALLOC(sizeof(struct con_od_list_node_record));
        
        INIT_OD_ENTRY_RECORD(OD_1400, 0x1400, &OD_1400_subelements[0], 2, "", "");
        err = con_od_add_element_to_od(&OD, OD_1400);
        if (err)
        {
                return NULL;
        }

        struct con_od_record_entry *OD_1600_subelements;
        OD_1600_subelements = MALLOC(sizeof(struct con_od_record_entry) * 8);
        uint32_t i;
        for (i = 0; 8 > i; i++)
        {
                INIT_RECORD_SUBENTRY(&OD_1600_subelements[i],
                                     OD_TYPE_UINT32,
                                     0x8D, //CO_ODA_WRITEABLE | CO_ODA_READABLE,
                                     &mappedObjects[i], "", "");
        }

        struct con_od_list_node_record *OD_1600;
        OD_1600 = MALLOC(sizeof(struct con_od_list_node_record));
        INIT_OD_ENTRY_RECORD(OD_1600, 0x1600, &OD_1600_subelements[0],
                             0, "", "");

        err = con_od_add_element_to_od(&OD, OD_1600);
        if (err)
        {
                return NULL;
        }

        return OD;
}

uint32_t nmt_startup = 0; // no automatic startup capabilities
int32_t make_nmt_od_entries(void *OD)
{
        int32_t err;
        struct con_od_list_node_var *nmt_automatic_startup;
        nmt_automatic_startup = MALLOC(sizeof(struct con_od_list_node_var));
        

        INIT_OD_ENTRY_VAR(nmt_automatic_startup, 0x1f80, OD_TYPE_UINT32, CO_ODA_WRITEABLE,
                          &nmt_startup, NULL, NULL);
        
        err = con_od_add_element_to_od(OD, nmt_automatic_startup);
        return err;
}

uint32_t device_type_data = 0;
struct CO_core *CO_init(uint32_t node_id,
                        void *can_driver,
                        void (*nmt_state_changed_callback)
                        (CO_NMT_internalState_t previous_state,
                         CO_NMT_internalState_t requested_state))
{
        int32_t err;
        void *OD = NULL;

        // CAN interface
        void *CANdev = (void *) 0x1234;

        // init OD
        struct con_od_list_node_var *device_type;
        device_type = MALLOC(sizeof(struct con_od_list_node_var));
        INIT_OD_ENTRY_VAR(device_type, 0x1000, OD_TYPE_UINT32, CO_ODA_READABLE,
                          &device_type_data, NULL, NULL);

        OD = &device_type;
//        err = con_od_add_element_to_od(OD, device_type);

        
        // init NMT
        err = make_nmt_od_entries(OD);
        if (err)
        {
                return NULL;
        }

        err = CO_NMT_init(
                &CO_core.NMT,
                node_id,
                OD,
                nmt_state_changed_callback,
                CO_CAN_ID_NMT_SERVICE,
                can_driver);
        if (err)
        {
                return NULL;
        }
        err = (int32_t) co_driver_register_callback(CO_CAN_ID_NMT_SERVICE + node_id,
                                                    CO_NMT_receive,
                                                    &CO_core.NMT);
        if (err)
        {
                return NULL;
        }


        // init SDO

        // init RPDO
//        CO_core.OD = make_rpdo_od_entries(OD);
//        if (NULL == CO_core.OD)
//        {
//                return NULL;
//        }
//
//        err = CO_RPDO_init(
//                &CO_core.RPDO,
//                CO_core.OD,
//                node_id,
//                0, // restriction flags
//                0x1400,
//                0x1600,
//                can_driver);
//        if (err)
//        {
//                return NULL;
//        }
//
//        err = (int32_t) co_driver_register_callback(CO_CAN_ID_RPDO_1 + node_id,
//                                    CO_RPDO_receive,
//                                    &CO_core.RPDO);
//        if (err)
//        {
//                return NULL;
//        }

        return &CO_core;
}

int32_t CO_process(void)
{

        int32_t r;
        r = CO_NMT_process(&CO_core.NMT);
//        CO_RPDO_process(&CO_core.RPDO, true);
        
        return r;
}
