#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "application.h"

class Button
{
public:
	Button(int pin = -1);
	void poll();
	bool on() {return _state == HIGH;}
private:
	int _pin;
	int _state;
	int _counter;
	int _readValue;
	long _lastSampleTime;
};

#endif
