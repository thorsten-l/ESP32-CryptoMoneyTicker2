#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include <Arduino.h>

#define DEBOUNCE_DURATION 100

class Button
{
private:
  int _gpioPort;
  time_t pressedTimestamp;

public:
  Button( int gpioPort );
  bool isPressed();
};

#endif

