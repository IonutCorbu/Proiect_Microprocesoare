#include "Adc.h"
#include "Uart.h"
#include "PWM.h"
#include "gpio.h"
#include "pit.h"
#include "Systick.h"
#include "shared.h"


int main() {

		float measured_voltage;
		UART0_Initialize(14400);
		PIT_Init();
		SystemClock_Configure();
		SystemClockTick_Configure();
		PWM_init();
    ADC0_Init();
    rgbLED_init();
    
   
		// deoarece servomotorul lucreaza la o frecventa de 50Hz nu are rost sa preluam date de la ADC continuu
		// prin urmare, am folosis systick pentru a prelua date la 1s si dupa ce preluam conversia, mutam si servomotorul
		for(;;) {
			if(flag_1s){
					// aici citim valoarea de la convertorul analogic-digital
					measured_voltage=ADC0_Read();
					// o reducem la spectrul 0-3.3 V astfel incat sa putem reduce valorile graficului la cele transmise de noi
					measured_voltage = ((float)(measured_voltage * 3.3f)) / (float)65535.0;
					// calculam partea zecimala a conversiei
					uint8_t parte_zecimala = (uint8_t) measured_voltage;
					// calculam doar 2 zecimale dupa virgula
					uint8_t parte_fractionara1 = ((uint8_t)(measured_voltage * 10)) % 10;
					uint8_t parte_fractionara2 = ((uint8_t)(measured_voltage * 100)) % 10;
					// pregatim transmisia UART transformand valorile trimise in caractere readable din tabela ASCII
					UART0_Transmit(parte_zecimala + 0x30);
					UART0_Transmit('.');
					UART0_Transmit(parte_fractionara1 + 0x30);
					UART0_Transmit(parte_fractionara2 + 0x30);
					// in functie de valoarea masurata in spectrul 0-3.3 trimitem o valoare pentru a selecta unghiul la care se roteste servomotorul
					if(measured_voltage>=0&&measured_voltage<1)
						PWM_move(0);
					else if(measured_voltage>1&&measured_voltage<=2)
						PWM_move(9);
					else 
						PWM_move(19);
					// resetam flagul de 1s
					flag_1s=0;
				}
		}
}
