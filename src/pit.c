#include "pit.h"
#include "gpio.h"
#include "shared.h"
#include "uart.h"

volatile uint8_t direction = '0';
void PIT_Init(void) {
	
	/* Activare semnal de ceas pentru perifericul PIT */
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	/* Utilizarea semnalului de ceas pentru tabloul de timere */
	PIT_MCR &= ~PIT_MCR_MDIS_MASK;
	/* Oprirea decrementarii valorilor numaratoarelor in modul debug */
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	/* Setarea valoarea numaratorului de pe canalul 0 */
	/* fiecare culoare in 1,6 sec 0x10F851D ; toate culorile in 1,6 sec 0x43E146*/
	PIT->CHANNEL[0].LDVAL = 0x10F851D;   
	
  /* Activarea intreruperilor pe canalul 0 */
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	/* Activarea timerului de pe canalul 0 */
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	/* Activarea intreruperii mascabile si setarea prioritatiis*/
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn,1<<3);
	NVIC_EnableIRQ(PIT_IRQn);

}

/* Rutina de tratare a intreruperilor de la modulul PIT
*/
void PIT_IRQHandler(void) {
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		
		/* protocolul pentru activarea ledurilor din interfata
		RIGHT_LED_ROTATION = '1'
		LEFT_LED_ROTATION = '2'
		STOP_LEDS='0'
		*/
		
		
		/*Verific daca modulul UART a primit date*/
		if (UART0->S1 & UART_S1_RDRF_MASK){
			/*Setez o variabila pentru directia ledurilor*/
			direction=UART0->D; 
		}
		/* 0x8c = corespondentul MSB inverted pentru '1' transmis din interfata
		 * in functie de directie sunt apelate culorile
		 */
			if(direction == 0x8C){
					switch (ledState) {
									case 0:
											WHITE();
											break;
									case 1:
											DARK_GREEN(); 
											break;
									case 2:
											PINK();
											break;
									case 3:
											BLACK();
											break;
							}
					 /*Incrementez ledstate si il pastrez in interval 
							pentru a avea secventa repetitiva*/
					 ledState = (ledState + 1) % 4; 
				}
			/* 0x4c corespondentul  MSB inverted pentru '2' transmis din interfata*/
			else if(direction == 0X4C){
				switch (ledState) {
								case 0:
										BLACK();
										break;
								case 1:
										PINK();
										break;
								case 2:
										DARK_GREEN();
										break;
								case 3:
										WHITE();
										break;
						}

				 ledState = (ledState + 1) % 4; // Move to the next color state
			}
			/* 0x0c corespondentul MSB inverted pentru '0' transmis din interfata
			*/
			else if(direction == 0x0C)
			{
				STOP_LEDS();
			}
			
		}
	
	
}
