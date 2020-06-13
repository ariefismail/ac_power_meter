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
	void Init(IInputCapture *pIc,IGpio *pAlarm);
	void Execute();
	float ReadFrequency();
	uint16_t ReadRaw();

	void SetAlarmThreshold(float upper,float lower);
	void GetAlarmThreshold(float *upper,float *lower);

	CACFrequencyMeter();
	virtual ~CACFrequencyMeter();

private:
	void executeAlarm();

private:
	IInputCapture *m_pIc;
	IGpio *m_pAlarm;
	uint16_t m_PrevTimeStamp;
	uint16_t m_CurrentTimeStamp;
//	uint16_t m_Delta;
	float m_Freq;
	CMovAvgBuffer<uint16_t, uint32_t, 100> m_deltaBuffer;
	float m_UpperAlarmThreshold;
	float m_LowerAlarmThreshold;
};

#endif /* SENSOR_ACFREQUENCYMETER_H_ */
