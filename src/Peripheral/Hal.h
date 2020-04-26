/*
 * Hal.h
 *
 *  Created on: Apr 18, 2020
 *      Author: Formulatrix
 */

#ifndef PERIPHERAL_HAL_H_
#define PERIPHERAL_HAL_H_

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <array>

class IGpio
{
public:
	virtual void Set() = 0;
	virtual void Clear() = 0;
	virtual bool Get() const = 0;
	void Toggle()
	{
		if (Get()) Clear();
		else Set();
	}
};

class ITimer
{
public:
	virtual uint16_t CurrentTicks() const = 0;
};

class IUsart
{
public:
	virtual uint16_t Write(const char* data, const uint16_t length) = 0;
	virtual uint16_t Write(const char* data) = 0;
	virtual bool HasData() = 0;
	virtual bool Read(unsigned char* pData, uint16_t* pLength) = 0;
	virtual char ReadByte() = 0;
};

class ISpi
{
	virtual void Transfer(char *data,uint16_t lenght) = 0;
};

class IAdc
{
public:
	virtual uint16_t Read() = 0;
};

class IInputCapture
{
public:
	virtual bool IsTriggered() const = 0;
	virtual uint16_t Read() const = 0;
};

class IDma
{
public:
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void SetDirection(uint32_t direction) = 0; // 0 = PerToMem , 1 = MemToPer
	virtual void SetAddress(uint32_t per, uint32_t mem) = 0;
	virtual uint16_t GetDataIndex() = 0;
	virtual void SetBufferSize(uint16_t size) = 0;
	virtual bool IsTransferFinished() = 0;
};

#endif /* PERIPHERAL_HAL_H_ */
