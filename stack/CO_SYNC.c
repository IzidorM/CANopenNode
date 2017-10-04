/*
 * CANopen SYNC object.
 *
 * @file        CO_SYNC.c
 * @ingroup     CO_SYNC
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
#include "CO_SYNC.h"

/*
 * Read received message from CAN module.
 *
 * Function will be called (by CAN receive interrupt) every time, when CAN
 * message with correct identifier will be received. For more information and
 * description of parameters see file CO_driver.h.
 */
void CO_SYNC_receive(void *object, const CO_CANrxMsg_t *msg){
    CO_SYNC_t *SYNC;

    SYNC = (CO_SYNC_t*)object;   /* this is the correct pointer type of the first argument */
    
    memcpy(&SYNC->msg_received, msg, sizeof(CO_CANrxMsg_t));
    SYNC->new_msg_received = true;
}

/*
 * Function for accessing _COB ID SYNC Message_ (index 0x1005) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
//static CO_SDO_abortCode_t CO_ODF_1005(CO_ODF_arg_t *ODF_arg){
//    CO_SYNC_t *SYNC;
//    uint32_t value;
//    CO_SDO_abortCode_t ret = CO_SDO_AB_NONE;
//
//    SYNC = (CO_SYNC_t*) ODF_arg->object;
//    value = CO_getUint32(ODF_arg->data);
//
//    if(!ODF_arg->reading){
//        uint8_t configureSyncProducer = 0;
//
//        /* only 11-bit CAN identifier is supported */
//        if(value & 0x20000000UL){
//            ret = CO_SDO_AB_INVALID_VALUE;
//        }
//        else{
//            /* is 'generate Sync messge' bit set? */
//            if(value & 0x40000000UL){
//                /* if bit was set before, value can not be changed */
//                if(SYNC->isProducer){
//                    ret = CO_SDO_AB_DATA_DEV_STATE;
//                }
//                else{
//                    configureSyncProducer = 1;
//                }
//            }
//        }
//
//        /* configure sync producer and consumer */
//        if(ret == CO_SDO_AB_NONE){
//            SYNC->COB_ID = (uint16_t)(value & 0x7FFU);
//
//            if(configureSyncProducer){
//                uint8_t len = 0U;
//                if(SYNC->counterOverflowValue != 0U){
//                    len = 1U;
//                    SYNC->counter = 0U;
//                    SYNC->timer = 0U;
//                }
////                SYNC->CANtxBuff = CO_CANtxBufferInit(
////                        SYNC->CANdevTx,         /* CAN device */
////                        SYNC->CANdevTxIdx,      /* index of specific buffer inside CAN module */
////                        SYNC->COB_ID,           /* CAN identifier */
////                        0,                      /* rtr */
////                        len,                    /* number of data bytes */
////                        0);                     /* synchronous message flag bit */
//                SYNC->isProducer = true;
//            }
//            else{
//                SYNC->isProducer = false;
//            }
//
////            CO_CANrxBufferInit(
////                    SYNC->CANdevRx,         /* CAN device */
////                    SYNC->CANdevRxIdx,      /* rx buffer index */
////                    SYNC->COB_ID,           /* CAN identifier */
////                    0x7FF,                  /* mask */
////                    0,                      /* rtr */
////                    (void*)SYNC,            /* object passed to receive function */
////                    CO_SYNC_receive);       /* this function will process received message */
//        }
//    }
//
//    return ret;
//}
//
//
///*
// * Function for accessing _Communication cycle period_ (index 0x1006) from SDO server.
// *
// * For more information see file CO_SDO.h.
// */
//static CO_SDO_abortCode_t CO_ODF_1006(CO_ODF_arg_t *ODF_arg){
//    CO_SYNC_t *SYNC;
//    uint32_t value;
//    CO_SDO_abortCode_t ret = CO_SDO_AB_NONE;
//
//    SYNC = (CO_SYNC_t*) ODF_arg->object;
//    value = CO_getUint32(ODF_arg->data);
//
//    if(!ODF_arg->reading){
//        /* period transition from 0 to something */
//        if((SYNC->periodTime == 0) && (value != 0)){
//            SYNC->counter = 0;
//        }
//
//        SYNC->periodTime = value;
//        SYNC->periodTimeoutTime = (value / 2U) * 3U;
//        /* overflow? */
//        if(SYNC->periodTimeoutTime < value){
//            SYNC->periodTimeoutTime = 0xFFFFFFFFUL;
//        }
//
//        SYNC->timer = 0;
//    }
//
//    return ret;
//}


/**
 * Function for accessing _Synchronous counter overflow value_ (index 0x1019) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
//static CO_SDO_abortCode_t CO_ODF_1019(CO_ODF_arg_t *ODF_arg){
//    CO_SYNC_t *SYNC;
//    uint8_t value;
//    CO_SDO_abortCode_t ret = CO_SDO_AB_NONE;
//
//    SYNC = (CO_SYNC_t*) ODF_arg->object;
//    value = ODF_arg->data[0];
//
//    if(!ODF_arg->reading){
//        uint8_t len = 0U;
//
//        if(SYNC->periodTime){
//            ret = CO_SDO_AB_DATA_DEV_STATE;
//        }
//        else{
//            SYNC->counterOverflowValue = value;
//            if(value != 0){
//                len = 1U;
//            }
//
////            SYNC->CANtxBuff = CO_CANtxBufferInit(
////                    SYNC->CANdevTx,         /* CAN device */
////                    SYNC->CANdevTxIdx,      /* index of specific buffer inside CAN module */
////                    SYNC->COB_ID,           /* CAN identifier */
////                    0,                      /* rtr */
////                    len,                    /* number of data bytes */
////                    0);                     /* synchronous message flag bit */
//        }
//    }
//
//    return ret;
//}

enum co_sync_return_codes CO_SYNC_set_sync_counter_overflow_value(
        CO_SYNC_t              *SYNC,
        uint8_t sync_counter_overflow_value)
{
        SYNC->sync_counter_overflow_value = sync_counter_overflow_value;

        return CO_SYNC_NO_ERROR;
}

/******************************************************************************/
CO_ReturnError_t CO_SYNC_init(
        CO_SYNC_t              *SYNC,
        uint32_t                COB_ID_SYNC,
        uint8_t nodeId,
        void *CANdev)
{
        
        if(NULL == SYNC || NULL == CANdev)
        {
            return CO_ERROR_ILLEGAL_ARGUMENT;
        }

        SYNC->isProducer = false;
        SYNC->COB_ID_SYNC = COB_ID_SYNC & 0x7FF;

        SYNC->communicatio_cycle_period = 0;
        SYNC->sync_window_length = 0;
        SYNC->sync_counter_overflow_value = 0;

        SYNC->new_msg_received = false;
        memset(&SYNC->msg_received, 0, sizeof(CO_CANrxMsg_t));

        SYNC->CANdev = CANdev;
        return CO_ERROR_NO;
}


/******************************************************************************/
enum co_sync_return_codes CO_SYNC_process(
        CO_SYNC_t              *SYNC,
        enum co_sync_types *sync_received,
        uint8_t *sync_counter,
        uint32_t                timeDifference_us)
{
        *sync_received = CO_NO_SYNC;
        *sync_counter = 0;

        if(NULL == SYNC)
        {
                return CO_SYNC_ILLEGAL_ARGUMENT_ERROR;
        }

        if (SYNC->isProducer)
        {
                // TODO: Implement SYNC producer
        }
        else  // sync consumer
        {
                if (SYNC->new_msg_received)
                {
                        SYNC->new_msg_received = false;
                        // check the size of the received CAN msg to figure out
                        // what kind of SYNC message we have in the buffer
                        if (0 < SYNC->msg_received.DLC)
                        {
                                // CAN msg has data, so this is SYNC with counter
                                if (0 == SYNC->sync_counter_overflow_value)
                                {
                                        return CO_SYNC_WRONG_MSG_LENGTH_ERROR;
                                }
                                *sync_received = CO_SYNC_WITH_COUNTER;
                                *sync_counter = SYNC->msg_received.data[0];
                        }
                        else
                        {
                                if (0 != SYNC->sync_counter_overflow_value)
                                {
                                        return CO_SYNC_WRONG_MSG_LENGTH_ERROR;
                                }

                                *sync_received = CO_SYNC_WITHOUT_COUNTER;
                        }
                }
        }
        return CO_SYNC_NO_ERROR;
}

//    if (true) {
//        /* update sync timer, no overflow */
//        timerNew = SYNC->timer + timeDifference_us;
//        if(timerNew > SYNC->timer) SYNC->timer = timerNew;
//
//        /* was SYNC just received */
//        if(SYNC->CANrxNew){
//            SYNC->timer = 0;
//            ret = 1;
//            SYNC->CANrxNew = false;
//        }
//
//        /* SYNC producer */
//        if(SYNC->isProducer && SYNC->periodTime){
//            if(SYNC->timer >= SYNC->periodTime){
//                if(++SYNC->counter > SYNC->counterOverflowValue) SYNC->counter = 1;
//                SYNC->timer = 0;
//                ret = 1;
//                SYNC->CANrxToggle = SYNC->CANrxToggle ? false : true;
////                SYNC->CANtxBuff->data[0] = SYNC->counter;
////                CO_CANsend(SYNC->CANdevTx, SYNC->CANtxBuff);
//            }
//        }
//
//        /* Synchronous PDOs are allowed only inside time window */
//        if(ObjDict_synchronousWindowLength){
//            if(SYNC->timer > ObjDict_synchronousWindowLength){
//                if(SYNC->curentSyncTimeIsInsideWindow){
//                    ret = 2;
//                }
//                SYNC->curentSyncTimeIsInsideWindow = false;
//            }
//            else{
//                SYNC->curentSyncTimeIsInsideWindow = true;
//            }
//        }
//        else{
//            SYNC->curentSyncTimeIsInsideWindow = true;
//        }
//
//        /* Verify timeout of SYNC */
//        if(SYNC->periodTime && SYNC->timer > SYNC->periodTimeoutTime && *SYNC->operatingState == CO_NMT_OPERATIONAL)
//            CO_errorReport(SYNC->em, CO_EM_SYNC_TIME_OUT, CO_EMC_COMMUNICATION, SYNC->timer);
//    }
//    else {
//        SYNC->CANrxNew = false;
//    }

    /* verify error from receive function */
//    if(SYNC->receiveError != 0U){
//        CO_errorReport(SYNC->em, CO_EM_SYNC_LENGTH, CO_EMC_SYNC_DATA_LENGTH, (uint32_t)SYNC->receiveError);
//        SYNC->receiveError = 0U;
//    }
