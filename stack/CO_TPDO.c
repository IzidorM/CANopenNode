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
//#include "CO_SDO.h"
//#include "CO_Emergency.h"
//#include "CO_NMT_Heartbeat.h"
//#include "CO_SYNC.h"
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
    uint32_t** pMap = &TPDO->TPDOMapPar_ptrs.mappedObjects[0];

    
    TPDO->sendIfCOSFlags = 0;

    for(i=noOfMappedObjects; i>0; i--){
        int16_t j;
        uint8_t* pData;
        uint8_t prevLength = length;
        uint8_t MBvar;
        uint32_t map = **(pMap++);

        /* function do much checking of errors in map */
        ret = CO_PDOfindMap(
                TPDO->OD,
                map,
                1,
                &pData,
                &length,
                &TPDO->sendIfCOSFlags,
                &MBvar);
        if(ret){
            length = 0;
//            CO_errorReport(TPDO->em, CO_EM_PDO_WRONG_MAPPING, CO_EMC_PROTOCOL_ERROR, map);
            break;
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
            TPDO->mapPointer[j] = pData++;
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

/******************************************************************************/
CO_ReturnError_t CO_TPDO_init(
        CO_TPDO_t              *TPDO,
//        CO_EM_t                *em,
        void               *OD,
//        uint8_t                *operatingState,
        uint8_t                 nodeId,
        uint8_t                 restrictionFlags,
//        const CO_TPDOCommPar_t *TPDOCommPar,
//        const CO_TPDOMapPar_t  *TPDOMapPar,
        uint16_t                idx_TPDOCommPar,
        uint16_t                idx_TPDOMapPar,
        void         *CANdev)
{
        int32_t err = 0;
    /* verify arguments */
        if(TPDO==NULL || OD==NULL || NULL == CANdev) {
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Configure object variables */
//    TPDO->em = em;
    TPDO->nodeId = nodeId;
    TPDO->restrictionFlags = restrictionFlags;
    TPDO->OD = OD;

    void *object = CO_OD_find(OD, idx_TPDOCommPar);
    if (NULL == object)
    {
            return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    if (6 == CO_OD_getMaxSubindex(object))
    {
            TPDO->TPDOCommPar_ptrs.maxSubIndex = CO_OD_getDataPointer(object, 0);
            TPDO->TPDOCommPar_ptrs.COB_IDUsedByTPDO = CO_OD_getDataPointer(object, 1);
            TPDO->TPDOCommPar_ptrs.transmissionType = CO_OD_getDataPointer(object, 2);
            TPDO->TPDOCommPar_ptrs.inhibitTime = CO_OD_getDataPointer(object, 3);
            TPDO->TPDOCommPar_ptrs.eventTimer = CO_OD_getDataPointer(object, 5);
            TPDO->TPDOCommPar_ptrs.SYNCStartValue = CO_OD_getDataPointer(object, 6);
    }
    else
    {
            return CO_ERROR_ILLEGAL_ARGUMENT;
    }


    object = CO_OD_find(OD, idx_TPDOMapPar);
    if (NULL == object)
    {
            return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    TPDO->TPDOMapPar_ptrs.numberOfMappedObjects = CO_OD_getDataPointer(object, 0);
    TPDO->TPDOMapPar_ptrs.mappedObjects[0] = CO_OD_getDataPointer(object, 1);

//    TPDO->TPDOCommPar = TPDOCommPar;

//    TPDO->TPDOMapPar = TPDOMapPar;

//    TPDO->operatingState = operatingState;
//    TPDO->defaultCOB_ID = defaultCOB_ID;


    /* Configure Object dictionary entry at index 0x1800+ and 0x1A00+ */
//    CO_OD_configure(&SDO->OD, idx_TPDOCommPar, CO_ODF_TPDOcom, (void*)TPDO, 0, 0);
//    CO_OD_configure(&SDO->OD, idx_TPDOMapPar, CO_ODF_TPDOmap, (void*)TPDO, 0, 0);

    /* configure communication and mapping */
//    TPDO->CANdevTx = CANdevTx;
//    TPDO->CANdevTxIdx = CANdevTxIdx;
    TPDO->syncCounter = 255;
    TPDO->inhibitTimer = 0;
//    TPDO->eventTimer = ((uint32_t) TPDOCommPar->eventTimer) * 1000;
    TPDO->eventTimer = ((uint32_t) *TPDO->TPDOCommPar_ptrs.eventTimer) * 1000;
//    if(TPDOCommPar->transmissionType>=254) TPDO->sendRequest = 1;
    if(*TPDO->TPDOCommPar_ptrs.transmissionType >= 254) TPDO->sendRequest = 1;
    // TODO: Error checking?
//    CO_TPDOconfigMap(TPDO, TPDOMapPar->numberOfMappedObjects);

    // TODO: Implement it as ptrs to OD
    err = CO_TPDOconfigMap(TPDO, *TPDO->TPDOMapPar_ptrs.numberOfMappedObjects);
    if (err)
    {
            return CO_ERROR_ILLEGAL_ARGUMENT;            
    }
//    CO_TPDOconfigCom(TPDO,
//                     *TPDO->TPDOCommPar_ptrs.COB_IDUsedByTPDO,
//                     ((*TPDO->TPDOCommPar_ptrs.transmissionType <= 240) ? 1 : 0));
    TPDO->valid = true;
    
    if((*TPDO->TPDOCommPar_ptrs.transmissionType > 240 &&
         *TPDO->TPDOCommPar_ptrs.transmissionType < 254) ||
         *TPDO->TPDOCommPar_ptrs.SYNCStartValue > 240){
            TPDO->valid = false;
    }

    TPDO->CANdev = CANdev;
    
    return CO_ERROR_NO;
}


/******************************************************************************/
uint8_t CO_TPDOisCOS(CO_TPDO_t *TPDO){

    /* Prepare TPDO data automatically from Object Dictionary variables */
    uint8_t* pPDOdataByte;
    uint8_t** ppODdataByte;

    pPDOdataByte = &TPDO->TXbuff.data[TPDO->dataLength];
    ppODdataByte = &TPDO->mapPointer[TPDO->dataLength];

    switch(TPDO->dataLength){
        case 8: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x80)) return 1;
        case 7: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x40)) return 1;
        case 6: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x20)) return 1;
        case 5: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x10)) return 1;
        case 4: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x08)) return 1;
        case 3: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x04)) return 1;
        case 2: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x02)) return 1;
        case 1: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x01)) return 1;
    }

    return 0;
}

//#define TPDO_CALLS_EXTENSION
/******************************************************************************/
int16_t CO_TPDOsend(CO_TPDO_t *TPDO){
    int16_t i;
    uint8_t* pPDOdataByte;
    uint8_t** ppODdataByte;

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

//    CO_CANtx_t TXbuff;      /**< CAN transmit buffer */
    
    TPDO->TXbuff.ident = *TPDO->TPDOCommPar_ptrs.COB_IDUsedByTPDO + TPDO->nodeId;
    TPDO->TXbuff.DLC = TPDO->dataLength;


    i = TPDO->dataLength;
    pPDOdataByte = &TPDO->TXbuff.data[0];
    ppODdataByte = &TPDO->mapPointer[0];

    /* Copy data from Object dictionary. */
    for(; i>0; i--) {
        *(pPDOdataByte++) = **(ppODdataByte++);
    }

    TPDO->sendRequest = 0;

    return CO_CANsend(TPDO->CANdev, &TPDO->TXbuff);
}

/******************************************************************************/
void CO_TPDO_process(
        CO_TPDO_t              *TPDO,
//        CO_SYNC_t              *SYNC,
        bool_t                  syncWas,
        uint32_t                timeDifference_us)
{
//    if(TPDO->valid && *TPDO->operatingState == CO_NMT_OPERATIONAL){
      if(TPDO->valid){

        /* Send PDO by application request or by Event timer */
//        if(TPDO->TPDOCommPar->transmissionType >= 253){
              if(*TPDO->TPDOCommPar_ptrs.transmissionType >= 253){
//            if(TPDO->inhibitTimer == 0 && (TPDO->sendRequest || (TPDO->TPDOCommPar->eventTimer && TPDO->eventTimer == 0))){
                      if(TPDO->inhibitTimer == 0 && (TPDO->sendRequest || (*TPDO->TPDOCommPar_ptrs.eventTimer && TPDO->eventTimer == 0))){
                if(CO_TPDOsend(TPDO) == CO_ERROR_NO){
                    /* successfully sent */
                        TPDO->inhibitTimer = ((uint32_t) (*TPDO->TPDOCommPar_ptrs.inhibitTime)) * 100;
                        TPDO->eventTimer = ((uint32_t) (*TPDO->TPDOCommPar_ptrs.eventTimer)) * 1000;
                }
            }
        }

        /* TODO: Synchronous PDOs */
//        else if(SYNC && syncWas){
//            /* send synchronous acyclic PDO */
//            if(TPDO->TPDOCommPar->transmissionType == 0){
//                if(TPDO->sendRequest) CO_TPDOsend(TPDO);
//            }
//            /* send synchronous cyclic PDO */
//            else{
//                /* is the start of synchronous TPDO transmission */
//                if(TPDO->syncCounter == 255){
//                    if(SYNC->counterOverflowValue && TPDO->TPDOCommPar->SYNCStartValue)
//                        TPDO->syncCounter = 254;   /* SYNCStartValue is in use */
//                    else
//                        TPDO->syncCounter = TPDO->TPDOCommPar->transmissionType;
//                }
//                /* if the SYNCStartValue is in use, start first TPDO after SYNC with matched SYNCStartValue. */
//                if(TPDO->syncCounter == 254){
//                    if(SYNC->counter == TPDO->TPDOCommPar->SYNCStartValue){
//                        TPDO->syncCounter = TPDO->TPDOCommPar->transmissionType;
//                        CO_TPDOsend(TPDO);
//                    }
//                }
//                /* Send PDO after every N-th Sync */
//                else if(--TPDO->syncCounter == 0){
//                    TPDO->syncCounter = TPDO->TPDOCommPar->transmissionType;
//                    CO_TPDOsend(TPDO);
//                }
//            }
//        }
//
//    }
    else{
        /* Not operational or valid. Force TPDO first send after operational or valid. */
            if(*TPDO->TPDOCommPar_ptrs.transmissionType >= 254) TPDO->sendRequest = 1;
            else                                         TPDO->sendRequest = 0;
    }

    /* update timers */
        TPDO->inhibitTimer = (TPDO->inhibitTimer > timeDifference_us) ? (TPDO->inhibitTimer - timeDifference_us) : 0;
        TPDO->eventTimer = (TPDO->eventTimer > timeDifference_us) ? (TPDO->eventTimer - timeDifference_us) : 0;
    }
}
