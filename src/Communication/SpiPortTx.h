/*
 * SpiPort.h
 *
 *  Created on: Apr 22, 2020
 *      Author: Formulatrix
 */

#ifndef COMMUNICATION_SPIPORTTX_H_
#define COMMUNICATION_SPIPORTTX_H_

#include <stm32f10x.h>
#include <Hal.h>

class CSpiPortTx
{
public:

	void Init(SPI_TypeDef *pSpi,IDma *pDma,IGpio *pSS);
	CSpiPortTx();
	virtual ~CSpiPortTx();

private:
	SPI_TypeDef *m_pSpi;
	IDma *m_pDma;
	IGpio *m_pSS;

};

#endif /* COMMUNICATION_SPIPORTTX_H_ */
