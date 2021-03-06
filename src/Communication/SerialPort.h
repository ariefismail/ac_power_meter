/*
 * SerialPort.h
 *
 *  Created on: May 8, 2020
 *      Author: Formulatrix
 */

#ifndef COMMUNICATION_SERIALPORT_H_
#define COMMUNICATION_SERIALPORT_H_

#include <Hal.h>
#include <Timeout.h>
#include <cstdint>
#include <string>
#include <array>

class CSerialPort
{
public:
	CSerialPort();
	virtual ~CSerialPort();
	void Init(IUsart *pUart,ITimer *pTimer);
	void AddFunction(uint8_t opcode, void (*pFunc)(char*, char*));
	void Execute();

public:
	const char DELIMITER[2] = ",";
private:
	static const uint16_t MAX_BUFFER = 322;
	char m_buffer[MAX_BUFFER];
	uint16_t m_index;
	static const uint8_t MAX_OPCODE = 20;

	// our argument is input string and output string
	void (*m_pFunc[MAX_OPCODE])(char*, char*);

	IUsart *m_pUart;
	CTimeout m_Timeout;
};

#endif /* COMMUNICATION_SERIALPORT_H_ */
