/*
 * STM32F10xTimer.h
 *
 *  Created on: Apr 18, 2020
 *      Author: Formulatrix
 */

#ifndef PERIPHERAL_STM32F10XTIMER_H_
#define PERIPHERAL_STM32F10XTIMER_H_

#include <stm32f10x.h>
#include <Hal.h>

class CSTM32F10xTimer : public ITimer
{
public:
	void Init(TIM_TypeDef *pTimer);
	uint16_t CurrentTicks() const;
	CSTM32F10xTimer();
	virtual ~CSTM32F10xTimer();

private:
	TIM_TypeDef *m_pTimer;
};

#endif /* PERIPHERAL_STM32F10XTIMER_H_ */
