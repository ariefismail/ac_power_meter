/*
 * STM32F10xInputCapture.h
 *
 *  Created on: Apr 19, 2020
 *      Author: Formulatrix
 */

#ifndef PERIPHERAL_STM32F10XINPUTCAPTURE_H_
#define PERIPHERAL_STM32F10XINPUTCAPTURE_H_

#include <stm32f10x.h>
#include <Hal.h>

class CSTM32F10xInputCapture : public IInputCapture
{
public:
	void Init(TIM_TypeDef *pTim, uint16_t channel);
	uint16_t Read() const;
	bool IsTriggered() const;

	CSTM32F10xInputCapture();
	virtual ~CSTM32F10xInputCapture();

private:
	TIM_TypeDef *m_pTim;
	uint16_t m_Channel;
};

#endif /* PERIPHERAL_STM32F10XINPUTCAPTURE_H_ */
