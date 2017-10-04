/*
 * CANopen Process Data Object.
 *
 * @file        CO_PDO.c
 * @ingroup     CO_PDO
 * @author      Janez Paternoster
 * @copyright   2004 - 2013 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free and open source software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Following clarification and special exception to the GNU General Public
 * License is included to the distribution terms of CANopenNode:
 *
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library. Thus, the terms and
 * conditions of the GNU General Public License cover the whole combination.
 *
 * As a special exception, the copyright holders of this library give
 * you permission to link this library with independent modules to
 * produce an executable, regardless of the license terms of these
 * independent modules, and to copy and distribute the resulting
 * executable under terms of your choice, provided that you also meet,
 * for each linked independent module, the terms and conditions of the
 * license of that module. An independent module is a module which is
 * not derived from or based on this library. If you modify this
 * library, you may extend this exception to your version of the
 * library, but you are not obliged to do so. If you do not wish
 * to do so, delete this exception statement from your version.
 */

#include "CO_driver.h"
#include "CO_OD_interface.h"
#include "CO_PDO.h"
#include "CO_TPDO.h"
#include <string.h>


/*
 * Configure TPDO Communication parameter.
 *
 * Function is called from commuincation reset or when parameter changes.
 *
 * Function configures following variable from CO_TPDO_t: _valid_. It also
 * configures CAN tx buffer. If configuration fails, emergency message is send
 * and device is not able to enter NMT operational.
 *
 * @param TPDO TPDO object.
 * @param COB_IDUsedByTPDO _TPDO communication parameter_, _COB-ID for PDO_ variable
 * from Object dictionary (index 0x1400+, subindex 1).
 * @param syncFlag Indicate, if TPDO is synchronous.
 */
//static void CO_TPDOconfigCom(CO_TPDO_t* TPDO, uint32_t COB_IDUsedByTPDO, uint8_t syncFlag){
//    uint16_t ID;
//
//    ID = (uint16_t)COB_IDUsedByTPDO;
//
//    /* is TPDO used? */
//    if((COB_IDUsedByTPDO & 0xBFFFF800L) == 0 && TPDO->dataLength && ID){
//        /* is used default COB-ID? */
//        if(ID == TPDO->defaultCOB_ID) ID += TPDO->nodeId;
//        TPDO->valid = true;
//    }
//    else{
//        ID = 0;
//        TPDO->valid = false;
//    }
//
//    TPDO->CANtxBuff = CO_CANtxBufferInit(
//            TPDO->CANdevTx,            /* CAN device */
//            TPDO->CANdevTxIdx,         /* index of specific buffer inside CAN module */
//            ID,                        /* CAN identifier */
//            0,                         /* rtr */
//            TPDO->dataLength,          /* number of data bytes */
//            syncFlag);                 /* synchronous message flag bit */
//
//    if(TPDO->CANtxBuff == 0){
//        TPDO->valid = false;
//    }
//}

/*
 * Configure TPDO Mapping parameter.
 *
 * Function is called from communication reset or when parameter changes.
 *
 * Function configures following variables from CO_TPDO_t: _dataLength_,
 * _mapPointer_ and _sendIfCOSFlags_.
 *
 * @param TPDO TPDO object.
 * @param noOfMappedObjects Number of mapped object (from OD).
 *
 * @return 0 on success, otherwise SDO abort code.
 */
static uint32_t CO_TPDOconfigMap(CO_TPDO_t* TPDO, uint8_t noOfMappedObjects){
    int16_t i;
    uint8_t length = 0;
    uint32_t ret = 0;

    for(i=0;noOfMappedObjects>i; i++){
        int16_t j;
        uint8_t* pData;
        uint8_t prevLength = length;
        uint8_t MBvar;
        uint32_t map = TPDO->TPDOMapPar.mapped_object_param[i];

        uint32_t (*dummy)(void*);
    
        /* function do much checking of errors in map */
        ret = CO_PDOfindMap(
                TPDO->OD,
                map,
                1,
                &pData,
                &length,
//                &TPDO->sendIfCOSFlags,
                &MBvar,
                &dummy);
        if(ret){
            // add right error code;
            return -1;
        }

        /* write PDO data pointers */
#ifdef CO_BIG_ENDIAN
        if(MBvar){
            for(j=length-1; j>=prevLength; j--)
                TPDO->mapPointer[j] = pData++;
        }
        else{
            for(j=prevLength; j<length; j++)
                TPDO->mapPointer[j] = pData++;
        }
#else
        for(j=prevLength; j<length; j++){
            TPDO->TPDOMapPar.mapPointer[j] = pData++;
        }
#endif

    }

    TPDO->dataLength = length;

    return ret;
}


/*
 * Function for accessing _TPDO communication parameter_ (index 0x1800+) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
//static CO_SDO_abortCode_t CO_ODF_TPDOcom(CO_ODF_arg_t *ODF_arg){
//    CO_TPDO_t *TPDO;
//
//    TPDO = (CO_TPDO_t*) ODF_arg->object;
//
//    if(ODF_arg->subIndex == 4) return CO_SDO_AB_SUB_UNKNOWN;  /* Sub-index does not exist. */
//
//    /* Reading Object Dictionary variable */
//    if(ODF_arg->reading){
//        if(ODF_arg->subIndex == 1){   /* COB_ID */
//            uint32_t *value = (uint32_t*) ODF_arg->data;
//
//            /* if default COB ID is used, write default value here */
//            if(((*value)&0xFFFF) == TPDO->defaultCOB_ID && TPDO->defaultCOB_ID)
//                *value += TPDO->nodeId;
//
//            /* If PDO is not valid, set bit 31 */
//            if(!TPDO->valid) *value |= 0x80000000L;
//        }
//        return CO_SDO_AB_NONE;
//    }
//
//    /* Writing Object Dictionary variable */
//    if(TPDO->restrictionFlags & 0x04)
//        return CO_SDO_AB_READONLY;  /* Attempt to write a read only object. */
//    if(*TPDO->operatingState == CO_NMT_OPERATIONAL && (TPDO->restrictionFlags & 0x01))
//        return CO_SDO_AB_DATA_DEV_STATE;   /* Data cannot be transferred or stored to the application because of the present device state. */
//
//    if(ODF_arg->subIndex == 1){   /* COB_ID */
//        uint32_t *value = (uint32_t*) ODF_arg->data;
//
//        /* bits 11...29 must be zero */
//        if(*value & 0x3FFF8000L)
//            return CO_SDO_AB_INVALID_VALUE;  /* Invalid value for parameter (download only). */
//
//        /* if default COB-ID is being written, write defaultCOB_ID without nodeId */
//        if(((*value)&0xFFFF) == (TPDO->defaultCOB_ID + TPDO->nodeId)){
//            *value &= 0xC0000000L;
//            *value += TPDO->defaultCOB_ID;
//        }
//
//        /* if PDO is valid, bits 0..29 can not be changed */
//        if(TPDO->valid && ((*value ^ TPDO->TPDOCommPar->COB_IDUsedByTPDO) & 0x3FFFFFFFL))
//            return CO_SDO_AB_INVALID_VALUE;  /* Invalid value for parameter (download only). */
//
//        /* configure TPDO */
//        CO_TPDOconfigCom(TPDO, *value, TPDO->CANtxBuff->syncFlag);
//        TPDO->syncCounter = 255;
//    }
//    else if(ODF_arg->subIndex == 2){   /* Transmission_type */
//        uint8_t *value = (uint8_t*) ODF_arg->data;
//
//        /* values from 241...253 are not valid */
//        if(*value >= 241 && *value <= 253)
//            return CO_SDO_AB_INVALID_VALUE;  /* Invalid value for parameter (download only). */
//        TPDO->CANtxBuff->syncFlag = (*value <= 240) ? 1 : 0;
//        TPDO->syncCounter = 255;
//    }
//    else if(ODF_arg->subIndex == 3){   /* Inhibit_Time */
//        /* if PDO is valid, value can not be changed */
//        if(TPDO->valid)
//            return CO_SDO_AB_INVALID_VALUE;  /* Invalid value for parameter (download only). */
//
//        TPDO->inhibitTimer = 0;
//    }
//    else if(ODF_arg->subIndex == 5){   /* Event_Timer */
//        uint16_t *value = (uint16_t*) ODF_arg->data;
//
//        TPDO->eventTimer = ((uint32_t) *value) * 1000;
//    }
//    else if(ODF_arg->subIndex == 6){   /* SYNC start value */
//        uint8_t *value = (uint8_t*) ODF_arg->data;
//
//        /* if PDO is valid, value can not be changed */
//        if(TPDO->valid)
//            return CO_SDO_AB_INVALID_VALUE;  /* Invalid value for parameter (download only). */
//
//        /* values from 240...255 are not valid */
//        if(*value > 240)
//            return CO_SDO_AB_INVALID_VALUE;  /* Invalid value for parameter (download only). */
//    }
//
//    return CO_SDO_AB_NONE;
//}


/*
 * Function for accessing _TPDO mapping parameter_ (index 0x1A00+) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
//static CO_SDO_abortCode_t CO_ODF_TPDOmap(CO_ODF_arg_t *ODF_arg){
//    CO_TPDO_t *TPDO;
//
//    TPDO = (CO_TPDO_t*) ODF_arg->object;
//
//    /* Reading Object Dictionary variable */
//    if(ODF_arg->reading){
//        uint8_t *value = (uint8_t*) ODF_arg->data;
//
//        if(ODF_arg->subIndex == 0){
//            /* If there is error in mapping, dataLength is 0, so numberOfMappedObjects is 0. */
//            if(!TPDO->dataLength) *value = 0;
//        }
//        return CO_SDO_AB_NONE;
//    }
//
//    /* Writing Object Dictionary variable */
//    if(TPDO->restrictionFlags & 0x08)
//        return CO_SDO_AB_READONLY;  /* Attempt to write a read only object. */
//    if(*TPDO->operatingState == CO_NMT_OPERATIONAL && (TPDO->restrictionFlags & 0x02))
//        return CO_SDO_AB_DATA_DEV_STATE;   /* Data cannot be transferred or stored to the application because of the present device state. */
//    if(TPDO->valid)
//        return CO_SDO_AB_INVALID_VALUE;  /* Invalid value for parameter (download only). */
//
//    /* numberOfMappedObjects */
//    if(ODF_arg->subIndex == 0){
//        uint8_t *value = (uint8_t*) ODF_arg->data;
//
//        if(*value > 8)
//            return CO_SDO_AB_VALUE_HIGH;  /* Value of parameter written too high. */
//
//        /* configure mapping */
//        return CO_TPDOconfigMap(TPDO, *value);
//    }
//
//    /* mappedObject */
//    else{
//        uint32_t *value = (uint32_t*) ODF_arg->data;
//        uint8_t* pData;
//        uint8_t length = 0;
//        uint8_t dummy = 0;
//        uint8_t MBvar;
//
//        if(TPDO->dataLength)
//            return CO_SDO_AB_INVALID_VALUE;  /* Invalid value for parameter (download only). */
//
//        /* verify if mapping is correct */
//        return CO_PDOfindMap(
//                TPDO->SDO,
//               *value,
//                1,
//               &pData,
//               &length,
//               &dummy,
//               &MBvar);
//    }
//
//    return CO_SDO_AB_NONE;
//}
//

CO_ReturnError_t CO_TPDO_set_transmission_type(
                CO_TPDO_t *TPDO,
                uint8_t transmission_type)
{
        if ((CO_TT_SYNCHRONOUS_EVERY_240_CYCLE >= transmission_type) ||
            (CO_TT_EVENT_DRIVEN_MANUFACTURER_SPECIFIC <= transmission_type))
        {
                TPDO->TPDOCommPar.transmissionType = transmission_type;
                return CO_ERROR_NO;
        }
        return CO_ERROR_ILLEGAL_ARGUMENT;
}


CO_ReturnError_t CO_TPDO_set_inhibit_time(
                CO_TPDO_t *TPDO,
                uint16_t inhibit_time)
{
        TPDO->TPDOCommPar.inhibitTime = inhibit_time;
        return CO_ERROR_NO;
}

CO_ReturnError_t CO_TPDO_set_event_time(
                CO_TPDO_t *TPDO,
                uint16_t event_time)
{
        TPDO->TPDOCommPar.eventTimer = event_time;
        return CO_ERROR_NO;
}

CO_ReturnError_t CO_TPDO_set_sync_start_value(
                CO_TPDO_t *TPDO,
                uint16_t sync_start_value)
{
        TPDO->TPDOCommPar.SYNCStartValue = sync_start_value;
        return CO_ERROR_NO;
}


CO_ReturnError_t CO_TPDO_add_mapping(
                CO_TPDO_t *TPDO,
                uint16_t index,
                uint8_t subindex, 
                uint8_t data_size)
{
        uint32_t r = 0;

        if (CO_TPDO_is_enabled(TPDO))
        {
                // TPDOS are enabled, so configuration is not accessible
                return CO_ERROR_TPDO_MAPPING_FAILED;
        }
        
        uint8_t tmp = TPDO->TPDOMapPar.numberOfMappedObjects;
        if (tmp < 8)
        {
                TPDO->TPDOMapPar.mapped_object_param[tmp] = 
                        ((index & 0xffff) << 16) |
                        ((subindex & 0xff) << 8) |
                        (data_size & 0xff);
                r = CO_TPDOconfigMap(TPDO, tmp + 1);
                if (r)
                {
                        TPDO->TPDOMapPar.mapped_object_param[tmp] = 0;
                        return CO_ERROR_TPDO_MAPPING_FAILED;
                }
                TPDO->TPDOMapPar.last_sent_data[tmp] = *TPDO->TPDOMapPar.mapPointer[tmp];
                TPDO->TPDOMapPar.numberOfMappedObjects += 1;
        }
        return CO_ERROR_NO;
}

void CO_TPDO_reset_mappings(CO_TPDO_t *TPDO)
{
        TPDO->TPDOMapPar.numberOfMappedObjects = 0;
        memset(&TPDO->TPDOMapPar, 0, sizeof(CO_TPDOCommPar_t));
}

bool CO_TPDO_is_enabled(CO_TPDO_t *TPDO)
{
        return TPDO->TPDOCommPar.COB_IDUsedByTPDO & (1 << 31);
}


void CO_TPDO_enable(CO_TPDO_t *TPDO)
{
        TPDO->TPDOCommPar.COB_IDUsedByTPDO |= (1 << 31);
}

void CO_TPDO_disable(CO_TPDO_t *TPDO)
{
        TPDO->TPDOCommPar.COB_IDUsedByTPDO &= ~(1 << 31);
}

/******************************************************************************/
CO_ReturnError_t CO_TPDO_init(
        CO_TPDO_t *TPDO,
        void *OD,
        uint32_t COB_IDUsedByTPDO,
        uint8_t nodeId,
        void         *CANdev)
{
    /* verify arguments */
        if(TPDO==NULL || OD==NULL || NULL == CANdev) {
        return CO_ERROR_ILLEGAL_ARGUMENT;
        }

        /* Configure object variables */
        TPDO->nodeId = nodeId;
        TPDO->OD = OD;

        // disable TPDO and set all TPDO config params to 0
        memset(&TPDO->TPDOCommPar, 0, sizeof(CO_TPDOCommPar_t));
        CO_TPDO_reset_mappings(TPDO);



        TPDO->TPDOCommPar.COB_IDUsedByTPDO = COB_IDUsedByTPDO;

        // used only when transmission type synchronous (cycle every Nth sync)
        // and sync packets have support for the overflow counter
        TPDO->waiting_for_right_sync_cnt_value = true;

        TPDO->CANdev = CANdev;

        return CO_ERROR_NO;
}

/******************************************************************************/
bool CO_TPDOisCOS(CO_TPDO_t *TPDO)
{
        uint32_t i;
        for (i = 0; TPDO->dataLength > i; i++)
        {
                if (TPDO->TPDOMapPar.last_sent_data[i] !=
                    *TPDO->TPDOMapPar.mapPointer[i])
                {
                        return true;
                }
        }
                
        return false;
}

//#define TPDO_CALLS_EXTENSION
/******************************************************************************/
int16_t CO_TPDOsend(CO_TPDO_t *TPDO){
    int16_t i;

#ifdef TPDO_CALLS_EXTENSION
    if(TPDO->SDO->ODExtensions){
        /* for each mapped OD, check mapping to see if an OD extension is available, and call it if it is */
        const uint32_t* pMap = &TPDO->TPDOMapPar->mappedObject1;
        CO_SDO_t *pSDO = TPDO->SDO;

        for(i=TPDO->TPDOMapPar->numberOfMappedObjects; i>0; i--){
            uint32_t map = *(pMap++);
            uint16_t index = (uint16_t)(map>>16);
            uint8_t subIndex = (uint8_t)(map>>8);
            uint16_t entryNo = CO_OD_find(pSDO, index);
            CO_OD_extension_t *ext = &pSDO->ODExtensions[entryNo];
            if( ext->pODFunc == NULL) continue;
            CO_ODF_arg_t ODF_arg;
            memset((void*)&ODF_arg, 0, sizeof(CO_ODF_arg_t));
            ODF_arg.reading = true;
            ODF_arg.index = index;
            ODF_arg.subIndex = subIndex;
            ODF_arg.object = ext->object;
            ODF_arg.attribute = CO_OD_getAttribute(pSDO, entryNo, subIndex);
            ODF_arg.pFlags = CO_OD_getFlagsPointer(pSDO, entryNo, subIndex);
            ODF_arg.data = pSDO->OD[entryNo].pData;
            ODF_arg.dataLength = CO_OD_getLength(pSDO, entryNo, subIndex);
            ext->pODFunc(&ODF_arg);
        }
    }
#endif

        CO_CANtx_t TXbuff;      /**< CAN transmit buffer */
        TXbuff.ident = (TPDO->TPDOCommPar.COB_IDUsedByTPDO & 0x7ff) + TPDO->nodeId;
        TXbuff.DLC = TPDO->dataLength;

        i = TPDO->dataLength;
        for (i = 0; TPDO->dataLength > i; i++)
        {
                TXbuff.data[i] = *TPDO->TPDOMapPar.mapPointer[i];
                TPDO->TPDOMapPar.last_sent_data[i] =
                        *TPDO->TPDOMapPar.mapPointer[i];
        }

        return co_driver_send(TPDO->CANdev, &TXbuff);
}

// Check if inhibit timer passed. If passed check COS and set flag (return val) to signal
// TPDO should or shouldn't be sent
static bool CO_TPDO_handle_inhibit_time(CO_TPDO_t *TPDO,
                                        uint32_t time_passed_from_previous_call)
{
        bool send_tpdo = false;
        TPDO->inhibitTimer_us_per_cnt += time_passed_from_previous_call;

        // did inhibit timer run out?
        if (TPDO->inhibitTimer_us_per_cnt >= TPDO->TPDOCommPar.inhibitTime)
        {
                // if change-of-state (COS) happend send tpdo
                if (CO_TPDOisCOS(TPDO))
                {
                        TPDO->inhibitTimer_us_per_cnt = 0;
                        send_tpdo = true;
                }
                else
                {
                        // inhibit timer passed but there was no COS
                        // hold inhibit timer just above the trigger time
                        // so it wont overflow while waiting for COS
                        TPDO->inhibitTimer_us_per_cnt = TPDO->TPDOCommPar.inhibitTime;
                }
        }

        return send_tpdo;
}

// Check if event timer passed. If passed set flag (return val) to signal
// TPDO should be sent
static bool CO_TPDO_handle_event_time(CO_TPDO_t *TPDO,
                                        uint32_t time_passed_from_previous_call)
{
        bool send_tpdo = false;
        TPDO->eventTimer_us_per_cnt += time_passed_from_previous_call;

        // did event timer run out?
        if (TPDO->eventTimer_us_per_cnt >= (TPDO->TPDOCommPar.eventTimer * 1000))
        {
                TPDO->eventTimer_us_per_cnt = 0;
                send_tpdo = true;
        }

        return send_tpdo;
}


/******************************************************************************/
void CO_TPDO_process(
        CO_TPDO_t *TPDO,
        enum co_tpdo_sync_types sync,
        const uint8_t  sync_data,
        uint32_t timeDifference_us)
{

        bool send_tpdo = false;

        // synchronous (acycle)
        if (CO_TT_SYNCHRONOUS_ACYCLE  == TPDO->TPDOCommPar.transmissionType)
        {
                if (CO_TPDO_NO_SYNC != sync)
                {
                        if (CO_TPDOisCOS(TPDO))
                        {
                                send_tpdo = true;
                        }
                }
        }
        // synchronous (cycle every Nth sync)
        else if  (CO_TT_SYNCHRONOUS_EVERY_240_CYCLE >= TPDO->TPDOCommPar.transmissionType)
        {
                if (CO_TPDO_NO_SYNC != sync)
                {
                        // handle SYNC start value
                        if (TPDO->waiting_for_right_sync_cnt_value &&
                            (0 != TPDO->TPDOCommPar.SYNCStartValue) &&
                            (CO_TPDO_SYNC_WITH_COUNTER == sync ))
                        {
                                if (sync_data == TPDO->TPDOCommPar.SYNCStartValue)
                                {
                                        TPDO->waiting_for_right_sync_cnt_value = false;
                                }
                        }

                        if ((false == TPDO->waiting_for_right_sync_cnt_value) ||
                            (0 == TPDO->TPDOCommPar.SYNCStartValue) ||
                            (CO_TPDO_SYNC_WITH_COUNTER != sync ))
                        {
                                TPDO->syncCounter += 1;
                                if (TPDO->TPDOCommPar.transmissionType <= TPDO->syncCounter)
                                {
                                        TPDO->syncCounter = 0;
                                        send_tpdo = true;
                                        TPDO->waiting_for_right_sync_cnt_value = true;
                                }
                        }
                }
        }
        // event driven (manufacturer specific) or
        // event driven (device and application profile specific)
        else if (CO_TT_EVENT_DRIVEN_MANUFACTURER_SPECIFIC >=
                 TPDO->TPDOCommPar.transmissionType)
        {

                // handle inhibit time
                if (TPDO->TPDOCommPar.inhibitTime) // is inhibit timer enabled
                {
                        send_tpdo = CO_TPDO_handle_inhibit_time(TPDO, timeDifference_us);
                }
                else
                {
                        if (CO_TPDOisCOS(TPDO))
                        {
                                send_tpdo = true;
                        }
                }

                // handle event time
                if (!send_tpdo)
                {
                        if (TPDO->TPDOCommPar.eventTimer) // is event timer enabled
                        {
                                send_tpdo = CO_TPDO_handle_event_time(TPDO, timeDifference_us);
                        }
                }
        }

        if (send_tpdo)
        {
                TPDO->eventTimer_us_per_cnt = 0; //TODO: Should we move this somewhere else?
                CO_TPDOsend(TPDO);
        }
}

//        /* Send PDO by application request or by Event timer */
////        if(TPDO->TPDOCommPar->transmissionType >= 253){
////              if(*TPDO->TPDOCommPar_ptrs.transmissionType >= 253){
////            if(TPDO->inhibitTimer == 0 && (TPDO->sendRequest || (TPDO->TPDOCommPar->eventTimer && TPDO->eventTimer == 0))){
////                      if(TPDO->inhibitTimer == 0 && (TPDO->sendRequest || (*TPDO->TPDOCommPar_ptrs.eventTimer && TPDO->eventTimer == 0))){
//                if(CO_TPDOsend(TPDO) == CO_ERROR_NO){
//                    /* successfully sent */
////                        TPDO->inhibitTimer = ((uint32_t) (*TPDO->TPDOCommPar_ptrs.inhibitTime)) * 100;
////                        TPDO->eventTimer = ((uint32_t) (*TPDO->TPDOCommPar_ptrs.eventTimer)) * 1000;
//                }
////            }
////        }
//
//        /* TODO: Synchronous PDOs */
////        else if(SYNC && syncWas){
////            /* send synchronous acyclic PDO */
////            if(TPDO->TPDOCommPar->transmissionType == 0){
////                if(TPDO->sendRequest) CO_TPDOsend(TPDO);
////            }
////            /* send synchronous cyclic PDO */
////            else{
////                /* is the start of synchronous TPDO transmission */
////                if(TPDO->syncCounter == 255){
////                    if(SYNC->counterOverflowValue && TPDO->TPDOCommPar->SYNCStartValue)
////                        TPDO->syncCounter = 254;   /* SYNCStartValue is in use */
////                    else
////                        TPDO->syncCounter = TPDO->TPDOCommPar->transmissionType;
////                }
////                /* if the SYNCStartValue is in use, start first TPDO after SYNC with matched SYNCStartValue. */
////                if(TPDO->syncCounter == 254){
////                    if(SYNC->counter == TPDO->TPDOCommPar->SYNCStartValue){
////                        TPDO->syncCounter = TPDO->TPDOCommPar->transmissionType;
////                        CO_TPDOsend(TPDO);
////                    }
////                }
////                /* Send PDO after every N-th Sync */
////                else if(--TPDO->syncCounter == 0){
////                    TPDO->syncCounter = TPDO->TPDOCommPar->transmissionType;
////                    CO_TPDOsend(TPDO);
////                }
////            }
////        }
////
////    }
////    else{
////        /* Not operational or valid. Force TPDO first send after operational or valid. */
////            if(*TPDO->TPDOCommPar_ptrs.transmissionType >= 254) TPDO->sendRequest = 1;
////            else                                         TPDO->sendRequest = 0;
////    }
////
////    /* update timers */
////        TPDO->inhibitTimer = (TPDO->inhibitTimer > timeDifference_us) ? (TPDO->inhibitTimer - timeDifference_us) : 0;
////        TPDO->eventTimer = (TPDO->eventTimer > timeDifference_us) ? (TPDO->eventTimer - timeDifference_us) : 0;
////    }
//}
