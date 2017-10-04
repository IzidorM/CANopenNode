#include <inttypes.h>
#include <stdlib.h>


#include "CO_Config.h"
#include "CO_OD_interface.h"
#include "CO_driver.h"
#include "CO_core.h"
#include "mem.h"

#define MALLOC mem_maloc_static
//#define MALLOC malloc

uint32_t COB_IDClientToServer = 0x600;
uint32_t COB_IDServerToClient = 0x580;

uint32_t     COB_IDUsedByTPDO = CO_CAN_ID_TPDO_1;
uint32_t     COB_IDUsedByRPDO = CO_CAN_ID_RPDO_1;
uint8_t      transmissionType = 0x1;
uint16_t     inhibitTime = 0x0102;
uint8_t      compatibilityEntry = 0x3;
uint16_t     eventTimer = 0x0405;
uint8_t      SYNCStartValue = 0x6;
uint32_t     mappedObjects[8];


struct CO_core CO_core;

int32_t make_rpdo_od_entries(void *OD)
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
        
        INIT_OD_ENTRY_RECORDp(OD_1400, 0x1400, &OD_1400_subelements[0], 2, "", "");
        err = con_od_add_element_to_od(&OD, OD_1400);
        if (err)
        {
                return err;
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
        INIT_OD_ENTRY_RECORDp(OD_1600, 0x1600, &OD_1600_subelements[0],
                             0, "", "");

        err = con_od_add_element_to_od(&OD, OD_1600);
        if (err)
        {
                return err;
        }

        return err;
}

//int32_t make_tpdo_od_entries(void *OD)
//{
//        int32_t err;
//
//        // Make a OD record
//        struct con_od_record_entry OD_1800_subelements[7];
//        struct con_od_record_entry *OD_1800_subelements = MALLOC(sizeof(struct con_od_record_entry) * 7);
//        INIT_RECORD_SUBENTRY(&OD_1800_subelements[0],
//                             OD_TYPE_UINT32,
//                             0x8D, //CO_ODA_WRITEABLE | CO_ODA_READABLE,
//                             &COB_IDUsedByTPDO, "", "");
//
//        INIT_RECORD_SUBENTRY(&OD_1800_subelements[1],
//                             OD_TYPE_UINT8,
//                             0x0D, //CO_ODA_WRITEABLE | CO_ODA_READABLE,
//                             &transmissionType, "", "");
//
//        INIT_RECORD_SUBENTRY(&OD_1800_subelements[2],
//                             OD_TYPE_UINT16,
//                             0x8D, //CO_ODA_WRITEABLE | CO_ODA_READABLE,
//                             &inhibitTime, "", "");
//
//        INIT_RECORD_SUBENTRY(&OD_1800_subelements[3],
//                             OD_TYPE_UINT32,
//                             0x0D, //CO_ODA_WRITEABLE | CO_ODA_READABLE,
//                             &compatibilityEntry, "", "");
//
//        INIT_RECORD_SUBENTRY(&OD_1800_subelements[4],
//                             OD_TYPE_UINT16,
//                             0x8D, //CO_ODA_WRITEABLE | CO_ODA_READABLE,
//                             &eventTimer, "", "");
//
//        INIT_RECORD_SUBENTRY(&OD_1800_subelements[5],
//                             OD_TYPE_UINT8,
//                             0x0D,
//                             &SYNCStartValue, "", "");
//        
////        struct con_od_list_node_record OD_1800;
//        struct con_od_list_node_record *OD_1800 = MALLOC(sizeof(struct con_od_list_node_record));
//        INIT_OD_ENTRY_RECORDp(OD_1800, 0x1800, &OD_1800_subelements[0], 6, "", "");
//        err = con_od_add_element_to_od(&OD, OD_1800);
//        if (err)
//        {
//                return err;
//        }
//
////        struct con_od_record_entry OD_1A00_subelements[8];
//        struct con_od_record_entry *OD_1A00_subelements = MALLOC(sizeof(struct con_od_record_entry) * 8);
//
//        uint32_t i;
//        for (i = 0; 8 > i; i++)
//        {
//                INIT_RECORD_SUBENTRY(&OD_1A00_subelements[i],
//                                     OD_TYPE_UINT32,
//                                     0x8D, //CO_ODA_WRITEABLE | CO_ODA_READABLE,
//                                     &mappedObjects[i], "", "");
//        }
//
////        struct con_od_list_node_record OD_1A00;
//        struct con_od_list_node_record *OD_1A00 = malloc(sizeof(struct con_od_list_node_record));
//        INIT_OD_ENTRY_RECORDp(OD_1A00, 0x1A00, &OD_1A00_subelements[0], 1, "", "");
//        // Add OD elements together in a OD linked list
//        err = con_od_add_element_to_od(&OD, OD_1A00);
//        if (err)
//        {
//                return err;
//        }
//
//        return err;
//}

int32_t make_sdo_od_entries(void *OD)
{
        struct con_od_record_entry *OD_1200_subelements;
        OD_1200_subelements = MALLOC(sizeof(struct con_od_record_entry)*2);
        INIT_RECORD_SUBENTRY(&OD_1200_subelements[0],
                             OD_TYPE_UINT32,
                             CO_ODA_WRITEABLE | CO_ODA_READABLE,
                             &COB_IDClientToServer, "", "");

        INIT_RECORD_SUBENTRY(&OD_1200_subelements[1],
                             OD_TYPE_UINT32,
                             CO_ODA_WRITEABLE | CO_ODA_READABLE,
                             &COB_IDServerToClient, "", "");

        struct con_od_list_node_record *OD_1200;
        OD_1200 = MALLOC(sizeof(struct con_od_list_node_record));
        INIT_OD_ENTRY_RECORDp(OD_1200, 0x1200, &OD_1200_subelements[0], 2, "", "");

        int32_t err = con_od_add_element_to_od(&OD, OD_1200);
        if (err)
        {
                return err;
        }

        return 0;
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

        // init OD
        struct con_od_list_node_var *device_type;
        device_type = MALLOC(sizeof(struct con_od_list_node_var));
        INIT_OD_ENTRY_VAR(device_type, 0x1000, OD_TYPE_UINT32, CO_ODA_READABLE,
                          &device_type_data, NULL, NULL);

        CO_core.OD = device_type;
//        err = con_od_add_element_to_od(OD, device_type);

        
        // init NMT
        err = make_nmt_od_entries(CO_core.OD);
        if (err)
        {
                return NULL;
        }

        err = CO_NMT_init(
                &CO_core.NMT,
                node_id,
                CO_core.OD,
                nmt_state_changed_callback,
                CO_CAN_ID_NMT_SERVICE,
                can_driver);
        if (err)
        {
                return NULL;
        }
        err = (int32_t) co_driver_register_callback(CO_CAN_ID_NMT_SERVICE,
                                                    CO_NMT_receive,
                                                    &CO_core.NMT);
        if (err)
        {
                return NULL;
        }


        // init SDO
        err = make_sdo_od_entries(CO_core.OD);
        if (err)
        {
                return NULL;
        }

                err = CO_SDO_init(
                &CO_core.SDO,
                CO_CAN_ID_RSDO, //0x600,
                CO_CAN_ID_TSDO, //0x580,
                0x1200, //OD_H1200_SDO_SERVER_PARAM,
                CO_core.OD,
                node_id,
                can_driver);
        if (err)
        {
                return NULL;
        }

        err = (int32_t) co_driver_register_callback(CO_CAN_ID_RSDO + node_id,
                                                    CO_SDO_receive,
                                                    &CO_core.SDO);
        if (err)
        {
                return NULL;
        }

        // init SYNC
        err = CO_SYNC_init(
                &CO_core.SYNC,
                CO_CAN_ID_SYNC,
                node_id,
                can_driver);
        if (err)
        {
                return NULL;
        }


        // init TPDO
        err = CO_TPDO_init(
                &CO_core.TPDO,
                CO_core.OD,
                CO_CAN_ID_TPDO_1,
                node_id,
                can_driver);
        if (err)
        {
                return NULL;
        }

        
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
        if (r)
        {
                return r;
        }
        r = CO_SDO_process(
                &CO_core.SDO,
                true,
                100,
                1000,
                NULL);
        if (r)
        {
                return r;
        }

        // handle SYNC
        enum co_sync_types sync_received = CO_NO_SYNC;
        uint8_t sync_counter = 0x0;
        r = CO_SYNC_process(&CO_core.SYNC, &sync_received,
                            &sync_counter, 0);

        if (r)
        {
                return r;
        }

        // handle TPDOs
        if (CO_NMT_OPERATIONAL == CO_NMT_getInternalState(&CO_core.NMT))
        {
                if (CO_NO_SYNC == sync_received)
                {
                        CO_TPDO_process(&CO_core.TPDO, CO_TPDO_NO_SYNC, 0, 10);
                }
                else if (CO_SYNC_WITHOUT_COUNTER == sync_received)
                {
                        CO_TPDO_process(&CO_core.TPDO, CO_TPDO_SYNC_WITHOUT_COUNTER,
                                        0, 10);
                }
                else if (CO_SYNC_WITH_COUNTER == sync_received)
                {
                        CO_TPDO_process(&CO_core.TPDO, CO_TPDO_SYNC_WITH_COUNTER,
                                        sync_counter, 10);
                }
                else
                {
                        //TODO: Return error
                }
        }

        
        return r;
}
