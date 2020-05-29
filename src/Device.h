/*
 * Device.h
 *
 *  Created on: May 8, 2020
 *      Author: Formulatrix
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <Hal.h>
#include <HeartBeat.h>
#include <DMD/DMD.h>
#include <DMD/SystemFont5x7.h>
#include <DMD/Arial_black_16.h>
#include <DMD/Arial14.h>
#include <ACFrequencyMeter.h>
#include <AnalogInput.h>
#include <ACDisplayDmd.h>
#include <Communication/SerialPort.h>
#include <DMD/font.h>

class SDevice
{
public:
	SDevice* Inst();

	CHeartBeat HeartBeat;
	CAnalogInput AnalogInput[2];
	DMD Dmd;
	CACDisplayDmd AcDisplay;
	CACFrequencyMeter ACFrequencyMeter;
	CSerialPort SerialPort;

};

extern SDevice Dev;

#endif /* DEVICE_H_ */
