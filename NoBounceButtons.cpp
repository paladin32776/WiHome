#include "NoBounceButtons.h"

NoBounceButtons::NoBounceButtons() // default constructor
{
}

int NoBounceButtons::create(unsigned int pin)  // create a new button
{
  if (Nbuttons<MAX_BUTTONS)
  {
    pinMode(pin,INPUT_PULLUP);
    buttonpin[Nbuttons] = pin;    // pin to which button is connected
    lastbuttonState[pin] = LOW;   // the previous reading from the input pin
    lastbuttonTime[pin] = 0;      // the last time the output pin was toggled
    buttonAction[pin] = false;        // button action state variable
    
    return Nbuttons++;
  }
  else 
    return -1;
}

bool NoBounceButtons::action(unsigned int button_id)  // check if button has been pressed
{
  return buttonAction[button_id];
}

void NoBounceButtons::reset(unsigned int button_id)  // reset button action
{
  buttonAction[button_id] = false;
}

void NoBounceButtons::check()
{
  int reading;
  for (unsigned int button_id=0; button_id<Nbuttons; button_id++)
  {
    // togglebutton read and debounce code
    reading = digitalRead(buttonpin[button_id]);
    if (reading != lastbuttonState[button_id])
      lastbuttonTime[button_id] = millis();
    if ((millis() - lastbuttonTime[button_id]) > DebounceDelay)
    {
      if (reading != buttonState[button_id])
      {
        buttonState[button_id] = reading;
        if (buttonState[button_id] == LOW)
          buttonAction[button_id] = true;
      }
    }
    lastbuttonState[button_id] = reading;
  }
}
