#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ESP8266mDNS.h>

// Function to connect to WiFi and mDNS
void Wifi_connect(char* ssid, char* passwd, char* mdns_client_name);

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect(Adafruit_MQTT_Client* mqtt);

// Function to create soft-AP
void Wifi_softAPmode(char* ssid);

// Class to simplify checking if enough time has been passed since last event
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

