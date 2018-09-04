#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "WiHome_HTML.h"
#include <pgmspace.h>
#include <EEPROM.h>
#include "WiHome_Config.h"
#include "NoBounceButtons.h"

#define DEBUG_ESP_DNS
#define DEBUG_ESP_PORT Serial

// Function to connect to WiFi and mDNS
bool Wifi_connect(char* ssid, char* passwd, char* mdns_client_name, NoBounceButtons* nbb, int button);


// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect(Adafruit_MQTT_Client* mqtt);


// Function to create soft-AP
void Wifi_softAPmode(char* ssid);

Adafruit_MQTT_Publish* Adafruit_MQTT_Publish_3A(Adafruit_MQTT_Client* mqtt, char* client_name, const char* topic_name);
Adafruit_MQTT_Subscribe* Adafruit_MQTT_Subscribe_3A(Adafruit_MQTT_Client* mqtt, char* client_name, const char* topic_name);


class UserData
{
  private:
    void CharArrayToEEPROM(char* ps, unsigned int offset, unsigned int bytes);
    void CharArrayFromEEPROM(char* ps, unsigned int offset, unsigned int bytes);
  
  public:
    byte ud_id = 0;
    char wlan_ssid[32];
    char wlan_pass[32];
    char mqtt_broker[32];
    char mdns_client_name[32];
    UserData();
    bool load();
    void save();
};


// Web server class
class ConfigWebServer
{
  private:
    ESP8266WebServer* webserver;
    const byte DNS_PORT = 53;
    DNSServer* dnsServer;
    UserData* userdata;
  public:
     ConfigWebServer(int port, UserData* pud);
     void handleRoot(); 
     void handleNotFound();
     void handleSaveAndRestart();
     void handleClient();
};


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




