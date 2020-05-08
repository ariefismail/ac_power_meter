/*
 * SerialPort.cpp
 *
 *  Created on: May 8, 2020
 *      Author: Formulatrix
 */

#include <Communication/SerialPort.h>

void CSerialPort::Init(IUsart *pUart)
{
	m_pUart = pUart;
}

void CSerialPort::AddFunction(uint8_t opcode, void (*pFunc)(char*, char*))
{
	if (opcode > MAX_OPCODE) return;
	m_pFunc[opcode] = pFunc;
}

void CSerialPort::Execute()
{
	if (!m_pUart->HasData()) return;
	m_buffer[m_index] = m_pUart->ReadByte();

	// if newline detected
	if (m_buffer[m_index] == '\n')
	{
		char *token;
		char returnString[322];
		memset(returnString, 0, sizeof(returnString));
		token = strtok(m_buffer, DELIMITER);
		uint16_t opcode = std::atoi(token);
		m_index = opcode;

		if (!m_pFunc[opcode])
		{
			m_index = 0;
			return;
		}

		m_pFunc[opcode](m_buffer, returnString);

		// if there is any returned data then send it
		if(strlen(returnString)>0)
		{
			m_pUart->Write(returnString);
		}
		m_index = 0;
	}
	else
	{
		m_index++;
		if (m_index > sizeof(m_buffer)) m_index = 0;
	}
}

CSerialPort::CSerialPort()
{
// TODO Auto-generated constructor stub
	m_index = 0;
	m_pUart = NULL;

}

CSerialPort::~CSerialPort()
{
// TODO Auto-generated destructor stub
}

