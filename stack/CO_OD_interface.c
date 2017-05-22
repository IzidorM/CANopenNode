#include "CO_driver.h"
#include "CO_SDO.h"

void CO_OD_configure(
//        CO_SDO_t               *SDO,
        void *OD,
        uint16_t                index,
        uint32_t    (*pODFunc)(CO_ODF_arg_t *ODF_arg),
        void                   *object,
        uint8_t                *flags,
        uint8_t                 flagsSize)
{
//    uint16_t entryNo;
//    CO_OD_entry_t OD_start = OD;
//
//    entryNo = CO_OD_find(OD_start, index);
//    if(entryNo < 0xFFFFU){
//        //CO_OD_extension_t *ext = &SDO->ODExtensions[entryNo];
//        uint8_t maxSubIndex = OD[entryNo].maxSubIndex;
//
//        ext->pODFunc = pODFunc;
//        ext->object = object;
//        if((flags != NULL) && (flagsSize != 0U) && (flagsSize == maxSubIndex)){
//            uint16_t i;
//            ext->flags = flags;
//            for(i=0U; i<=maxSubIndex; i++){
//                ext->flags[i] = 0U;
//            }
//        }
//        else{
//            ext->flags = NULL;
//        }
//    }
}

/******************************************************************************/
void *CO_OD_find(void *node, uint16_t index)
{
    /* Fast search in ordered Object Dictionary. If indexes are mixed, this won't work. */
    /* If Object Dictionary has up to 2^N entries, then N is max number of loop passes. */
    struct CO_OD *n = node;
    uint16_t cur, min, max;
    CO_OD_entry_t* object;

    if ((NULL == n) || (NULL == n->od) || (0 == n->od_size))
    {
            return NULL;
    }

    min = 0U;
//    max = SDO->ODSize - 1U;
    max = n->od_size - 1U;

    while(min < max)
    {
            cur = (min + max) / 2;
            object = &n->od[cur];

            /* Is object matched */
            if (index == object->index)
            {
                    return object;
            }
            
            if (index < object->index)
            {
                    max = cur;
                    if (max)
                    {
                            max--;
                    }
            }
            else
            {
                min = cur + 1U;
            }
    }

    if (min == max)
    {
            //object = &SDO->OD[min];
            object = &n->od[min];            

            /* Is object matched */
            if(index == object->index)
            {
                    return object;
            }
    }

    return NULL;
}

/******************************************************************************/
uint16_t CO_OD_getLength(void *n, uint16_t entryNo, uint8_t subIndex)
{
//    const CO_OD_entry_t* object = &SDO->OD[entryNo];
//
//    if(entryNo == 0xFFFFU){
//        return 0U;
//    }
//
//    if(object->maxSubIndex == 0U){    /* Object type is Var */
//        if(object->pData == 0){ /* data type is domain */
//            return CO_SDO_BUFFER_SIZE;
//        }
//        else{
//            return object->length;
//        }
//    }
//    else if(object->attribute != 0U){ /* Object type is Array */
//        if(subIndex == 0U){
//            return 1U;
//        }
//        else if(object->pData == 0){
//            /* data type is domain */
//            return CO_SDO_BUFFER_SIZE;
//        }
//        else{
//            return object->length;
//        }
//    }
//    else{                            /* Object type is Record */
//        if(((const CO_OD_entryRecord_t*)(object->pData))[subIndex].pData == 0){
//            /* data type is domain */
//            return CO_SDO_BUFFER_SIZE;
//        }
//        else{
//            return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].length;
//        }
//    }
        return 0;
}


/******************************************************************************/
uint16_t CO_OD_getAttribute(void *n, uint16_t entryNo, uint8_t subIndex)
{
//    const CO_OD_entry_t* object = &SDO->OD[entryNo];
//
//    if(entryNo == 0xFFFFU){
//        return 0U;
//    }
//
//    if(object->maxSubIndex == 0U){   /* Object type is Var */
//        return object->attribute;
//    }
//    else if(object->attribute != 0U){/* Object type is Array */
//        uint16_t attr = object->attribute;
//        if(subIndex == 0U){
//            /* First subIndex is readonly */
//            attr &= ~(CO_ODA_WRITEABLE | CO_ODA_RPDO_MAPABLE);
//            attr |= CO_ODA_READABLE;
//        }
//        return attr;
//    }
//    else{                            /* Object type is Record */
//        return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].attribute;
//    }
}


/******************************************************************************/
void* CO_OD_getDataPointer(void *n, uint16_t entryNo, uint8_t subIndex)
{
//    const CO_OD_entry_t* object = &SDO->OD[entryNo];
//
//    if(entryNo == 0xFFFFU){
//        return 0;
//    }
//
//    if(object->maxSubIndex == 0U){   /* Object type is Var */
//        return object->pData;
//    }
//    else if(object->attribute != 0U){/* Object type is Array */
//        if(subIndex==0){
//            /* this is the data, for the subIndex 0 in the array */
//            return (void*) &object->maxSubIndex;
//        }
//        else if(object->pData == 0){
//            /* data type is domain */
//            return 0;
//        }
//        else{
//            return (void*)(((int8_t*)object->pData) + ((subIndex-1) * object->length));
//        }
//    }
//    else{                            /* Object Type is Record */
//        return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].pData;
//    }
        return NULL;
}


/******************************************************************************/
uint8_t* CO_OD_getFlagsPointer(void *n, uint16_t entryNo, uint8_t subIndex){
//    CO_OD_extension_t* ext;
//
//    if((entryNo == 0xFFFFU) || (SDO->ODExtensions == 0)){
//        return 0;
//    }
//
//    ext = &SDO->ODExtensions[entryNo];
//
//    return &ext->flags[subIndex];
        return NULL;
}

