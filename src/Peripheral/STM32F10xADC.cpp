/*
 * STM32F10xADC.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: Formulatrix
 */

#include <STM32F10xADC.h>

void CSTM32F10xADC::Init(uint16_t *pAdcBuffer)
{
	m_pAdcBuffer = pAdcBuffer;
}

uint16_t CSTM32F10xADC::Read()
{
	return *m_pAdcBuffer;
}

CSTM32F10xADC::CSTM32F10xADC()
{
	// TODO Auto-generated constructor stub

}

CSTM32F10xADC::~CSTM32F10xADC()
{
	// TODO Auto-generated destructor stub
}

