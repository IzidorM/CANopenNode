/**
 * CANopen SYNC object protocol.
 *
 * @file        CO_SYNC.h
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


#ifndef CO_SYNC_H
#define CO_SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "CO_driver.h"
#include "CO_OD_interface.h"
#include <string.h>


/**
 * @defgroup CO_SYNC SYNC
 * @ingroup CO_CANopen
 * @{
 *
 * CANopen SYNC object protocol.
 *
 * For CAN identifier see #CO_Default_CAN_ID_t
 *
 * SYNC message is used for synchronization of the nodes on network. One node
 * can be SYNC producer, others can be SYNC consumers. Synchronous TPDOs are
 * transmitted after the CANopen SYNC message. Synchronous received PDOs are
 * accepted(copied to OD) immediatelly after the reception of the next SYNC
 * message.
 *
 * ####Contents of SYNC message
 * By default SYNC message has no data. If _Synchronous counter overflow value_
 * from Object dictionary (index 0x1019) is different than 0, SYNC message has
 * one data byte: _counter_ incremented by 1 with every SYNC transmission.
 *
 * ####SYNC in CANopenNode
 * According to CANopen, synchronous RPDOs must be processed after reception of
 * the next sync messsage. For that reason, there is a double receive buffer
 * for each synchronous RPDO. At the moment, when SYNC is received or
 * transmitted, internal variable CANrxToggle toggles. That variable is then
 * used by synchronous RPDO to determine, which of the two buffers is used for
 * RPDO reception and which for RPDO processing.
 */


enum co_sync_return_codes {
        CO_SYNC_NO_ERROR = 0,
        CO_SYNC_ILLEGAL_ARGUMENT_ERROR,
        CO_SYNC_WRONG_MSG_LENGTH_ERROR,
};
        
enum co_sync_types {
        CO_NO_SYNC,
        CO_SYNC_WITHOUT_COUNTER,
        CO_SYNC_WITH_COUNTER,
};

/**
 * SYNC producer and consumer object.
 */
typedef struct{
//    CO_EM_t            *em;             /**< From CO_SYNC_init() */
//    uint8_t            *operatingState; /**< From CO_SYNC_init() */
    /** True, if device is SYNC producer. Calculated from _COB ID SYNC Message_
    variable from Object dictionary (index 0x1005). */
    bool isProducer;

    //Object dictionary (index 0x1005)
    uint32_t COB_ID_SYNC;
    //Object dictionary (index 0x1006)
    uint32_t communicatio_cycle_period;
    //Object dictionary (index 0x1007)
    uint32_t sync_window_length;
    //Object dictionary (index 0x1019)
    uint8_t sync_counter_overflow_value;

    bool new_msg_received;
    CO_CANrxMsg_t msg_received;
    void *CANdev;
//    
//    /** Sync period timeout time in [microseconds].
//    (periodTimeoutTime = periodTime * 1,5) */
//    uint32_t            periodTimeoutTime;
//    /** Value from _Synchronous counter overflow value_ variable from Object
//    dictionary (index 0x1019) */
//    uint8_t             counterOverflowValue;
//    /** True, if current time is inside synchronous window.
//    In this case synchronous PDO may be sent. */
//    bool              curentSyncTimeIsInsideWindow;
//    /** Counter of the SYNC message if counterOverflowValue is different than zero */
//    uint8_t             counter;
//    /** Timer for the SYNC message in [microseconds].
//    Set to zero after received or transmitted SYNC message */
//    uint32_t            timer;
} CO_SYNC_t;


/**
 * Initialize SYNC object.
 *
 * Function must be called in the communication reset section.
 *
 * @param SYNC This object will be initialized.
 * @param em Emergency object.
 * @param SDO SDO server object.
 * @param operatingState Pointer to variable indicating CANopen device NMT internal state.
 * @param COB_ID_SYNCMessage From Object dictionary (index 0x1005).
 * @param communicationCyclePeriod From Object dictionary (index 0x1006).
 * @param synchronousCounterOverflowValue From Object dictionary (index 0x1019).
 * @param CANdevRx CAN device for SYNC reception.
 * @param CANdevRxIdx Index of receive buffer in the above CAN device.
 * @param CANdevTx CAN device for SYNC transmission.
 * @param CANdevTxIdx Index of transmit buffer in the above CAN device.
 *
 * @return #CO_ReturnError_t: CO_ERROR_NO or CO_ERROR_ILLEGAL_ARGUMENT.
 */
CO_ReturnError_t CO_SYNC_init(
        CO_SYNC_t              *SYNC,
        uint32_t                COB_ID_SYNC,
        uint8_t nodeId,
        void *CANdev);

/**
 * Process SYNC communication.
 *
 * Function must be called cyclically.
 *
 * @param SYNC This object.
 * @param timeDifference_us Time difference from previous function call in [microseconds].
 * @param ObjDict_synchronousWindowLength _Synchronous window length_ variable from
 * Object dictionary (index 0x1007).
 *
 * @return 0: No special meaning.
 * @return 1: New SYNC message recently received or was just transmitted.
 * @return 2: SYNC time was just passed out of window.
 */
enum co_sync_return_codes CO_SYNC_process(
        CO_SYNC_t              *SYNC,
        enum co_sync_types *sync_received,
        uint8_t *sync_counter,
        uint32_t                timeDifference_us);


void CO_SYNC_receive(void *object, const CO_CANrxMsg_t *msg);

enum co_sync_return_codes CO_SYNC_set_sync_counter_overflow_value(
        CO_SYNC_t              *SYNC,
        uint8_t sync_counter_overflow_value);

        
#ifdef __cplusplus
}
#endif /*__cplusplus*/

/** @} */
#endif
