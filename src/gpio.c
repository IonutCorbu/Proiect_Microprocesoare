#include "gpio.h"
#define RED_LED_PIN 18 /*PORT B PIN 18 */
#define GREEN_LED_PIN 19 /*PORT B PIN 19 */
#define BLUE_LED_PIN 1 /*PORT D PIN 1*/
#define SEQUENCE_DURATION_MS 1697    /* Durata secventei în milisecunde*/
#define REPEAT_COUNT 3               /* Numarul de repetari ale secventei */


void rgbLED_init(void){
	/*Activarea ceasului PORTB*/
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; 
	/*Activarea ceasului PORTD*/
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	/*Resetez MUX pentru a seta pinii ca GPIO*/
	PORTB->PCR[RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	/*Setez MUX pentru a configura pinii ca GPIO*/
	PORTB->PCR[RED_LED_PIN] |= PORT_PCR_MUX(1);
	PORTB->PCR[GREEN_LED_PIN] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED_PIN] |= PORT_PCR_MUX(1);
	
	/*Setez directia pinilor ca output*/
	GPIOB->PDDR |= (1 << RED_LED_PIN);  
	GPIOB->PDDR |= (1 << GREEN_LED_PIN);
	GPIOD->PDDR |= (1 << BLUE_LED_PIN);
	
	/*Setez starea initiala a pinilor pe high*/
	GPIOB->PSOR = (1 << RED_LED_PIN); 
	GPIOB->PSOR = (1 << GREEN_LED_PIN);
	GPIOD->PSOR = (1 << BLUE_LED_PIN);
}

/* Functie care opreste ledurile 
 * prin setarea Port Set Output Register
 */
void STOP_LEDS(void){
	GPIOB->PSOR = (1 << RED_LED_PIN); 
	GPIOB->PSOR = (1 << GREEN_LED_PIN);
	GPIOD->PSOR = (1 << BLUE_LED_PIN);
}

/*Functie pentru a obtine lumina alba prin pornirea tuturor ledurilor*/
void WHITE(void){
	GPIOB->PCOR = (1 << RED_LED_PIN); 
	GPIOB->PCOR = (1 << GREEN_LED_PIN);
	GPIOD->PCOR = (1 << BLUE_LED_PIN);
}

/*Functie pentru obtine culoarea negru (le-am stins pe toate)*/
void BLACK(void){
	GPIOB->PSOR = (1 << RED_LED_PIN); 
	GPIOB->PSOR = (1 << GREEN_LED_PIN);
	GPIOD->PSOR = (1 << BLUE_LED_PIN);
}

/* Functie pentru a obtine verde inchis 
 * Se obtine prin aprinderea  repetata si stingerea ledului dupa un scurt delay
 */
void DARK_GREEN(void) {
	volatile uint32_t i;
	uint32_t j;
    /*Sting ledurile rosu si albastru*/
    GPIOB->PSOR = (1 << RED_LED_PIN); 
		GPIOD->PSOR = (1 << BLUE_LED_PIN);
	
	for(j = 0; j < 5000; ++j){
		/*Sting ledul verde*/
		GPIOB->PSOR = (1 << GREEN_LED_PIN);
		/*Creez un delay*/
		for (i = 0; i < 1000; ++i);
		/*Activez ledul verde */
		GPIOB->PCOR = (1 << GREEN_LED_PIN);
	}
	GPIOB->PCOR = (1 << GREEN_LED_PIN);
	
}

/* Functie pentru a obtine culoarea roz 
 * Se obtine prin aprinderea concomitenta a ledului rosu si albastru
 */
void PINK(void){
	GPIOB->PCOR = (1 << RED_LED_PIN);
	GPIOB->PSOR = (1 << GREEN_LED_PIN);
	GPIOD->PCOR = (1 << BLUE_LED_PIN);
}

