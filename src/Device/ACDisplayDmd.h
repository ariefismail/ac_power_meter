/*
 * ACDisplayDmd.h
 *
 *  Created on: May 4, 2020
 *      Author: Formulatrix
 */

#ifndef DEVICE_ACDISPLAYDMD_H_
#define DEVICE_ACDISPLAYDMD_H_

#include <DMD/DMD.h>
#include <Hal.h>
#include <Timeout.h>
#include <AnalogInput.h>
#include <ACFrequencyMeter.h>

class CACDisplayDmd
{
public:
	void Init(CACFrequencyMeter *pAcFreqMeter, CAnalogInput *pAcVoltage, DMD *pDmd, ITimer *pTimer);
	void Execute();

	CACDisplayDmd();
	virtual ~CACDisplayDmd();

private:
	CTimeout m_timeout;
	CACFrequencyMeter *m_pAcFreqMeter;
	CAnalogInput *m_pAcVoltage;
	DMD *m_pDmd;
};

#endif /* DEVICE_ACDISPLAYDMD_H_ */
