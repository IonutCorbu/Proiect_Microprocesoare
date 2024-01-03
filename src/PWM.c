#include "PWM.h"
#define TPM_PIN (1) /* PORT B , PIN 1 - Pinout, PG.163 */

void PWM_init(void) {
  /* Activarea semnalului de ceas pentru portul B */
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	/* Utilizarea alternativei de functionare pentru perifericul TPM - TPM1_CH1, alternativa 3 */
	PORTB->PCR[TPM_PIN] |= PORT_PCR_MUX(3);
	
	/* Folosim frecventa MCGFLLCLK - 48 MHz */
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	/* Activarea semnalului de ceas pentru modulul TPM */
	SIM->SCGC6 |= SIM_SCGC6_TPM1(1);
	
	/* Ne dorim o perioada de 20 ms => MOD/frecventa=20/1000=0.02 */
	TPM1->MOD = 375 * 20;	
	
	/* Edge-aligned PWM, High True pulses */
	/* Clear on match, set on reload */
	TPM1->CONTROLS[1].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);
}


void PWM_move(uint32_t value)
{
		/* Oprim timerul*/
		TPM1->SC = 0; 
		/* Resetam valoarea numaratorului */
		TPM1->CNT = 0x0000;

		/* Setam duty cycle pentru PWM in functie de valoarea primita (selectam unghiul la care se intoarce servomotorul) */ 
		TPM1->CONTROLS[1].CnV = 37 * 5  + 37 * value;
		
		/* Folosim un divizor de frecventa => PWM CLK -> 48MHz / 128 = 48.000.000 / 128 [Hz] = 375.000 [Hz] = 375 kHz PG. 553 */
		TPM1->SC |= TPM_SC_PS(7);
	
		/* Selectam modul up-counting PG. 553 */
		TPM1->SC |= TPM_SC_CPWMS(0);
		
		/* LPTPM counter creste la fiecare semnal de ceas */
		TPM1->SC |= TPM_SC_CMOD(1);	
}
