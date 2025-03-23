# D22-NguyenDucCanh

## A. Công việc đã làm

### 1. Các kiểu dữ liệu

- Struct: dựa vào allocate của biến đầu tiên

```c 
struct kt {
	char a, // 4 byte
	char b, // 4 byte
	char c, // 1 byte
	char d // 4 byte
};
```

- Static: giới hạn scope khi trong file

### 2. Blink led PC13 trên kit stm32f103c8t6

- Header
```c
#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
```

- Hàm delay tương đối

```c
void delay_ms(int time){
	int i, j;
	for (i = 0; i < time; i++){
		for (j = 0; j < time*80; j++){
			
		}
	}
}
```

- Hàm kích Port C và cấu hình chân 13

```c
void GPIO_Configure(void){
	GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
}
```

- Hàm điều khiển led dựa trên setbits và resetbits

```c 
void blink (void){
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	delay_ms(100);
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	delay_ms(100);
}
```

- Hàm main

```c 
int main(void){	
	GPIO_Configure();
	while (1){
		blink();
	}
}
```

**Lưu ý:** Trong KeilC dòng cuối cùng sẽ phải cách ra nếu không chương trình sẽ báo lỗi
