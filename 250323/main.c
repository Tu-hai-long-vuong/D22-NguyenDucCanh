#include "stm32f10x.h"                  // Device header
#include "stm32f10x_exti.h"             // Keil::Device:StdPeriph Drivers:EXTI

#define latch GPIO_Pin_1
#define clk   GPIO_Pin_0
#define data  GPIO_Pin_2

#define MSBFIRST 1
#define LSBFIRST 0

void delay(int n);
//void led(void);
void signal(void);
void button(void);
void shiftOut(uint16_t dataPin, uint16_t clockPin, uint16_t bitOrder, uint16_t val);
void sendToShiftRegister(uint8_t dt);

void giot_nuoc(void);
void sanmoi(void);
void led_butterfly(void);

int main (void){
    signal();
//    button();
    
    while (1){
//        if (!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13))){
					giot_nuoc();
					sanmoi();
					led_butterfly();
    }
}

void delay(int n){
    int i, j;
    for (i = 0; i < n; i++){
        for (j = 0; j < n*0x2AF; j++){}
    }
}

void signal(void){
    GPIO_InitTypeDef gpio_init;
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
    
    gpio_init.GPIO_Pin = latch | clk | data;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);
}

void button(void){
    GPIO_InitTypeDef gpio_but;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    gpio_but.GPIO_Pin = GPIO_Pin_13;
    gpio_but.GPIO_Mode = GPIO_Mode_IPU;
    gpio_but.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpio_but);
}

void shiftOut(uint16_t dataPin, uint16_t clockPin, uint16_t bitOrder, uint16_t val){
    int i;
    for (i = 0; i < 8; i++){
        if (bitOrder == MSBFIRST){
            GPIO_WriteBit(GPIOA, dataPin, (val & (1 << (7 - i))) ? Bit_SET : Bit_RESET);
        } else {
            GPIO_WriteBit(GPIOA, dataPin, (val & (1 << i)) ? Bit_SET : Bit_RESET);
        }

        GPIO_WriteBit(GPIOA, clockPin, Bit_SET);
        delay(1);
        GPIO_WriteBit(GPIOA, clockPin, Bit_RESET);
    }
}

void sendToShiftRegister(uint8_t dt){
    GPIO_WriteBit(GPIOA, latch, Bit_RESET);  
    shiftOut(data, clk, MSBFIRST, dt);     
    GPIO_WriteBit(GPIOA, latch, Bit_SET);   
}

void giot_nuoc(void){
	int i;
	uint16_t led1[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 
										0x81, 0x41, 0x21, 0x11, 0x09, 0x05, 0x03, 0x83, 
										0x43, 0x23, 0x13, 0x0B, 0x07, 0x87, 0x47, 0x27, 
										0x17, 0x0F, 0x8F, 0x4F, 0x2F, 0x1F, 0x9F, 0x5F, 
										0x3F, 0xBF, 0x7F, 0xFF};
		for (i = 0; i < 36; i++){
			sendToShiftRegister(led1[i]);
			delay(50);
	}
}

void sanmoi(void){
		int i;
    uint8_t led2[] = {0x81, 0x41, 0x21, 0x11, 0x09, 0x05, 0x03, 0xC1, 
											0x61, 0x31, 0x19, 0x0D, 0x07, 0xE1, 0x71, 0x39, 
											0x1D, 0x0F, 0xF1, 0x79, 0x3D, 0x1F, 0xF9, 0x7D, 
											0x3F, 0xFD, 0x7F, 0xFF}; 
    for (i = 0; i < 27; i++){
        sendToShiftRegister(led2[i]);
        delay(50);
    }
}

void led_butterfly(void) {
    int i;
    uint8_t in[] = {0x18, 0x3C, 0x7E, 0xFF}; 
		uint8_t out[] = {0x7E, 0x3C, 0x18,0x00};
		
		for (i = 0; i < 4; i++){
			sendToShiftRegister(in[i]);
			delay(50);
		}
		
		for (i = 0; i < 4; i++){
			sendToShiftRegister(out[i]);
			delay(50);
		}
}
