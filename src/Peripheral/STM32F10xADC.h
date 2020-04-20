/*
 * STM32F10xADC.h
 *
 *  Created on: Apr 19, 2020
 *      Author: Formulatrix
 */

#ifndef PERIPHERAL_STM32F10XADC_H_
#define PERIPHERAL_STM32F10XADC_H_

#include <stm32f10x.h>
#include <Hal.h>

class CSTM32F10xADC : public IAdc
{
public:
	void Init(uint16_t *pAdcBuffer);
	uint16_t Read();
	CSTM32F10xADC();
	virtual ~CSTM32F10xADC();

private:
	uint16_t *m_pAdcBuffer;
};

#endif /* PERIPHERAL_STM32F10XADC_H_ */
