#include <stddef.h>
#include <stdio.h>
#include <functional>
#include "stm32f10x.h"

#include <Hal.h>
#include <STM32F10xTimer.h>
#include <STM32F10xUSART.h>
#include <STM32F10xGpio.h>
#include <STM32F10xADC.h>
#include <STM32F10xInputCapture.h>
#include <STM32F10xDMA.h>

#include <ACFrequencyMeter.h>
#include <AnalogInput.h>
#include <HeartBeat.h>

int main(void)
{
	// ----------------- INIT BASIC ---------------------------------------
	// all 72MHz clock and flash init
	SystemInit();
	std::function<void()>pSi = SystemInit;
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
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

	GPIO_InitTypeDef sGpio;
	// heartbeat
	sGpio.GPIO_Pin = GPIO_Pin_13;
	sGpio.GPIO_Speed = GPIO_Speed_2MHz;
	sGpio.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOC, &sGpio); // pc13

	CSTM32F10xTimer MainTimer;
	MainTimer.Init(TIM3);
	CSTM32F10xGpio GpioHeartBeat;
	GpioHeartBeat.Init(GPIOC, 13);
	CHeartBeat HeartBeat;
	HeartBeat.Init(&GpioHeartBeat, &MainTimer, 5000);

	// --------------------------------------------------------------------

	// --------------- INIT EEPROM ----------------------------------------
	sGpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // sda1 scl1
	sGpio.GPIO_Speed = GPIO_Speed_2MHz;
	sGpio.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &sGpio);

	// --------------------------------------------------------------------

	// --------------- UART Initialization -----------------------------
	sGpio.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // tx3 rx3
	sGpio.GPIO_Speed = GPIO_Speed_2MHz;
	sGpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &sGpio);

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
	CSTM32F10xUSART Uart;
	Uart.Init(USART3);
	// -----------------------------------------------------------------

	// ------------------------- Init ADC ------------------------------
	CSTM32F10xADC Adc[2];
	uint16_t adcBuffer[2];
	for (uint16_t i = 0; i < 2; i++)
	{
		Adc[i].Init(&adcBuffer[i]);
	}
	// adc io init
	GPIO_StructInit(&sGpio);
	sGpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // adc0 adc1
	sGpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &sGpio);

	// adc dma init
	DMA_InitTypeDef dma;
	DMA_StructInit(&dma);
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_PeripheralDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_BufferSize = 2;
	dma.DMA_Priority = DMA_Priority_Low;
	dma.DMA_MemoryBaseAddr = (uint32_t)adcBuffer;
	dma.DMA_PeripheralBaseAddr = (uint32_t)ADC1->DR;
	DMA_Init(DMA1_Channel1, &dma);
	DMA_Cmd(DMA1_Channel1, ENABLE);

	// adc peripheral init
	ADC_InitTypeDef adc;
	ADC_StructInit(&adc);
	adc.ADC_ContinuousConvMode = ENABLE;
	adc.ADC_NbrOfChannel = 2;
	adc.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC1, &adc);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	// -----------------------------------------------------------------

	// ------------------------- Init input capture ------------------------------
	TIM_ICInitTypeDef sInputCapture;
	TIM_ICStructInit(&sInputCapture);
	sInputCapture.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM3, &sInputCapture);
	CSTM32F10xInputCapture InputCapture;
	InputCapture.Init(TIM3, TIM_Channel_1);

	// ------------------------- Init p10 ------------------------------
	// spi init
	GPIO_StructInit(&sGpio);
	sGpio.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13; // mosi2 miso2 sck
	sGpio.GPIO_Speed = GPIO_Speed_50MHz;
	sGpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &sGpio);

	// output enable
	GPIO_StructInit(&sGpio);
	sGpio.GPIO_Pin = GPIO_Pin_1;
	sGpio.GPIO_Speed = GPIO_Speed_50MHz;
	sGpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &sGpio);

	// a pa9 , b pa8
	GPIO_StructInit(&sGpio);
	sGpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8; // mosi2 miso2 sck
	sGpio.GPIO_Speed = GPIO_Speed_50MHz;
	sGpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &sGpio);

	// spi DMA init


	// spi init

	// ----------------------------------------------------------------

	// init our basic need !
	// frequency meter
	CACFrequencyMeter ACFrequencyMeter;
	ACFrequencyMeter.Init(&InputCapture);
	// voltage and current measurement
	CAnalogInput AnalogInput[2];
	for(uint16_t i=0 ;i<2;i++)
	{
		AnalogInput[i].Initialize(&Adc[i]);
	}


	while (1)
	{
		ACFrequencyMeter.Execute();

		HeartBeat.Execute();
		Uart.Execute();
	}
}

