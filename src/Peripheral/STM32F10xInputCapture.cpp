/*
 * STM32F10xInputCapture.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: Formulatrix
 */

#include <STM32F10xInputCapture.h>

void CSTM32F10xInputCapture::Init(TIM_TypeDef *pTim, uint16_t channel)
{
	m_pTim = pTim;
	m_Channel = channel;
}

uint16_t CSTM32F10xInputCapture::Read() const
{
	switch (m_Channel)
	{
		case TIM_Channel_1:
			return TIM_GetCapture1(m_pTim);

		case TIM_Channel_2:
			return TIM_GetCapture2(m_pTim);

		case TIM_Channel_3:
			return TIM_GetCapture3(m_pTim);

		case TIM_Channel_4:
			return TIM_GetCapture4(m_pTim);

	}
	return false;
}

bool CSTM32F10xInputCapture::IsTriggered() const
{
	switch (m_Channel)
	{
		case TIM_Channel_1:
			return TIM_GetFlagStatus(m_pTim, TIM_FLAG_CC1);

		case TIM_Channel_2:
			return TIM_GetFlagStatus(m_pTim, TIM_FLAG_CC2);

		case TIM_Channel_3:
			return TIM_GetFlagStatus(m_pTim, TIM_FLAG_CC3);

		case TIM_Channel_4:
			return TIM_GetFlagStatus(m_pTim, TIM_FLAG_CC4);

	}
	return false;
}

CSTM32F10xInputCapture::CSTM32F10xInputCapture()
{
	// TODO Auto-generated constructor stub

}

CSTM32F10xInputCapture::~CSTM32F10xInputCapture()
{
	// TODO Auto-generated destructor stub
}

