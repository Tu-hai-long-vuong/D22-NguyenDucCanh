#include "stm32f10x.h"                  // Device header

void GPIO_Config(){
	RCC -> APB2ENR |= 0x00000004;
	GPIOA -> CRL &= 0xFF0FFFFF;
	/*MODE5[1:0] : 11 Speed_50MHz*/
	GPIOA -> CRL |= 0x00300000;
}

void delay_ms(int t){
	int i, j;
	for (i = 0; i < t; i++){
		for (j = 0; j < 8000*t; j++){
			
		}
	}
}

int main (void){
	while (1){
	
	}
}
		