#include "MKL25Z4.h"

void PIT_Init(void); 
void PIT_IRQHandler(void); 

static volatile uint32_t ledState = 0;

