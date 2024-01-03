#include "Systick.h"

static uint16_t base_tick;
volatile uint8_t flag_1s;

void SystemClock_Configure(void) {
	
	/* MCGOUTCLOCK ca si iesire */
	MCG->C1 |= MCG_C1_CLKS(0);
	
	/* Selectarea ceasului intern */
	MCG->C1 |= MCG_C1_IREFS_MASK;
	
	/* Folosim 48MHz pentru frecventa ceasului intern */
	MCG->C4 |= MCG_C4_DRST_DRS(1);
	MCG->C4 |= MCG_C4_DMX32(1);
	
}

void SystemClockTick_Configure(void){
	
	/* Ne dorim numararea atomica a 1s bazandu-ne pe tickuri de 1ms
	Daca la 1s avem 48000000 de tickuri, rezulta ca pentru 1ms vom avea de 1000 de ori mai putin */
	SysTick->LOAD = (uint32_t)(48000000UL / 1000 - 1UL);
	
	/* Setam prioritatea Systick ca fiind cea mai mare - 0 */
	NVIC_SetPriority(SysTick_IRQn,0);
	/* Resetam numaratorul digital */ 
	SysTick->VAL = 0UL;
	
	/* Folosim ceasul micro-procesorului -48Mhz
	Cand numaratorul ajunge la 0, va porni SysTick_Handler
	Se va incarca valoarea pusa in registrul RELOAD si incepe numaratoarea din nou */
	SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk | 
									SysTick_CTRL_TICKINT_Msk | 
									SysTick_CTRL_ENABLE_Msk); 
}

void SysTick_Handler(void) {
	
	/* Incrementam base_tick la fiecare intrerupere - numara 1ms */
	++base_tick;
	
	/* La 1000 de base_tickuri - setam flagul pentru 1s */
	if(base_tick >= 1000U) {
		base_tick = 0U;
		flag_1s = 1U;
	}
	
}
