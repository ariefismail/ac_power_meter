/*
 * HeartBeat.h
 *
 *  Created on: Apr 18, 2020
 *      Author: Formulatrix
 */

#ifndef DEVICE_HEARTBEAT_H_
#define DEVICE_HEARTBEAT_H_

#include <Hal.h>
#include <Timeout.h>

class CHeartBeat
{
public:
	void Init(IGpio *pIo,ITimer *pTimer,uint16_t period);
	void Execute();
	CHeartBeat();
	virtual ~CHeartBeat();

private:
	CTimeout m_timer;
	IGpio *m_pIo;
};

#endif /* DEVICE_HEARTBEAT_H_ */
