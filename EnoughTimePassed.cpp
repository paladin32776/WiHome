// Class to simplify checking if enough time 
// has been passed since last event
// Author: Gernot Fattinger (2018)

#include "EnoughTimePassed.h"

EnoughTimePassed::EnoughTimePassed(unsigned long desired_intervall) // setup object with desired intervall
{
  intervall = desired_intervall;
  last_time = 0;
  once_called = false;  
}

bool EnoughTimePassed::enough_time()      // create a new button
{
  unsigned long t = millis();
  if ( (t-last_time>intervall)  || (once_called==false) )
    {
      last_time = t;
      once_called = true;
      return true;
    }
  return false;
}

void EnoughTimePassed::event()             // manually tell that an event has happened just now
{
  last_time = millis();
  once_called = true;
}

void EnoughTimePassed::change_intervall(unsigned long desired_intervall)
{
  intervall = desired_intervall;
}