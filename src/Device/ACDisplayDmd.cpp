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
	m_timeout.SetExpiry(5000);
}

void CACDisplayDmd::Execute()
{
	switch (m_State)
	{
		case TSAcDisplay:
			if (executeAcDisplay())
			{
				m_pDmd->DrawMarquee(m_TextDisplay, strlen(m_TextDisplay), 0, 0);
				m_timeout.SetExpiry(100);
				m_State = TSText;
			}
			break;

		case TSText:
			if (executeText())
			{
				m_timeout.SetExpiry(5000);
				m_State = TSAcDisplay;
			}
			break;

	}
}

bool CACDisplayDmd::executeText()
{
	static uint16_t loop = 0;
	if (!m_timeout.HasElapsed()) return false;
	m_timeout.Reset();

	m_pDmd->StepMarquee(-1, 0);

	loop++;
	if (loop > m_TextLenght)
	{
		loop = 0;
		return true;
	}

	return false;
}

bool CACDisplayDmd::executeAcDisplay()
{
	static uint16_t loop = 0;
	if (!m_timeout.HasElapsed()) return false;
	m_timeout.Reset();

	uint16_t freq = (uint16_t)(m_pAcFreqMeter->ReadFrequency() * 100.0);
	uint16_t freqHigh = freq / 100;
	uint16_t freqLow = freq % 100;
	char freqDisplay[20];
	memset(freqDisplay, 0, sizeof(freqDisplay));
	sprintf(freqDisplay, "%d.%dHz", freqHigh, freqLow);
	uint16_t acVoltage = (uint16_t)(m_pAcVoltage->ReadFiltered() * 100);
	uint16_t acVoltageHigh = acVoltage / 100;
	uint16_t acVoltageLow = acVoltage % 100;
	char acVoltageDisplay[20];
	memset(acVoltageDisplay, 0, sizeof(acVoltageDisplay));
	sprintf(acVoltageDisplay, "%d.%dV", acVoltageHigh, acVoltageLow);

//	m_pDmd->ClearScreen(0);
	m_pDmd->DrawString(0, 0, freqDisplay, strlen(freqDisplay), 0);
	m_pDmd->DrawString(32, 0, acVoltageDisplay, strlen(acVoltageDisplay), 0);

	if (m_TextLenght > 0) loop++;
	if (loop > 10)
	{
		loop = 0;
		return true;
	}
	return false;
}

void CACDisplayDmd::SetText(const char *runningText)
{
	strcpy(m_TextDisplay, runningText);
	m_TextLenght = strlen(m_TextDisplay);
	m_State = TSAcDisplay;
	m_timeout.SetExpiry(5000);
}

CACDisplayDmd::CACDisplayDmd()
{
	// TODO Auto-generated constructor stub
	m_State = TSAcDisplay;
	m_TextLenght = 0;

}

CACDisplayDmd::~CACDisplayDmd()
{
	// TODO Auto-generated destructor stub
}

