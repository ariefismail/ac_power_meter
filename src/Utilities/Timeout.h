/*
 * Timeout.h
 *
 *  Created on: Apr 18, 2020
 *      Author: Formulatrix
 */

#ifndef UTILITIES_TIMEOUT_H_
#define UTILITIES_TIMEOUT_H_

#include <Hal.h>

class CTimeout
{
public:
	void Init(ITimer *pTimer);
	void Reset();
	void SetExpiry(uint16_t tick);
	bool HasElapsed();

	CTimeout();
	virtual ~CTimeout();

private:
	ITimer *m_timer;
	uint16_t m_expiry;
	uint16_t m_prevTimer;
	bool m_hasElapsed;
};

#endif /* UTILITIES_TIMEOUT_H_ */
