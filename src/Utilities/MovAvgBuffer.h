/*
* MovAvguffer .h
* 
* Template of class to store last N samples
* inside a buffer and calculate their moving
* average. This template supports both normal
* and weighted moving average. Note that in case
* of weighted average, it is assumed that the
* weight is already normalized (sum of weight equal 
* to 1). 
*
* (c) 2017 Formulatrix
* All rights reserved
*/

#ifndef MOVAVG_BUFFER_H_
#define MOVAVG_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <CircularBuffer.h>


/************* Template Class  *****************/

template<class ELEMTYPE, class SUMTYPE, uint16_t BUFFSIZE>
class CMovAvgBuffer
{

public:
	CMovAvgBuffer(void);
	~CMovAvgBuffer(void);

	void AddSample(ELEMTYPE sample);
	float Average(void) const;

	void Clear(void);

	uint16_t Size(void) const;
	uint16_t Count(void) const;
	bool IsFull(void) const;

private:
	float m_Average;
	SUMTYPE m_Sum;
	CCircularBuffer<ELEMTYPE, BUFFSIZE> m_Buffer;
};


/********* Function Definitions *****************/

template<class ELEMTYPE, class SUMTYPE, uint16_t BUFFSIZE>
CMovAvgBuffer<ELEMTYPE, SUMTYPE, BUFFSIZE>::CMovAvgBuffer(void)
{
	this->Clear();
}

template<class ELEMTYPE, class SUMTYPE, uint16_t BUFFSIZE>
CMovAvgBuffer<ELEMTYPE, SUMTYPE, BUFFSIZE>::~CMovAvgBuffer(void)
{
}

template<class ELEMTYPE, class SUMTYPE, uint16_t BUFFSIZE>
void CMovAvgBuffer<ELEMTYPE, SUMTYPE, BUFFSIZE>::AddSample(ELEMTYPE sample)
{
	SUMTYPE firstSample = 0;
	if(this->IsFull())
		firstSample = (SUMTYPE) m_Buffer.Get(0);
	m_Buffer.Add(sample);
	m_Sum += (SUMTYPE)m_Buffer.Get(this->Count()-1) - firstSample;
}

template<class ELEMTYPE, class SUMTYPE, uint16_t BUFFSIZE>
inline float CMovAvgBuffer<ELEMTYPE, SUMTYPE, BUFFSIZE>::Average(void) const
{
	return (float)m_Sum / (float)this->Count();
}

template<class ELEMTYPE, class SUMTYPE, uint16_t BUFFSIZE>
void CMovAvgBuffer<ELEMTYPE, SUMTYPE, BUFFSIZE>::Clear(void)
{
	m_Buffer.Clear();
	m_Average = 0.0;
	m_Sum = 0;
}

template<class ELEMTYPE, class SUMTYPE, uint16_t BUFFSIZE>
inline uint16_t CMovAvgBuffer<ELEMTYPE, SUMTYPE, BUFFSIZE>::Size(void) const
{
	return m_Buffer.Size();
}

template<class ELEMTYPE, class SUMTYPE, uint16_t BUFFSIZE>
inline uint16_t CMovAvgBuffer<ELEMTYPE, SUMTYPE, BUFFSIZE>::Count(void) const
{
	return m_Buffer.Count();
}

template<class ELEMTYPE, class SUMTYPE, uint16_t BUFFSIZE>
inline bool CMovAvgBuffer<ELEMTYPE, SUMTYPE, BUFFSIZE>::IsFull(void) const
{
	return m_Buffer.IsFull();
}

#endif
