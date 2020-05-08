/*
 * SerialPort.cpp
 *
 *  Created on: May 8, 2020
 *      Author: Formulatrix
 */

#include <Communication/SerialPort.h>

void CSerialPort::Init(IUsart *pUart,ITimer *pTimer)
{
	m_pUart = pUart;
	m_Timeout.Init(pTimer);
	m_Timeout.SetExpiry(5000);
}

void CSerialPort::AddFunction(uint8_t opcode, void (*pFunc)(char*, char*))
{
	if (opcode > MAX_OPCODE) return;
	m_pFunc[opcode] = pFunc;
}

void CSerialPort::Execute()
{
	// reset the index if the '\n' is taking too long
	if(m_Timeout.HasElapsed() && m_index>0)
		m_index = 0;

	if (!m_pUart->HasData()) return;
	m_buffer[m_index] = m_pUart->ReadByte();
	m_Timeout.Reset();

	// if newline detected
	if (m_buffer[m_index] == '\n')
	{
		char *token;
		char returnString[322];
		memset(returnString, 0, sizeof(returnString));
		token = strtok(m_buffer, DELIMITER);
		uint16_t opcode = std::atoi(token);

		if (!m_pFunc[opcode])
		{
			m_index = 0;
			return;
		}

		m_pFunc[opcode](&m_buffer[strlen(m_buffer)+1], returnString);

		// if there is any returned data then send it
		if(strlen(returnString)>0)
		{
			char buf[324];
			sprintf(buf,"%d,",opcode);
			strcat(buf,returnString);
			m_pUart->Write(buf);
		}
		m_index = 0;
	}
	else
	{
		m_index++;
		if (m_index > MAX_BUFFER) m_index = 0;
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

