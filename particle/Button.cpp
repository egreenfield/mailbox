#include "Button.h"


Button::Button(int pin)
: _pin(pin) 
, _on(false)
, _counter(0)
{
  if(pin >= 0)
    pinMode(_pin, INPUT);
}

int debounce_count = 10; // number of millis/samples to consider before declaring a debounced input


void Button::poll()
{
  // If we have gone on to the next millisecond
  if(millis() != _lastSampleTime)
  {
    _readValue = digitalRead(inPin);

    if(_readValue == _state && _counter > 0)
    {
      _counter--;
    }
    if(_readValue != _state)
    {
       _counter++; 
    }
    // If the Input has shown the same value for long enough let's switch it
    if(_counter >= debounce_count)
    {
      _counter = 0;
      _state = _readValue;
    }
    time = millis();
  }
}
