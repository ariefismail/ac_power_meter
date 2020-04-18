/*
 * STM32F10xUSART.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: Formulatrix
 */

#include <STM32F10xUSART.h>

uint16_t CSTM32F10xUSART::Write(const char* data, const uint16_t length)
{
	uint16_t i = 0;
	char txData;
	for (i = 0; i < length; i++)
	{
		txData = *data++;
		if (!m_TxBuffer.Push(txData)) break;
	}
	return i;
}

uint16_t CSTM32F10xUSART::Write(const char* data)
{
	uint16_t dataSend = 0;
	char txData;
	while (data)
	{
		txData = *data++;
		if (m_TxBuffer.Push(txData)) dataSend++;
		else break;
	}
	return dataSend;
}

bool CSTM32F10xUSART::HasData()
{
	return !m_RxBuffer.IsEmpty();
}

bool CSTM32F10xUSART::Read(unsigned char* pData, uint16_t* pLength)
{
//	for(uint16_t i=0;i<)
//	pData
	return false;
}

char CSTM32F10xUSART::ReadByte()
{
	char data;
	m_RxBuffer.Pop(data);
	return data;
}

void CSTM32F10xUSART::Init(USART_TypeDef *pUsart)
{
	m_pUsart = pUsart;
}

void CSTM32F10xUSART::Execute()
{
	executeRx();
	executeTx();
}

void CSTM32F10xUSART::executeRx()
{
	if (!USART_GetFlagStatus(m_pUsart, USART_FLAG_RXNE)) return;
	char data = USART_ReceiveData(m_pUsart);
	m_RxBuffer.Push(data);
}

void CSTM32F10xUSART::executeTx()
{
	if (m_TxBuffer.IsEmpty()) return;
	if (!USART_GetFlagStatus(m_pUsart, USART_FLAG_TXE)) return;
	char data;
	m_TxBuffer.Pop(data);
	USART_SendData(m_pUsart, data);
}

CSTM32F10xUSART::CSTM32F10xUSART()
{
	// TODO Auto-generated constructor stub

}

CSTM32F10xUSART::~CSTM32F10xUSART()
{
	// TODO Auto-generated destructor stub
}

