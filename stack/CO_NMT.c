/*
 * CANopen NMT and Heartbeat producer object.
 *
 * @file        CO_NMT_Heartbeat.c
 * @ingroup     CO_NMT_Heartbeat
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

#include "CO_Config.h"
#include "CO_driver.h"
#include "CO_OD_interface.h"
#include "CO_NMT.h"

//static bool CO_NMT_is_command(uint8_t command)
//{
//        return (CO_NMT_ENTER_OPERATIONAL == command || CO_NMT_ENTER_STOPPED == command ||
//                CO_NMT_ENTER_PRE_OPERATIONAL == command || CO_NMT_RESET_NODE == command ||
//                CO_RESET_APP == command || CO_NMT_RESET_COMMUNICATION == command);
//}

/*
 * Read received message from CAN module.
 *
 * Function will be called (by CAN receive interrupt) every time, when CAN
 * message with correct identifier will be received. For more information and
 * description of parameters see file CO_driver.h.
 */
int32_t CO_NMT_receive(void *object, const CO_CANrxMsg_t *msg){
    CO_NMT_t *NMT;
    uint8_t nodeId;

    NMT = (CO_NMT_t*)object;   /* this is the correct pointer type of the first argument */

    nodeId = msg->data[1];

    if((msg->DLC == 2) && ((nodeId == 0) || (nodeId == NMT->nodeId))){
        uint8_t command = msg->data[0];
        NMT->requestedState = command; // NMT command will check if command is ok

    }
    return 0;
}


/******************************************************************************/
CO_ReturnError_t CO_NMT_init(
        CO_NMT_t               *NMT,
        uint8_t                 nodeId,
        void                    *OD,
        void (*state_changed_callback)
        (CO_NMT_internalState_t previous_state,
         CO_NMT_internalState_t requested_state),
        uint16_t                CANidRxNMT, // can id
        void *CANdev)
{
    /* verify arguments */
    if(NMT==NULL || CANdev==NULL || NULL == state_changed_callback){
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Configure object variables */
    NMT->operatingState         = CO_NMT_INITIALIZING;
    NMT->nodeId                 = nodeId;
    NMT->OD = OD;
    NMT->state_changed_callback = state_changed_callback;
    NMT->CANdev = CANdev;
    // Does the device have startup capabilities
    void *nmt_startup = CO_OD_find(NMT->OD, 0x1f80);
    
    /* configure NMT CAN reception */
//    CO_CANrxBufferInit(
//            NMT_CANdev,         /* CAN device */
//            NMT_rxIdx,          /* rx buffer index */
//            CANidRxNMT,         /* CAN identifier */
//            0x7FF,              /* mask */
//            0,                  /* rtr */
//            (void*)NMT,         /* object passed to receive function */
//            CO_NMT_receive);    /* this function will process received message */


    /* produce bootup message */
    CO_CANtx_t TXbuff;      /**< CAN transmit buffer */
    TXbuff.ident = CO_CAN_ID_BOOTUP | nodeId;
    TXbuff.DLC = 1;
    TXbuff.data[0] = 0;
    co_driver_send(NMT->CANdev, &TXbuff);

    if (nmt_startup)
    {
            uint32_t configuration = *(uint32_t *) CO_OD_getDataPointer(nmt_startup, 0);
            if (0x2 == configuration)
            {
                    // TODO: Implement

            }
            else if (0x8 == configuration)
            {
                    NMT->operatingState = CO_NMT_OPERATIONAL;
            }
            else if (0x22 == configuration)
            {
                    // TODO: Implement
            }
            else if (0x28 == configuration)
            {
                    // TODO: Implement
            }
            else
            {
                    NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
            }
    }
    else
    {
                    NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
    }
            
    
    NMT->state_changed_callback(CO_NMT_INITIALIZING,
                                NMT->operatingState);
    
    return CO_ERROR_NO;
}

/******************************************************************************/
CO_NMT_reset_cmd_t CO_NMT_process(CO_NMT_t *NMT)
{
        CO_NMT_reset_cmd_t r = CO_RESET_NOT;
        if (NMT->requestedState != NMT->operatingState)
        {
                uint8_t current_operating_state = NMT->operatingState;
                switch(NMT->requestedState) {
                case CO_NMT_ENTER_OPERATIONAL:
                        NMT->operatingState = CO_NMT_OPERATIONAL;
                        if (NMT->state_changed_callback)
                        {
                                NMT->state_changed_callback(current_operating_state,
                                                            NMT->operatingState);
                        }
                        break;
                case CO_NMT_ENTER_STOPPED:
                        NMT->operatingState = CO_NMT_STOPPED;
                        if (NMT->state_changed_callback)
                        {
                                NMT->state_changed_callback(current_operating_state,
                                                            NMT->operatingState);
                        }
                        break;
                case CO_NMT_ENTER_PRE_OPERATIONAL:
                        NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
                        if (NMT->state_changed_callback)
                        {
                                NMT->state_changed_callback(current_operating_state,
                                                            NMT->operatingState);
                        }
                        break;
                case CO_NMT_RESET_NODE:
                        r = CO_RESET_APP;
                        NMT->operatingState = CO_NMT_INITIALIZING;
                        break;
                case CO_NMT_RESET_COMMUNICATION:
                        r = CO_RESET_COMM;
                        NMT->operatingState = CO_NMT_INITIALIZING;
                        break;
                default:
                        // TODO: What to do if unvalid nmt command is received?
                        break;
                }
        }
    
    return r;
}

/******************************************************************************/
CO_NMT_internalState_t CO_NMT_getInternalState(CO_NMT_t *NMT)
{
    if(NMT != NULL){
        return NMT->operatingState;
    }
    return CO_NMT_INITIALIZING;
}

