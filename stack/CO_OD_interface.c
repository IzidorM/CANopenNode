#include "CO_driver.h"
#include "CO_SDO.h"
#include "CO_OD.h"

struct CO_OD {
        uint32_t od_size;
        CO_OD_entry_t const *od;
        CO_OD_extension_t *od_extensions;
};

struct CO_OD OD_MASTER = {0,0,0};
//};

struct CO_OD *CO_OD_interface_init(void)
{
        OD_MASTER.od_size = od_get_od_size();
        OD_MASTER.od = od_get_od_ptr();
        OD_MASTER.od_extensions = od_get_od_ext_ptr();
        return &OD_MASTER;
}

uint16_t CO_OD_find_internal(void *OD, uint16_t index)
{
    /* Fast search in ordered Object Dictionary. If indexes are mixed, this won't work. */
    /* If Object Dictionary has up to 2^N entries, then N is max number of loop passes. */
    uint16_t cur, min, max;

    struct CO_OD *pOD = OD;    
    const CO_OD_entry_t* object = NULL;

    min = 0U;
    max = pOD->od_size - 1U;
    while(min < max){
        cur = (min + max) / 2;
        object = &pOD->od[cur];
        /* Is object matched */
        if(index == object->index){
            return cur;
        }
        if(index < object->index){
            max = cur;
            if(max) max--;
        }
        else
            min = cur + 1U;
    }

    if(min == max){
        object = &pOD->od[min];
        /* Is object matched */
        if(index == object->index){
            return min;
        }
    }

    return 0xffff;
}


void CO_OD_configure(
//        void *OD,
        struct CO_OD *OD,
        uint16_t                index,
        uint32_t    (*pODFunc)(void *ODF_arg),
        void                   *object)
//        uint8_t                *flags,
//        uint8_t                 flagsSize)
{
    struct CO_OD *pOD = OD;
    uint16_t entryNo;

    entryNo = CO_OD_find_internal(pOD, index);
    if(entryNo < 0xFFFFU){
        CO_OD_extension_t *ext = &pOD->od_extensions[entryNo];
//        uint8_t maxSubIndex = pOD->od[entryNo].maxSubIndex;

        ext->pODFunc = pODFunc;
        ext->object = object;
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
    }
}

//uint16_t CO_OD_find(void *OD, uint16_t index)
const void *CO_OD_find(void *OD, uint16_t index)
{
    /* Fast search in ordered Object Dictionary. If indexes are mixed, this won't work. */
    /* If Object Dictionary has up to 2^N entries, then N is max number of loop passes. */
    uint16_t cur, min, max;

    struct CO_OD *pOD = OD;    
    const CO_OD_entry_t* object = NULL;

    min = 0U;
    max = pOD->od_size - 1U;
    while(min < max){
        cur = (min + max) / 2;
        object = &pOD->od[cur];
        /* Is object matched */
        if(index == object->index){
            return object;
        }
        if(index < object->index){
            max = cur;
            if(max) max--;
        }
        else
            min = cur + 1U;
    }

    if(min == max){
        object = &pOD->od[min];
        /* Is object matched */
        if(index == object->index){
            return object;
        }
    }

    return NULL;
}


/******************************************************************************/
//uint16_t CO_OD_getLength(void *OD, uint16_t entryNo, uint8_t subIndex)
uint16_t CO_OD_getLength(const CO_OD_entry_t* object, uint8_t subIndex)
{
//    struct CO_OD *pOD = OD;    
//    const CO_OD_entry_t* object = &pOD->od[entryNo];
//
//    if(entryNo == 0xFFFFU){
//        return 0U;
//    }

        if(NULL == object)
        {
                return 0U;
        }


    if(object->maxSubIndex == 0U){    /* Object type is Var */
        if(object->pData == 0){ /* data type is domain */
            return CO_SDO_BUFFER_SIZE;
        }
        else{
            return object->length;
        }
    }
    else if(object->attribute != 0U){ /* Object type is Array */
        if(subIndex == 0U){
            return 1U;
        }
        else if(object->pData == 0){
            /* data type is domain */
            return CO_SDO_BUFFER_SIZE;
        }
        else{
            return object->length;
        }
    }
    else{                            /* Object type is Record */
        if(((const CO_OD_entryRecord_t*)(object->pData))[subIndex].pData == 0){
            /* data type is domain */
            return CO_SDO_BUFFER_SIZE;
        }
        else{
            return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].length;
        }
    }
//        return 0;
}


/******************************************************************************/
//uint16_t CO_OD_getAttribute(void *OD, uint16_t entryNo, uint8_t subIndex)
uint16_t CO_OD_getAttribute(const CO_OD_entry_t* object, uint8_t subIndex)
{
//    struct CO_OD *pOD = OD;    
//    const CO_OD_entry_t* object = &pOD->od[entryNo];
//
//    if(entryNo == 0xFFFFU){
//        return 0U;
//    }

        if(NULL == object)
        {
                return 0U;
        }

    if(object->maxSubIndex == 0U){   /* Object type is Var */
        return object->attribute;
    }
    else if(object->attribute != 0U){/* Object type is Array */
        uint16_t attr = object->attribute;
        if(subIndex == 0U){
            /* First subIndex is readonly */
            attr &= ~(CO_ODA_WRITEABLE | CO_ODA_RPDO_MAPABLE);
            attr |= CO_ODA_READABLE;
        }
        return attr;
    }
    else{                            /* Object type is Record */
        return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].attribute;
    }
}


/******************************************************************************/
void* CO_OD_getDataPointer(const CO_OD_entry_t* object, uint8_t subIndex)
{
//    struct CO_OD *pOD = OD;    
//    const CO_OD_entry_t* object = &pOD->od[entryNo];
//
//    if(entryNo == 0xFFFFU){
//        return 0;
//    }

        if(NULL == object)
        {
                return 0U;
        }

        
    if(object->maxSubIndex == 0U){   /* Object type is Var */
        return object->pData;
    }
    else if(object->attribute != 0U){/* Object type is Array */
        if(subIndex==0){
            /* this is the data, for the subIndex 0 in the array */
            return (void*) &object->maxSubIndex;
        }
        else if(object->pData == 0){
            /* data type is domain */
            return 0;
        }
        else{
            return (void*)(((int8_t*)object->pData) + ((subIndex-1) * object->length));
        }
    }
    else{                            /* Object Type is Record */
        return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].pData;
    }
}


/******************************************************************************/
//uint8_t* CO_OD_getFlagsPointer(void *OD, uint16_t entryNo, uint8_t subIndex)
//uint8_t* CO_OD_getFlagsPointer(CO_OD_extension_t *od_extension, uint8_t subIndex)
//{
//    if(NULL == od_extension){
//        return 0;
//    }
//
//    return &od_extension->flags[subIndex];
//}

uint8_t CO_OD_getMaxSubindex(const CO_OD_entry_t* object)
{
        return object->maxSubIndex;
}

//void *CO_OD_getExtension(void *OD, const CO_OD_entry_t* object)
//{
//        struct CO_OD *pOD = OD;    
//
//        uint16_t i = CO_OD_find_internal(OD, object->index);
//        return &pOD->od_extensions[i];
//}

void *CO_OD_getCallback(void *OD, const CO_OD_entry_t* object)
{
        struct CO_OD *pOD = OD;    

        uint16_t i = CO_OD_find_internal(OD, object->index);
        if (0xffff == i)
        {
                return NULL;
        }
        return &pOD->od_extensions[i];
}
