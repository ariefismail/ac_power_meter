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

	uint16_t freq = (uint16_t)(m_pAcFreqMeter->ReadFrequency() * 1000.0);
	uint16_t freqDisplay[4];
	freqDisplay[0] = freq / 1000;
	freqDisplay[1] = (freq / 100) % 10;
	freqDisplay[2] = (freq / 10) % 10;
	freqDisplay[3] = freq % 10;

	char freqDisplayBuffer[20];
	memset(freqDisplayBuffer, 0, sizeof(freqDisplayBuffer));
	sprintf(freqDisplayBuffer, "%d%d.%d%d Hz", freqDisplay[0], freqDisplay[1], freqDisplay[2],
			freqDisplay[3]);
	uint16_t acVoltage = (uint16_t)(m_pAcVoltage->ReadFiltered() * 100);
	uint16_t acVoltageDisplay[5];
	acVoltageDisplay[0] = acVoltage / 10000;
	acVoltageDisplay[1] = (acVoltage / 1000) % 10;
	acVoltageDisplay[2] = (acVoltage / 100) % 10;
	acVoltageDisplay[3] = (acVoltage / 10) % 10;
	acVoltageDisplay[4] = acVoltage % 10;

	char acVoltageDisplayBuffer[20];
	memset(acVoltageDisplayBuffer, 0, sizeof(acVoltageDisplayBuffer));
	sprintf(acVoltageDisplayBuffer, "%d%d%d.%d%d V", acVoltageDisplay[0], acVoltageDisplay[1],acVoltageDisplay[2],acVoltageDisplay[3],acVoltageDisplay[4]);

//	m_pDmd->ClearScreen(0);
	m_pDmd->DrawString(15, 7, freqDisplayBuffer, strlen(freqDisplayBuffer), 0);
	m_pDmd->DrawString(15, -1, acVoltageDisplayBuffer, strlen(acVoltageDisplayBuffer), 0);

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

