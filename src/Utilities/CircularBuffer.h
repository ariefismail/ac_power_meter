/*
* CircularBuffer.h
* 
* Template of class to store the last N elements.
* it behaves as same as hardware buffer. Unlike the 
* Circular FIFO queue, even if it is full, new element
* will overwrite the oldest (first) element. There is
* no method to erase element by element.
*
* (c) 2017 Formulatrix
* All rights reserved
*/

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>


/************* Template Class  *****************/

template<class ELEMTYPE, uint16_t BUFFSIZE>
class CCircularBuffer
{
public:
	CCircularBuffer(void) :
		m_Size(BUFFSIZE)
	{
		m_IsFull = 0;
		m_Head = 0;
		this->Clear();
	}
	~CCircularBuffer(void);

	void Add(ELEMTYPE value);
	ELEMTYPE Get(uint16_t index) const;

	uint16_t Size(void) const;
	uint16_t Count(void) const;
	bool IsFull(void) const;

	void Clear(void);

private:
	const uint16_t m_Size;
    bool m_IsFull;
	ELEMTYPE m_Array[BUFFSIZE];
	uint16_t m_Head;
};


/********* Function Definitions *****************/


//template<class ELEMTYPE, uint16_t BUFFSIZE>
//CCircularBuffer<ELEMTYPE,BUFFSIZE>::CCircularBuffer(void)
//{
//}

template<class ELEMTYPE, uint16_t BUFFSIZE>
CCircularBuffer<ELEMTYPE,BUFFSIZE>::~CCircularBuffer(void)
{
}

template<class ELEMTYPE, uint16_t BUFFSIZE>
void CCircularBuffer<ELEMTYPE,BUFFSIZE>::Add(ELEMTYPE value)
{
	m_Array[m_Head] = value;
	
	m_Head = (m_Head + 1) % m_Size;
	
	if(m_Head == 0)
	{
		m_IsFull = 1;
	}
}

template<class ELEMTYPE, uint16_t BUFFSIZE>
ELEMTYPE CCircularBuffer<ELEMTYPE,BUFFSIZE>::Get(uint16_t index) const
{
	if(index > m_Size)
	{
		return NULL;
	}

	if(!m_IsFull && (index >= m_Head))
	{
		return NULL;
	}

	//basically, if not full yet -> m_Array[index]
	return m_Array[(m_Head * m_IsFull + index) % m_Size];
}

template<class ELEMTYPE, uint16_t BUFFSIZE>
uint16_t CCircularBuffer<ELEMTYPE,BUFFSIZE>::Size(void) const
{
	return m_Size;
}

template<class ELEMTYPE, uint16_t BUFFSIZE>
uint16_t CCircularBuffer<ELEMTYPE,BUFFSIZE>::Count(void) const
{
	if(m_IsFull)
	{
		return m_Size;
	}
	else
	{
		return m_Head;
	}
}

template<class ELEMTYPE, uint16_t BUFFSIZE>
bool CCircularBuffer<ELEMTYPE,BUFFSIZE>::IsFull(void) const
{
	return m_IsFull;
}

template<class ELEMTYPE, uint16_t BUFFSIZE>
void CCircularBuffer<ELEMTYPE,BUFFSIZE>::Clear(void)
{
	m_IsFull = 0;
	m_Head = 0;
}

#endif /* CIRCULARBUFFER_H_ */
