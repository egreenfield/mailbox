#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "application.h"

class Button
{
public:
	Button(int pin = -1);
	void init(int pin = -1);
	void poll();
	bool on() {return _state == HIGH;}
	bool changed() {return _changed;}
private:
	int _pin;
	int _state;
	int _counter;
	int _readValue;
	unsigned int _lastSampleTime;
	bool _changed;
};

#endif
