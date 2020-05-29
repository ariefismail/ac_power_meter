/*
 * AnalogInput.h
 *
 *  (c) 2016 Formulatrix
 *  All rights reserved
 */

#ifndef ANALOGINPUT_H_
#define ANALOGINPUT_H_

#include <Hal.h>
#include <Timeout.h>
#include <MovAvgBuffer.h>

class CAnalogInput
{
public:
	CAnalogInput();
	virtual ~CAnalogInput();

	void Initialize(IAdc* pAdc, ITimer *pTimer = NULL);
	void SetConfig(float scale, float offset);
	void GetConfig(float *scale, float *offset);
	void SetSamplingPeriod(uint16_t period);
	float Read();
	float ReadFiltered();
	float ReadAdcFiltered();
	uint16_t ReadAdc();
	void Execute();

private:
	static const uint16_t MAX_MOV_AVG_BUFFER = 100;
	float m_Scale;
	float m_Offset;
	CTimeout m_Timeout;
	ITimer *m_pTimer;
	IAdc* m_pAdc;
	CMovAvgBuffer<uint16_t, uint32_t, MAX_MOV_AVG_BUFFER> m_adcDataBuffer;
};

inline CAnalogInput::CAnalogInput()
{
	m_pAdc = NULL;
	m_Scale = 0.06378600823045268;
	m_Offset = 0;
	m_pTimer = NULL;
}

inline CAnalogInput::~CAnalogInput()
{

}

inline void CAnalogInput::Initialize(IAdc* pAdc, ITimer *pTimer)
{
	m_pAdc = pAdc;
	m_pTimer = pTimer;
	m_Timeout.Init(m_pTimer);
	m_Timeout.SetExpiry(100);
	m_adcDataBuffer.Clear();
}

inline float CAnalogInput::Read()
{
	return m_Scale * Read() + m_Offset;
}

inline void CAnalogInput::SetConfig(float scale, float offset)
{
	m_Scale = scale;
	m_Offset = offset;
}

inline void CAnalogInput::GetConfig(float *scale, float *offset)
{
	*scale = m_Scale;
	*offset = m_Offset;
}

inline void CAnalogInput::SetSamplingPeriod(uint16_t period)
{
	m_Timeout.SetExpiry(period);
}

inline uint16_t CAnalogInput::ReadAdc()
{
	return m_pAdc->Read();
}

inline float CAnalogInput::ReadAdcFiltered()
{
	return m_adcDataBuffer.Average();
}

inline float CAnalogInput::ReadFiltered()
{
	return m_Scale * m_adcDataBuffer.Average() + m_Offset;
}

inline void CAnalogInput::Execute()
{
	if (!m_pTimer) return;
	if (!m_Timeout.HasElapsed()) return;
	m_Timeout.Reset();
	m_adcDataBuffer.AddSample(m_pAdc->Read());
}

#endif /* ANALOGINPUT_H_ */
