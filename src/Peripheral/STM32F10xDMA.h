/*
 * STM32F10xDMA.h
 *
 *  Created on: Apr 20, 2020
 *      Author: Formulatrix
 */

#ifndef PERIPHERAL_STM32F10XDMA_H_
#define PERIPHERAL_STM32F10XDMA_H_

#include <stm32f10x.h>
#include <Hal.h>

class CSTM32F10xDMA : public IDma
{
public:
	void Init(DMA_TypeDef *dma,DMA_Channel_TypeDef *pDmaChannel,uint32_t mode,uint32_t priority);
	void Start();
	void Stop();
	void SetDirection(uint32_t direction); // 0 = PerToMem , 1 = MemToPer
	void SetAddress(uint32_t src, uint32_t dst);
	uint16_t GetDataIndex();
	void SetBufferSize(uint16_t size);
	bool IsTransferFinished();
	CSTM32F10xDMA();
	virtual ~CSTM32F10xDMA();
private:
	DMA_TypeDef *m_pDma;
	DMA_Channel_TypeDef *m_pDmaChannel;
	DMA_InitTypeDef m_DmaStruct;
	uint32_t m_DmaTCflagMask;
};

#endif /* PERIPHERAL_STM32F10XDMA_H_ */
