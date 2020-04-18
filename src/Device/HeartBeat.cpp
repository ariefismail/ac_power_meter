/*
 * HeartBeat.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: Formulatrix
 */

#include <HeartBeat.h>

void CHeartBeat::Init(IGpio *pIo,ITimer *pTimer,uint16_t period)
{
	m_pIo=pIo;
	m_timer.Init(pTimer);
	m_timer.SetExpiry(period);
}

void CHeartBeat::Execute()
{
	if(!m_timer.HasElapsed())
		return;
	m_pIo->Toggle();

}

CHeartBeat::CHeartBeat()
{
	// TODO Auto-generated constructor stub

}

CHeartBeat::~CHeartBeat()
{
	// TODO Auto-generated destructor stub
}

