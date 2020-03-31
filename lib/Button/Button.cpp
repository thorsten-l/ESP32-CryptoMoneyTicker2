#include "Button.hpp"

Button::Button( int gpioPort )
{
  pressedTimestamp = 0;
  _gpioPort = gpioPort;
  pinMode(_gpioPort, INPUT_PULLUP);
  Serial.printf( "Button(%d) enabled", _gpioPort );
}

bool Button::isPressed()
{ 
  // Serial.printf( "%d = %s\n", _gpioPort, ( digitalRead(_gpioPort) ? "true" : "false" ));
  if ( digitalRead(_gpioPort) == true )
  {
    pressedTimestamp = millis();
  }
  else if ( millis() - pressedTimestamp >= DEBOUNCE_DURATION )
  {
    return true;
  }

  return false;
}
