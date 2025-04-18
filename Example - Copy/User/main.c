/*
============ BAI TAP LON KY THUAT VI XU LY =============

************STM32 + LCD 4-BIT + MQ2 + BUZZER************
		Date:	16/04/2025
		
		Created By Nguyen Duc Canh ft Hoang Tuan Anh

********************************************************
*/

#include "stm32f10x.h"
#include <stdio.h>
#include <math.h>

#define LCD_RS      GPIO_Pin_13
#define LCD_RW      GPIO_Pin_14
#define LCD_EN      GPIO_Pin_15
#define LCD_D4      GPIO_Pin_8
#define LCD_D5      GPIO_Pin_9
#define LCD_D6      GPIO_Pin_10
#define LCD_D7      GPIO_Pin_11

#define V_REF       5.0f        // Voltage (5V)
#define RL_VALUE    10000.0f    // Rt: 10k Ohm
#define R0_VALUE    10000.0f    // Varaible Resistor

static __IO uint32_t usTicks;

// ========================= Delay Functions =========================
void SysTick_Handler(void) {
    if (usTicks != 0) usTicks--;
}

void DelayInit(void) {
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000000);
}

void DelayUs(uint32_t us) {
    usTicks = us;
    while (usTicks);
}

void DelayMs(uint32_t ms) {
    while (ms--) DelayUs(1000);
}

// ========================= GPIO Config =========================
void GPIO_Config(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef gpio;
    // C?u hình chân cho LCD D4-D7 (PA8-PA11)
    gpio.GPIO_Pin = LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    
    // C?u hình chân cho LCD RS, RW, EN (PB13-PB15)
    gpio.GPIO_Pin = LCD_RS | LCD_RW | LCD_EN;
    GPIO_Init(GPIOB, &gpio);

}

// ========================= ADC Config =========================
void ADC_Config(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    ADC_InitTypeDef adc;
    adc.ADC_Mode 															= ADC_Mode_Independent;
    adc.ADC_ScanConvMode 											= DISABLE;
    adc.ADC_ContinuousConvMode 								= ENABLE;
    adc.ADC_ExternalTrigConv 									= ADC_ExternalTrigConv_None;
    adc.ADC_DataAlign 												= ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel 											= 1;
    ADC_Init(ADC1, &adc);
    
    // Cau hinh kenh ADC cho cam bien MQ2
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_Cmd(ADC1, ENABLE);
    
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

uint16_t Read_Gas_Value(void) {
    return ADC_GetConversionValue(ADC1);
}

// ========================= LCD Functions =========================
void LCD_Enable(void) {
    GPIO_SetBits(GPIOB, LCD_EN);
    DelayMs(1);
    GPIO_ResetBits(GPIOB, LCD_EN);
    DelayMs(1);
}

void LCD_Send4Bit(uint8_t data) {
    GPIO_WriteBit(GPIOA, LCD_D4, data & 0x01);
    GPIO_WriteBit(GPIOA, LCD_D5, (data >> 1) & 0x01);
    GPIO_WriteBit(GPIOA, LCD_D6, (data >> 2) & 0x01);
    GPIO_WriteBit(GPIOA, LCD_D7, (data >> 3) & 0x01);
}

void LCD_SendCommand(uint8_t cmd) {
    GPIO_ResetBits(GPIOB, LCD_RS); // L?nh
    GPIO_ResetBits(GPIOB, LCD_RW); // Ch? d? ghi
    LCD_Send4Bit(cmd >> 4);
    LCD_Enable();
    LCD_Send4Bit(cmd & 0x0F);
    LCD_Enable();
}

void LCD_PutChar(char c) {
    GPIO_SetBits(GPIOB, LCD_RS); // D? li?u
    GPIO_ResetBits(GPIOB, LCD_RW);
    LCD_Send4Bit(c >> 4);
    LCD_Enable();
    LCD_Send4Bit(c & 0x0F);
    LCD_Enable();
}

void LCD_Puts(char* s) {
    while (*s) {
        LCD_PutChar(*s++);
    }
}

void LCD_Gotoxy(uint8_t x, uint8_t y) {
    uint8_t address = (y == 0) ? (0x80 + x) : (0xC0 + x);
    LCD_SendCommand(address);
}

void LCD_Clear(void) {
    LCD_SendCommand(0x01);
    DelayMs(2);
}

void LCD_Init(void) {
    LCD_Send4Bit(0x00);
    DelayMs(20);
    LCD_Send4Bit(0x03);
    LCD_Enable();
    LCD_Enable();
    LCD_Enable();
    LCD_Send4Bit(0x02);
    LCD_Enable();
    LCD_SendCommand(0x28); // Ch? d? 4-bit, 2 dòng, font 5x8
    LCD_SendCommand(0x0C); // B?t hi?n th?, t?t con tr?
    LCD_SendCommand(0x06); // Entry mode: tang con tr?, không dich
    LCD_Clear();
}

// ========================= PWM Buzzer Functions using TIM2 (Channel 2 on PA1) =========================
void PWM_Buzzer_Init(void) {
    // Bat clock cho TIMER2 và GPIOA 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_1; // PA1: TIM2_CH2
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
}

void Buzzer_SetFrequency(uint16_t freq) {
    uint16_t prescaler 									= 720 - 1;
    uint16_t period 										= (100000 / freq) - 1;
    
    TIM_TimeBaseInitTypeDef tim;
    tim.TIM_Prescaler 									= prescaler;
    tim.TIM_Period 											= period;
    tim.TIM_CounterMode 								= TIM_CounterMode_Up;
    tim.TIM_ClockDivision 							= TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &tim);
    
    TIM_OCInitTypeDef oc;
    oc.TIM_OCMode 											= TIM_OCMode_PWM1;
    oc.TIM_OutputState 									= TIM_OutputState_Enable;
    oc.TIM_Pulse 												= period / 2;  // 50% duty cycle
    oc.TIM_OCPolarity 									= TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &oc);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    TIM_Cmd(TIM2, ENABLE);
}

void Buzzer_Stop(void) {
    TIM_Cmd(TIM2, DISABLE);
}

// ========================= Conversion Functions =========================
float ADC_to_Voltage(uint16_t adc_value) {
    return ((float)adc_value * V_REF) / 4095.0f;
}

float Calculate_RS(float Vout) {
    if (Vout == 0) return 1000000;  // Tránh chia cho 0
    return RL_VALUE * ((V_REF - Vout) / Vout);
}

float Calculate_PPM(float RS) {
    float ratio = RS / R0_VALUE;
    // Công th?c m?u: ppm = 100 * (ratio)^(-2.3)
    return 100.0f * pow(ratio, -2.3f);
}

// ========================= MAIN =========================
int main(void) {
    char str[16];
    uint16_t adc_value;
    float voltage, RS, ppm;
    
    DelayInit();
    GPIO_Config();
    ADC_Config();
    LCD_Init();
    PWM_Buzzer_Init();
    
    while (1) {
        adc_value 						= Read_Gas_Value();
        voltage 							= ADC_to_Voltage(adc_value);
        RS 										= Calculate_RS(voltage);
        ppm 									= Calculate_PPM(RS);
        
        if (ppm > 200) {  // High
            LCD_Clear();
            LCD_Gotoxy(0, 0);
            LCD_Puts("GAS DETECTED!");
            // Buzzer sound at 1500Hz 
            Buzzer_SetFrequency(1500);
            LCD_Gotoxy(0, 1);
            sprintf(str, "Gas: %.2f ppm", ppm);
            LCD_Puts(str);
        }
        else if (ppm > 100) {  // Average 
            Buzzer_Stop();
            LCD_Clear();
            LCD_Gotoxy(0, 0);
            LCD_Puts("CAUTION");
            LCD_Gotoxy(0, 1);
            sprintf(str, "Gas: %.2f ppm", ppm);
            LCD_Puts(str);
        }
        else { // LOW
            Buzzer_Stop();
            LCD_Clear();
            LCD_Gotoxy(0, 0);
            LCD_Puts("SAFE");
            LCD_Gotoxy(0, 1);
            sprintf(str, "Gas: %.2f ppm", ppm);
            LCD_Puts(str);
        }
        DelayMs(1000);
    }
}
