#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "FIRFilter.h"

#define SIZE_COEF 65

void FIRFilter_Init(FIRFilter *fir){
    int16_t array[SIZE_COEF] = {  
        322,
        61, -325, -163, 299, 266, -242, -360, 152, 438, -33,
        -490, -113, 507, 281, -480, -465, 399, 656, -255, -846,
        33, 1026, 291, -1187, -769, 1322, 1538, -1423, -3117, 1486,
        10216, 14713, 10216, 1486, -3117, -1423, 1538, 1322, -769, -1187,
        291, 1026, 33, -846, -255, 656, 399, -465, -480, 281,
        507, -113, -490, -33, 438, 152, -360, -242, 266, 299,
        -163, -325, 61, 322
    };

    //assign all necessary information
    fir->buffer = (int16_t*)malloc(1000 * sizeof(int16_t));
    fir->count = SIZE_COEF;
    fir->out = 0;
    fir->size = 1000;

    //set input values to be 0
    for(int i=0; i<fir->size; i++) fir->buffer[i] = 0;
    
    for(int i=0; i<fir->count; i++) fir->array[i] = array[i];
    fir->coef = fir->array;
}

int16_t FIRFilter_Update(FIRFilter *fir, int16_t input) {
    // set output values to be 0
    fir->out = 0;

    //set rounding value to result
    int32_t result = 1 << 14;

    //insert input to the buffer
    fir->buffer[0] = input;

    int count = 0;
    for(int i=0; i<fir->count; i+=5){
        result = result+(int32_t)fir->coef[i]*(int32_t)fir->buffer[count++]
                +(int32_t)fir->coef[i+1]*(int32_t)fir->buffer[count++]
                +(int32_t)fir->coef[i+2]*(int32_t)fir->buffer[count++]
                +(int32_t)fir->coef[i+3]*(int32_t)fir->buffer[count++]
                +(int32_t)fir->coef[i+4]*(int32_t)fir->buffer[count++];

    }
    //move the buffer forward
    memmove(&fir->buffer[1], &fir->buffer[0], (fir->count)*sizeof(int16_t));

    //saturation part
    if(result > 0x3fffffff) result = 0x3fffffff;
    else if(result < -0x40000000) result = -0x40000000;
    fir->out =(int16_t)(result>>15);

    return fir->out;
}
