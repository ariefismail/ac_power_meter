#include <stddef.h>
#include <stdio.h>
#include "stm32f10x.h"

#include <Hal.h>
#include <STM32F10xTimer.h>
#include <STM32F10xUSART.h>
#include <STM32F10xGpio.h>
#include <STM32F10xADC.h>
#include <STM32F10xInputCapture.h>
#include <STM32F10xDMA.h>

#include <Device.h>

int main(void)
{
	// ----------------- INIT BASIC ---------------------------------------
	// all 72MHz clock and flash init
	SystemInit();
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	// peripheral clock enable
	uint32_t apb1Enable = RCC_APB1Periph_SPI2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM2
			| RCC_APB1Periph_USART3;
	RCC_APB1PeriphClockCmd(apb1Enable, ENABLE);
	uint32_t apb2Enable = RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
			| RCC_APB2Periph_GPIOC;
	RCC_APB2PeriphClockCmd(apb2Enable, ENABLE);
	uint32_t ahbEnable = RCC_AHBPeriph_DMA1;
	RCC_AHBPeriphClockCmd(ahbEnable, ENABLE);

	// frequency meter timer init
	TIM_TimeBaseInitTypeDef timer;
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Period = 0xffff;
	timer.TIM_Prescaler = 719; // 10us timer period!
	TIM_TimeBaseInit(TIM3, &timer);
	TIM_Cmd(TIM3, ENABLE);

	// main timer init
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Period = 0xffff;
	timer.TIM_Prescaler = 7199; // 100us timer period!
	TIM_TimeBaseInit(TIM2, &timer);
	TIM_Cmd(TIM2, ENABLE);

	GPIO_InitTypeDef sGpio;
	// heartbeat
	sGpio.GPIO_Pin = GPIO_Pin_13;
	sGpio.GPIO_Speed = GPIO_Speed_2MHz;
	sGpio.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOC, &sGpio); // pc13

	CSTM32F10xTimer MainTimer;
	MainTimer.Init(TIM2);
	CSTM32F10xGpio GpioHeartBeat;
	GpioHeartBeat.Init(GPIOC, 13);
	Dev.HeartBeat.Init(&GpioHeartBeat, &MainTimer, 5000);

	// --------------------------------------------------------------------

	// --------------- INIT EEPROM ----------------------------------------
//	sGpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // sda1 scl1
//	sGpio.GPIO_Speed = GPIO_Speed_2MHz;
//	sGpio.GPIO_Mode = GPIO_Mode_AF_OD;
//	GPIO_Init(GPIOB, &sGpio);

	// --------------------------------------------------------------------

	// --------------- UART Initialization -----------------------------
	sGpio.GPIO_Pin = GPIO_Pin_11; // rx3
	sGpio.GPIO_Speed = GPIO_Speed_2MHz;
	sGpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &sGpio);
	sGpio.GPIO_Pin = GPIO_Pin_10; // tx3
	sGpio.GPIO_Speed = GPIO_Speed_2MHz;
	sGpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &sGpio);

	USART_InitTypeDef usart;
	usart.USART_BaudRate = 38400;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart.USART_Parity = USART_Parity_No;
	usart.USART_StopBits = USART_StopBits_1;
	usart.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &usart);
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
//	 adc io init
	GPIO_StructInit(&sGpio);
	sGpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // adc0 adc1
	sGpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &sGpio);

	// adc dma init
	DMA_InitTypeDef dma;
	DMA_StructInit(&dma);
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_BufferSize = 2;
	dma.DMA_Priority = DMA_Priority_Low;
	dma.DMA_MemoryBaseAddr = (uint32_t)adcBuffer;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
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
	ADC1->CR2 |= 7 << 17; // external trigger on software, NO SPL IMPLEMENTATION, WTF !!!
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	ADC_StartCalibration(ADC1);

	// -----------------------------------------------------------------

	// ------------------------- Init input capture ------------------------------
	GPIO_StructInit(&sGpio);
	sGpio.GPIO_Pin = GPIO_Pin_6; // tim3 channel 1
	sGpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	sGpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &sGpio);

	TIM_ICInitTypeDef sInputCapture;
	TIM_ICStructInit(&sInputCapture);
	sInputCapture.TIM_Channel = TIM_Channel_1;
	sInputCapture.TIM_ICSelection = TIM_ICSelection_DirectTI;
	sInputCapture.TIM_ICPolarity = TIM_ICPolarity_Falling;
	sInputCapture.TIM_ICFilter = 0x9;
	TIM_ICInit(TIM3, &sInputCapture);
	CSTM32F10xInputCapture InputCapture;
	InputCapture.Init(TIM3, TIM_Channel_1);

	// ------------------------- Init p10 ------------------------------
	// spi init
	GPIO_StructInit(&sGpio);
	sGpio.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13; // mosi2 sck2
	sGpio.GPIO_Speed = GPIO_Speed_50MHz;
	sGpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &sGpio);

	// output enable and chip select (SCLK)
	GPIO_StructInit(&sGpio);
	sGpio.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_12;
	sGpio.GPIO_Speed = GPIO_Speed_50MHz;
	sGpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &sGpio);
	CSTM32F10xGpio pOE;
	pOE.Init(GPIOB, 14);
	CSTM32F10xGpio pSS;
	pSS.Init(GPIOB, 12);

	// a pa9 , b pa8
	GPIO_StructInit(&sGpio);
	sGpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
	sGpio.GPIO_Speed = GPIO_Speed_50MHz;
	sGpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &sGpio);
	CSTM32F10xGpio pA;
	pA.Init(GPIOA, 9);
	CSTM32F10xGpio pB;
	pB.Init(GPIOA, 8);

	// spi DMA init
	CSTM32F10xDMA SpiDma;
	SpiDma.Init(DMA1, DMA1_Channel5, DMA_Mode_Normal, DMA_Priority_VeryHigh);

	// spi init
	SPI_InitTypeDef spiStruct;
	SPI_StructInit(&spiStruct);
	spiStruct.SPI_Mode = SPI_Mode_Master;
	spiStruct.SPI_Direction = SPI_Direction_1Line_Tx;
	spiStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	spiStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI2, &spiStruct);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_Cmd(SPI2, ENABLE);

	// ----------------------------------------------------------------

	// init our basic need !
	// frequency meter
	CACFrequencyMeter ACFrequencyMeter;
	ACFrequencyMeter.Init(&InputCapture);

	DMD Dmd(2, 1);
	Dmd.SelectFont(Arial_14);
	Dmd.Init(SPI2, &SpiDma, &MainTimer, &pSS, &pA, &pB, &pOE);
	for (uint16_t i = 0; i < 2; i++)
	{
		Dev.AnalogInput[i].Initialize(&Adc[i], &MainTimer);
	}

	Dev.AcDisplay.Init(&ACFrequencyMeter, &Dev.AnalogInput[0], &Dmd, &MainTimer);

	// initialize our communcation opcode!
	Dev.SerialPort.Init(&Uart);

	auto getAppName = [](char *rx,char *tx)
	{
		const char DELIMITER[2] = ",";
		char *token;
		// get opcode data
		token = strtok(rx,DELIMITER);

		strcpy(tx,token);
		strcat(tx,",AC POWER METER INDO-WARE\n");
	};

	auto setAnalogInputParams = [](char *rx,char *tx)
	{
		const char DELIMITER[2] = ",";
		char *token;
		// dump opcode data
		token = strtok(rx,DELIMITER);

		// fetch string data
		token = strtok(NULL, DELIMITER);
		uint16_t id = std::atoi(token);
		token = strtok(NULL, DELIMITER);
		float scale = std::atof(token);
		token = strtok(NULL, DELIMITER);
		float offset = std::atof(token);

		Dev.AnalogInput[id].SetConfig(scale,offset);

		// no returned data
		*tx=0;
	};

	Dev.SerialPort.AddFunction(0,getAppName);
	Dev.SerialPort.AddFunction(1,setAnalogInputParams);
	while (1)
	{
		Uart.Execute();
		Dev.SerialPort.Execute();

		ACFrequencyMeter.Execute();
		for (uint16_t i = 0; i < 2; i++)
			Dev.AnalogInput[i].Execute();

		Dev.AcDisplay.Execute();
		Dmd.Execute();

		Dev.HeartBeat.Execute();
	}
}

