#ifndef CO_HB_H
#define CO_HB_H

typedef struct{
    uint8_t             nodeId;         /**< CANopen Node ID of this device */
    uint16_t            HBproducerTimer;/**< Internal timer for HB producer */
}CO_HB_t;


/**
 * Initialize NMT and Heartbeat producer object.
 *
 * Function must be called in the communication reset section.
 *
 * @param NMT This object will be initialized.
 * @param emPr Emergency main object.
 * @param nodeId CANopen Node ID of this device.
 * @param firstHBTime Time between bootup and first heartbeat message in milliseconds.
 * If firstHBTime is greater than _Producer Heartbeat time_
 * (object dictionary, index 0x1017), latter is used instead.
 * @param NMT_CANdev CAN device for NMT reception.
 * @param NMT_rxIdx Index of receive buffer in above CAN device.
 * @param CANidRxNMT CAN identifier for NMT message.
 * @param HB_CANdev CAN device for HB transmission.
 * @param HB_txIdx Index of transmit buffer in the above CAN device.
 * @param CANidTxHB CAN identifier for HB message.
 *
 * @return #CO_ReturnError_t CO_ERROR_NO or CO_ERROR_ILLEGAL_ARGUMENT.
 */
CO_ReturnError_t CO_HB_init(
        CO_HB_t               *HB,
        uint8_t                 nodeId);

/**
 * Process received NMT and produce Heartbeat messages.
 *
 * Function must be called cyclically.
 *
 * @param NMT This object.
 * @param timeDifference_ms Time difference from previous function call in [milliseconds].
 * @param HBtime _Producer Heartbeat time_ (object dictionary, index 0x1017).
 * @param NMTstartup _NMT startup behavior_ (object dictionary, index 0x1F80).
 * @param errorRegister _Error register_ (object dictionary, index 0x1001).
 * @param errorBehavior pointer to _Error behavior_ array (object dictionary, index 0x1029).
 *        Object controls, if device should leave NMT operational state.
 *        Length of array must be 6. If pointer is NULL, no calculation is made.
 * @param timerNext_ms Return value - info to OS - see CO_process().
 *
 * @return #CO_NMT_reset_cmd_t
 */
void CO_HB_process(
        CO_HB_t               *HB,
        uint8_t operating_state,
        uint16_t                timeDifference_ms,
        uint16_t                HBtime,
        uint16_t               *timerNext_ms);



#endif
