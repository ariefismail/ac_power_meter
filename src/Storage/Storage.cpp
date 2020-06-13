/*
 * Storage.cpp
 *
 *  Created on: Jun 13, 2020
 *      Author: Formulatrix
 */
#include <Storage/Storage.h>

void CStorage::Save()
{
	FLASH_Unlock();
	// erase all sector first
	for (uint8_t i = 0; i < ROM_FLASH_SIZE_KB; i++)
		FLASH_ErasePage(ROM_FLASH_MEMORY_ADDRESS + (PAGE_SIZE * i));

	uint16_t *pMemory = (uint16_t*)&m_Param;
	// save all param struct
	// write magic number first
	FLASH_ProgramHalfWord(ROM_FLASH_MEMORY_ADDRESS, MAGIC_NUMBER);
	// then save all config
	for (uint8_t i = 0; i < (sizeof(m_Param) / sizeof(uint16_t)); i++)
		FLASH_ProgramHalfWord(ROM_FLASH_MEMORY_ADDRESS + 2 + (i * 2), *pMemory++);
	FLASH_Lock();
}

void CStorage::Load()
{
	uint16_t magicNumber = *(uint16_t*)ROM_FLASH_MEMORY_ADDRESS;
	if (magicNumber != MAGIC_NUMBER)
	{
		setDefault();
		Save();
	}
	else
	{
		memcpy(&m_Param, (uint8_t*)(ROM_FLASH_MEMORY_ADDRESS+2), sizeof(m_Param));
		storeConfig();
	}
}

void CStorage::storeConfig()
{
	for (uint8_t i = 0; i < Dev.AnalogInput.size(); i++)
	{
		Dev.AnalogInput[i].SetConfig(m_Param.Scale[i], m_Param.Offset[i]);
	}
	Dev.Dmd.SetBrightness(m_Param.Brightness);
	Dev.ACFrequencyMeter.SetAlarmThreshold(m_Param.UpperAlarmFreq, m_Param.LowerAlarmFreq);
}

void CStorage::setDefault()
{
	for (uint8_t i = 0; i < Dev.AnalogInput.size(); i++)
	{
		m_Param.Scale[i] = 1;
		m_Param.Offset[i] = 0;
		Dev.AnalogInput[i].SetConfig(m_Param.Scale[i], m_Param.Offset[i]);
	}
	m_Param.Brightness = 1;
	Dev.Dmd.SetBrightness(m_Param.Brightness);

	m_Param.UpperAlarmFreq = 51;
	m_Param.LowerAlarmFreq = 49;
	Dev.ACFrequencyMeter.SetAlarmThreshold(m_Param.UpperAlarmFreq, m_Param.LowerAlarmFreq);

}

CStorage Storage;


