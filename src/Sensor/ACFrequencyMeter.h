/*
 * ACFrequencyMeter.h
 *
 *  Created on: Apr 19, 2020
 *      Author: Formulatrix
 */

#ifndef SENSOR_ACFREQUENCYMETER_H_
#define SENSOR_ACFREQUENCYMETER_H_

#include <Hal.h>

class CACFrequencyMeter
{
public:
	void Init(IInputCapture *pIc);
	void Execute();
	float ReadFrequency();

	CACFrequencyMeter();
	virtual ~CACFrequencyMeter();

private:
	IInputCapture *m_pIc;
	uint16_t m_PrevTimeStamp;
	uint16_t m_CurrentTimeStamp;
	float m_Freq;
};

#endif /* SENSOR_ACFREQUENCYMETER_H_ */
