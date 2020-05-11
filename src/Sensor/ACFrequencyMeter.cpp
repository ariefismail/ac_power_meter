/*
 * ACFrequencyMeter.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: Formulatrix
 */

#include <ACFrequencyMeter.h>

void CACFrequencyMeter::Init(IInputCapture *pIc)
{
	m_pIc = pIc;
}

void CACFrequencyMeter::Execute()
{
	if (!m_pIc->IsTriggered()) return;
	m_CurrentTimeStamp = m_pIc->Read();
	m_Delta = (uint16_t)(m_CurrentTimeStamp - m_PrevTimeStamp);
	m_Freq = 10.0f / m_Delta;
	m_PrevTimeStamp = m_CurrentTimeStamp;
}

float CACFrequencyMeter::ReadFrequency()
{
	return m_Freq;
}

uint16_t CACFrequencyMeter::ReadRaw()
{
	return m_Delta;
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

