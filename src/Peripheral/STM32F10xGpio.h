/*
 * STM32F10xGpio.h
 *
 *  Created on: Apr 18, 2020
 *      Author: Formulatrix
 */

#ifndef STM32F10XGPIO_H_
#define STM32F10XGPIO_H_

#include "stm32f10x.h"
#include <Hal.h>

class CSTM32F10xGpio : public IGpio
{
public:
	void Set();
	void Clear();
	bool Get() const;

	void Init(GPIO_TypeDef *pGpio,uint8_t pinNumber);


	CSTM32F10xGpio();
	virtual ~CSTM32F10xGpio();

private:
	GPIO_TypeDef *m_pGpio;
	uint32_t m_bitMask;
	uint32_t m_clearBitMask;
};

#endif /* STM32F10XGPIO_H_ */
