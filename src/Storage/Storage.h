/*
 * Storage.h
 *
 *  Created on: Jun 13, 2020
 *      Author: Formulatrix
 */

#ifndef STORAGE_STORAGE_H_
#define STORAGE_STORAGE_H_

#include <Device.h>
#include "stm32f10x.h"
#include <cstdint>

#define ROM_FLASH_MEMORY_ADDRESS 0x0800F800
#define PAGE_SIZE 0x400
#define ROM_FLASH_SIZE_KB 2
#define MAGIC_NUMBER 0x4269

class CStorage
{
public:
	struct Param
	{
		float Scale[2];
		float Offset[2];
		float UpperAlarmFreq;
		float LowerAlarmFreq;
		float Brightness;
	} m_Param;

	void Save();
	void Load();
	void setDefault();

	CStorage()
	{
		memset(&m_Param,0,sizeof(m_Param));
	}
	;
	~CStorage()
	{
	}
	;
private:
	void storeConfig();
};



extern CStorage Storage;

#endif /* STORAGE_STORAGE_H_ */
