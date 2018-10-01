#include "Arduino.h"

// WiFi Settings
// #define WLAN_SSID       ""
// #define WLAN_PASS       ""

// MDSN Settings
// #define MDNS_CLIENT_NAME ""

// MQTT Server Settings
// #define MQTT_SERVER      ""
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL
#define MQTT_USERNAME    ""
#define MQTT_KEY         ""
#define MQTT_KEEPALIVE   300000

// Pin configurations SparkFun Dev Thing:
// #define PIN_LED     5
// #define PIN_BUTTON  12
// #define PIN_RELAY   13
// #define PIN_LED_ACTIVE_LOW true
// #define PIN_RELAY_ACTIVE_LOW false

// Pin config Gosund wall switch
#define PIN_LED     13
#define PIN_BUTTON  0
#define PIN_RELAY   12
#define PIN_LED_ACTIVE_LOW true
#define PIN_RELAY_ACTIVE_LOW false

// Pin configurations ESP-01: *** so far not working ***
// #define PIN_LED     13
// #define PIN_BUTTON  0
// #define PIN_RELAY   13
// #define PIN_LED_ACTIVE_LOW true
// #define PIN_RELAY_ACTIVE_LOW false
