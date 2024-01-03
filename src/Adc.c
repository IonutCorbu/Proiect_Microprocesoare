#include "Adc.h"
#include "Uart.h"
#include "PWM.h"

#define ADC_CHANNEL (15) /* PORT C PIN 1 - din reference manual, Pinout pag.163*/

void ADC0_Init() {
	
		/* Activarea semnalului de ceas pentru modulul periferic ADC*/
		SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
	
		/* Functia de calibrare*/
		ADC0_Calibrate();
	
		/*Initial, setam pe 0 registrul pentru configurare*/
		ADC0->CFG1 = 0x00;

		/* Selectam modul de conversie pe 16 biti single-ended - MODE(3)
		 Selectam BUS_CLOCK/2 ca frecventa
		 Selectam rata de divizare ADIV(3) => frecventa/8 (BUS_CLOCK/2)/8<2MHz
		 Frecventa ADC trebuie sa fie <4MHz*/
		ADC0->CFG1 |= ADC_CFG1_MODE(3) |
								 ADC_CFG1_ADICLK(1) |
								 ADC_CFG1_ADIV(3);
	
		/* Setam registrul Status and Control 1 la 0 initial pentru a-l configura */
		ADC0->SC1[0] = 0x00;
		/* Setam pe 0 Status and Control 3 */
		ADC0->SC3 = 0x00;

		/* Setam bitul AIEN care activeaza o intrerupere cand este High si COCO este setat*/
		ADC0->SC1[0] |= ADC_SC1_AIEN_MASK;
		
		/* Setam bitul DIFF pe 0 pentru a avea conversii single-ended*/
		ADC0->SC1[0] &= ~ADC_SC1_DIFF_MASK;
		
		/* Setam bitul pentru average hardware (medierea valorile din harware)*/
		ADC0->SC3 &= ~ADC_SC3_AVGE_MASK;
	
}

/* functia de calibrare a convertorului conform paginilor 494-495 din reference manual*/
int ADC0_Calibrate() {
	
	ADC0_CFG1 |= ADC_CFG1_MODE(3)  |  
                ADC_CFG1_ADICLK(1)|  
                ADC_CFG1_ADIV(3);   
	
	ADC0->SC2 &= ~ADC_SC2_ADTRG_MASK;
	
	ADC0->SC3 |= (ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3));
	
	ADC0->SC3 |= ADC_SC3_CAL_MASK;
	
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
	
	if(ADC0->SC3 & ADC_SC3_CALF_MASK){
		return (1);
	}
	
	uint16_t calibration_var = 0x0000;
	
	calibration_var += ADC0->CLP0;
	calibration_var += ADC0->CLP1;
	calibration_var += ADC0->CLP2;
	calibration_var += ADC0->CLP3;
	calibration_var += ADC0->CLP4;
	calibration_var += ADC0->CLPS;
	
	calibration_var /= 2;
	
	calibration_var |= 0x8000;
	
	ADC0->PG = ADC_PG_PG(calibration_var);
	
	calibration_var = 0x0000;
	
	calibration_var += ADC0->CLM0;
	calibration_var += ADC0->CLM1;
	calibration_var += ADC0->CLM2;
	calibration_var += ADC0->CLM3;
	calibration_var += ADC0->CLM4;
	calibration_var += ADC0->CLMS;
	
	calibration_var /= 2;
	
	calibration_var |= 0x8000;
	
	ADC0->MG = ADC_MG_MG(calibration_var);
	
	ADC0->SC3 &= ~ADC_SC3_CAL_MASK;
	
	return (0);
}

uint16_t ADC0_Read(){
	
	/* Conversia incepe odata cu selectarea canalului pe care se vor primi valorile analogice
	Canalul ales in cazul nostru este 15*/
	ADC0->SC1[0] = ADC_SC1_ADCH(ADC_CHANNEL);
	
	/* Folosim un while pentru a seta bitul ADACT -Indicates that a conversion or hardware averaging is in progress-- pag. 471*/
	while(ADC0->SC2 & ADC_SC2_ADACT_MASK);
	
	/* Asteptam intr-un while incheierea conversiei
	Aceasta este incheiata cand bitul COCO este setat*/
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
	
	/* Modulul este dezactivat -- va fi activat la urmatoarea conversie
	Astfel impiedicam citirea continua a valorilor daca nu este nevoie*/
	ADC0->SC1[0] |= ADC_SC1_ADCH(31);
	
	/* Returnam rezultatul conversiei*/
	return (uint16_t) ADC0->R[0];
	
}



