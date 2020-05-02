/*
 * DmdDisplay.h
 *
 *  Created on: Apr 26, 2020
 *      Author: Formulatrix
 */

#ifndef CONTROL_ACDMDDISPLAY_H_
#define CONTROL_ACDMDDISPLAY_H_

#include <Hal.h>
#include <ACFrequencyMeter.h>
#include <AnalogInput.h>
#include <DMD/DMD.h>

class CACDmdDisplay
{
public:
//	void Init(DMD *pDmd, CACFrequencyMeter *pFmeter, CAnalogInput *pAnalogInput,ITimer);
	void Execute();
	CACDmdDisplay();
	virtual ~CACDmdDisplay();

private:
	DMD *m_pDmd;
	CACFrequencyMeter *m_pFmeter;
	CAnalogInput *m_pAnalogInput;
	CTimeout m_Timer;
};

#endif /* CONTROL_ACDMDDISPLAY_H_ */
