/*
 * ACDisplayDmd.h
 *
 *  Created on: May 4, 2020
 *      Author: Formulatrix
 */

#ifndef DEVICE_ACDISPLAYDMD_H_
#define DEVICE_ACDISPLAYDMD_H_

#include <DMD/DMD.h>
#include <Hal.h>
#include <Timeout.h>
#include <ACFrequencyMeter.h>

class CACDisplayDmd
{
public:
	CACDisplayDmd();
	virtual ~CACDisplayDmd();

private:
	CTimeout m_timeout;

};

#endif /* DEVICE_ACDISPLAYDMD_H_ */
