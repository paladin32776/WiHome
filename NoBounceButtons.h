#include "Arduino.h"

#define MAX_BUTTONS 16

class NoBounceButtons
{
	private:
		unsigned long DebounceDelay = 50;     			// the debounce time (50ms); increase if the output flickers
		unsigned int Nbuttons = 0;
		unsigned int buttonpin[MAX_BUTTONS];
		unsigned int buttonState[MAX_BUTTONS];          // the current reading from the input pin
		unsigned int lastbuttonState[MAX_BUTTONS];      // the previous reading from the input pin
		unsigned long lastbuttonTime[MAX_BUTTONS] ;   	// the last time the output pin was toggled
		bool buttonAction[MAX_BUTTONS];
  public:
    NoBounceButtons(); 						// default constructor
    int create(unsigned int pin); 			// create a new button
  	bool action(unsigned int button_id);	// check if button has been pressed
  	void reset(unsigned int button_id);		// reset button action
    void check();	
};
