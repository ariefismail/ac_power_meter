/*
 * ACFrequencyMeter.h
 *
 *  Created on: Apr 19, 2020
 *      Author: Formulatrix
 */

#ifndef SENSOR_ACFREQUENCYMETER_H_
#define SENSOR_ACFREQUENCYMETER_H_

#include <Hal.h>
#include <MovAvgBuffer.h>

class CACFrequencyMeter
{
public:
	void Init(IInputCapture *pIc);
	void Execute();
	float ReadFrequency();
	uint16_t ReadRaw();

	CACFrequencyMeter();
	virtual ~CACFrequencyMeter();

private:
	IInputCapture *m_pIc;
	uint16_t m_PrevTimeStamp;
	uint16_t m_CurrentTimeStamp;
//	uint16_t m_Delta;
	float m_Freq;
	CMovAvgBuffer<uint16_t, uint32_t, 100> m_deltaBuffer;
};

#endif /* SENSOR_ACFREQUENCYMETER_H_ */
