/*
 * ACFrequencyMeter.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: Formulatrix
 */

#include <ACFrequencyMeter.h>

void CACFrequencyMeter::Init(IInputCapture *pIc, IGpio *pAlarm)
{
	m_pIc = pIc;
	m_pAlarm = pAlarm;
}

void CACFrequencyMeter::Execute()
{
	if (!m_pIc->IsTriggered()) return;
	m_CurrentTimeStamp = m_pIc->Read();
//	m_Delta = (uint16_t)(m_CurrentTimeStamp - m_PrevTimeStamp);
	m_deltaBuffer.AddSample((uint16_t)(m_CurrentTimeStamp - m_PrevTimeStamp));
//	m_Freq = 10000.0f / m_Delta;
	m_PrevTimeStamp = m_CurrentTimeStamp;

	executeAlarm();
}

void CACFrequencyMeter::executeAlarm()
{
	float f = ReadFrequency();
	if (f > m_UpperAlarmThreshold || f < m_LowerAlarmThreshold) m_pAlarm->Set();
	else m_pAlarm->Clear();
}

float CACFrequencyMeter::ReadFrequency()
{
	return 10000.0f / m_deltaBuffer.Average();
}

uint16_t CACFrequencyMeter::ReadRaw()
{
//	return m_Delta;
	return m_deltaBuffer.Average();
}

void CACFrequencyMeter::SetAlarmThreshold(float upper, float lower)
{
	m_UpperAlarmThreshold = upper;
	m_LowerAlarmThreshold = lower;
}

void CACFrequencyMeter::GetAlarmThreshold(float *upper, float *lower)
{
	*upper = m_UpperAlarmThreshold;
	*lower = m_LowerAlarmThreshold;
}

CACFrequencyMeter::CACFrequencyMeter()
{
	// TODO Auto-generated constructor stub
	m_Freq = 0;

}

CACFrequencyMeter::~CACFrequencyMeter()
{
	// TODO Auto-generated destructor stub
}

