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
#include "CO_RPDO.h"
#include <string.h>

/*
 * Read received message from CAN module.
 *
 * Function will be called (by CAN receive interrupt) every time, when CAN
 * message with correct identifier will be received. For more information and
 * description of parameters see file CO_driver.h.
 * If new message arrives and previous message wasn't processed yet, then
 * previous message will be lost and overwritten by new message. That's OK with PDOs.
 */
int32_t CO_RPDO_receive(void *object, const CO_CANrxMsg_t *msg){
    CO_RPDO_t *RPDO;

    RPDO = (CO_RPDO_t*)object;   /* this is the correct pointer type of the first argument */

    //        (*RPDO->operatingState == CO_NMT_OPERATIONAL) &&
    if((RPDO->valid) && (msg->DLC >= RPDO->dataLength))
    {
//        if(RPDO->synchronous && RPDO->SYNC->CANrxToggle)
//        {
//            /* copy data into second buffer and set 'new message' flag */
//            RPDO->CANrxData[1][0] = msg->data[0];
//            RPDO->CANrxData[1][1] = msg->data[1];
//            RPDO->CANrxData[1][2] = msg->data[2];
//            RPDO->CANrxData[1][3] = msg->data[3];
//            RPDO->CANrxData[1][4] = msg->data[4];
//            RPDO->CANrxData[1][5] = msg->data[5];
//            RPDO->CANrxData[1][6] = msg->data[6];
//            RPDO->CANrxData[1][7] = msg->data[7];
//
//            RPDO->CANrxNew[1] = true;
//        }
//        else {
            /* copy data into default buffer and set 'new message' flag */
            RPDO->CANrxData[0][0] = msg->data[0];
            RPDO->CANrxData[0][1] = msg->data[1];
            RPDO->CANrxData[0][2] = msg->data[2];
            RPDO->CANrxData[0][3] = msg->data[3];
            RPDO->CANrxData[0][4] = msg->data[4];
            RPDO->CANrxData[0][5] = msg->data[5];
            RPDO->CANrxData[0][6] = msg->data[6];
            RPDO->CANrxData[0][7] = msg->data[7];

            RPDO->CANrxNew[0] = true;
//        }
    }
    return 0;
}

/*
 * Configure RPDO Communication parameter.
 *
 * Function is called from commuincation reset or when parameter changes.
 *
 * Function configures following variable from CO_RPDO_t: _valid_. It also
 * configures CAN rx buffer. If configuration fails, emergency message is send
 * and device is not able to enter NMT operational.
 *
 * @param RPDO RPDO object.
 * @param COB_IDUsedByRPDO _RPDO communication parameter_, _COB-ID for PDO_ variable
 * from Object dictionary (index 0x1400+, subindex 1).
 */
//static void CO_RPDOconfigCom(CO_RPDO_t* RPDO, uint32_t COB_IDUsedByRPDO){
//    uint16_t ID;
//    CO_ReturnError_t r;
//
//    ID = (uint16_t)COB_IDUsedByRPDO;
//
//    /* is RPDO used? */
//    if((COB_IDUsedByRPDO & 0xBFFFF800L) == 0 && RPDO->dataLength && ID){
//        /* is used default COB-ID? */
//        if(ID == RPDO->defaultCOB_ID) ID += RPDO->nodeId;
//        RPDO->valid = true;
//        RPDO->synchronous = (RPDO->RPDOCommPar->transmissionType <= 240) ? true : false;
//    }
//    else{
//        ID = 0;
//        RPDO->valid = false;
//        RPDO->CANrxNew[0] = RPDO->CANrxNew[1] = false;
//    }
//    r = CO_CANrxBufferInit(
//            RPDO->CANdevRx,         /* CAN device */
//            RPDO->CANdevRxIdx,      /* rx buffer index */
//            ID,                     /* CAN identifier */
//            0x7FF,                  /* mask */
//            0,                      /* rtr */
//            (void*)RPDO,            /* object passed to receive function */
//            CO_PDO_receive);        /* this function will process received message */
//    if(r != CO_ERROR_NO){
//        RPDO->valid = false;
//        RPDO->CANrxNew[0] = RPDO->CANrxNew[1] = false;
//    }
//}

/*
 * Configure RPDO Mapping parameter.
 *
 * Function is called from communication reset or when parameter changes.
 *
 * Function configures following variables from CO_RPDO_t: _dataLength_ and
 * _mapPointer_.
 *
 * @param RPDO RPDO object.
 * @param noOfMappedObjects Number of mapped object (from OD).
 *
 * @return 0 on success, otherwise SDO abort code.
 */
static uint32_t CO_RPDOconfigMap(CO_RPDO_t* RPDO){
    int16_t i;
    uint8_t length = 0;
    uint32_t ret = 0;
    void *m = RPDO->RPDO_mapping_object;

    RPDO->RPDOMapPar_ptrs.numberOfMappedObjects = CO_OD_getDataPointer(m, 0);
    uint8_t noOfMappedObjects = *RPDO->RPDOMapPar_ptrs.numberOfMappedObjects;

    for(i=1; noOfMappedObjects >= i; i++){
        int16_t j;
        uint8_t* pData;
//        uint8_t dummy = 0;
        uint8_t prevLength = length;
        uint8_t MBvar;

        void *tmp_ptr = CO_OD_getDataPointer(m, i);
        if (NULL == tmp_ptr)
        {
                return 1;
        }
        
        uint32_t map = *(uint32_t *) tmp_ptr;

        /* function do much checking of errors in map */
        ret = CO_PDOfindMap(
                RPDO->OD,
                map,
                0,
                &pData,
                &length,
//                &dummy,
                &MBvar,
                &RPDO->map_callback[i-1]);
        if(ret){
            length = 0;
//            CO_errorReport(RPDO->em, CO_EM_PDO_WRONG_MAPPING, CO_EMC_PROTOCOL_ERROR, map);
            break;
        }

        /* write PDO data pointers */
#ifdef CO_BIG_ENDIAN
        if(MBvar){
            for(j=length-1; j>=prevLength; j--)
                RPDO->mapPointer[j] = pData++;
        }
        else{
            for(j=prevLength; j<length; j++)
                RPDO->mapPointer[j] = pData++;
        }
#else
        for(j=prevLength; j<length; j++){
            RPDO->mapPointer[j] = pData++;
        }
#endif

    }

    RPDO->dataLength = length;

    return ret;
}


/*
 * Function for accessing _RPDO communication parameter_ (index 0x1400+) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
//static CO_SDO_abortCode_t CO_ODF_RPDOcom(CO_ODF_arg_t *ODF_arg){
//    CO_RPDO_t *RPDO;
//
//    RPDO = (CO_RPDO_t*) ODF_arg->object;
//
//    /* Reading Object Dictionary variable */
//    if(ODF_arg->reading){
//        if(ODF_arg->subIndex == 1){
//            uint32_t *value = (uint32_t*) ODF_arg->data;
//
//            /* if default COB ID is used, write default value here */
//            if(((*value)&0xFFFF) == RPDO->defaultCOB_ID && RPDO->defaultCOB_ID)
//                *value += RPDO->nodeId;
//
//            /* If PDO is not valid, set bit 31 */
//            if(!RPDO->valid) *value |= 0x80000000L;
//        }
//        return CO_SDO_AB_NONE;
//    }
//
//    /* Writing Object Dictionary variable */
//    if(RPDO->restrictionFlags & 0x04)
//        return CO_SDO_AB_READONLY;  /* Attempt to write a read only object. */
//    if(*RPDO->operatingState == CO_NMT_OPERATIONAL && (RPDO->restrictionFlags & 0x01))
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
//        if(((*value)&0xFFFF) == (RPDO->defaultCOB_ID + RPDO->nodeId)){
//            *value &= 0xC0000000L;
//            *value += RPDO->defaultCOB_ID;
//        }
//
//        /* if PDO is valid, bits 0..29 can not be changed */
//        if(RPDO->valid && ((*value ^ RPDO->RPDOCommPar->COB_IDUsedByRPDO) & 0x3FFFFFFFL))
//            return CO_SDO_AB_INVALID_VALUE;  /* Invalid value for parameter (download only). */
//
//        /* configure RPDO */
//        CO_RPDOconfigCom(RPDO, *value);
//    }
//    else if(ODF_arg->subIndex == 2){   /* Transmission_type */
//        uint8_t *value = (uint8_t*) ODF_arg->data;
//        bool_t synchronousPrev = RPDO->synchronous;
//
//        /* values from 241...253 are not valid */
//        if(*value >= 241 && *value <= 253)
//            return CO_SDO_AB_INVALID_VALUE;  /* Invalid value for parameter (download only). */
//
//        RPDO->synchronous = (*value <= 240) ? true : false;
//
//        /* Remove old message from second buffer. */
//        if(RPDO->synchronous != synchronousPrev) {
//            RPDO->CANrxNew[1] = false;
//        }
//    }
//
//    return CO_SDO_AB_NONE;
//}

/*
 * Function for accessing _RPDO mapping parameter_ (index 0x1600+) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
//static CO_SDO_abortCode_t CO_ODF_RPDOmap(CO_ODF_arg_t *ODF_arg){
//    CO_RPDO_t *RPDO;
//
//    RPDO = (CO_RPDO_t*) ODF_arg->object;
//
//    /* Reading Object Dictionary variable */
//    if(ODF_arg->reading){
//        uint8_t *value = (uint8_t*) ODF_arg->data;
//
//        if(ODF_arg->subIndex == 0){
//            /* If there is error in mapping, dataLength is 0, so numberOfMappedObjects is 0. */
//            if(!RPDO->dataLength) *value = 0;
//        }
//        return CO_SDO_AB_NONE;
//    }
//
//    /* Writing Object Dictionary variable */
//    if(RPDO->restrictionFlags & 0x08)
//        return CO_SDO_AB_READONLY;  /* Attempt to write a read only object. */
//    if(*RPDO->operatingState == CO_NMT_OPERATIONAL && (RPDO->restrictionFlags & 0x02))
//        return CO_SDO_AB_DATA_DEV_STATE;   /* Data cannot be transferred or stored to the application because of the present device state. */
//    if(RPDO->valid)
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
//        return CO_RPDOconfigMap(RPDO, *value);
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
//        if(RPDO->dataLength)
//            return CO_SDO_AB_INVALID_VALUE;  /* Invalid value for parameter (download only). */
//
//        /* verify if mapping is correct */
//        return CO_PDOfindMap(
//                RPDO->SDO,
//               *value,
//                0,
//               &pData,
//               &length,
//               &dummy,
//               &MBvar);
//    }
//
//    return CO_SDO_AB_NONE;
//}


/******************************************************************************/
CO_ReturnError_t CO_RPDO_init(
        CO_RPDO_t              *RPDO,
        void               *OD,
        uint8_t                 nodeId,
        uint8_t                 restrictionFlags,
        uint16_t                idx_RPDOCommPar,
        uint16_t                idx_RPDOMapPar,
        void *CANdev)
{
        int32_t err = 0;
    /* verify arguments */
    if(NULL==RPDO || NULL==OD || CANdev==NULL){
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Configure object variables */
    RPDO->nodeId = nodeId;
    RPDO->restrictionFlags = restrictionFlags;
    RPDO->OD = OD;
    RPDO->CANdev = CANdev;
    RPDO->CANrxNew[0] = RPDO->CANrxNew[1] = false;
    
    void *object = CO_OD_find(OD, idx_RPDOCommPar);
    if (NULL == object)
    {
            return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    if (2 == CO_OD_getMaxSubindex(object))
    {
            RPDO->RPDOCommPar_ptrs.maxSubIndex = CO_OD_getDataPointer(object, 0);
            RPDO->RPDOCommPar_ptrs.COB_IDUsedByRPDO = CO_OD_getDataPointer(object, 1);
            RPDO->RPDOCommPar_ptrs.transmissionType = CO_OD_getDataPointer(object, 2);
    }
    else
    {
            return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    RPDO->RPDO_mapping_object = CO_OD_find(OD, idx_RPDOMapPar);
    if (NULL == RPDO->RPDO_mapping_object)
    {
            return CO_ERROR_ILLEGAL_ARGUMENT;
    }

//    /* Configure Object dictionary entry at index 0x1400+ and 0x1600+ */
//    CO_OD_configure(&SDO->OD, idx_RPDOCommPar, CO_ODF_RPDOcom, (void*)RPDO, 0, 0);
//    CO_OD_configure(&SDO->OD, idx_RPDOMapPar, CO_ODF_RPDOmap, (void*)RPDO, 0, 0);

    err = CO_RPDOconfigMap(RPDO);//, *RPDO->RPDOMapPar_ptrs.numberOfMappedObjects);
    if (err)
    {
            return CO_ERROR_ILLEGAL_ARGUMENT;            
    }

//    CO_RPDOconfigCom(RPDO, RPDOCommPar->COB_IDUsedByRPDO);

    RPDO->valid = true;
    
    return CO_ERROR_NO;
}

#define RPDO_CALLS_EXTENSION
///******************************************************************************/
void CO_RPDO_process(CO_RPDO_t *RPDO, bool syncWas){

//    if(!RPDO->valid || !(*RPDO->operatingState == CO_NMT_OPERATIONAL))
        if(!RPDO->valid)
    {
        RPDO->CANrxNew[0] = RPDO->CANrxNew[1] = false;
    }
    else if(!RPDO->synchronous || syncWas)
    {
        uint8_t bufNo = 0;

        /* Determine, which of the two rx buffers, contains relevant message. */
//        if(RPDO->synchronous && !RPDO->SYNC->CANrxToggle) {
//            bufNo = 1;
//        }

        while(RPDO->CANrxNew[bufNo]){
            int16_t i;
            uint8_t* pPDOdataByte;
            uint8_t** ppODdataByte;

            i = RPDO->dataLength;
            pPDOdataByte = &RPDO->CANrxData[bufNo][0];
            ppODdataByte = &RPDO->mapPointer[0];

            /* Copy data to Object dictionary. If between the copy operation CANrxNew
             * is set to true by receive thread, then copy the latest data again. */
            RPDO->CANrxNew[bufNo] = false;
            for(; i>0; i--) {
                **(ppODdataByte++) = *(pPDOdataByte++);
            }

#ifdef RPDO_CALLS_EXTENSION
                /* for each mapped OD, check mapping to see if an OD extension is available, and call it if it is */
//                const uint32_t* pMap = &RPDO->RPDOMapPar->mappedObject1;
//                CO_SDO_t *pSDO = RPDO->SDO;

                void *m = RPDO->RPDO_mapping_object;
                RPDO->RPDOMapPar_ptrs.numberOfMappedObjects = CO_OD_getDataPointer(m, 0);
                uint8_t noOfMappedObjects = *RPDO->RPDOMapPar_ptrs.numberOfMappedObjects;

                for(i=noOfMappedObjects; i>0; i--){
                        uint32_t *tmp_p = (uint32_t *) CO_OD_getDataPointer(m, i);
                        if (NULL == tmp_p)
                        {
                                // return error
                                return;
                        }
                        uint32_t map = *(uint32_t *) tmp_p;
                        uint16_t index = (uint16_t)(map>>16);
                        uint8_t subIndex = (uint8_t)(map>>8);
                        
                        if( RPDO->map_callback[i] == NULL) continue;

                        void *obj = CO_OD_find(RPDO->OD, index);
                        if (NULL == obj)
                        {
                                return ;
                        }

                        CO_ODF_arg_t ODF_arg;
                        memset((void*)&ODF_arg, 0, sizeof(CO_ODF_arg_t));
                        ODF_arg.reading = false;
                        ODF_arg.index = index;
                        ODF_arg.subIndex = subIndex;
                        ODF_arg.object = NULL;
                        ODF_arg.attribute = CO_OD_getAttribute(obj, subIndex);
                        ODF_arg.data = CO_OD_getDataPointer(obj, subIndex);
                        ODF_arg.dataLength = CO_OD_getLength(obj, subIndex);

                        RPDO->map_callback[i](&ODF_arg);
                }
#endif
        }
    }
}
