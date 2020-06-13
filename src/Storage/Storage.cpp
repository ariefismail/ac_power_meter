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

	uint16_t *pMemory = (uint16_t*)&Params;
	// save all param struct
	// write magic number first
	FLASH_ProgramHalfWord(ROM_FLASH_MEMORY_ADDRESS, MAGIC_NUMBER);
	// then save all config
	for (uint8_t i = 0; i < (sizeof(Params) / sizeof(uint16_t)); i++)
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
		memcpy(&Params, (uint8_t*)(ROM_FLASH_MEMORY_ADDRESS+2), sizeof(Params));
		storeConfig();
	}
}

void CStorage::storeConfig()
{
	for (uint8_t i = 0; i < Dev.AnalogInput.size(); i++)
	{
		Dev.AnalogInput[i].SetConfig(Params.Scale[i], Params.Offset[i]);
	}
	Dev.Dmd.SetBrightness(Params.Brightness);
	Dev.ACFrequencyMeter.SetAlarmThreshold(Params.UpperAlarmFreq, Params.LowerAlarmFreq);
}

void CStorage::setDefault()
{
	for (uint8_t i = 0; i < Dev.AnalogInput.size(); i++)
	{
		Params.Scale[i] = 1;
		Params.Offset[i] = 0;
		Dev.AnalogInput[i].SetConfig(Params.Scale[i], Params.Offset[i]);
	}
	Params.Brightness = 1;
	Dev.Dmd.SetBrightness(Params.Brightness);

	Params.UpperAlarmFreq = 51;
	Params.LowerAlarmFreq = 49;
	Dev.ACFrequencyMeter.SetAlarmThreshold(Params.UpperAlarmFreq, Params.LowerAlarmFreq);

}

CStorage Storage;


