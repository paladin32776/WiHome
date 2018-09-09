// Class to simplify checking if enough time 
// has been passed since last event
// Author: Gernot Fattinger (2018)

#include "Arduino.h"

class EnoughTimePassed
{
  private:
    unsigned long last_time;
    unsigned long intervall; 
    bool once_called;
  public:
    EnoughTimePassed(unsigned long desired_intervall);  // setup object with desired intervall
    bool enough_time();       // check if enough time has passed since last event
    void event();             // manually tell that an event has happened just now
    void change_intervall(unsigned long desired_intervall);
};