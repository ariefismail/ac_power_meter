/*
 * STM32F10xGpio.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: Formulatrix
 */

#include <STM32F10xGpio.h>

void CSTM32F10xGpio::Set()
{
	m_pGpio->BSRR |= m_bitMask;
}

void CSTM32F10xGpio::Clear()
{
	m_pGpio->BSRR |= m_clearBitMask;
}

bool CSTM32F10xGpio::Get() const
{
	return m_pGpio->IDR & m_bitMask;
}

void CSTM32F10xGpio::Init(GPIO_TypeDef *pGpio, uint8_t pinNumber)
{
	if (pinNumber > 15) return;
	m_pGpio = pGpio;
	m_bitMask = 1 << pinNumber;
	m_clearBitMask = 1 << (pinNumber + 16);
}

CSTM32F10xGpio::CSTM32F10xGpio()
{
	// TODO Auto-generated constructor stub
	m_pGpio = NULL;
	m_bitMask = 0;

}

CSTM32F10xGpio::~CSTM32F10xGpio()
{
	// TODO Auto-generated destructor stub
}

