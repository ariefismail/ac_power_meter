/*
 * STM32F10xDMA.cpp
 *
 *  Created on: Apr 20, 2020
 *      Author: Formulatrix
 */

#include <STM32F10xDMA.h>

void CSTM32F10xDMA::Init(DMA_TypeDef *dma, DMA_Channel_TypeDef *pDmaChannel, uint32_t mode,
		uint32_t priority)
{
	m_pDma = dma;
	m_pDmaChannel = pDmaChannel;
	m_DmaStruct.DMA_Mode = mode;
	m_DmaStruct.DMA_Priority = priority;
	DMA_Init(m_pDmaChannel, &m_DmaStruct);

	if(m_pDmaChannel ==  DMA1_Channel1)m_DmaTCflagMask = DMA1_IT_TC1;
	else if(m_pDmaChannel ==  DMA1_Channel2)m_DmaTCflagMask = DMA1_IT_TC2;
	else if(m_pDmaChannel ==  DMA1_Channel3)m_DmaTCflagMask = DMA1_IT_TC3;
	else if(m_pDmaChannel ==  DMA1_Channel4)m_DmaTCflagMask = DMA1_IT_TC4;
	else if(m_pDmaChannel ==  DMA1_Channel5)m_DmaTCflagMask = DMA1_IT_TC5;
	else if(m_pDmaChannel ==  DMA1_Channel6)m_DmaTCflagMask = DMA1_IT_TC6;
	else if(m_pDmaChannel ==  DMA1_Channel7)m_DmaTCflagMask = DMA1_IT_TC7;
	else if(m_pDmaChannel ==  DMA2_Channel1)m_DmaTCflagMask = DMA2_IT_TC1;
	else if(m_pDmaChannel ==  DMA2_Channel2)m_DmaTCflagMask = DMA2_IT_TC2;
	else if(m_pDmaChannel ==  DMA2_Channel3)m_DmaTCflagMask = DMA2_IT_TC3;
	else if(m_pDmaChannel ==  DMA2_Channel4)m_DmaTCflagMask = DMA2_IT_TC4;
	else if(m_pDmaChannel ==  DMA2_Channel5)m_DmaTCflagMask = DMA2_IT_TC5;

}

void CSTM32F10xDMA::Start()
{
	DMA_Cmd(m_pDmaChannel,ENABLE);
}

void CSTM32F10xDMA::Stop()
{
	DMA_Cmd(m_pDmaChannel,DISABLE);
}

void CSTM32F10xDMA::SetDirection(uint32_t direction)
{
	m_DmaStruct.DMA_DIR = direction;
	DMA_Init(m_pDmaChannel, &m_DmaStruct);
}

void CSTM32F10xDMA::SetAddress(uint32_t per, uint32_t mem)
{
	m_DmaStruct.DMA_PeripheralBaseAddr = per;
	m_DmaStruct.DMA_MemoryBaseAddr = mem;
	DMA_Init(m_pDmaChannel, &m_DmaStruct);
}

uint16_t CSTM32F10xDMA::GetDataIndex()
{
	return m_DmaStruct.DMA_BufferSize - DMA_GetCurrDataCounter(m_pDmaChannel);
}

void CSTM32F10xDMA::SetBufferSize(uint16_t size)
{
	m_DmaStruct.DMA_BufferSize = size;
}

bool CSTM32F10xDMA::IsTransferFinished()
{
	if(DMA_GetFlagStatus(m_DmaTCflagMask))
	{
		DMA_ClearFlag(m_DmaTCflagMask);
		return true;
	}
	return false;
}

CSTM32F10xDMA::CSTM32F10xDMA()
{
	// TODO Auto-generated constructor stub
	DMA_StructInit(&m_DmaStruct);
	m_pDma = NULL;

}

CSTM32F10xDMA::~CSTM32F10xDMA()
{
	// TODO Auto-generated destructor stub
}

