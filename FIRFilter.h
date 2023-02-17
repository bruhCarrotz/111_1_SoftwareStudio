#include <stdint.h>

#define SIZE_COEF 65

typedef struct
{
    int16_t out;
    int16_t *coef;
    int16_t *buffer;
    int count; 
    int size;
    int16_t array[SIZE_COEF];

} FIRFilter;

void FIRFilter_Init(FIRFilter *fir);
int16_t FIRFilter_Update(FIRFilter *fir, int16_t input);
