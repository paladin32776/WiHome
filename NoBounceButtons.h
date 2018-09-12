#include "Arduino.h"

#define MAX_BUTTONS 16
#define DEBOUNCE_DELAY 50
#define MIN_HOLD_TIME 2000

class NoBounceButtons
{
	private:
		unsigned char N = 0;
		unsigned char Pin[MAX_BUTTONS];
		unsigned char State[MAX_BUTTONS];          // current "official" state of the button
		unsigned long LastStateTime[MAX_BUTTONS];  // last time the state changed
		unsigned char LastValue[MAX_BUTTONS];      // previous reading from the input pin
		unsigned long LastValueTime[MAX_BUTTONS] ;   	// last time the input pin reading changed
		unsigned char Action[MAX_BUTTONS];
  public:
    NoBounceButtons(); 											// default constructor
    char create(unsigned char pin); 				// create a new button and return id
  	unsigned char action(unsigned char id);	// check if button has been pressed
  	void reset(unsigned char id);						// reset button action
    void check();														// call in main loop; contains all the important code
};
