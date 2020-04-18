#include <stddef.h>
#include <stdio.h>
#include "stm32f10x.h"

#include <Hal.h>
#include <STM32F10xTimer.h>
#include <STM32F10xUSART.h>
#include <STM32F10xGpio.h>

#include <HeartBeat.h>

int main(void)
{
	// ----------------- INIT BASIC ---------------------------------------
	// all 72MHz clock and flash init
	SystemInit();
	// peripheral clock enable
	uint32_t apb1Enable = RCC_APB1Periph_SPI2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_USART3;
	RCC_APB1PeriphClockCmd(apb1Enable, ENABLE);
	uint32_t apb2Enable = RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA;
	RCC_APB2PeriphClockCmd(apb2Enable, ENABLE);
	uint32_t ahbEnable = RCC_AHBPeriph_DMA1;
	RCC_AHBPeriphClockCmd(ahbEnable, ENABLE);

	// main timer init
	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Period = 0xffff;
	timer.TIM_Prescaler = 7199; // 100us timer period!
	TIM_TimeBaseInit(TIM3, &timer);
	TIM_Cmd(TIM3, ENABLE);

	GPIO_InitTypeDef gpio;
	// heartbeat
	gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOC, &gpio); // pc13

	CSTM32F10xTimer MainTimer;
	MainTimer.Init(TIM3);
	CSTM32F10xGpio GpioHeartBeat;
	GpioHeartBeat.Init(GPIOC, 13);
	CHeartBeat HeartBeat;
	HeartBeat.Init(&GpioHeartBeat, &MainTimer, 5000);

	// --------------------------------------------------------------------

	// --------------- INIT EEPROM ----------------------------------------
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // sda1 scl1
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &gpio);

	// --------------------------------------------------------------------

	// --------------- UART Initialization -----------------------------
	gpio.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // tx3 rx3
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &gpio);

	CSTM32F10xUSART Uart;

	USART_InitTypeDef usart;
	usart.USART_BaudRate = 115200;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart.USART_Parity = USART_Parity_No;
	usart.USART_StopBits = USART_StopBits_1;
	usart.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &usart);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);

	USART_Cmd(USART3, ENABLE);
	// -----------------------------------------------------------------

	// ------------------------- Init ADC ------------------------------

	// -----------------------------------------------------------------

	// ------------------------- Init P10 ------------------------------
	// init spi
	gpio.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13; // mosi2 miso2 sck
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &gpio);
	// ----------------------------------------------------------------

	while (1)
	{
		HeartBeat.Execute();
		Uart.Execute();
	}
}
