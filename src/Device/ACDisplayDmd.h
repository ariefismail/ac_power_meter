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

	void SetText(const char *runningText);

	CACDisplayDmd();
	virtual ~CACDisplayDmd();

private:
	bool executeText();
	bool executeAcDisplay();

private:
	enum TextState
	{
		TSAcDisplay,
		TSText
	}m_State;
	CTimeout m_timeout;
	CACFrequencyMeter *m_pAcFreqMeter;
	CAnalogInput *m_pAcVoltage;
	DMD *m_pDmd;
	char m_TextDisplay[1000];
	uint16_t m_TextLenght;
};

#endif /* DEVICE_ACDISPLAYDMD_H_ */
