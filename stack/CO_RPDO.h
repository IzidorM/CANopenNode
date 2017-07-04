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


#ifndef CO_RPDO_H
#define CO_RPDO_H

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


/**
 * RPDO communication parameter. The same as record from Object dictionary (index 0x1400+).
 */
typedef struct{
    uint8_t             maxSubIndex;    /**< Equal to 2 */
    /** Communication object identifier for message received. Meaning of the specific bits:
        - Bit  0-10: COB-ID for PDO, to change it bit 31 must be set.
        - Bit 11-29: set to 0 for 11 bit COB-ID.
        - Bit 30:    If true, rtr are NOT allowed for PDO.
        - Bit 31:    If true, node does NOT use the PDO. */
    uint32_t            COB_IDUsedByRPDO;
    /** Transmission type. Values:
        - 0-240:   Reciving is synchronous, process after next reception of the SYNC object.
        - 241-253: Not used.
        - 254:     Manufacturer specific.
        - 255:     Asynchronous. */
    uint8_t             transmissionType;
}CO_RPDOCommPar_t;

typedef struct{
    uint8_t             *maxSubIndex;    /**< Equal to 2 */
    /** Communication object identifier for message received. Meaning of the specific bits:
        - Bit  0-10: COB-ID for PDO, to change it bit 31 must be set.
        - Bit 11-29: set to 0 for 11 bit COB-ID.
        - Bit 30:    If true, rtr are NOT allowed for PDO.
        - Bit 31:    If true, node does NOT use the PDO. */
    uint32_t            *COB_IDUsedByRPDO;
    /** Transmission type. Values:
        - 0-240:   Reciving is synchronous, process after next reception of the SYNC object.
        - 241-253: Not used.
        - 254:     Manufacturer specific.
        - 255:     Asynchronous. */
    uint8_t             *transmissionType;
}CO_RPDOCommPar_ptrs_t;
        

/**
 * RPDO mapping parameter. The same as record from Object dictionary (index 0x1600+).
 */
typedef struct{
    /** Actual number of mapped objects from 0 to 8. To change mapped object,
    this value must be 0. */
    uint8_t             numberOfMappedObjects;
    /** Location and size of the mapped object. Bit meanings `0xIIIISSLL`:
        - Bit  0-7:  Data Length in bits.
        - Bit 8-15:  Subindex from object distionary.
        - Bit 16-31: Index from object distionary. */
    uint32_t            mappedObject1;
    uint32_t            mappedObject2;  /**< Same */
    uint32_t            mappedObject3;  /**< Same */
    uint32_t            mappedObject4;  /**< Same */
    uint32_t            mappedObject5;  /**< Same */
    uint32_t            mappedObject6;  /**< Same */
    uint32_t            mappedObject7;  /**< Same */
    uint32_t            mappedObject8;  /**< Same */
}CO_RPDOMapPar_t;

typedef struct{
    /** Actual number of mapped objects from 0 to 8. To change mapped object,
    this value must be 0. */
    uint8_t             *numberOfMappedObjects;
    /** Location and size of the mapped object. Bit meanings `0xIIIISSLL`:
        - Bit  0-7:  Data Length in bits.
        - Bit 8-15:  Subindex from object distionary.
        - Bit 16-31: Index from object distionary. */
    uint32_t            *mappedObjects[8];
//    uint32_t            *mappedObject2;  /**< Same */
//    uint32_t            *mappedObject3;  /**< Same */
//    uint32_t            *mappedObject4;  /**< Same */
//    uint32_t            *mappedObject5;  /**< Same */
//    uint32_t            *mappedObject6;  /**< Same */
//    uint32_t            *mappedObject7;  /**< Same */
//    uint32_t            *mappedObject8;  /**< Same */
}CO_RPDOMapPar_ptrs_t;

        
/**
 * RPDO object.
 */
typedef struct{
//    CO_EM_t            *em;             /**< From CO_RPDO_init() */
    void           *OD;            /**< From CO_RPDO_init() */
//    CO_SYNC_t          *SYNC;           /**< From CO_RPDO_init() */
    CO_RPDOCommPar_ptrs_t RPDOCommPar_ptrs;/**< From CO_RPDO_init() */
    CO_RPDOMapPar_ptrs_t  RPDOMapPar_ptrs; /**< From CO_RPDO_init() */
//    uint8_t            *operatingState; /**< From CO_RPDO_init() */
    uint8_t             nodeId;         /**< From CO_RPDO_init() */
//    uint16_t            defaultCOB_ID;  /**< From CO_RPDO_init() */
    uint8_t             restrictionFlags;/**< From CO_RPDO_init() */
    /** True, if PDO is enabled and valid */
    bool              valid;
    /** True, if PDO synchronous (transmissionType <= 240) */
    bool              synchronous;
    /** Data length of the received PDO message. Calculated from mapping */
    uint8_t             dataLength;
    void * RPDO_mapping_object;
    /** Pointers to 8 data objects, where PDO will be copied */
    uint8_t            *mapPointer[8];
    uint32_t (*map_callback[8])(void*);
    /** Variable indicates, if new PDO message received from CAN bus. */
    volatile bool     CANrxNew[2];
    /** 8 data bytes of the received message. */
    uint8_t             CANrxData[2][8];
//    CO_CANmodule_t     *CANdevRx;       /**< From CO_RPDO_init() */
//    uint16_t            CANdevRxIdx;    /**< From CO_RPDO_init() */
    void *CANdev;
}CO_RPDO_t;


/**
 * Initialize RPDO object.
 *
 * Function must be called in the communication reset section.
 *
 * @param RPDO This object will be initialized.
 * @param em Emergency object.
 * @param SDO SDO server object.
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
 * @param RPDOCommPar Pointer to _RPDO communication parameter_ record from Object
 * dictionary (index 0x1400+).
 * @param RPDOMapPar Pointer to _RPDO mapping parameter_ record from Object
 * dictionary (index 0x1600+).
 * @param idx_RPDOCommPar Index in Object Dictionary.
 * @param idx_RPDOMapPar Index in Object Dictionary.
 * @param CANdevRx CAN device for PDO reception.
 * @param CANdevRxIdx Index of receive buffer in the above CAN device.
 *
 * @return #CO_ReturnError_t: CO_ERROR_NO or CO_ERROR_ILLEGAL_ARGUMENT.
 */
CO_ReturnError_t CO_RPDO_init(
        CO_RPDO_t              *RPDO,
//        CO_EM_t                *em,
        void *               OD,
//        CO_SYNC_t              *SYNC,
//        uint8_t                *operatingState,
        uint8_t                 nodeId,
//        uint16_t                defaultCOB_ID,
        uint8_t                 restrictionFlags,
//        const CO_RPDOCommPar_t *RPDOCommPar,
//        const CO_RPDOMapPar_t  *RPDOMapPar,
        uint16_t                idx_RPDOCommPar,
        uint16_t                idx_RPDOMapPar,
        void * CANdev);
//        CO_CANmodule_t         *CANdevRx,
//        uint16_t                CANdevRxIdx);

/**
 * Process received PDO messages.
 *
 * Function must be called cyclically in any NMT state. It copies data from RPDO
 * to Object Dictionary variables if: new PDO receives and PDO is valid and NMT
 * operating state is operational. It does not verify _transmission type_.
 *
 * @param RPDO This object.
 * @param syncWas True, if CANopen SYNC message was just received or transmitted.
 */
void CO_RPDO_process(CO_RPDO_t *RPDO, bool syncWas);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

int32_t CO_RPDO_receive(void *object, const CO_CANrxMsg_t *msg);

/** @} */

#endif
