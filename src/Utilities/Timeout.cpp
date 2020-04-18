/*
 * Timeout.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: Formulatrix
 */

#include <Timeout.h>

void CTimeout::Init(ITimer *pTimer)
{
	m_timer = pTimer;
}

void CTimeout::Reset()
{
	m_prevTimer = m_timer->CurrentTicks();
	m_hasElapsed = false;
}

void CTimeout::SetExpiry(uint16_t tick)
{
	m_expiry = tick;
	Reset();
}

bool CTimeout::HasElapsed()
{
	if (!m_hasElapsed) if (m_timer->CurrentTicks() - m_prevTimer >= m_expiry) m_hasElapsed = true;
	return m_hasElapsed;
}

CTimeout::CTimeout()
{
	// TODO Auto-generated constructor stub

}

CTimeout::~CTimeout()
{
	// TODO Auto-generated destructor stub
}

