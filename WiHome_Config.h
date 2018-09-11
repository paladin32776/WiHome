#include "Arduino.h"

// WiFi Settings
#define WLAN_SSID       "ExtremeG"
#define WLAN_PASS       "rideldibixwpa2"

// MDSN Settings
#define MDNS_CLIENT_NAME "wihome1"

// MQTT Server Settings
#define MQTT_SERVER      "cncmill.local"
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL
#define MQTT_USERNAME    ""
#define MQTT_KEY         ""
#define MQTT_KEEPALIVE   300000

// Pin configurations:
#define PIN_OUTPUT  5
#define PIN_INPUT   12
#define SOFT_AP_BUTTON 13
