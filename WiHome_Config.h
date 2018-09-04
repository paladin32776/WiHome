// WiFi Settings 

// MDSN Settings
#define MDNS_CLIENT_NAME "wihome1"

// MQTT Server Settings 
#define MQTT_SERVER      "raspi2.local"
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL
#define MQTT_USERNAME    ""
#define MQTT_KEY         ""
#define MQTT_KEEPALIVE   300000

// Pin configurations:
#define PIN_OUTPUT  LED_BUILTIN
#define PIN_INPUT   12
#define SOFT_AP_BUTTON 13

// EEPROM Adress Config:
#define EEPROM_ud_id 213
#define EEPROM_UserData 0
