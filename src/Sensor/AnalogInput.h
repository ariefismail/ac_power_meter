/*
 * AnalogInput.h
 *
 *  (c) 2016 Formulatrix
 *  All rights reserved
 */

#ifndef ANALOGINPUT_H_
#define ANALOGINPUT_H_

#include <Hal.h>

class CAnalogInput
{
public:
	CAnalogInput();
	virtual ~CAnalogInput();

	void Initialize(IAdc* pAdc);
	void SetConfig(float scale, float offset);
	float Read();
	uint16_t ReadAdc();

private:
	float m_Scale;
	float m_Offset;
	IAdc* m_pAdc;
};

inline CAnalogInput::CAnalogInput()
{
	m_pAdc = NULL;
	m_Scale = 1;
	m_Offset = 0;
}

inline CAnalogInput::~CAnalogInput()
{

}

inline void CAnalogInput::Initialize(IAdc* pAdc)
{
	m_pAdc = pAdc;
}

inline float CAnalogInput::Read()
{
	return m_Scale * m_pAdc->Read() + m_Offset;
}

void CAnalogInput::SetConfig(float scale, float offset)
{
	m_Scale = scale;
	m_Offset = offset;
}

inline uint16_t CAnalogInput::ReadAdc()
{
	return m_pAdc->Read();
}

#endif /* ANALOGINPUT_H_ */
