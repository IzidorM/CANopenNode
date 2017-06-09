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


#include "CO_driver.h"
#include "CO_Config.h"
#include "CO_HB.h"


/******************************************************************************/
CO_ReturnError_t CO_HB_init( CO_HB_t *HB, uint8_t nodeId)
{
        /* verify arguments */
        if(HB==NULL) {
                return CO_ERROR_ILLEGAL_ARGUMENT;
        }

        /* Configure object variables */
        HB->nodeId                 = nodeId;
        HB->HBproducerTimer        = 0x0;

        return CO_ERROR_NO;
}

/******************************************************************************/
void CO_HB_process(
        CO_HB_t               *HB,
        // If used in canopen compliant device, caller must assure that valid
        // canopen operating_state is used
        uint8_t               operating_state,
        uint16_t                timeDifference_ms,
        uint16_t                HBtime, //ob 0x1017 producer hb time
        uint16_t               *timerNext_ms)
{
    HB->HBproducerTimer += timeDifference_ms;

    /* Heartbeat producer message */
    if((HBtime != 0 && HB->HBproducerTimer >= HBtime)) {

        /* Start from the beginning. If OS is slow, time sliding may occur. 
           However, heartbeat is
           not for synchronization, it is for health report. */
        HB->HBproducerTimer = 0;

        // produce hb msg
        CO_CANtx_t TXbuff;
        void *HB_CANdev;
        TXbuff.ident = CO_CAN_ID_HEARTBEAT | HB->nodeId;
        TXbuff.DLC = 1;
        // not all devices need to be canopen compliant, so operating_state is
        // not checked to leave the possibility to add some vendor specific states...
        TXbuff.data[0] = operating_state;
        CO_CANsend(HB_CANdev, &TXbuff);
    }

    /* Calculate, when next Heartbeat needs to be send and lower timerNext_ms if necessary. */
    if(HBtime != 0 && timerNext_ms != NULL){
        if(HB->HBproducerTimer < HBtime){
            uint16_t diff = HBtime - HB->HBproducerTimer;
            if(*timerNext_ms > diff){
                *timerNext_ms = diff;
            }
        }else{
            *timerNext_ms = 0;
        }
    }
}

