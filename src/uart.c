#include "uart.h"
#include "shared.h"

void UART0_Transmit(uint8_t data) {
	/*Asteapta pana cand bufferul de transmisie e gol*/
	while(!(UART0->S1 & UART_S1_TDRE_MASK)) {}
	/*Trimite un byte*/
	UART0->D = data;
}

uint8_t UART0_Receive(void) {
	while(!(UART0->S1 & UART_S1_RDRF_MASK)) {}
	return UART0->D;
}

void UART0_Initialize(uint32_t baud_rate) {
	uint16_t osr = 32;  /* Over Sampling Rate */
	uint16_t sbr;   /* Baud Rate Setting */
	
	/*Setarea sursei de ceas pentru modulul UART */
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	
	/*Activarea semnalului de ceas pentru portul A
	intrucat dorim sa folosim pinii PTA1, respectiv PTA2 
	pentru comunicarea UART*/
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	UART0->C2 &= ~((UART0_C2_RE_MASK) | (UART0_C2_TE_MASK)); 
	/*Setarea sursei de ceas pentru modulul UART */
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(01);
	
	/*Configurarea pinilor PTA1 si PTA 2 pentru comunicatia UART*/
	PORTA->PCR[1] = ~PORT_PCR_MUX_MASK;
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2);
	PORTA->PCR[2] = ~PORT_PCR_MUX_MASK;
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2);
	
	/* Setarea Baud Rate
	48MHz = frecventa ceasului pentru modulul UART
	*/
	sbr = 48000000UL / ((osr + 1)*baud_rate);
	UART0->BDH &= UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(sbr>>8);
	UART0->BDL = UART_BDL_SBR(sbr);
	UART0->C4 |= UART0_C4_OSR(osr - 1);
	UART0->C1 = UART0_C1_M(0) | UART0_C1_PE(0);	

	/*Setare MSB first*/
	UART0->S2 = UART0_S2_MSBF(1);
	
	/*Dezactivare intreruperi la transmisie */
	UART0->C2 |= UART0_C2_TIE(0);
	UART0->C2 |= UART0_C2_TCIE(0);
	
	/*Dezactivare intreruperi la receptie */
	UART0->C2 |= UART0_C2_RIE(0);
	/*Activare UART pentru receptie si transmisie*/
	UART0->C2 |= ((UART_C2_RE_MASK) | (UART_C2_TE_MASK));
	
}

