#include <stdint.h>
#include "FIRFilter.h"

int main() {
  FIRFilter fir;
  FIRFilter_Init(&fir);
  int16_t input = 100;
  for (int i = 0; i < 100000; i++) {
    input = FIRFilter_Update(&fir, input);
  }
  volatile int16_t output = input;
  
  return 0;
}



