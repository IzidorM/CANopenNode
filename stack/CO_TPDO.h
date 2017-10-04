/**
 * CANopen Process Data Object protocol.
 *
 * @file        CO_PDO.h
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


#ifndef CO_TPDO_H
#define CO_TPDO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "CO_driver.h"
        
/**
 * @defgroup CO_PDO PDO
 * @ingroup CO_CANopen
 * @{
 *
 * CANopen Process Data Object protocol.
 *
 * Process data objects are used for real-time data transfer with no protocol
 * overhead.
 *
 * TPDO with specific identifier is transmitted by one device and recieved by
 * zero or more devices as RPDO. PDO communication parameters(COB-ID,
 * transmission type, etc.) are in Object Dictionary at index 0x1400+ and
 * 0x1800+. PDO mapping parameters (size and contents of the PDO) are in Object
 * Dictionary at index 0x1600+ and 0x1A00+.
 *
 * Features of the PDO as implemented here, in CANopenNode:
 *  - Dynamic PDO mapping.
 *  - Map granularity of one byte.
 *  - After RPDO is received from CAN bus, its data are copied to buffer.
 *    Function CO_RPDO_process() (called by application) copies data to
 *    mapped objects in Object Dictionary. Synchronous RPDOs are processed AFTER
 *    reception of the next SYNC message.
 *  - Function CO_TPDO_process() (called by application) sends TPDO if
 *    necessary. There are possible different transmission types, including
 *    automatic detection of Change of State of specific variable.
 */



enum co_transmission_types
{
        CO_TT_SYNCHRONOUS_ACYCLE = 0,
        CO_TT_SYNCHRONOUS_EVERY_CYCLE = 1,
        CO_TT_SYNCHRONOUS_EVERY_240_CYCLE = 0xf0,
        CO_TT_EVENT_DRIVEN_MANUFACTURER_SPECIFIC = 0xfe,
        CO_TT_EVENT_DRIVEN_DEV_APP_PROFILE = 0xff,
};

enum co_tpdo_sync_types
{
        CO_TPDO_NO_SYNC = 0,
        CO_TPDO_SYNC_WITHOUT_COUNTER = 1,
        CO_TPDO_SYNC_WITH_COUNTER = 2,
};

        
/**
 * TPDO communication parameter. The same as record from Object dictionary (index 0x1800+).
 */
typedef struct {
    uint32_t            COB_IDUsedByTPDO;
    /** Transmission type. Values:
        - 0:       Transmiting is synchronous, specification in device profile.
        - 1-240:   Transmiting is synchronous after every N-th SYNC object.
        - 241-251: Not used.
        - 252-253: Transmited only on reception of Remote Transmission Request.
        - 254:     Manufacturer specific.
        - 255:     Asinchronous, specification in device profile. */
    /** Minimum time between transmissions of the PDO in 100micro seconds.
    Zero disables functionality. */
    uint16_t            inhibitTime;
    /** Time between periodic transmissions of the PDO in milliseconds.
    Zero disables functionality. */
    uint16_t            eventTimer;
    uint8_t             transmissionType;
    /** Used with numbered SYNC messages. Values:
        - 0:       Counter of the SYNC message shall not be processed.
        - 1-240:   The SYNC message with the counter value equal to this value
                   shall be regarded as the first received SYNC message. */
    uint8_t             SYNCStartValue;
}CO_TPDOCommPar_t;

/**
 * Pointers to TPDO communication parameter entries in OD.
 */
//typedef struct{
//    uint8_t             *maxSubIndex;    /**< Equal to 6 */
//    /** Communication object identifier for transmitting message. Meaning of the specific bits:
//        - Bit  0-10: COB-ID for PDO, to change it bit 31 must be set.
//        - Bit 11-29: set to 0 for 11 bit COB-ID.
//        - Bit 30:    If true, rtr are NOT allowed for PDO.
//        - Bit 31:    If true, node does NOT use the PDO. */
//    uint32_t            *COB_IDUsedByTPDO;
//    /** Transmission type. Values:
//        - 0:       Transmiting is synchronous, specification in device profile.
//        - 1-240:   Transmiting is synchronous after every N-th SYNC object.
//        - 241-251: Not used.
//        - 252-253: Transmited only on reception of Remote Transmission Request.
//        - 254:     Manufacturer specific.
//        - 255:     Asinchronous, specification in device profile. */
//    uint8_t             *transmissionType;
//    /** Minimum time between transmissions of the PDO in 100micro seconds.
//    Zero disables functionality. */
//    uint16_t            *inhibitTime;
//    /** Time between periodic transmissions of the PDO in milliseconds.
//    Zero disables functionality. */
//    uint16_t            *eventTimer;
//    /** Used with numbered SYNC messages. Values:
//        - 0:       Counter of the SYNC message shall not be processed.
//        - 1-240:   The SYNC message with the counter value equal to this value
//                   shall be regarded as the first received SYNC message. */
//    uint8_t             *SYNCStartValue;
//}CO_TPDOCommPar_ptrs_t;

        
/**
 * TPDO mapping parameter. The same as record from Object dictionary (index 0x1A00+).
 */
typedef struct{
    /** Actual number of mapped objects from 0 to 8. To change mapped object,
    this value must be 0. */
        uint8_t numberOfMappedObjects;
    /** Location and size of the mapped object. Bit meanings `0xIIIISSLL`:
        - Bit  0-7:  Data Length in bits.
        - Bit 8-15:  Subindex from object distionary.
        - Bit 16-31: Index from object distionary. */
        uint32_t mapped_object_param[8];
        uint8_t *mapPointer[8];
        uint8_t last_sent_data[8];

} CO_TPDOMapPar_t;

//typedef struct{
//    /** Actual number of mapped objects from 0 to 8. To change mapped object,
//    this value must be 0. */
//    uint8_t             *numberOfMappedObjects;
//    /** Location and size of the mapped object. Bit meanings `0xIIIISSLL`:
//        - Bit  0-7:  Data Length in bits.
//        - Bit 8-15:  Subindex from object distionary.
//        - Bit 16-31: Index from object distionary. */
//    uint32_t            *mappedObjects[8];
////    uint32_t            *mappedObject2;  /**< Same */
////    uint32_t            *mappedObject3;  /**< Same */
////    uint32_t            *mappedObject4;  /**< Same */
////    uint32_t            *mappedObject5;  /**< Same */
////    uint32_t            *mappedObject6;  /**< Same */
////    uint32_t            *mappedObject7;  /**< Same */
////    uint32_t            *mappedObject8;  /**< Same */
//}CO_TPDOMapPar_ptrs_t;
        
/**
 * TPDO object.
 */
typedef struct{
        void *OD;    
        CO_TPDOCommPar_t TPDOCommPar;/**< From CO_TPDO_init() */
        CO_TPDOMapPar_t  TPDOMapPar; /**< From CO_TPDO_init() */
        uint8_t             nodeId;         /**< From CO_TPDO_init() */
        /** Data length of the transmitting PDO message. Calculated from mapping */
        uint8_t             dataLength;
        /** SYNC counter used for PDO sending */
        uint8_t             syncCounter;
        bool                waiting_for_right_sync_cnt_value; 
        /** Inhibit timer used for inhibit PDO sending translated to microseconds */
        uint32_t            inhibitTimer_us_per_cnt;
        /** Event timer used for PDO sending translated to microseconds */
        uint32_t            eventTimer_us_per_cnt;
        void  *CANdev;       /**< From CO_TPDO_init() */
}CO_TPDO_t;

/**
 * Initialize TPDO object.
 *
 * Function must be called in the communication reset section.
 *
 * @param TPDO This object will be initialized.
 * @param em Emergency object.
 * @param SDO SDO object.
 * @param operatingState Pointer to variable indicating CANopen device NMT internal state.
 * @param nodeId CANopen Node ID of this device. If default COB_ID is used, value will be added.
 * @param defaultCOB_ID Default COB ID for this PDO (without NodeId).
 * See #CO_Default_CAN_ID_t
 * @param restrictionFlags Flag bits indicates, how PDO communication
 * and mapping parameters are handled:
 *  - Bit1: If true, communication parameters are writeable only in pre-operational NMT state.
 *  - Bit2: If true, mapping parameters are writeable only in pre-operational NMT state.
 *  - Bit3: If true, communication parameters are read-only.
 *  - Bit4: If true, mapping parameters are read-only.
 * @param TPDOCommPar Pointer to _TPDO communication parameter_ record from Object
 * dictionary (index 0x1400+).
 * @param TPDOMapPar Pointer to _TPDO mapping parameter_ record from Object
 * dictionary (index 0x1600+).
 * @param idx_TPDOCommPar Index in Object Dictionary.
 * @param idx_TPDOMapPar Index in Object Dictionary.
 * @param CANdevTx CAN device used for PDO transmission.
 * @param CANdevTxIdx Index of transmit buffer in the above CAN device.
 *
 * @return #CO_ReturnError_t: CO_ERROR_NO or CO_ERROR_ILLEGAL_ARGUMENT.
 */
CO_ReturnError_t CO_TPDO_init(
        CO_TPDO_t              *TPDO,
        void               *OD,
        uint32_t            COB_IDUsedByTPDO,
        uint8_t                 nodeId,
        void *CANdev);

/**
 * Verify Change of State of the PDO.
 *
 * Function verifies if variable mapped to TPDO has changed its value. Verified
 * are only variables, which has set attribute _CO_ODA_TPDO_DETECT_COS_ in
 * #CO_SDO_OD_attributes_t.
 *
 * Function may be called by application just before CO_TPDO_process() function,
 * for example: `TPDOx->sendRequest = CO_TPDOisCOS(TPDOx); CO_TPDO_process(TPDOx, ....`
 *
 * @param TPDO TPDO object.
 *
 * @return True if COS was detected.
 */
bool CO_TPDOisCOS(CO_TPDO_t *TPDO);


/**
 * Send TPDO message.
 *
 * Function prepares TPDO data from Object Dictionary variables. It should not
 * be called by application, it is called from CO_TPDO_process().
 *
 *
 * @param TPDO TPDO object.
 *
 * @return Same as CO_CANsend().
 */
int16_t CO_TPDOsend(CO_TPDO_t *TPDO);


/**
 * Process transmitting PDO messages.
 *
 * Function must be called cyclically in any NMT state. It prepares and sends
 * TPDO if necessary. If Change of State needs to be detected, function
 * CO_TPDOisCOS() must be called before.
 *
 * @param TPDO This object.
 * @param SYNC SYNC object. Ignored if NULL.
 * @param syncWas True, if CANopen SYNC message was just received or transmitted.
 * @param timeDifference_us Time difference from previous function call in [microseconds].
 */
void CO_TPDO_process(
        CO_TPDO_t              *TPDO,
//        CO_SYNC_t              *SYNC,
        enum co_tpdo_sync_types sync,
        uint8_t  sync_data,
        uint32_t                timeDifference_us);

bool CO_TPDO_is_enabled(CO_TPDO_t *TPDO);
void CO_TPDO_enable(CO_TPDO_t *TPDO);
void CO_TPDO_disable(CO_TPDO_t *TPDO);

CO_ReturnError_t CO_TPDO_set_transmission_type(
        CO_TPDO_t *TPDO,
        uint8_t transmission_type);

CO_ReturnError_t CO_TPDO_set_inhibit_time(
                CO_TPDO_t *TPDO,
                uint16_t inhibit_time);

CO_ReturnError_t CO_TPDO_set_event_time(
                CO_TPDO_t *TPDO,
                uint16_t event_time);

CO_ReturnError_t CO_TPDO_set_sync_start_value(
                CO_TPDO_t *TPDO,
                uint16_t sync_start_value);
        
CO_ReturnError_t CO_TPDO_add_mapping(
                CO_TPDO_t *TPDO,
                uint16_t index,
                uint8_t subindex, 
                uint8_t data_size);
        
#ifdef __cplusplus
}
#endif /*__cplusplus*/

/** @} */

#endif
