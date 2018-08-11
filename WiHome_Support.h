#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect(Adafruit_MQTT_Client* mqtt);

