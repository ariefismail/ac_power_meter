/*
 * STM32F10xUSART.h
 *
 *  Created on: Apr 18, 2020
 *      Author: Formulatrix
 */

#ifndef PERIPHERAL_STM32F10XUSART_H_
#define PERIPHERAL_STM32F10XUSART_H_

#include "stm32f10x.h"
#include <Hal.h>
#include <CircularFifo.h>

class CSTM32F10xUSART : public IUsart
{
public:
	static const uint16_t RX_BUFFER_SIZE = 322;
	static const uint16_t TX_BUFFER_SIZE = 322;

	uint16_t Write(const char* data, const uint16_t length);
	uint16_t Write(const char* data);
	bool HasData();
	bool Read(unsigned char* pData, uint16_t* pLength);
	char ReadByte();

	void Init(USART_TypeDef *pUsart);
	void Execute();
	CSTM32F10xUSART();
	virtual ~CSTM32F10xUSART();

private:
	void executeRx();
	void executeTx();

private:
	USART_TypeDef *m_pUsart;
	CCircularFifo<char,TX_BUFFER_SIZE>m_TxBuffer;
	CCircularFifo<char,RX_BUFFER_SIZE>m_RxBuffer;
};

#endif /* PERIPHERAL_STM32F10XUSART_H_ */
