#include "NoBounceButtons.h"

NoBounceButtons::NoBounceButtons() // default constructor
{
}

char NoBounceButtons::create(unsigned char pin)  // create a new button
{
  if (N<MAX_BUTTONS)
  {
    pinMode(pin,INPUT_PULLUP);
    Pin[N] = pin;    // pin to which button is connected
    LastValue[N] = HIGH;       // last value of the pin before it was toggled
    LastValueTime[N] = 0;      // last time the pin was toggled
    LastStateTime[N] = 0;      // last time the "official" state was changed
    State[N] = HIGH;           // current "official" state of the button
    Action[N] = 0;         // button action state variable

    return N++;  // Return N, and then only increase it by one
                 // (so the value before the increase gets returned)
  }
  else
    return -1;
}

unsigned char NoBounceButtons::action(unsigned char id)  // check if button has been pressed
{
  return Action[id];
}


void NoBounceButtons::reset(unsigned char id)  // reset button action
{
  Action[id] = 0;
}

void NoBounceButtons::check()
{
  int Value;
  unsigned long Time;
  for (unsigned int id=0; id<N; id++)
  {
    // togglebutton read and debounce code
    Value = digitalRead(Pin[id]);
    Time = millis();
    // Detect a change at the pin:
    if (Value != LastValue[id])
      LastValueTime[id] = Time;
    LastValue[id] = Value;
    // If last change/slope is further in the past than a pre-defined time span,
    // and the current value differs from the "official" state:
    if ((Time - LastValueTime[id]) > DEBOUNCE_DELAY && Value != State[id])
    {
      State[id] = Value;  // make the value the "official" state
      // Check for short click: state=HIGH and time since last state change <MIN_HOLD_TIME
      if ((State[id] == HIGH) && (Time-LastStateTime[id]<MIN_HOLD_TIME))
        Action[id] = 1;
      // Check for long click: state=HIGH and time since last state change >=MIN_HOLD_TIME
      else if ((State[id] == HIGH) && (Time-LastStateTime[id]>=MIN_HOLD_TIME))
        Action[id] = 2;
      LastStateTime[id] = Time; // make the current time the last state change time
    }
  }
}
