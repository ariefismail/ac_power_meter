/*
 * ACDisplayDmd.cpp
 *
 *  Created on: May 4, 2020
 *      Author: Formulatrix
 */

#include <ACDisplayDmd.h>

void CACDisplayDmd::Init(CACFrequencyMeter *pAcFreqMeter, CAnalogInput *pAcVoltage, DMD *pDmd,
		ITimer *pTimer)
{
	m_pAcFreqMeter = pAcFreqMeter;
	m_pAcVoltage = pAcVoltage;
	m_pDmd = pDmd;
	m_timeout.Init(pTimer);
}

void CACDisplayDmd::Execute()
{
	if (!m_timeout.HasElapsed()) return;
	m_timeout.Reset();

	uint16_t freq = (uint16_t)(m_pAcFreqMeter->ReadFrequency()*100.0);
	uint16_t freqHigh = freq / 100;
	uint16_t freqLow = freq % 100;
	char freqDisplay[20];
	memset(freqDisplay,0,sizeof(freqDisplay));
	sprintf(freqDisplay,"%d.%dHz",freqHigh,freqLow);
	uint16_t acVoltage = (uint16_t)(m_pAcVoltage->ReadFiltered()*100);
	uint16_t acVoltageHigh = acVoltage / 100;
	uint16_t acVoltageLow = acVoltage % 100;
	char acVoltageDisplay[20];
	memset(acVoltageDisplay,0,sizeof(acVoltageDisplay));
	sprintf(acVoltageDisplay,"%d.%dHz",acVoltageHigh,acVoltageLow);


	m_pDmd->ClearScreen(1);
	m_pDmd->DrawString(0,0,freqDisplay,strlen(freqDisplay),1);
	m_pDmd->DrawString(0,15,acVoltageDisplay,strlen(acVoltageDisplay),1);
}

CACDisplayDmd::CACDisplayDmd()
{
	// TODO Auto-generated constructor stub

}

CACDisplayDmd::~CACDisplayDmd()
{
	// TODO Auto-generated destructor stub
}

