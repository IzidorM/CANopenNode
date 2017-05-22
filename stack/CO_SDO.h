/**
 * CANopen Service Data Object - server protocol.
 *
 * @file        CO_SDO.h
 * @ingroup     CO_SDO
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


#ifndef CO_SDO_H
#define CO_SDO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "CO_OD_interface.h"
        
/**
 * @defgroup CO_SDO SDO server
 * @ingroup CO_CANopen
 * @{
 *
 * CANopen Service Data Object - server protocol.
 *
 * Service data objects (SDOs) allow the access to any entry of the CANopen
 * Object dictionary. An SDO establishes a peer-to-peer communication channel
 * between two devices. In addition, the SDO protocol enables to transfer any
 * amount of data in a segmented way. Therefore the SDO protocol is mainly used
 * in order to communicate configuration data.
 *
 * All CANopen devices must have implemented SDO server and first SDO server
 * channel. Servers serves data from Object dictionary. Object dictionary
 * is a collection of variables, arrays or records (structures), which can be
 * used by the stack or by the application. This file (CO_SDO.h) implements
 * SDO server.
 *
 * SDO client can be (optionally) implemented on one (or multiple, if multiple
 * SDO channels are used) device in CANopen network. Usually this is master
 * device and provides also some kind of user interface, so configuration of
 * the network is possible. Code for the SDO client is in file CO_SDOmaster.h.
 *
 * SDO communication cycle is initiated by the client. Client can upload (read) data
 * from device or can download (write) data to device. If data are less or equal
 * of 4 bytes long, communication is finished by one server response (expedited
 * transfer). If data are longer, they are split into multiple segments of
 * request/response pairs (normal or segmented transfer). For longer data there
 * is also a block transfer protocol, which transfers larger block of data in
 * secure way with little protocol overhead. If error occurs during SDO transfer
 * #CO_SDO_abortCode_t is send by client or server and transfer is terminated.
 */


/**
 * @defgroup CO_SDO_messageContents SDO message contents
 *
 * Excerpt from CiA DS301, V4.2.
 *
 * For CAN identifier see #CO_Default_CAN_ID_t
 *
 * Expedited transfer is used for transmission of up to 4 data bytes. It consists
 * of one SDO request and one response. For longer variables is used segmented
 * or block transfer.
 *
 * ####Initiate SDO download (client request)
 *  - byte 0:       SDO command specifier. 8 bits: `0010nnes` (nn: if e=s=1,
 *                  number of data bytes, that do *not* contain data; e=1 for
 *                  expedited transfer; s=1 if data size is indicated).
 *  - byte 1..2:    Object index.
 *  - byte 3:       Object subIndex.
 *  - byte 4..7:    Expedited data or data size if segmented transfer.
 *
 * ####Initiate SDO download (server response)
 *  - byte 0:       SDO command specifier. 8 bits: `01100000`.
 *  - byte 1..2:    Object index.
 *  - byte 3:       Object subIndex.
 *  - byte 4..7:    reserved.
 *
 * ####Download SDO segment (client request)
 *  - byte 0:       SDO command specifier. 8 bits: `000tnnnc` (t: toggle bit set
 *                  to 0 in first segment; nnn: number of data bytes, that do
 *                  *not* contain data; c=1 if this is the last segment).
 *  - byte 1..7:    Data segment.
 *
 * ####Download SDO segment (server response)
 *  - byte 0:       SDO command specifier. 8 bits: `001t0000` (t: toggle bit set
 *                  to 0 in first segment).
 *  - byte 1..7:    Reserved.
 *
 * ####Initiate SDO upload (client request)
 *  - byte 0:       SDO command specifier. 8 bits: `01000000`.
 *  - byte 1..2:    Object index.
 *  - byte 3:       Object subIndex.
 *  - byte 4..7:    Reserved.
 *
 * ####Initiate SDO upload (server response)
 *  - byte 0:       SDO command specifier. 8 bits: `0100nnes` (nn: if e=s=1,
 *                  number of data bytes, that do *not* contain data; e=1 for
 *                  expedited transfer; s=1 if data size is indicated).
 *  - byte 1..2:    Object index.
 *  - byte 3:       Object subIndex.
 *  - byte 4..7:    reserved.
 *
 * ####Upload SDO segment (client request)
 *  - byte 0:       SDO command specifier. 8 bits: `011t0000` (t: toggle bit set
 *                  to 0 in first segment).
 *  - byte 1..7:    Reserved.
 *
 * ####Upload SDO segment (server response)
 *  - byte 0:       SDO command specifier. 8 bits: `000tnnnc` (t: toggle bit set
 *                  to 0 in first segment; nnn: number of data bytes, that do
 *                  *not* contain data; c=1 if this is the last segment).
 *  - byte 1..7:    Data segment.
 *
 * ####Abort SDO transfer (client or server)
 *  - byte 0:       SDO command specifier. 8 bits: `10000000`.
 *  - byte 1..2:    Object index.
 *  - byte 3:       Object subIndex.
 *  - byte 4..7:    #CO_SDO_abortCode_t.
 *
 * ####Block transfer
 *     See DS301 V4.2.
 */


/**
 * SDO abort codes.
 *
 * Send with Abort SDO transfer message.
 *
 * The abort codes not listed here are reserved.
 */
typedef enum{
    CO_SDO_AB_NONE                  = 0x00000000UL, /**< 0x00000000, No abort */
    CO_SDO_AB_TOGGLE_BIT            = 0x05030000UL, /**< 0x05030000, Toggle bit not altered */
    CO_SDO_AB_TIMEOUT               = 0x05040000UL, /**< 0x05040000, SDO protocol timed out */
    CO_SDO_AB_CMD                   = 0x05040001UL, /**< 0x05040001, Command specifier not valid or unknown */
    CO_SDO_AB_BLOCK_SIZE            = 0x05040002UL, /**< 0x05040002, Invalid block size in block mode */
    CO_SDO_AB_SEQ_NUM               = 0x05040003UL, /**< 0x05040003, Invalid sequence number in block mode */
    CO_SDO_AB_CRC                   = 0x05040004UL, /**< 0x05040004, CRC error (block mode only) */
    CO_SDO_AB_OUT_OF_MEM            = 0x05040005UL, /**< 0x05040005, Out of memory */
    CO_SDO_AB_UNSUPPORTED_ACCESS    = 0x06010000UL, /**< 0x06010000, Unsupported access to an object */
    CO_SDO_AB_WRITEONLY             = 0x06010001UL, /**< 0x06010001, Attempt to read a write only object */
    CO_SDO_AB_READONLY              = 0x06010002UL, /**< 0x06010002, Attempt to write a read only object */
    CO_SDO_AB_NOT_EXIST             = 0x06020000UL, /**< 0x06020000, Object does not exist */
    CO_SDO_AB_NO_MAP                = 0x06040041UL, /**< 0x06040041, Object cannot be mapped to the PDO */
    CO_SDO_AB_MAP_LEN               = 0x06040042UL, /**< 0x06040042, Number and length of object to be mapped exceeds PDO length */
    CO_SDO_AB_PRAM_INCOMPAT         = 0x06040043UL, /**< 0x06040043, General parameter incompatibility reasons */
    CO_SDO_AB_DEVICE_INCOMPAT       = 0x06040047UL, /**< 0x06040047, General internal incompatibility in device */
    CO_SDO_AB_HW                    = 0x06060000UL, /**< 0x06060000, Access failed due to hardware error */
    CO_SDO_AB_TYPE_MISMATCH         = 0x06070010UL, /**< 0x06070010, Data type does not match, length of service parameter does not match */
    CO_SDO_AB_DATA_LONG             = 0x06070012UL, /**< 0x06070012, Data type does not match, length of service parameter too high */
    CO_SDO_AB_DATA_SHORT            = 0x06070013UL, /**< 0x06070013, Data type does not match, length of service parameter too short */
    CO_SDO_AB_SUB_UNKNOWN           = 0x06090011UL, /**< 0x06090011, Sub index does not exist */
    CO_SDO_AB_INVALID_VALUE         = 0x06090030UL, /**< 0x06090030, Invalid value for parameter (download only). */
    CO_SDO_AB_VALUE_HIGH            = 0x06090031UL, /**< 0x06090031, Value range of parameter written too high */
    CO_SDO_AB_VALUE_LOW             = 0x06090032UL, /**< 0x06090032, Value range of parameter written too low */
    CO_SDO_AB_MAX_LESS_MIN          = 0x06090036UL, /**< 0x06090036, Maximum value is less than minimum value. */
    CO_SDO_AB_NO_RESOURCE           = 0x060A0023UL, /**< 0x060A0023, Resource not available: SDO connection */
    CO_SDO_AB_GENERAL               = 0x08000000UL, /**< 0x08000000, General error */
    CO_SDO_AB_DATA_TRANSF           = 0x08000020UL, /**< 0x08000020, Data cannot be transferred or stored to application */
    CO_SDO_AB_DATA_LOC_CTRL         = 0x08000021UL, /**< 0x08000021, Data cannot be transferred or stored to application because of local control */
    CO_SDO_AB_DATA_DEV_STATE        = 0x08000022UL, /**< 0x08000022, Data cannot be transferred or stored to application because of present device state */
    CO_SDO_AB_DATA_OD               = 0x08000023UL, /**< 0x08000023, Object dictionary not present or dynamic generation fails */
    CO_SDO_AB_NO_DATA               = 0x08000024UL  /**< 0x08000024, No data available */
}CO_SDO_abortCode_t;


/**
 * @defgroup CO_SDO_objectDictionary Object dictionary
 *
 * CANopen Object dictionary implementation in CANopenNode.
 *
 * CANopen Object dictionary is a collection of different data items, which can
 * be used by the stack or by the application.
 *
 * Each Object dictionary entry is located under 16-bit index, as specified
 * by the CANopen:
 *  - 0x0001..0x025F: Data type definitions.
 *  - 0x1000..0x1FFF: Communication profile area.
 *  - 0x2000..0x5FFF: Manufacturer-specific profile area.
 *  - 0x6000..0x9FFF: Standardized device profile area for eight logical devices.
 *  - 0xA000..0xAFFF: Standardized network variable area.
 *  - 0xB000..0xBFFF: Standardized system variable area.
 *  - Other:          Reserved.
 *
 * If Object dictionary entry has complex data type (array or structure),
 * then 8-bit subIndex specifies the sub-member of the entry. In that case
 * subIndex 0x00 is encoded as uint8_t and specifies the highest available
 * subIndex with that entry. Subindex 0xFF has special meaning in the standard
 * and is not supported by CANopenNode.
 *
 * ####Object type of one Object dictionary entry
 *  - NULL:         Not used by CANopenNode.
 *  - DOMAIN:       Block of data of variable length. Data and length are
 *                  under control of the application.
 *  - DEFTYPE:      Definition of CANopen basic data type, for example
 *                  INTEGER16.
 *  - DEFSTRUCT:    Definition of complex data type - structure, which is
 *                  used with RECORD.
 *  - VAR:          Variable of CANopen basic data type. Subindex is 0.
 *  - ARRAY:        Array of multiple variables of the same CANopen basic
 *                  data type. Subindex 1..arrayLength specifies sub-member.
 *  - RECORD:       Record or structure of multiple variables of different
 *                  CANopen basic data type. Subindex specifies sub-member.
 *
 *
 * ####Implementation in CANopenNode
 * Object dictionary in CANopenNode is implemented in CO_OD.h and CO_OD.c files.
 * These files are application specific and must be generated by Object
 * dictionary editor (application is included by the stack).
 *
 * CO_OD.h and CO_OD.c files include:
 *  - Structure definitions for records.
 *  - Global declaration and initialization of all variables, arrays and records
 *    mapped to Object dictionary. Variables are distributed in multiple objects,
 *    depending on memory location. This eases storage to different memories in
 *    microcontroller, like eeprom or flash.
 *  - Constant array of multiple Object dictionary entries of type
 *    CO_OD_entry_t. If object type is record, then entry includes additional
 *    constant array with members of type CO_OD_entryRecord_t. Each OD entry
 *    includes information: index, maxSubIndex, #CO_SDO_OD_attributes_t, data size and
 *    pointer to variable.
 *
 *
 * Function CO_SDO_init() initializes object CO_SDO_t, which includes SDO
 * server and Object dictionary. 
 *
 * Application doesn't need to know anything about the Object dictionary. It can
 * use variables specified in CO_OD.h file directly. If it needs more control
 * over the CANopen communication with the variables, it can configure additional
 * functionality with function CO_OD_configure(). Additional functionality
 * include: @ref CO_SDO_OD_function and #CO_SDO_OD_flags_t.
 * 
 * Interface to Object dictionary is provided by following functions: CO_OD_find() 
 * finds OD entry by index, CO_OD_getLength() returns length of variable, 
 * CO_OD_getAttribute returns attribute and CO_OD_getDataPointer() returns pointer 
 * to data. These functions are used by SDO server and by PDO configuration. They 
 * can also be used to access the OD by index like this.
 * 
 * \code{.c}
 * index = CO_OD_find(CO->SDO[0], OD_H1001_ERR_REG);
 * if (index == 0xffff) {
 *     return;
 * }
 * length = CO_OD_getLength(CO->SDO[0], index, 1);
 * if (length != sizeof(new_data)) {
 *    return;
 * }
 *
 * p = CO_OD_getDataPointer(CO->SDO[0], index, 1);
 * if (p == NULL) {
 *     return;
 * }
 * CO_LOCK_OD();
 * *p = new_data;
 * CO_UNLOCK_OD();
 * \endcode
 * 
 * Be aware that accessing the OD directly using CO_OD.h files is more CPU 
 * efficient as CO_OD_find() has to do a search everytime it is called.
 * 
 */


/**
 * @defgroup CO_SDO_OD_function Object Dictionary function
 *
 * Optional application specific function, which may manipulate data downloaded
 * or uploaded via SDO.
 *
 * Object dictionary function is external function defined by application or
 * by other stack files. It may be registered for specific Object dictionary
 * entry (with specific index). If it is registered, it is called (through
 * function pointer) from SDO server. It may verify and manipulate data during
 * SDO transfer. Object dictionary function can be registered by function
 * CO_OD_configure().
 *
 * ####SDO download (writing to Object dictionary)
 *     After SDO client transfers data to the server, data are stored in internal
 *     buffer. If data contains multibyte variable and processor is big endian,
 *     then data bytes are swapped. Object dictionary function is called if
 *     registered. Data may be verified and manipulated inside that function. After
 *     function exits, data are copied to location as specified in CO_OD_entry_t.
 *
 * ####SDO upload (reading from Object dictionary)
 *     Before start of SDO upload, data are read from Object dictionary into
 *     internal buffer. If necessary, bytes are swapped.
 *     Object dictionary function is called if registered. Data may be
 *     manipulated inside that function. After function exits, data are
 *     transferred via SDO server.
 *
 * ####Domain data type
 *     If data type is domain, then length is not specified by Object dictionary.
 *     In that case Object dictionary function must be used. In case of
 *     download it must store the data in own location. In case of upload it must
 *     write the data (maximum size is specified by length) into data buffer and
 *     specify actual length. With domain data type it is possible to transfer
 *     data, which are longer than #CO_SDO_BUFFER_SIZE. In that case
 *     Object dictionary function is called multiple times between SDO transfer.
 *
 * ####Parameter to function:
 *     ODF_arg     - Pointer to CO_ODF_arg_t object filled before function call.
 *
 * ####Return from function:
 *  - 0: Data transfer is successful
 *  - Different than 0: Failure. See #CO_SDO_abortCode_t.
 */


/**
 * SDO buffer size.
 *
 * Size of the internal SDO buffer.
 *
 * Size must be at least equal to size of largest variable in @ref CO_SDO_objectDictionary.
 * If data type is domain, data length is not limited to SDO buffer size. If
 * block transfer is implemented, value should be set to 889.
 *
 * Value can be in range from 7 to 889 bytes.
 */
    #ifndef CO_SDO_BUFFER_SIZE
        #define CO_SDO_BUFFER_SIZE    32
    #endif



/**
 * Internal states of the SDO server state machine
 */
typedef enum {
    CO_SDO_ST_IDLE                  = 0x00U,
    CO_SDO_ST_DOWNLOAD_INITIATE     = 0x11U,
    CO_SDO_ST_DOWNLOAD_SEGMENTED    = 0x12U,
    CO_SDO_ST_DOWNLOAD_BL_INITIATE  = 0x14U,
    CO_SDO_ST_DOWNLOAD_BL_SUBBLOCK  = 0x15U,
    CO_SDO_ST_DOWNLOAD_BL_SUB_RESP  = 0x16U,
    CO_SDO_ST_DOWNLOAD_BL_END       = 0x17U,
    CO_SDO_ST_UPLOAD_INITIATE       = 0x21U,
    CO_SDO_ST_UPLOAD_SEGMENTED      = 0x22U,
    CO_SDO_ST_UPLOAD_BL_INITIATE    = 0x24U,
    CO_SDO_ST_UPLOAD_BL_INITIATE_2  = 0x25U,
    CO_SDO_ST_UPLOAD_BL_SUBBLOCK    = 0x26U,
    CO_SDO_ST_UPLOAD_BL_END         = 0x27U
} CO_SDO_state_t;



/**
 * SDO server object.
 */
typedef struct{
    /** 8 data bytes of the received message. */
    uint8_t             CANrxData[8];
    /** SDO data buffer of size #CO_SDO_BUFFER_SIZE. */
    uint8_t             databuffer[CO_SDO_BUFFER_SIZE];
    /** Internal flag indicates, that this object has own OD */
    bool_t              ownOD;
    /** Pointer to the @ref CO_SDO_objectDictionary (array) */
//    const CO_OD_entry_t *OD;
//    /** Size of the @ref CO_SDO_objectDictionary */
//    uint16_t            ODSize;
    /** Pointer to array of CO_OD_extension_t objects. Size of the array is
    equal to ODSize. */
    CO_OD_extension_t  *ODExtensions;
    /** Offset in buffer of next data segment being read/written */
    uint16_t            bufferOffset;
    /** Sequence number of OD entry as returned from CO_OD_find() */
    uint16_t            entryNo;
    /** CO_ODF_arg_t object with additional variables. Reference to this object
    is passed to @ref CO_SDO_OD_function */
    CO_ODF_arg_t        ODF_arg;
    /** From CO_SDO_init() */
    uint8_t             nodeId;
    /** Current internal state of the SDO server state machine #CO_SDO_state_t */
    CO_SDO_state_t      state;
    /** Toggle bit in segmented transfer or block sequence in block transfer */
    uint8_t             sequence;
    /** Timeout timer for SDO communication */
    uint16_t            timeoutTimer;
    /** Number of segments per block with 1 <= blksize <= 127 */
    uint8_t             blksize;
    /** True, if CRC calculation by block transfer is enabled */
    bool_t              crcEnabled;
    /** Calculated CRC code */
    uint16_t            crc;
    /** Length of data in the last segment in block upload */
    uint8_t             lastLen;
    /** Indication end of block transfer */
    bool_t              endOfTransfer;
    /** Variable indicates, if new SDO message received from CAN bus */
    bool_t              CANrxNew;
    /** From CO_SDO_initCallback() or NULL */
    void              (*pFunctSignal)(void);
    /** From CO_SDO_init() */
    CO_CANmodule_t     *CANdevTx;
    /** CAN transmit buffer inside CANdev for CAN tx message */
    CO_CANtx_t         *CANtxBuff;
    struct CO_OD OD;
}CO_SDO_t;


/**
 * Helper union for manipulating data bytes.
 */
typedef union{
    uint8_t  u8[8];  /**< 8 bytes */
    uint16_t u16[4]; /**< 4 words */
    uint32_t u32[2]; /**< 2 double words */
}CO_bytes_t;


/**
 * Helper function like memcpy.
 *
 * Function copies n data bytes from source to destination.
 *
 * @param dest Destination location.
 * @param src Source location.
 * @param size Number of data bytes to be copied (max 0xFFFF).
 */
void CO_memcpy(uint8_t dest[], const uint8_t src[], const uint16_t size);


/**
 * Helper function returns uint16 from byte array.
 *
 * @param data Location of source data.
 * @return Variable of type uint16_t.
 */
uint16_t CO_getUint16(const uint8_t data[]);


/**
 * Helper function returns uint32 from byte array.
 *
 * @param data Location of source data.
 * @return Variable of type uint32_t.
 */
uint32_t CO_getUint32(const uint8_t data[]);


/**
 * Helper function writes uint16 to byte array.
 *
 * @param data Location of destination data.
 * @param value Variable of type uint16_t to be written into data.
 */
void CO_setUint16(uint8_t data[], const uint16_t value);


/**
 * Helper function writes uint32 to byte array.
 *
 * @param data Location of destination data.
 * @param value Variable of type uint32_t to be written into data.
 */
void CO_setUint32(uint8_t data[], const uint32_t value);


/**
 * Copy 2 data bytes from source to destination. Swap bytes if
 * microcontroller is big-endian.
 *
 * @param dest Destination location.
 * @param src Source location.
 */
void CO_memcpySwap2(void* dest, const void* src);


/**
 * Copy 4 data bytes from source to destination. Swap bytes if
 * microcontroller is big-endian.
 *
 * @param dest Destination location.
 * @param src Source location.
 */
void CO_memcpySwap4(void* dest, const void* src);


/**
 * Copy 8 data bytes from source to destination. Swap bytes if
 * microcontroller is big-endian.
 *
 * @param dest Destination location.
 * @param src Source location.
 */
void CO_memcpySwap8(void* dest, const void* src);


/**
 * Initialize SDO object.
 *
 * Function must be called in the communication reset section.
 *
 * @param SDO This object will be initialized.
 * @param COB_IDClientToServer COB ID for client to server for this SDO object.
 * @param COB_IDServerToClient COB ID for server to client for this SDO object.
 * @param ObjDictIndex_SDOServerParameter Index in Object dictionary.
 * @param parentSDO Pointer to SDO object, which contains object dictionary and
 * its extension. For first (default) SDO object this argument must be NULL.
 * If this argument is specified, then OD, ODSize and ODExtensions arguments
 * are ignored.
 * @param OD Pointer to @ref CO_SDO_objectDictionary array defined externally.
 * @param ODSize Size of the above array.
 * @param ODExtensions Pointer to the externally defined array of the same size
 * as ODSize.
 * @param nodeId CANopen Node ID of this device.
 * @param CANdevRx CAN device for SDO server reception.
 * @param CANdevRxIdx Index of receive buffer in the above CAN device.
 * @param CANdevTx CAN device for SDO server transmission.
 * @param CANdevTxIdx Index of transmit buffer in the above CAN device.
 *
 * @return #CO_ReturnError_t: CO_ERROR_NO or CO_ERROR_ILLEGAL_ARGUMENT.
 */
CO_ReturnError_t CO_SDO_init(
        CO_SDO_t               *SDO,
        uint32_t                COB_IDClientToServer,
        uint32_t                COB_IDServerToClient,
        uint16_t                ObjDictIndex_SDOServerParameter,
        CO_SDO_t               *parentSDO,
        CO_OD_entry_t const     OD[],
        uint16_t                ODSize,
        CO_OD_extension_t       ODExtensions[],
        uint8_t                 nodeId,
        CO_CANmodule_t         *CANdevRx,
        uint16_t                CANdevRxIdx,
        CO_CANmodule_t         *CANdevTx,
        uint16_t                CANdevTxIdx);


/**
 * Initialize SDOrx callback function.
 *
 * Function initializes optional callback function, which is called after new
 * message is received from the CAN bus. Function may wake up external task,
 * which processes mainline CANopen functions.
 *
 * @param SDO This object.
 * @param pFunctSignal Pointer to the callback function. Not called if NULL.
 */
void CO_SDO_initCallback(
        CO_SDO_t               *SDO,
        void                  (*pFunctSignal)(void));


/**
 * Process SDO communication.
 *
 * Function must be called cyclically.
 *
 * @param SDO This object.
 * @param NMTisPreOrOperational Different than zero, if #CO_NMT_internalState_t is
 * NMT_PRE_OPERATIONAL or NMT_OPERATIONAL.
 * @param timeDifference_ms Time difference from previous function call in [milliseconds].
 * @param SDOtimeoutTime Timeout time for SDO communication in milliseconds.
 * @param timerNext_ms Return value - info to OS - see CO_process().
 *
 * @return 0: SDO server is idle.
 * @return 1: SDO server is in transfer state.
 * @return -1: SDO abort just occurred.
 */
int8_t CO_SDO_process(
        CO_SDO_t               *SDO,
        bool_t                  NMTisPreOrOperational,
        uint16_t                timeDifference_ms,
        uint16_t                SDOtimeoutTime,
        uint16_t               *timerNext_ms);




/**
 * Initialize SDO transfer.
 *
 * Find object in OD, verify, fill ODF_arg s.
 *
 * @param SDO This object.
 * @param index Index of the object in Object dictionary.
 * @param subIndex subIndex of the object in Object dictionary.
 *
 * @return 0 on success, otherwise #CO_SDO_abortCode_t.
 */
uint32_t CO_SDO_initTransfer(CO_SDO_t *SDO, uint16_t index, uint8_t subIndex);


/**
 * Read data from @ref CO_SDO_objectDictionary to internal buffer.
 *
 * ODF_arg s must be initialized before with CO_SDO_initTransfer().
 * @ref CO_SDO_OD_function is called if configured.
 *
 * @param SDO This object.
 * @param SDOBufferSize Total size of the SDO buffer.
 *
 * @return 0 on success, otherwise #CO_SDO_abortCode_t.
 */
uint32_t CO_SDO_readOD(CO_SDO_t *SDO, uint16_t SDOBufferSize);


/**
 * Write data from internal buffer to @ref CO_SDO_objectDictionary.
 *
 * ODF_arg s must be initialized before with CO_SDO_initTransfer().
 * @ref CO_SDO_OD_function is called if configured.
 *
 * @param SDO This object.
 * @param length Length of data (received from network) to write.
 *
 * @return 0 on success, otherwise #CO_SDO_abortCode_t.
 */
uint32_t CO_SDO_writeOD(CO_SDO_t *SDO, uint16_t length);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

/** @} */
#endif
