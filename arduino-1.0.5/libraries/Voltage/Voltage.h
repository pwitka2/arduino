// Voltage.h

#ifndef _VOLTAGE_h
#define _VOLTAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

long readVcc(void);

#endif

