/*
 * STM32F10xTimer.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: Formulatrix
 */

#include <STM32F10xTimer.h>

void CSTM32F10xTimer::Init(TIM_TypeDef *pTimer)
{
	m_pTimer=pTimer;
}

uint16_t CSTM32F10xTimer::CurrentTicks() const
{
	return m_pTimer->CNT;
}

CSTM32F10xTimer::CSTM32F10xTimer()
{
	// TODO Auto-generated constructor stub

}

CSTM32F10xTimer::~CSTM32F10xTimer()
{
	// TODO Auto-generated destructor stub
}

