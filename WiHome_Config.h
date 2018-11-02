#include "Arduino.h"

// MQTT Server Settings
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL
#define MQTT_USERNAME    ""
#define MQTT_KEY         ""
#define MQTT_KEEPALIVE   300000
#define MAX_MQTT_CONNECT_COUNT 10

// Pin configurations SparkFun Dev Thing:
// #define PIN_LED     5
// #define PIN_BUTTON  12
// #define PIN_RELAY   13
// #define PIN_LED_ACTIVE_LOW true
// #define PIN_RELAY_ACTIVE_LOW false

// Pin config Gosund wall switch
#define PIN_LED     5
#define PIN_BUTTON  0
// #define PIN_RELAY   12
#define PIN_LED_ACTIVE_LOW true
// #define PIN_RELAY_ACTIVE_LOW false

// Pin configurations ESP-01: *** so far not working ***
// #define PIN_LED     13
// #define PIN_BUTTON  0
// #define PIN_RELAY   13
// #define PIN_LED_ACTIVE_LOW true
// #define PIN_RELAY_ACTIVE_LOW false

// WiHome GateOpener config
#define GO_MOT_PIN_A 13
#define GO_MOT_PIN_B 12
#define GO_POS_PIN 0 // Analog0 of ADS1015
#define GO_ISENS_PIN 1 // Analog1 of ADS1015
#define GO_LED_PIN 4
#define GO_NVM_OFFSET 256

#define POSITION_FEEBACK_INTERVALL 500
